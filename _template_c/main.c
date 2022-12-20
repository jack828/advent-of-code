#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#define TEST_MODE
#include "../../utils.h"


void lineHandler(char *line) {
  fputs("line: ", stdout);
  fputs(line, stdout);
  fputs("\n", stdout);
}

int main() {
  readInput(__FILE__, lineHandler);

  /* well implemented program goes here */

  fprintf(stdout, "Part one: %d\n", 69);
#ifdef TEST_MODE
  assert(69 == 420);
#else
  assert(69 == 420);
#endif

  fprintf(stdout, "Part two: %d\n", 420);
#ifdef TEST_MODE
  assert(420 == 69);
#else
  assert(420 == 69);
#endif
}
