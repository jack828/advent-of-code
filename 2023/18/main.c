#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
// #define TEST_MODE
#include "../utils.h"

#define HEIGHT 768
#define WIDTH 768

char **grid;
int y_pos = HEIGHT / 2;
int x_pos = WIDTH / 2;
int y_max = 0;
int x_max = 0;
int y_min = INT_MAX;
int x_min = INT_MAX;

void fileHandler(int lines) {
  printf("lines: %d\n", lines);
  grid = calloc(HEIGHT, sizeof(char *));
  for (int y = 0; y <= HEIGHT; y++) {
    grid[y] = calloc(WIDTH, sizeof(char));
  }
}

void lineHandler(char *line, int length) {
  printf("line (%d): %s\n", length, line);

  int dY[] = {-1, 1, 0, 0};
  int dX[] = {0, 0, 1, -1};
  char *dir_s = strtok(line, " ");
  char *dis_s = strtok(NULL, " ");
  char *col_s = strtok(NULL, " ");

  int distance = strtol(dis_s, NULL, 10);

  int dI = 0;
  switch (dir_s[0]) {
  case 'U':
    dI = 0;
    break;
  case 'D':
    dI = 1;
    break;
  case 'R':
    dI = 2;
    break;
  case 'L':
    dI = 3;
    break;
  }

  while (distance--) {
    grid[y_pos][x_pos] = 1;
    y_pos += dY[dI];
    x_pos += dX[dI];
    y_max = max(y_max, y_pos);
    x_max = max(x_max, x_pos);
    y_min = min(y_min, y_pos);
    x_min = min(x_min, x_pos);
  }
}

void print_grid() {
  for (int y = y_min; y <= y_max; y++) {
    for (int x = x_min; x <= x_max; x++) {
      char c = grid[y][x];
      printf("%c", c == 0 ? '.' : '#');
    }
    printf("\n");
  }
}

void flood(int y, int x) {
  // printf("flood (%d,%d) - %d\n", y, x, grid[y][x]);
  // stop if we hit the edge of the grid
  if (y < 0 || y == HEIGHT) {
    return;
  }
  if (x < 0 || x == WIDTH) {
    return;
  }
  // stop if we've been here before, or if there is a cube there
  if (grid[y][x]) {
    return;
  }

  // mark that we have been here
  grid[y][x] = 1;

  // spread out
  flood(y, x + 1);
  flood(y, x - 1);
  flood(y + 1, x);
  flood(y - 1, x);
}

int *find_flood_point(char **grid_to_flood) {
  int *pos = calloc(2, sizeof(int));
  // find a point to start flooding
  for (int y = y_min; y <= y_max; y++) {
    for (int x = x_min; x <= x_max; x++) {
      char prev_c = grid_to_flood[y][x - 1];
      char c = grid_to_flood[y][x];
      char next_c = grid_to_flood[y][x + 1];
      if (!prev_c && c && !next_c) {
        pos[0] = y;
        pos[1] = x + 1;
        break;
      }
    }
  }
  return pos;
}

int main() {
  init();
  readInputFile(__FILE__, lineHandler, fileHandler);

  print_grid();

  int *p1_flood_point = find_flood_point(grid);

  flood(p1_flood_point[0], p1_flood_point[1]);

  printf("---\n");
  print_grid();

  int filled = 0;
  for (int y = y_min; y <= y_max; y++) {
    for (int x = x_min; x <= x_max; x++) {
      char c = grid[y][x];
      filled += c;
    }
  }

  printf("Part one: %d\n", filled);
#ifdef TEST_MODE
  assert(filled == 62);
#else
  assert(filled == 42317);
#endif

  printf("Part two: %d\n", 420);
#ifdef TEST_MODE
  assert(420 == 69);
#else
  assert(420 == 69);
#endif
  exit(EXIT_SUCCESS);
}
