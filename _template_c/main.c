#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#define TEST_MODE
#include "../utils.h"

void fileHandler(int lines) {
  fprintf(stdout, "lines: %d\n", lines);
}

void lineHandler(char *line) {
  fprintf(stdout, "line: %s\n", line);
}

int main() {
  init();
  readInputFile(__FILE__, lineHandler, fileHandler);

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
  exit(EXIT_SUCCESS);
}
