#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
// #define TEST_MODE
#include "../../lib/pqueue.h"
#include "../utils.h"

#ifdef TEST_MODE
#define MAX_STEPS 6
#else
#define MAX_STEPS 64
#endif

char **grid;
// any point visited in an even number of steps
char **end_positions;
int height = -1;
int width = 0;

typedef struct {
  int y;
  int x;
  int s;
} point_t;

void fileHandler(int lines) {
  printf("lines: %d\n", lines);
  grid = calloc(lines, sizeof(char *));
  end_positions = calloc(lines, sizeof(char *));
}

void lineHandler(char *line, int length) {
  printf("line (%d): %s\n", length, line);

  if (width == 0) {
    width = length - 2;
  }

  grid[++height] = calloc(width, sizeof(char));
  end_positions[height] = calloc(width, sizeof(char));
  strncpy(grid[height], line, length);
}

void print_grid(char **grid_to_print) {
  for (int y = 0; y <= height; y++) {
    for (int x = 0; x <= width; x++) {
      char c = grid_to_print[y][x];
      if (c == 0) {
        printf(".");
      } else if (c <= MAX_STEPS) {
        printf("%d", c);
      } else {
        printf("%c", c);
      }
    }
    printf("\n");
  }
}

void add_point(pqueue_t *queue, int y, int x, int s) {
  point_t *new_point = malloc(sizeof(point_t));
  new_point->y = y;
  new_point->x = x;
  new_point->s = s;
  pq_enqueue(queue, new_point, s);
}

// or maybe DFS...i dont know!
int BFS(int start_y, int start_x, int max_steps) {
  printf("start (%d, %d)\n", start_y, start_x);

  pqueue_t *queue = pq_create();
  point_t *start_point = calloc(1, sizeof(point_t));
  start_point->y = start_y;
  start_point->x = start_x;
  start_point->s = 0;

  add_point(queue, start_y + 1, start_x, 1);
  add_point(queue, start_y - 1, start_x, 1);
  add_point(queue, start_y, start_x + 1, 1);
  add_point(queue, start_y, start_x - 1, 1);

  while (!pq_empty(queue)) {
    point_t *point = pq_dequeue(queue);
    int y = point->y;
    int x = point->x;
    int s = point->s;

    // stop if we hit the edge of the grid
    if (y < 0 || y > height || x < 0 || x > width) {
      free(point);
      continue;
    }
    // stop if we're a wall
    if (grid[y][x] == '#') {
      free(point);
      continue;
    }

    // stop if we can visit this already
    if (end_positions[y][x]) {
      if (end_positions[y][x] % 2 == 0 && s % 2 == 0) {
        free(point);
        continue;
      }
    }
    // if steps is even, this is a valid end point
    if (s % 2 == 0) {
      end_positions[y][x] = s;
    }

    if (s == max_steps) {
      free(point);
      continue;
    }

    int dY[] = {-1, 1, 0, 0};
    int dX[] = {0, 0, 1, -1};
    for (int i = 0; i < 4; i++) {
      add_point(queue, y + dY[i], x + dX[i], s + 1);
    }
    free(point);
  }
  pq_destroy(queue);

  return 0;
}

int main() {
  init();
  readInputFile(__FILE__, lineHandler, fileHandler);

  // print_grid(grid);

  printf("centre: %c\n", grid[height / 2][width / 2]);
  BFS(height / 2, width / 2, MAX_STEPS);

  // printf("\n end_positions \n");
  // print_grid(end_positions);

  int part_one = 0;
  for (int y = 0; y <= height; y++) {
    for (int x = 0; x <= width; x++) {
      char c = end_positions[y][x];
      if (c != 0) {
        part_one++;
      }
    }
  }
  printf("Part one: %d\n", part_one);
#ifdef TEST_MODE
  assert(part_one == 16);
#else
  assert(part_one == 3764);
#endif

  printf("Part two: %d\n", 420);
#ifdef TEST_MODE
  assert(420 == 69);
#else
  // assert(420 == 69);
#endif
  exit(EXIT_SUCCESS);
}
