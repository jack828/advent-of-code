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
  int value;
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

// wtf is this realloc stuff
void lineHandler(char *line, int length) {
  printf("line (%d): %s\n", length, line);
  if (line[0] == '\n') {
    // new pattern
    patterns = realloc(patterns, ++pattern_count * sizeof(pattern_t));
    patterns[pattern_count] = calloc(1, sizeof(pattern_t));
    input_pattern = patterns[pattern_count];
    input_pattern->height = 0;
    input_pattern->width = 0;
    input_pattern->value = -1;
    input_pattern->pattern = calloc(1, sizeof(char *));
    return;
  }
  if (input_pattern->width == 0) {
    input_pattern->width = length - 1;
  }

  input_pattern->pattern = realloc(
      input_pattern->pattern, (input_pattern->height + 1) * sizeof(char *));

  input_pattern->pattern[input_pattern->height++] = strdup(line);
}

void print_pattern(pattern_t *pattern) {
  for (int y = 0; y < pattern->height; y++) {
    printf("%.*s\n", pattern->width, pattern->pattern[y]);
  }
}

void print_patterns() {
  for (int i = 0; i <= pattern_count; i++) {
    pattern_t *pattern = patterns[i];
    printf("Pattern #%d = %dx%d\n", i, pattern->height, pattern->width);
    print_pattern(pattern);
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
  while (++i) {
    int left = col - i;
    int right = col + i - 1;
    if (left < 0 || right >= width) {
      // reached the edge without failing, must be reflected
      return true;
    }

    for (int y = 0; y < height; y++) {
      // printf("y %d - l %c, r %c\n", y, pattern[y][left], pattern[y][right]);
      if (pattern[y][left] != pattern[y][right]) {
        return false;
      }
    }
  }
  return false;
}

bool has_h_reflection(pattern_t *test_pattern, int row) {
  int width = test_pattern->width;
  int height = test_pattern->height;
  char **pattern = test_pattern->pattern;
  // pivot is at row
  // need to start by testing at row
  // check if row-1 == row+1
  // and continue until row-1 < 0 or row > height
  int i = 0;
  while (++i) {
    int above = row - i;
    int below = row + i - 1;
    if (above < 0 || below >= height) {
      // reached the edge without failing, must be reflected
      return true;
    }

    for (int x = 0; x < width; x++) {
      // printf("x %d - l %c, r %c\n", x, pattern[above][x], pattern[below][x]);
      if (pattern[above][x] != pattern[below][x]) {
        return false;
      }
    }
  }
  return false;
}

int get_reflection_value(pattern_t *pattern) {
  for (int x = 1; x < pattern->width; x++) {
    if (pattern->value == x) {
      continue;
    }
    bool has = has_v_reflection(pattern, x);
    if (has) {
      return x;
    }
  }

  for (int y = 1; y < pattern->height; y++) {
    if (pattern->value == 100 * y) {
      continue;
    }
    bool has = has_h_reflection(pattern, y);
    if (has) {
      return 100 * y;
    }
  }
  return -1; // no reflection!
}

void swap(char **pattern, int y, int x) {
  if (pattern[y][x] == '#') {
    pattern[y][x] = '.';
  } else {
    pattern[y][x] = '#';
  }
}

int main() {
  init();
  readInputFile(__FILE__, lineHandler, fileHandler);

  // printf("patterns: %d\n", pattern_count + 1);
  // print_patterns();

  int part_one = 0;
  for (int i = 0; i <= pattern_count; i++) {
    pattern_t *pattern = patterns[i];
    // printf("\nPattern #%d = %dx%d\n", i, pattern->height, pattern->width);
    int value = get_reflection_value(pattern);
    pattern->value = value;
    part_one += value;
  }

  printf("Part one: %d\n", part_one);
#ifdef TEST_MODE
  assert(part_one == 405);
#else
  assert(part_one == 37561);
#endif

  int part_two = 0;
  for (int i = 0; i <= pattern_count; i++) {
    pattern_t *pattern = patterns[i];
    // printf("\nPattern #%d = %dx%d\n", i, pattern->height, pattern->width);
    bool has_new_reflection = false;
    for (int y = 0; y <= pattern->height - 1; y++) {
      if (has_new_reflection) {
        break;
      }
      for (int x = 0; x <= pattern->width; x++) {
        if (has_new_reflection) {
          break;
        }
        // TODO speed improvement?
        // if (pattern->pattern[y][x] == '.') {
        //   continue;
        // }
        // swap the value at [y,x]
        swap(pattern->pattern, y, x);
        // print_pattern(pattern);

        int value = get_reflection_value(pattern);
        if (value != -1) {
          // printf("val %d\n",value);
          part_two += value;
          has_new_reflection = true;
        }
        // unswap the value at [y,x]
        swap(pattern->pattern, y, x);
      }
    }
  }
  printf("Part two: %d\n", part_two);
#ifdef TEST_MODE
  assert(part_two == 400);
#else
  assert(part_two == 31108);
#endif
  exit(EXIT_SUCCESS);
}
