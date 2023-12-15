#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
// #define TEST_MODE
#include "../utils.h"

typedef struct pattern_t {
  int width;
  int height;
  char **pattern;
} pattern_t;

pattern_t **patterns;
int pattern_count = 0;

pattern_t *input_pattern;

void fileHandler(int lines) {
  printf("lines: %d\n", lines);
  patterns = calloc(1, sizeof(pattern_t));
  patterns[pattern_count] = calloc(1, sizeof(pattern_t));
  input_pattern = patterns[0];
  input_pattern->height = 0;
  input_pattern->width = 0;
  input_pattern->pattern = calloc(1, sizeof(char *));
}

void lineHandler(char *line, int length) {
  printf("line (%d): %s\n", length, line);
  if (line[0] == '\n') {
    // new pattern
    patterns = realloc(patterns, ++pattern_count * sizeof(pattern_t));
    patterns[pattern_count] = calloc(1, sizeof(pattern_t));
    input_pattern = patterns[pattern_count];
    input_pattern->height = 0;
    input_pattern->width = 0;
    input_pattern->pattern = calloc(1, sizeof(char *));
    return;
  }
  if (input_pattern->width == 0) {
    input_pattern->width = length;
  }

  input_pattern->pattern = realloc(
      input_pattern->pattern, (input_pattern->height + 1) * sizeof(char *));

  input_pattern->pattern[input_pattern->height++] = strdup(line);
}

void print_patterns() {
  for (int i = 0; i <= pattern_count; i++) {
    pattern_t *pattern = patterns[i];
    printf("Pattern #%d = %dx%d\n", i, pattern->height, pattern->width);
    for (int y = 0; y < pattern->height; y++) {
      printf("%.*s\n", pattern->width, pattern->pattern[y]);
    }
  }
}

bool has_v_reflection(pattern_t *test_pattern, int col) {
  int width = test_pattern->width;
  int height = test_pattern->height;
  char **pattern = test_pattern->pattern;
  // pivot is at COL
  // need to start by testing at COL
  // check if COL-1 == COL+1
  // and continue until COL-1 < 0 or COL > height
  int i = 0;
  bool all_same = true;
  while (++i) {
    int left = col - i;
    int right = col + i - 1;
    if (left < 0 || right >= width - 1) {
      return all_same;
    }

    for (int y = 0; y < height; y++) {
      all_same = !all_same ? false : pattern[y][left] == pattern[y][right];
    }
    if (!all_same) {
      return false;
    }
  }
  return false;
}

bool has_y_reflection(pattern_t *test_pattern, int row) {
  int width = test_pattern->width;
  int height = test_pattern->height;
  char **pattern = test_pattern->pattern;
  // pivot is at row
  // need to start by testing at row
  // check if row-1 == row+1
  // and continue until row-1 < 0 or row > height
  int i = 0;
  bool all_same = true;
  while (++i) {
    int above = row - i;
    int below = row + i - 1;
    if (above < 0 || below >= height - 1) {
      return all_same;
    }

    for (int x = 0; x < width; x++) {
      all_same = !all_same ? false : pattern[above][x] == pattern[below][x];
    }
    if (!all_same) {
      return false;
    }
  }
  return false;
}

int main() {
  init();
  readInputFile(__FILE__, lineHandler, fileHandler);

  // printf("patterns: %d\n", pattern_count);
  // print_patterns();

  int part_one = 0;
  for (int i = 0; i <= pattern_count; i++) {
    pattern_t *pattern = patterns[i];
    // printf("\nPattern #%d = %dx%d\n", i, pattern->height, pattern->width);

    int v_reflection = 0;
    // horizontal reflection
    for (int x = 1; x < pattern->width - 1; x++) {
      bool has = has_v_reflection(pattern, x);
      if (has) {
        v_reflection = x;
        break;
      }
    }
    if (v_reflection) {
      part_one += v_reflection;
    } else {
      // only test for h if v fails
      for (int y = 1; y < pattern->height; y++) {
        bool has = has_y_reflection(pattern, y);
        if (has) {
          part_one += 100 * y;
          break;
        }
      }
    }
  }

  printf("Part one: %d\n", part_one);
#ifdef TEST_MODE
  assert(part_one == 405);
#else
  assert(part_one == 37561);
#endif

  printf("Part two: %d\n", 420);
#ifdef TEST_MODE
  assert(420 == 69);
#else
  assert(420 == 69);
#endif
  exit(EXIT_SUCCESS);
}
