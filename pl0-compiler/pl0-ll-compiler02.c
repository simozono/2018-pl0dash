/* PL/0' 用 LL(1)再帰下降型コンパイラ No.02
 *              2018年後期 鹿児島高専
 *              3年生 言語処理系 授業用
 *   * 最終バージョン(関数対応版)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "misc.h"
#include "../scanner/tokentable.h"
#include "symbol_table02.h"
#include "codegen.h"

extern FILE *yyin;   /* 読み込むソースファイル */
extern int yylex();  /* lex の字句解析 */
extern int line_no;  /* 行番号 */
extern char *yytext; /* lex よりレクシムが入る */

extern int t_num_value;  /* T_NUMBER の実際の値 */

int getToken(void);  /* トークンを取得する関数 */

/* 非終端記号に対応した関数 */
void parse_Program(void);
void parse_Block(void);
void parse_ConstDeclList(void);
void parse_ConstDecl(void);
void parse_ConstIdList(void);
void parse_ConstIdList_dash(void);
void parse_VarDeclList(void);
void parse_VarDecl(void);
void parse_VarIdList(void);
void parse_VarIdList_dash(void);
void parse_FuncDeclList(void);
void parse_FuncDecl(void);
void parse_FuncDeclIdList(int func_ptr);
void parse_FuncDeclIdList_dash(int func_ptr);
void parse_FuncBlock(void);
void parse_Statement(void);
void parse_StatementList(void);
void parse_StatementList_dash(void);
void parse_Condition(void);
void parse_Expression(void);
void parse_Expression_dash(void);
void parse_Term(void);
void parse_Term_dash(void);
void parse_Factor(void);
int parse_FuncArgList(int n_args);
int parse_FuncArgList_dash(int n_args);

int nextToken;    /* 次のトークンが入る変数 */
int current_func; /* 現在処理している関数の記号表での位置 */

int getToken(void) { /* トークンを取得する関数 */
  int token = yylex();
  /* yylex()が0を返す時がEOFのようだ */
  if (token == 0)
    token = T_EOF;
  return token;
}

int main(int argc, char *argv[]) {

  int opts;
  int n_flg = 0;
  int o_flg = 0;
  FILE *out_fp;

  char out_filename[FILENAME_MAX];

  while ((opts = getopt(argc, argv, "hno:")) != -1) {
    switch (opts) {
    case 'h':
      fprintf(stderr, "%s [-h] [-n] [-o output_file] source_file\n",
	      argv[0]);
      exit(EXIT_SUCCESS);
      break;
    case 'n':
      n_flg = 1;
      break;
    case 'o':
      o_flg = 1;
      strcpy(out_filename, optarg);
      break;
    case '?':
      exit(EXIT_FAILURE);
      break;
    }
  }

  if (optind >= argc) {
    fprintf(stderr, "ソースプログラムのファイル名を指定してください\n");
    exit(EXIT_FAILURE);
  }

  yyin = fopen(argv[optind], "r");
  if (yyin  == NULL) {
    fprintf(stderr, "%s というファイルがありません\n", argv[optind]);
    exit(EXIT_FAILURE);
  }

  out_fp = stdout;
  if (o_flg) {
    if ((out_fp = fopen(out_filename, "w")) == NULL) {
      fprintf(stderr, "%s という出力ファイルを作成できません\n",
	      out_filename);
      exit(EXIT_FAILURE);
    }
  }

  /* 構文解析スタート */
  nextToken = getToken();
  parse_Program();
  if (nextToken != T_EOF)
    pl0_error("",line_no, "EOFでない");
  gencode_no_arg(end);

  /* 生成コード出力 */
  list_code(out_fp, n_flg);
  exit(EXIT_SUCCESS);
}

void parse_Program() {
  /* <Program> -> <Block> T_PRIOD */
  parse_Block();
  if (nextToken != T_PERIOD)
    pl0_error(yytext, line_no, "ピリオドでない");
  nextToken = getToken();
}

