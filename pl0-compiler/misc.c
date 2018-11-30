#include "misc.h"

void pl0_error(char *lexeme,  int line, char *error_message);

/* エラーを出す関数 */
void pl0_error(char *lexeme, int line, char *error_message) { 
  fprintf(stderr, "エラー: %d 行目 \'%s\'付近: %s\n",
          line, lexeme, error_message);
  exit(EXIT_FAILURE);
}
