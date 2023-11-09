#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#define TEST_MODE
#include "../../utils.h"

int increases = 0;
int measurements[2048];
int mIndex = 0;

void lineHandler(char *line) {
  fputs("line: ", stdout);
  fputs(line, stdout);
  measurements[mIndex] = atoi(line);

  if (mIndex != 0) {
    if (measurements[mIndex] > measurements[mIndex - 1]) {
      increases++;
    }
  }

  mIndex++;
  fputs("\n", stdout);
}

int main() {
  fputs("1\n", stdout);
  readInput(__FILE__, lineHandler);
  fprintf(stdout, "Part one: %d\n", increases);
}