void parse_Block() {
  /* <Block> -> <ConstDeclList> <VarDeclList> <FuncDeclList> <Statement> */
  int goto_main;

  parse_ConstDeclList();
  parse_VarDeclList();
  goto_main = gencode_arg_V(jmp,0); /* 関数のコードを飛ばす */
  parse_FuncDeclList();
  backpatch(goto_main); /* jmp を書き換え */
  parse_Statement();
}

void parse_ConstDeclList() {
  /* <ConstDeclList> -> <ConstDecl> <ConstDeclList> | ε */
  if (nextToken == T_CONST) { /* First(<ConstDecl>) */
    parse_ConstDecl();
    parse_ConstDeclList();
  } else {
    /* ε */
  }
}

void parse_ConstDecl() {
  /* <ConstDecl> -> T_CONST <ConstIdList> T_SEMIC */
  /* T_CONST では何もしない。次のトークンを読む */
  nextToken = getToken();
  parse_ConstIdList();
  if (nextToken != T_SEMIC)
    pl0_error(yytext, line_no, ";でない");
  nextToken = getToken();
}

void parse_ConstIdList() {
  /* <ConstIdList> -> T_ID T_EQ T_NUMBER <ConstIdList_dash> */
  char const_name[MAX_ID_NAME];

  if (nextToken != T_ID)
    pl0_error(yytext, line_no, "定数名でない");
  strcpy(const_name, yytext);
  nextToken = getToken();
  if (nextToken != T_EQ)
    pl0_error(yytext, line_no, "=でない");
  nextToken = getToken();
  if (nextToken != T_NUMBER)
    pl0_error(yytext, line_no, "数でない");
  /* 定数名の登録および値の設定 */
  reg_const_in_tbl(const_name, line_no, t_num_value);
  nextToken = getToken();
  parse_ConstIdList_dash();
}

void parse_ConstIdList_dash() {
  /* <ConstIdList_dash> -> T_COMMA T_ID T_EQ T_NUMBER <ConstIdList_dash> | ε */
  char const_name[MAX_ID_NAME];

  if (nextToken == T_COMMA) {
    nextToken = getToken();
    if (nextToken != T_ID)
      pl0_error(yytext, line_no, "定数名でない");
    strcpy(const_name, yytext);
    nextToken = getToken();
    if (nextToken != T_EQ)
      pl0_error(yytext, line_no, "=でない");
    nextToken = getToken();
    if (nextToken != T_NUMBER)
      pl0_error(yytext, line_no, "数でない");
    /* 定数名の登録および値の設定 */
    reg_const_in_tbl(const_name, line_no, t_num_value);
    nextToken = getToken();
    parse_ConstIdList_dash();
  } else {
    /* ε */
  }
}

void parse_VarDeclList() {
  /* <VarDeclList> -> <VarDecl> <VarDeclList> | ε */
  if (nextToken == T_VAR) { /* First(<VarDecl>) */
    parse_VarDecl();
    parse_VarDeclList();
  } else {
    /* ε */
  }
}

void parse_VarDecl() {
  /* <VarDec> -> T_VAR <VarIdList> T_SEMIC */
  /* T_VAR では何もしない。次のトークンを読む */
  nextToken = getToken();
  parse_VarIdList();
  if (nextToken != T_SEMIC)
    pl0_error(yytext, line_no, ";でない");
  nextToken = getToken();
}

void parse_VarIdList() {
  /* <VarIdList_dash> -> T_ID <VarIdList_dash> */
  char var_name[MAX_ID_NAME];

  if (nextToken != T_ID)
    pl0_error(yytext, line_no, "変数名でない");
  /* 変数名の登録 */
  strcpy(var_name, yytext);
  reg_var_in_tbl(var_name, line_no);
  if (get_blocklevel() > 0) gencode_no_arg(pushup); /* 関数内なら領域確保 */
  nextToken = getToken();
  parse_VarIdList_dash();
}

void parse_VarIdList_dash() {
  /* <VarIdList_dash> -> T_COMMA T_ID <VarIdList_dash> | ε */
  char var_name[MAX_ID_NAME];

  if (nextToken == T_COMMA) {
    nextToken = getToken();
    if (nextToken != T_ID)
      pl0_error(yytext, line_no, "変数名でない");
    /* 変数名の登録 */
    strcpy(var_name, yytext);
    reg_var_in_tbl(var_name, line_no);
    if (get_blocklevel() > 0) gencode_no_arg(pushup); /* 関数内なら領域確保 */
    nextToken = getToken();
    parse_VarIdList_dash();
  } else {
    /* ε */
  }
}

