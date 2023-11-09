#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
// #define TEST_MODE
#include "../../utils.h"

char *decToSnafu(double num) {
  char *str = calloc(20, sizeof(char));
  int power = 0;

  fprintf(stdout, "input: %0.f, output %s\n", num, str);
  return str;
}

double snafuToDec(char *str) {
  double num = 0;
  int length = strlen(str);

  for (int i = 0; i < length; i++) {
    int power = pow(5, length - i - 1);
    if (isdigit(str[i])) {
      num += (str[i] - 48) * power;
    } else if (str[i] == '-') {
      num -= 1 * power;
    } else if (str[i] == '=') {
      num -= 2 * power;
    }
  }

  return num;
}

double total = 0;

void lineHandler(char *line) {
  fprintf(stdout, "line: %s\n", line);
  double val = snafuToDec(line);
  total += val;
  fprintf(stdout, "total: %.f - %.f\n", total, val);
}

void snafuAdd(char *one, char *two, char *dest) {
  int onelen = strlen(one);
  int twolen = strlen(two);
  // 1 + 1 = 2
  // 1 + 2 = 1=
  // 1 + 3 = 1-
  // 2 + 3 = 12
  char *str = onelen > twolen ? one : two;

  dest = malloc(sizeof(onelen + twolen));
  char remainder = 0;
  for (int i = max(onelen, twolen); i; i--) {
    char lhs = one[i];
    char rhs = two[i];

    if (!lhs || !rhs) {
    }
  }
}

int main() {
  readInput(__FILE__, lineHandler);

#ifdef TEST_MODE
  assert(snafuToDec("1") == 1);
  assert(snafuToDec("2") == 2);
  assert(snafuToDec("1=") == 3);
  assert(snafuToDec("1-") == 4);
  assert(snafuToDec("10") == 5);
  assert(snafuToDec("11") == 6);
  assert(snafuToDec("12") == 7);
  assert(snafuToDec("2=") == 8);
  assert(snafuToDec("2-") == 9);
  assert(snafuToDec("20") == 10);
  assert(snafuToDec("1=0") == 15);
  assert(snafuToDec("1-0") == 20);
  assert(snafuToDec("1=11-2") == 2022);
  assert(snafuToDec("1-0---0") == 12345);
  assert(snafuToDec("1121-1110-1=0") == 314159265);

  /*
  assert(strcmp(decToSnafu(1), "1") == 0);
  assert(strcmp(decToSnafu(2), "2") == 0);
  assert(strcmp(decToSnafu(3), "1=") == 0);
  assert(strcmp(decToSnafu(4), "1-") == 0);
  assert(strcmp(decToSnafu(5), "10") == 0);
  assert(strcmp(decToSnafu(6), "11") == 0);
  assert(strcmp(decToSnafu(7), "12") == 0);
  assert(strcmp(decToSnafu(8), "2=") == 0);
  assert(strcmp(decToSnafu(9), "2-") == 0);
  assert(strcmp(decToSnafu(10), "2-") == 0);
  assert(strcmp(decToSnafu(15), "1=0") == 0);
  assert(strcmp(decToSnafu(20), "10-0") == 0);
  assert(strcmp(decToSnafu(2022), "1=11-2") == 0);
  assert(strcmp(decToSnafu(12345), "1-0---0") == 0);
  assert(strcmp(decToSnafu(314159265), "1121-1110-1=0") == 0);
    */
#endif

  fprintf(stdout, "Part one: %ld\n", total);
#ifdef TEST_MODE
  assert(total == 4890);
#else
  assert(total == 420);
#endif
}
