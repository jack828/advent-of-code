#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
// #define TEST_MODE
#include "../utils.h"

int *calories;
int elfIndex = 0;

void fileHandler(int lines) {
  fprintf(stdout, "lines: %d\n", lines);
  calories = calloc(lines, sizeof(int));
}

void lineHandler(char *line) {
  fprintf(stdout, "line: %s\n", line);
  int length = strlen(line);
  if (length == 1) {
    elfIndex++;
  } else {
    calories[elfIndex] += atoi(line);
  }
}

void printCalories() {
  for (int i = 0; i < elfIndex; i++) {
    fprintf(stdout, "Elf %d: %d\n", i, calories[i]);
  }
}

int compare(const void *a, const void *b) {
  int int_a = *((int *)a);
  int int_b = *((int *)b);

  if (int_a == int_b)
    return 0;
  else if (int_a < int_b)
    return 1;
  else
    return -1;
}

int main() {
  init();
  readInputFile(__FILE__, lineHandler, fileHandler);
  // lack of trailing newline in input requires us to manually increment
  // elfIndex, so the last elf is recognised
  elfIndex++;

  // printCalories();

  qsort(calories, elfIndex, sizeof(int), compare);

  // printCalories();

  int elfWithTheMostCalories = calories[0];
  fprintf(stdout, "Part one: %d\n", elfWithTheMostCalories);
#ifdef TEST_MODE
  assert(elfWithTheMostCalories == 24000);
#else
  assert(elfWithTheMostCalories == 75501);
#endif

  int threeFattestElves = calories[0] + calories[1] + calories[2];
  fprintf(stdout, "Part two: %d\n", threeFattestElves);
#ifdef TEST_MODE
  assert(threeFattestElves == 45000);
#else
  assert(threeFattestElves == 215594);
#endif
  exit(EXIT_SUCCESS);
}