void parse_FuncDeclList() {
  /* <FuncDeclList> -> <FuncDecl> <FuncDeclList> | ε */
  if (nextToken == T_FUNC) { /* First(<FuncDecl>) */
    parse_FuncDecl();
    parse_FuncDeclList();
  } else {
    /* ε */
  }
}

void parse_FuncDecl() {
  /* <FuncDecl> -> T_FUNC T_ID T_LPAR <FuncDeclIdList> T_RPAR <Block> T_SEMIC */
  char func_name[MAX_ID_NAME];
  /* int func_ptr; 現在処理している関数の記号表での位置 */

  /* T_FUNC では何もしない。次のトークンを読む */
  nextToken = getToken();
  if (nextToken != T_ID)
    pl0_error(yytext, line_no, "関数名でない");
  /* 関数名の登録 */
  strcpy(func_name, yytext);
  current_func = reg_func_in_tbl(func_name, line_no);
  reg_func_address(current_func, next_code_address());
  nextToken = getToken();
  if (nextToken != T_LPAR)
    pl0_error(yytext, line_no, "(でない");
  blocklevel_up(); /* ブロックレベルを上げる */
  nextToken = getToken();
  parse_FuncDeclIdList(current_func);
  if (nextToken != T_RPAR)
    pl0_error(yytext, line_no, ")でない");
  end_param(current_func);
  nextToken = getToken();
  gencode_no_arg(enterf); /* 関数内コード前処理 */
  parse_FuncBlock();
  if (nextToken != T_SEMIC)
    pl0_error(yytext, line_no, ";でない");
  /* 関数後処理 */
  /* block に return がなかった場合の処理はじめ */
  gencode_arg_V(lod, 0);
  gencode_no_arg(leavef);
  gencode_arg_V(ret, get_func_params(current_func));
  /* block に return がなかった場合の処理 おわり */
  blocklevel_down(); /* ブロックレベルを下げる */
  nextToken = getToken();
}

void parse_FuncDeclIdList(int func_ptr) {
  /* <FuncDeclIdList> -> T_ID <FuncDeclIdList_dash> | ε */
  char param_name[MAX_ID_NAME];

  if (nextToken == T_ID) {
    /* 仮引数の登録 */
    strcpy(param_name, yytext);
    reg_param_in_tbl(param_name, line_no, func_ptr);
    nextToken = getToken();
    parse_FuncDeclIdList_dash(func_ptr);
  } else {
    /* ε */
  }
}

void parse_FuncDeclIdList_dash(int func_ptr) {
  /* <FuncDeclIdList_dash> -> T_COMMA T_ID <FuncDeclIdList_dash> | ε */
  char param_name[MAX_ID_NAME];

  if (nextToken == T_COMMA) {
    nextToken = getToken();
    if (nextToken != T_ID) pl0_error(yytext, line_no, "仮引数名でない");
    /* 仮引数の登録 */
    strcpy(param_name, yytext);
    reg_param_in_tbl(param_name, line_no, func_ptr);
    nextToken = getToken();
    parse_FuncDeclIdList_dash(func_ptr);
  } else {
    /* ε */
  }
}

void parse_FuncBlock() {
  /* <FuncBlock> -> <ConstDeclList> <VarDeclList>  <Statement> */
  parse_ConstDeclList();
  parse_VarDeclList();
  parse_Statement();
}


