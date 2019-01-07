/* symbol_table02.h */

#define MAX_ID_NAME     31  /* ID の最大値 */

#define MAX_TABLE_SIZE 200  /* 記号表の大きさ */
#define MAX_STACK_SIZE  30  /* スタックの大きさ(ブロックレベルに対応) */
#define START_HEAP_ADDRESS 800 /* ヒープ領域の開始アドレス */

typedef enum { /* 記号表に登録する名前の種別 */
  const_id,  /* 定数 */
  var_id,    /* 変数 */
  func_id,   /* 関数 */
  param_id   /* 関数の仮引数 */
} id_type;

struct table_entry { /* 記号表に登録する要素 */
  id_type type;               /* 種別 */
  char id_name[MAX_ID_NAME];  /* 名前 */
  int line_no;                /* 宣言されたソースコード上の行数 */
  union {
    int value;      /* 定数の場合、値 */
    struct {
      int n_params; /* 関数の場合、仮引数の個数 */
      int address;  /* 関数の場合、先頭アドレス */
    } func;
    int address;    /* 変数/仮引数の場合格納アドレス */
  } data;
};

extern struct table_entry get_table(int ptr);
extern int get_symbol_address(int ptr);
extern int get_func_params(int func_ptr);
extern int search_table(char *id_name);

extern int reg_const_in_tbl(char *id_name, int line_no, int t_num_value);
extern int reg_var_in_tbl(char *id_name, int line_no);
extern int reg_func_in_tbl(char *id_name, int line_no);
extern int reg_param_in_tbl(char *id_name, int line_no, int func_ptr);
extern int reg_func_address(int func_ptr, int code_ptr);
extern int end_param(int func_ptr);

extern int get_blocklevel();
extern void blocklevel_up();
extern void blocklevel_down();

extern void reference_info(char *ref_name, int ref_line, int ptr);
