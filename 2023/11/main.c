#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#define TEST_MODE
#include "../utils.h"

char **map;
int height = 0;
int width = 0;
int line_i = 0;

int galaxies[512][2];
int galaxy_count = 0;
int *empty_y;
int empty_y_count = 0;
int *empty_x;
int empty_x_count = 0;

void fileHandler(int lines) {
  printf("lines: %d\n", lines);
  map = calloc(lines, sizeof(char *));
  empty_y = calloc(lines, sizeof(int));
  empty_x = calloc(lines, sizeof(int));
  height = lines;
}

void lineHandler(char *line, int length) {
  printf("line (%d): %s\n", length, line);

  if (width == 0) {
    width = length - 1;
    for (int i = 0; i < height; i++) {
      map[i] = calloc(width, sizeof(char));
    }
  }

  strncpy(map[line_i++], line, width);
}

void find_galaxies() {
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      char c = map[y][x];
      if (c == '#') {
        galaxies[galaxy_count][0] = y;
        galaxies[galaxy_count][1] = x;
        galaxy_count++;
      }
    }
  }
}

void print_map(char **map_to_print, int h, int w) {
  int num = 1;
  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
      int c = map_to_print[y][x];
      if (c == '#') {
        printf("%c", '0' + num++);
      } else {
        printf("%c", c);
      }
    }
    printf("\n");
  }
}

// taxicab distance
int taxi_dist(int aY, int aX, int bY, int bX) {
  return abs(aY - bY) + abs(aX - bX);
}

// taxicab distance, but for expanded uni
int64_t taxi_dist_expanded(int a[2], int b[2], int factor) {
  // how many empty cols are between aY and bY
  int maxY = max(a[0], b[0]);
  int maxX = max(a[1], b[1]);
  int minY = min(a[0], b[0]);
  int minX = min(a[1], b[1]);
  // printf("a (%d,%d)\n", a[0], a[1]);
  // printf("b (%d,%d)\n", b[0], b[1]);

  // assert(empty_rows == 2);
  // assert(empty_cols == 1);
  int64_t dist = abs(a[0] - b[0]) + abs(a[1] - b[1]);
  // printf("dist: %d\n", dist);

  for (int i = 0; i < empty_y_count; i++) {
    int y = empty_y[i];
    if (y > minY && y < maxY) {
      // printf("y row between %d\n", y);
      dist += factor - 1;
    }
  }
  // how many empty rows are between aX and bX
  for (int i = 0; i < empty_x_count; i++) {
    int x = empty_x[i];
    if (x > minX && x < maxX) {
      // printf("x col between %d\n", x);
      dist += factor - 1;
    }
  }

  return dist;
}

int main() {
  init();
  readInputFile(__FILE__, lineHandler, fileHandler);

  // print_map(map, height, width);

  find_galaxies();

  // for each y column
  for (int x = 0; x < width; x++) {
    // if they are all empty
    bool all_y_empty = true;
    for (int y = 0; y < height; y++) {
      all_y_empty = !all_y_empty ? false : map[y][x] == '.';
    }

    if (all_y_empty) {
      // printf("empty x col %d \n", x);
      empty_x[empty_x_count++] = x;
    }
  }

  for (int y = 0; y < height; y++) {
    bool all_x_empty = true;
    for (int x = 0; x < width; x++) {
      all_x_empty = !all_x_empty ? false : map[y][x] == '.';
    }

    if (all_x_empty) {
      // printf("empty y row %d \n", y);
      empty_y[empty_y_count++] = y;
    }
  }
  int EXPANSION_FACTOR = 2;
  int part_one = 0;
  for (int i = 0; i < galaxy_count; i++) {
    int *pair_a = galaxies[i];

    for (int j = i + 1; j < galaxy_count; j++) {
      int *pair_b = galaxies[j];
      part_one += taxi_dist_expanded(pair_a, pair_b, EXPANSION_FACTOR);
    }
  }
  // assert(taxi_dist_expanded(galaxies[0], galaxies[6], 2) == 15); // 1 + 7
  printf("Part one: %d\n", part_one);
#ifdef TEST_MODE
  assert(part_one == 374);
#else
  assert(part_one == 9545480);
#endif

  EXPANSION_FACTOR = 1000000;
  int64_t part_two = 0;
  for (int i = 0; i < galaxy_count; i++) {
    int *pair_a = galaxies[i];

    for (int j = i + 1; j < galaxy_count; j++) {
      int *pair_b = galaxies[j];
      part_two += taxi_dist_expanded(pair_a, pair_b, EXPANSION_FACTOR);
    }
  }

  printf("Part two: %ld\n", part_two);
#ifdef TEST_MODE
  assert(part_two == 82000210);
#else
  assert(part_two == 406725732046);
#endif
  exit(EXIT_SUCCESS);
}
