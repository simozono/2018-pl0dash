/* 文法G4用 LL(1)再帰下降型構文解析器
 * (文法G4 は 第20回授業(後期第05回)p.267～)
 *
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
 
  /* 終了アセンブリコード生成はじめ */
  printf ("END\n");
  /* 終了アセンブリコード生成おわり */
  exit(EXIT_SUCCESS);
}

void parse_E() { /* E → TE' */
  parse_T();
  parse_E_dash();
}

void parse_E_dash() { /* E' → +TE'|ε */
  if (nextToken == T_PLUS) {
    nextToken = getToken();
    parse_T();
    /* T_PLUS のコード生成はじまり */
    printf("POP B\n");
    printf("POP A\n");
    printf("PLUS\n");
    printf("PUSH C\n");
    /* T_PLUS のコード生成おわり */
    parse_E_dash();
  }
}

void parse_T() { /* T → FT' */
  parse_F();
  parse_T_dash();
}

void parse_T_dash() { /* T' → *FT'|ε */
  if (nextToken == T_MULTI) {
    nextToken = getToken();
    parse_F();
    /* T_MULTI のコード生成はじまり */
    printf("POP B\n");
    printf("POP A\n");
    printf("MULTI\n");
    printf("PUSH C\n");
    /* T_MULTI のコード生成おわり */
    parse_T_dash();
  }
}

void parse_F() { /* F → (E)|T_NUMBER */
  if (nextToken == T_LPAR) {
    nextToken = getToken();
    parse_E();
    if (nextToken != T_RPAR) parse_error(")がない");
    nextToken = getToken();
  } else if (nextToken == T_NUMBER) {
    /* T_NUMBER のコード生成はじまり */
    printf("MOVE %s, A\n", yytext);
    printf("PUSH A\n");
    /* T_NUMBER のコード生成おわり */
    nextToken = getToken();
  } else {
    parse_error("数または(でない");
  }
}
