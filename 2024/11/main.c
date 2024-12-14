#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
// #define TEST_MODE
#include "../utils.h"

typedef struct stone_t stone_t;

struct stone_t {
  long value;
  int digit_count;

  stone_t *next;
  stone_t *prev;
};

stone_t *head = NULL;
stone_t *start = NULL;

void fileHandler(int lines) { printf("lines: %d\n", lines); }

void lineHandler(char *line, int length) {
  printf("line (%d): %s\n", length, line);
  char *token = strtok(line, " ");

  do {
    stone_t *next = calloc(1, sizeof *next);
    *next = (stone_t){
        .prev = head, .value = atoi(token), .digit_count = strlen(token)};
    if (start == NULL) {
      start = next;
    }
    head = next;
    printf("tok: %s\n", token);
  } while ((token = strtok(NULL, " ")) != NULL);

  // link the list the other way
  // could probably be done in one with above
  stone_t *prev = head;
  stone_t *next = NULL;
  while (prev) {
    // printf("p: %ld\n", prev->value);
    // if (next) {
    //   printf("n: %ld\n", next->value);
    // }
    prev->next = next;
    next = prev;
    prev = prev->prev;
  }
}

void blink() {
  stone_t *next = start;
  while (next) {
    // printf("b: %ld\n", next->value);
    // If the stone is engraved with the number 0, it is replaced by a stone
    // engraved with the number 1.
    if (next->value == 0) {
      next->value = 1;
    }

    // If the stone is engraved with a number that has an even number of digits,
    // it is replaced by two stones. The left half of the digits are engraved on
    // the new left stone, and the right half of the digits are engraved on the
    // new right stone. (The new numbers don't keep extra leading zeroes: 1000
    // would become stones 10 and 0.)
    else if (next->digit_count % 2 == 0) {
      int digits_each = next->digit_count / 2;
      long start_digits = floor(next->value / pow(10, digits_each));
      long end_digits = next->value - (start_digits * pow(10, digits_each));
      // printf(">>>: %ld\n", start_digits);
      // printf(">>>: %ld\n", end_digits);

      next->value = start_digits;
      next->digit_count = log10(start_digits) + 1;

      stone_t *new_stone = calloc(1, sizeof *new_stone);
      *new_stone =
          (stone_t){.value = end_digits,
                    .digit_count = end_digits == 0 ? 1 : log10(end_digits) + 1};

      next->prev = new_stone;

      new_stone->prev = next;
      new_stone->next = next->next;
      next->next = new_stone;
      next = new_stone;

    }

    // If none of the other rules apply, the stone is replaced by a new stone;
    // the old stone's number multiplied by 2024 is engraved on the new stone.
    else {
      next->value *= 2024;
      next->digit_count = log10(next->value) + 1;
    }

    next = next->next;
  }
}

void printStones(bool short_format) {
  int count = 0;
  stone_t *next = start;
  while (next) {
    if (short_format) {
      printf("%ld ", next->value);
    } else {
      printf("stone: %ld\t[%d]\n", next->value, next->digit_count);
    }
    count++;
    next = next->next;
  }
  if (short_format) {
    printf("\n");
  } else {
    printf("count: %d\n", count);
  }
}

int countStones() {
  int count = 0;
  stone_t *next = start;
  while (next) {
    count++;
    next = next->next;
  }
  return count;
}

int main() {
  init();
  readInputFile(__FILE__, lineHandler, fileHandler);

  printf("order:\n");
  printStones(false);

  printf("blink:\n");

  int blink_count = 25;
  while (blink_count--) {
    blink();
  }

  int part_one = countStones();

  printf("Part one: %d\n", part_one);
#ifdef TEST_MODE
  assert(part_one == 55312);
#else
  assert(part_one == 184927);
#endif

  // TODO presumably, the pattern of stones is repeating, and you could cache
  // using that?
  printf("Part two: %d\n", 420);
#ifdef TEST_MODE
  assert(420 == 69);
#else
  assert(420 == 69);
#endif
  exit(EXIT_SUCCESS);
}
