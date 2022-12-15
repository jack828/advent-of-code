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
  fprintf(stdout, "Part one: %d\n", 69);
  fprintf(stdout, "Part two: %d\n", 420);
}