void parse_Statement() {
  struct table_entry t_ent;
  char id_name[MAX_ID_NAME];
  int t_ptr;

  int backpatch_if ;     /* IF文用バックパッチ */
  int backpatch_while1 ; /* while文用バックパッチ1 */
  int backpatch_while2 ; /* while文用バックパッチ2 */

  switch(nextToken) {
  case T_ID: /* 代入文 */
    strcpy(id_name, yytext);
    t_ptr = search_table(id_name); /* T_ID を検索 */
    if (t_ptr == 0)
      pl0_error(id_name, line_no, "その変数/仮引数はない");
    t_ent = get_table(t_ptr);
    if (t_ent.type != var_id && t_ent.type != param_id)
      pl0_error(id_name, line_no, "それは変数/仮引数ではない");
    nextToken = getToken();
    if (nextToken != T_COLEQ) pl0_error(yytext, line_no, ":=がない");
    nextToken = getToken();
    parse_Expression();
    /* 代入のコード生成 */
    gencode_arg_ST(str, t_ptr);
    break;
  case T_BEGIN: /* begn ～ end */
    nextToken = getToken();
    parse_StatementList();
    if (nextToken != T_END)
      pl0_error(yytext, line_no, "endがない");
    nextToken = getToken();
    break;
  case T_IF: /* if then */
    nextToken = getToken();
    parse_Condition();
    backpatch_if = gencode_arg_V(jpc, 0); /* 偽の場合の飛び先を仮に0とおく */
    if (nextToken != T_THEN)
      pl0_error(yytext, line_no, "thenがない");
    nextToken = getToken();
    parse_Statement();
    backpatch(backpatch_if);  /* jpc 0 をここで backpatch */
    break;
  case T_WHILE: /* while do */
    nextToken = getToken();
    backpatch_while1 = next_code_address(); /* while の条件先頭 */
    parse_Condition();
    if (nextToken != T_DO)
      pl0_error(yytext, line_no, "doがない");
    backpatch_while2 = gencode_arg_V(jpc, 0); /* 偽の場合のとび先を仮に0とおく */
    nextToken = getToken();
    parse_Statement();
    gencode_arg_V(jmp, backpatch_while1); /* JMP の飛び先を条件先頭へ */
    backpatch(backpatch_while2); /* jpc 0 をここで backpatch */
    break;
  case T_RETURN: /* return */
    nextToken = getToken();
    parse_Expression();
    gencode_no_arg(leavef);
    gencode_arg_V(ret, get_func_params(current_func));
    break;
  case T_WRITE: /* write */
    nextToken = getToken();
    parse_Expression();
    /* write のコード生成 */
    gencode_no_arg(wrt);
    break;
  case T_WRITELN: /* writeln */
    /* writeln のコード生成 */
    gencode_no_arg(wrl);
    nextToken = getToken();
    break;
  default:
    break;
  }
}

void parse_StatementList() {
  /* <StatementList> -> <Statement> <StatementList_dash> */
  parse_Statement();
  parse_StatementList_dash();
}

void parse_StatementList_dash() {
  /* <StatementList_dash> -> T_SEMIC <Statement> <StatementList_dash> | ε */
  if (nextToken == T_SEMIC) {
    nextToken = getToken();
    parse_Statement();
    parse_StatementList_dash();
  } else {
    /* ε */
  }
}

void parse_Condition() {
  int operator = -1; /* T_EQ や T_GT を一時格納 */

  if (nextToken == T_ODD) {
    nextToken = getToken();
    parse_Expression();
    /* ここで T_ODDの処理 */
  } else {
    parse_Expression();
    if (nextToken == T_EQ || nextToken == T_NOTEQ
        || nextToken == T_LT || nextToken == T_GT
        || nextToken == T_LE || nextToken == T_GE) {
      operator = nextToken;
    } else {
      pl0_error(yytext, line_no, "比較演算子がない");
    }
    nextToken = getToken();
    parse_Expression();
    /* ここで operator 処理 */
    switch(operator) {
    case T_EQ:
      gencode_no_arg(eq);
      break;
    case T_NOTEQ:
      gencode_no_arg(neq);
      break;
    case T_LT:
      gencode_no_arg(lt);
      break;
    case T_GT:
      gencode_no_arg(gt);
      break;
    case T_LE:
      gencode_no_arg(le);
      break;
    case T_GE:
      gencode_no_arg(ge);
      break;
    default:
      break;
    }
  }
}

