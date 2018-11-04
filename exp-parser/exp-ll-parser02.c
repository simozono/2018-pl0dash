/* 文法G5'用 LL(1)再帰下降型構文解析器
 * (文法G5' は 第20回授業(後期第05回)p.275～)
 *   2018年後期 鹿児島高専
 *   3年生 言語処理系 授業用
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../scanner/tokentable.h"

extern FILE *yyin;
extern int yylex();
extern char *yytext;

int getToken(void); /* トークンを取得する関数 */
void parse_E(void);
void parse_E_dash(void);
void parse_T(void);
void parse_T_dash(void);
void parse_F(void);
void parse_error(char *error_message); /* エラーメッセージを出す */

int nextToken; /* 次のトークンが入る変数 */

int getToken(void) {
  int token = yylex();
  if (token == 0) token = T_EOF; /* yylex()が0を返す時がEOFのようだ */
  return token;
}

void parse_error(char *error_message) {
  printf ("parse error: %s\n", error_message);
  exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf ("ソースプログラムのファイル名のみ指定してください\n");
    exit(EXIT_FAILURE);
  }

  yyin = fopen(argv[1], "r"); /* ファイルを開く処理 */
  if (yyin  == NULL) {
    printf ("%s というファイルがない\n", argv[1]);
    exit(EXIT_FAILURE);
  }

  /* 最初に1トークン読み込んでおく */
  nextToken = getToken(); 

  /* 構文解析スタート S=E */
  parse_E();
  if (nextToken != T_EOF) parse_error("EOFでない");

  printf ("END\n");
  exit(EXIT_SUCCESS);
}

void parse_E() { /* E → T E' */
  parse_T();
  parse_E_dash();
}

void parse_E_dash() { /* E' → + T E'| - T E'| ε */
  int op;
  if (nextToken == T_PLUS || nextToken == T_MINUS) {
    op = nextToken;
    nextToken = getToken();
    parse_T();
    printf("POP B\n");
    printf("POP A\n");
    if (op == T_PLUS) {
      printf("PLUS\n");
    } else {
      printf("MINUS\n");
    }
    printf("PUSH C\n");
    parse_E_dash();
  } else {
    /* 何もしない */
  }
}

void parse_T() { /* T → F T' */
  parse_F();
  parse_T_dash();
}

void parse_T_dash() { /* T' → * F T' | / F T'| ε */
  int op;
  if (nextToken == T_MULTI || nextToken == T_DIVIDE) {
    op = nextToken;
    nextToken = getToken();
    parse_F();
    printf("POP B\n");
    printf("POP A\n");
    if (op == T_MULTI) {
      printf("MULTI\n");
    } else {
      printf("DIV\n");
    }
    printf("PUSH C\n");
    parse_T_dash();
  } else {
    /* 何もしない */
  }
}

void parse_F() { /* F → ( E ) | T_NUMBER */
  if (nextToken == T_LPAR) {
    nextToken = getToken();
    parse_E();
    if (nextToken != T_RPAR) parse_error(")がない");
    nextToken = getToken();
  } else if (nextToken == T_NUMBER) {
    printf("MOVE %s, A\n", yytext);
    printf("PUSH A\n");
    nextToken = getToken();
  } else {
    parse_error("数または(でない");
  }
}
