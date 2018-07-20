%{
  #include <stdio.h>
  #include <string.h>
%}
%option noyywrap
%%
[[:blank:]\n]+		{ printf("T_WSP: \n"); }
"const"			{ printf("T_CONST:   \"%s\"\n", yytext); }
"var"			{ printf("T_VAR:     \"%s\"\n", yytext); }
"function"		{ printf("T_FUNC:    \"%s\"\n", yytext); }
"begin"			{ printf("T_BEGIN:   \"%s\"\n", yytext); }
"end"			{ printf("T_END:     \"%s\"\n", yytext); }
"if"			{ printf("T_IF:      \"%s\"\n", yytext); }
"then"			{ printf("T_THEN:    \"%s\"\n", yytext); }
"while"			{ printf("T_WHILE:   \"%s\"\n", yytext); }
"do"			{ printf("T_DO:      \"%s\"\n", yytext); }
"return"		{ printf("T_RETURN:  \"%s\"\n", yytext); }
"write"			{ printf("T_WRITE:   \"%s\"\n", yytext); }
"writeln"		{ printf("T_WRITELN: \"%s\"\n", yytext); }
"odd"			{ printf("T_ODD:     \"%s\"\n", yytext); }
":="			{ printf("T_COLEQ:   \"%s\"\n", yytext); }
"="			{ printf("T_EQ:      \"%s\"\n", yytext); }
"<>"			{ printf("T_NOTEQ:   \"%s\"\n", yytext); }
"<"			{ printf("T_LT:      \"%s\"\n", yytext); }
">"			{ printf("T_GT:      \"%s\"\n", yytext); }
"<="			{ printf("T_LE:      \"%s\"\n", yytext); }
">="			{ printf("T_GE:      \"%s\"\n", yytext); }
"."			{ printf("T_PERIOD:  \"%s\"\n", yytext); }
";"			{ printf("T_SEMIC:   \"%s\"\n", yytext); }
","			{ printf("T_COMMA:   \"%s\"\n", yytext); }
"("			{ printf("T_LPAR:    \"%s\"\n", yytext); }
")"			{ printf("T_RPAR:    \"%s\"\n", yytext); }
"+"			{ printf("T_PLUS:    \"%s\"\n", yytext); }
"-"			{ printf("T_MINUS:   \"%s\"\n", yytext); }
"*"			{ printf("T_MULTI:   \"%s\"\n", yytext); }
"/"			{ printf("T_DEVIDE:  \"%s\"\n", yytext); }
[[:alpha:]][[:alpha:][:digit:]_]*    { printf("T_ID:      \"%s\"\n", yytext); }
[[:digit:]]+|"0x"[0-9a-h]+    { printf("T_NUMBER:  \"%s\"\n", yytext); }
.                             { printf("T_ERROR      \"%s\"\n", yytext); }
%%
int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf ("argument error\n");
    exit(0);
  }

  yyin = fopen(argv[1], "r"); /* ファイルを開く処理 */
  if (yyin  == NULL) {
    printf ("%s file not found.\n", argv[1]);
    exit(0);
  }
  yylex();

  return 0;
}
