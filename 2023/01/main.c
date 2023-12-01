#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#define TEST_MODE
#include "../utils.h"

void fileHandler(int lines) { fprintf(stdout, "lines: %d\n", lines); }

char *alpha = "abcdefghijklmnopqrstuvwxyz";

int partOne = 0;
int partTwo = 0;

struct digit_t {
  int value;
  char *str;
  int length;
} digit_t;

struct digit_t allDigits[] = {{.value = 0, .str = "0", .length = 1},
                              {.value = 1, .str = "1", .length = 1},
                              {.value = 2, .str = "2", .length = 1},
                              {.value = 3, .str = "3", .length = 1},
                              {.value = 4, .str = "4", .length = 1},
                              {.value = 5, .str = "5", .length = 1},
                              {.value = 6, .str = "6", .length = 1},
                              {.value = 7, .str = "7", .length = 1},
                              {.value = 8, .str = "8", .length = 1},
                              {.value = 9, .str = "9", .length = 1},

                              {.value = 1, .str = "one", .length = 3},
                              {.value = 2, .str = "two", .length = 3},
                              {.value = 3, .str = "three", .length = 5},
                              {.value = 4, .str = "four", .length = 4},
                              {.value = 5, .str = "five", .length = 4},
                              {.value = 6, .str = "six", .length = 3},
                              {.value = 7, .str = "seven", .length = 5},
                              {.value = 8, .str = "eight", .length = 5},
                              {.value = 9, .str = "nine", .length = 4}};
int allDigitsLength = 19;

int getDigit(char *str, int length) {
  // printf("str: '%s' (%d) [%x]\n", str, length, str[0]);

  for (int i = 0; i < allDigitsLength; i++) {
    struct digit_t digit = allDigits[i];
    // Impossible for it to fit
    if (length < digit.length) {
      continue;
    }
    if (strncmp(str, digit.str, digit.length) == 0) {
      return digit.value;
    }
  }
  return -1;
}

void lineHandler(char *line, int length) {
  printf("line: %s\n", line);
  char *lineCpy = strdup(line);
  char *ptr;
  char *token = strtok_r(line, alpha, &ptr);
  char *firstDigit1 = token;
  char *lastDigit1 = token;
  while (token != NULL) {
    token = strtok_r(NULL, alpha, &ptr);
    lastDigit1 = token != NULL ? token : lastDigit1;
  }
  if (firstDigit1 == NULL)
    firstDigit1 = "0";
  if (lastDigit1 == NULL)
    lastDigit1 = "0";
  int numOne = ((firstDigit1[0] - '0') * 10) +
               (lastDigit1[strlen(lastDigit1) - 1] - '0');
  partOne += numOne;

  int firstDigit2 = 0;
  int lastDigit2 = 0;
  int digit = 0;
  for (int i = 0; i < length; i++) {
    char *str = lineCpy + i;
    int d = getDigit(str, length - i);
    if (d == -1) {
      continue;
    }
    digit = d;
    if (firstDigit2 == 0)
      firstDigit2 = digit;
  }
  lastDigit2 = digit;

  partTwo += (firstDigit2 * 10) + lastDigit2;
}

int main() {
  init();
  readInputFile(__FILE__, lineHandler, fileHandler);

  printf("Part one: %d\n", partOne);
#ifdef TEST_MODE
  // assert(partOne == 142); // for first example
  assert(partOne == 209); // for second example
#else
  assert(partOne == 55621);
#endif

  printf("Part two: %d\n", partTwo);
#ifdef TEST_MODE
  assert(partTwo == 281);
#else
  assert(partTwo == 53592);
#endif
  exit(EXIT_SUCCESS);
}
