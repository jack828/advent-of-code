#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
// #define TEST_MODE
#include "../utils.h"

typedef enum { FORWARDS, BACKWARDS } dir_t;

typedef struct {
  int size;
  int *items;
} history_t;

history_t **histories;
int history_count = 0;
int hist_index = 0;

void fileHandler(int lines) {
  printf("lines: %d\n", lines);
  history_count = lines;
  histories = calloc(history_count, sizeof(history_t));
}

void lineHandler(char *line, int length) {
  printf("line (%d): %s\n", length, line);

  history_t *history = calloc(1, sizeof(history_t));
  history->items = calloc(30, sizeof(int));

  char *token = strtok(line, " ");
  while (token != NULL) {
    history->items[history->size++] = strtol(token, NULL, 10);
    token = strtok(NULL, " ");
  }

  histories[hist_index++] = history;
}

void print_histories() {
  for (int i = 0; i < history_count; i++) {
    history_t *history = histories[i];
    printf("history %d, size %d\n", i, history->size);
    printf("[ ");
    for (int j = 0; j < history->size; j++) {
      printf("%d ", history->items[j]);
    }
    printf("]\n");
  }
}

history_t *get_diff(history_t *history) {
  // for each item in the array
  // calculate the difference between them
  history_t *diff_hist = calloc(1, sizeof(history_t));
  diff_hist->items = calloc(30, sizeof(int));
  for (int i = 0; i < history->size - 1; i++) {
    int diff = history->items[i + 1] - history->items[i];
    diff_hist->items[diff_hist->size++] = diff;
  }

  return diff_hist;
}

int calculate_difference(history_t *history, dir_t dir) {
  history_t *diffs = get_diff(history);
  // if all differences are 0, goto DONE:
  bool all_zero = true;

  for (int i = 0; i < diffs->size; i++) {
    if (diffs->items[i] != 0) {
      all_zero = false;
    }
  }
  // if they are not, repeat by calculating the difference between differences
  if (!all_zero) {
    if (dir == FORWARDS) {
      return diffs->items[diffs->size - 1] + calculate_difference(diffs, dir);
    } else {
      // BACKWARDS
      return diffs->items[0] - calculate_difference(diffs, dir);
    }
  }
  // DONE:
  // sum the last values of each difference array
  return 0;
}

int main() {
  init();
  readInputFile(__FILE__, lineHandler, fileHandler);

  // print_histories();

  int part_one = 0;
  int part_two = 0;

  for (int i = 0; i < history_count; i++) {
    history_t *history = histories[i];
    int diff_fwd = calculate_difference(history, FORWARDS);
    int next_term = history->items[history->size - 1] + diff_fwd;
    part_one += next_term;

    int diff = calculate_difference(history, BACKWARDS);
    int prev_term = history->items[0] - diff;
    part_two += prev_term;
  }

  printf("Part one: %d\n", part_one);
#ifdef TEST_MODE
  assert(part_one == 114);
#else
  assert(part_one == 2098530125);
#endif

  printf("Part two: %d\n", part_two);
#ifdef TEST_MODE
  assert(part_two == 2);
#else
  assert(part_two == 1016);
#endif
  exit(EXIT_SUCCESS);
}