void parse_Expression() {
  if (nextToken == T_PLUS) {
    nextToken = getToken();
    gencode_arg_V(lod, 0);
    parse_Term();
    /* ここで 0 Term + をスタックにつむ */
    gencode_no_arg(pls);
    parse_Expression_dash();
  } else if (nextToken == T_MINUS) {
    nextToken = getToken();
    gencode_arg_V(lod, 0);
    parse_Term();
    /* ここで 0 Term - をスタックにつむ */
    gencode_no_arg(min);
    parse_Expression_dash();
  } else {
    parse_Term();
    parse_Expression_dash();
  }
}

void parse_Expression_dash() {
  if (nextToken == T_PLUS) {
    nextToken = getToken();
    parse_Term();
    gencode_no_arg(pls); /* 足し算コード生成 */
    parse_Expression_dash();
  } else if (nextToken == T_MINUS) {
    nextToken = getToken();
    parse_Term();
    gencode_no_arg(min); /* 引き算コード生成 */
    parse_Expression_dash();
  } else {
    /* ε */
  }
}

void parse_Term() {
  parse_Factor();
  parse_Term_dash();
}

void parse_Term_dash() {
  if (nextToken == T_MULTI) {
    nextToken = getToken();
    parse_Factor();
    gencode_no_arg(mul); /* 掛け算コード生成 */
    parse_Term_dash();
  } else if (nextToken == T_DIVIDE) {
    nextToken = getToken();
    parse_Factor();
    gencode_no_arg(divi); /* 割り算コード生成 */
    parse_Term_dash();
  } else {
    /* ε */
  }
}

void parse_Factor() {
  int t_ptr;
  int n_args = 0; /* 引数のカウント用 */
  struct table_entry t_ent;
  char id_name[MAX_ID_NAME];

  switch(nextToken) {
  case T_ID:
    strcpy(id_name, yytext);
    t_ptr = search_table(id_name); /* T_ID を検索 */
    if (t_ptr == 0)
      pl0_error(id_name, line_no, "その変数/定数/関数はない");
    t_ent = get_table(t_ptr);
    /* reference_info(id_name, line_no, t_ptr); */
    switch(t_ent.type) {
    case func_id: /* T_IDが関数名の場合 */
      nextToken = getToken();
      if (nextToken != T_LPAR) {
        pl0_error("", line_no, "( がない");
      } else {
	nextToken = getToken();
	n_args = parse_FuncArgList(n_args);
	if (t_ent.data.func.n_params != n_args)
	  pl0_error(t_ent.id_name, line_no, "関数の引数の数が違います");
	if (nextToken != T_RPAR)
	  pl0_error("", line_no, ") がない");
	gencode_arg_V(call, t_ent.data.func.address);
      }
      break;
    case const_id: /* 定数の場合 */
      gencode_arg_V(lod, t_ent.data.value);
      break;
    default: /* 変数/仮引数の場合 */
      gencode_arg_ST(lod, t_ptr);
      break;
    }
    nextToken = getToken();
    break;
  case T_NUMBER: /* 数字のの場合 */
    gencode_arg_V(lod, t_num_value);
    nextToken = getToken();
    break;
  case T_LPAR:
    nextToken = getToken();
    parse_Expression();
    if (nextToken != T_RPAR)
      pl0_error(yytext, line_no, ")がない");
    nextToken = getToken();
    break;
  default:
    pl0_error(yytext, line_no, "式がおかしい");
    break;
  }
}

int parse_FuncArgList(int n_args) {
  /* <FuncArgList> -> <Expression> <FuncArgList_dash> | ε */
  if (nextToken == T_PLUS || nextToken == T_MINUS
      || nextToken == T_ID || nextToken == T_NUMBER
      || nextToken == T_LPAR) { /* First(<Expression>)に含まれるもの */
    parse_Expression();
    n_args++;
    n_args = parse_FuncArgList_dash(n_args);
  } else {
    /* ε  引数なし */
  }
  return n_args;
}

int parse_FuncArgList_dash(int n_args) {
  /* <FuncArgList_dash> -> T_COMMA <Expression> <FuncArgList_dash> | ε */
  if (nextToken == T_COMMA) {
    nextToken = getToken();
    parse_Expression();
    n_args++;
    n_args = parse_FuncArgList_dash(n_args);
  } else {
    /* ε */
  }
  return n_args;
}
