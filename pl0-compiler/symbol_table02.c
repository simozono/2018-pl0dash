/* pl0-ll-compiler 用記号表
 *
 */

#include <string.h>
#include "misc.h"
#include "symbol_table02.h"

static struct table_entry symbol_table[MAX_TABLE_SIZE]; /* 記号表 */
static int symbol_table_ptr = 0; /* 記号表の現在位置を示すポインタ */

static int heap_address   = START_HEAP_ADDRESS; /* 大域変数(ブロック0)用ヒープアドレス */
static int func_var_addr  = 0;   /* 関数内定数/変数のアドレス  */
static int func_parm_addr = 0;   /* 関数内仮引数のアドレス     */

static int stack_for_symbol_table[MAX_STACK_SIZE]; /* 意味解析用 ptr を覚えておく */
static int stack_ptr = 0; /* 上記スタックの現在位置を示すポインタ */


/* 同一スコープ内で二重登録していないかチェック */
int double_register_check(char *id_name) {
  int i = symbol_table_ptr;
  int bottom = stack_for_symbol_table[stack_ptr]; /* 現在のスコープの先頭 */
  while(strcmp(id_name, symbol_table[i].id_name) && (i-bottom) > 0) {
    i--;
  }
  if (i == bottom) {
    return 0; /* 0だったら登録されていない */
  } else {
    return i;
  }
}

/* id_name 名で記号表を検索 */
int search_table(char *id_name) {
  int i = symbol_table_ptr;
  while(strcmp(id_name, symbol_table[i].id_name) && i > 0) {
    i--;
  }
  return i; /* 0 だったら登録されていない。1以上は記号表の位置 */
}

/* 記号表の ptr 位置の物の情報を得る */
struct table_entry get_table(int ptr) {
  return symbol_table[ptr];
}

/* 記号表 ptr 位置の物のアドレスを返す */
int get_symbol_address(int ptr) {
  int address;
  if (symbol_table[ptr].type != func_id) {
    address = symbol_table[ptr].data.address;
  } else {
    address = symbol_table[ptr].data.func.address;
  }
  return address;
}

/* 定数名/変数名/関数名/仮引数を記号表に登録する低レベル関数 */
int add_table(id_type type, char *id_name, int line_no) {
  int check;
  struct table_entry t_ent;

  check = double_register_check(id_name);
  if (check > 0) {
    t_ent = get_table(check);
    printf ("エラー: %d行目%sは、%d行目で既に登録されています\n",
              line_no, id_name, t_ent.line_no);
    /* pl0_error(id_name, line_no, "既に登録されています"); */
    exit(EXIT_FAILURE);
  }

  symbol_table_ptr++;

  if (symbol_table_ptr >= MAX_TABLE_SIZE) {
    pl0_error(id_name, line_no, "名前の登録数が多すぎます");
  }  else {
    symbol_table[symbol_table_ptr].type = type;
    strcpy(symbol_table[symbol_table_ptr].id_name, id_name);
    symbol_table[symbol_table_ptr].line_no = line_no;
  }

  return symbol_table_ptr;
}

/* 定数を記号表に登録 */
int reg_const_in_tbl(char *id_name, int line_no, int t_num_value) {
  int t_ptr ;
  t_ptr = add_table(const_id, id_name, line_no);
  symbol_table[t_ptr].data.value = t_num_value;
  return t_ptr;
}

/* 変数を記号表に登録 */
int reg_var_in_tbl(char *id_name, int line_no) {
  int t_ptr ;
  t_ptr = add_table(var_id, id_name, line_no);
  if (stack_ptr < 1) { /* ブロックレベル0 */
    symbol_table[t_ptr].data.address = heap_address;
    heap_address++;
  } else { /* ブロックレベル1以上 */
    func_var_addr--;
    symbol_table[t_ptr].data.address = func_var_addr;
  }
  return t_ptr;
}

/* 関数を記号表に登録 */
int reg_func_in_tbl(char *id_name, int line_no) {
  int t_ptr ;
  t_ptr = add_table(func_id, id_name, line_no);
  symbol_table[t_ptr].data.func.n_params = 0;
  return t_ptr;
}

/* 仮引数を記号表に登録 */
int reg_param_in_tbl(char *id_name, int line_no, int func_ptr) {
  int t_ptr ;
  t_ptr = add_table(param_id, id_name, line_no);
  symbol_table[func_ptr].data.func.n_params++;
  func_parm_addr++;
  symbol_table[t_ptr].data.address = func_parm_addr;
  return t_ptr;
}

/* 関数の開始コードアドレス登録 */
int reg_func_address(int func_ptr, int code_ptr) {
  if (symbol_table[func_ptr].type != func_id) {
    pl0_error(symbol_table[func_ptr].id_name, 0,
              "set_func_addressがおかしい");
  }
  symbol_table[func_ptr].data.func.address = code_ptr;
  return func_ptr;
}

int get_func_params(int func_ptr) {
  if (symbol_table[func_ptr].type != func_id) {
    pl0_error(symbol_table[func_ptr].id_name, 0,
              "get_func_paramsがおかしい");
  }
  return symbol_table[func_ptr].data.func.n_params;
}


/* 仮引数宣言部の最後で呼ばれ、仮引数のアドレス再計算 */
int end_param(int func_ptr) {
  /* 以下危険なコード */
  /* func_ptr の次のエントリーがfuncの */
  /* 仮引数であることを想定している */

  int n = symbol_table[func_ptr].data.func.n_params ; /* 関数の引数の個数 */
  int i;
  int addr = 0;
  for (i = 0; i < n; i++) {
    addr = symbol_table[func_ptr+1+i].data.address;
    symbol_table[func_ptr+1+i].data.address = 2+(n-addr);
  }
  return 0;
}


/* ブロックレベル処理関連 */
/* ブロックレベルを上げる */
void blocklevel_up() {
  func_var_addr =  0;
  func_parm_addr = 0;

  if (stack_ptr == 0) { /* 初期化 */
    stack_for_symbol_table[stack_ptr] = 0;
  }

  stack_ptr++;
  if (stack_ptr >= MAX_STACK_SIZE)
    pl0_error("", 0, "ブロックレベルが最大値を超えた");
  stack_for_symbol_table[stack_ptr] = symbol_table_ptr;
}

/* ブロックレベルを下げる */
void blocklevel_down() {
  if (stack_ptr < 1) pl0_error("", 0, "ブロックレベルがおかしい");
  symbol_table_ptr = stack_for_symbol_table[stack_ptr];
  stack_ptr--;
}

/* 現在のブロックレベルを調べる */
int get_blocklevel() {
  return stack_ptr;
}


/* 参照情報表示 */
void reference_info(char *ref_name, int ref_line, int ptr) {
  struct table_entry t_ent ;
  char *type_st;
  int address;

  t_ent = get_table(ptr);

  switch (t_ent.type) {
  case const_id:
    type_st = "定数"; address = t_ent.data.value;
    break;
  case var_id:
    type_st = "変数"; address = t_ent.data.address;
    break;
  case func_id:
    type_st = "関数名"; address = t_ent.data.func.address;
    break;
  case param_id:
    type_st = "仮引数"; address = t_ent.data.address;
    break;
  default:
    type_st = ""; break;
  }

  printf ("情報:%3d 行目の %s は%3d 行目で宣言された %s です。アドレスは %d\n",
            ref_line, ref_name, t_ent.line_no, type_st, address);
}
