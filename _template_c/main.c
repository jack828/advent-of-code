#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#define TEST_MODE
#include "../utils.h"

void fileHandler(int lines) {
  printf("lines: %d\n", lines);
}

void lineHandler(char *line, int length) {
  printf("line (%d): %s\n", length, line);
}

int main() {
  init();
  readInputFile(__FILE__, lineHandler, fileHandler);

  /* well implemented program goes here */

  printf("Part one: %d\n", 69);
#ifdef TEST_MODE
  assert(69 == 420);
#else
  assert(69 == 420);
#endif

  printf("Part two: %d\n", 420);
#ifdef TEST_MODE
  assert(420 == 69);
#else
  assert(420 == 69);
#endif
  exit(EXIT_SUCCESS);
}
