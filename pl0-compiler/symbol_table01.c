/* pl0-ll-parser02 用記号表完成バージョン
 *
 */

#include <string.h>
#include "misc.h"
#include "symbol_table01.h"

#define MAX_TABLE_SIZE 200  /* 記号表の大きさ */
#define MAX_STACK_SIZE  30  /* スタックの大きさ(ブロックレベルに対応) */

static struct table_entry symbol_table[MAX_TABLE_SIZE]; /* 記号表 */
static int symbol_table_ptr = 0; /* 記号表の現在位置を示すポインタ */

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

/* 記号表の ptr 位置の情報を得る */
struct table_entry get_table(int ptr) {
  return symbol_table[ptr];
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
  return t_ptr;
}

/* ブロックレベル処理関連 */

/* ブロックレベルを上げる */
void blocklevel_up() {
  if (stack_ptr == 0) { /* 初期化 */
    stack_for_symbol_table[stack_ptr] = 0;
  }

  stack_ptr++;
  if (stack_ptr >= MAX_STACK_SIZE) pl0_error("", 0, "ブロックレベルが最大値を超えた");
  stack_for_symbol_table[stack_ptr] = symbol_table_ptr;
}

/* ブロックレベルを下げる */
void blocklevel_down() {
  if (stack_ptr < 1) pl0_error("", 0, "ブロックレベルがおかしい");
  symbol_table_ptr = stack_for_symbol_table[stack_ptr];
  stack_ptr--;
}

/* 参照情報表示 */
void reference_info(char *ref_name, int ref_line, id_type type, int def_line) {
  char *type_st ;

  switch (type) {
  case const_id:
    type_st = "定数"; break;
  case var_id:
    type_st = "変数"; break;
  case func_id:
    type_st = "関数名"; break;
  case param_id:
    type_st = "仮引数"; break;
  default:
    type_st = ""; break;
  }

  printf ("情報:%3d 行目の %s は%3d 行目で宣言された %s です\n",
	  ref_line, ref_name, def_line, type_st);
}
