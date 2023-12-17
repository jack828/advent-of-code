#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#define TEST_MODE
#include "../../lib/queue.h"
#include "../utils.h"

char **grid;
char **energised;
int height = 0;
int width = 0;
int line_i = 0;

typedef enum dir_t { NORTH = 1, SOUTH = 2, EAST = 4, WEST = 8 } dir_t;

typedef struct beam_t {
  dir_t dir;
  int y;
  int x;
} beam_t;

void fileHandler(int lines) {
  printf("lines: %d\n", lines);
  grid = calloc(lines, sizeof(char *));
  energised = calloc(lines, sizeof(char *));
  height = lines;
}

void lineHandler(char *line, int length) {
  printf("line (%d): %s\n", length, line);

  if (width == 0) {
    width = length - 1;
    for (int i = 0; i < height; i++) {
      grid[i] = calloc(width, sizeof(char));
      energised[i] = calloc(width, sizeof(char));
    }
  }

  strncpy(grid[line_i++], line, length);
}

void print_grid(char **grid_to_print) {
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      int c = grid_to_print[y][x];
      printf("%c", c == 0 ? '.' : c);
    }
    printf("\n");
  }
}

void print_energised() {
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      int c = energised[y][x];
      printf("%c", c == 0 ? '.' : '0' + c);
    }
    printf("\n");
  }
}

int count_energised() {
  int count = 0;
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      int c = energised[y][x];

      if (c) {
        count++;
      }
    }
  }
  return count;
}

bool loop_check(dir_t dir, int y, int x) { return energised[y][x] & dir; }
int main() {
  init();
  readInputFile(__FILE__, lineHandler, fileHandler);

  print_grid(grid);

  queue_t *queue = q_create();

  beam_t *start_beam = malloc(sizeof(beam_t));
  start_beam->dir = EAST;
  start_beam->y = 0;
  start_beam->x = -1;
  q_enqueue(queue, start_beam);

  while (!q_empty(queue)) {
    // printf("energised: %d\n", count_energised());
    beam_t *beam = q_dequeue(queue);
    dir_t dir = beam->dir;
    int y = beam->y;
    int x = beam->x;
    int dY = 0;
    int dX = 0;
    switch (dir) {
    case NORTH:
      dY = -1;
      break;
    case SOUTH:
      dY = 1;
      break;
    case EAST:
      dX = 1;
      break;
    case WEST:
      dX = -1;
      break;
    }

    int newY = y + dY;
    int newX = x + dX;
    if (newY < 0 || newY >= height) {
      free(beam);
      continue;
    }
    if (newX < 0 || newX >= width) {
      free(beam);
      continue;
    }
    char next = grid[newY][newX];

    if (next == '|' && (dir == EAST || dir == WEST)) {
      if (loop_check(dir, newY, newX)) {
        free(beam);
        continue;
      }
      // splits
      beam_t *beam_n = malloc(sizeof(beam_t));
      beam_n->dir = NORTH;
      beam_n->y = newY;
      beam_n->x = newX;
      q_enqueue(queue, beam_n);
      beam_t *beam_s = malloc(sizeof(beam_t));
      beam_s->dir = SOUTH;
      beam_s->y = newY;
      beam_s->x = newX;
      q_enqueue(queue, beam_s);
    } else if (next == '-' && (dir == NORTH || dir == SOUTH)) {
      if (loop_check(dir, newY, newX)) {
        free(beam);
        continue;
      }
      // splits
      beam_t *beam_e = malloc(sizeof(beam_t));
      beam_e->dir = EAST;
      beam_e->y = newY;
      beam_e->x = newX;
      q_enqueue(queue, beam_e);
      beam_t *beam_w = malloc(sizeof(beam_t));
      beam_w->dir = WEST;
      beam_w->y = newY;
      beam_w->x = newX;
      q_enqueue(queue, beam_w);
    } else if (next == '/') {
      // rotate
      switch (dir) {
      case NORTH:
        beam->dir = EAST;
        break;
      case EAST:
        beam->dir = NORTH;
        break;
      case SOUTH:
        beam->dir = WEST;
        break;
      case WEST:
        beam->dir = SOUTH;
        break;
      }
      beam->y = newY;
      beam->x = newX;
      q_enqueue(queue, beam);
    } else if (next == '\\') {
      // rotate
      switch (dir) {
      case NORTH:
        beam->dir = WEST;
        break;
      case WEST:
        beam->dir = NORTH;
        break;
      case SOUTH:
        beam->dir = EAST;
        break;
      case EAST:
        beam->dir = SOUTH;
        break;
      }
      beam->y = newY;
      beam->x = newX;
      q_enqueue(queue, beam);
    } else {
      energised[newY][newX] |= dir;
      // empty, just move in same direction
      beam->y = newY;
      beam->x = newX;
      q_enqueue(queue, beam);
    }
    energised[newY][newX] |= dir;
  }
  printf("\n");
  q_destroy(queue);

  // print_grid(energised);
  print_energised();
  int part_one = count_energised();
  printf("Part one: %d\n", part_one);
#ifdef TEST_MODE
  assert(part_one == 46);
#else
  assert(part_one == 6795);
#endif

  printf("Part two: %d\n", 420);
#ifdef TEST_MODE
  assert(420 == 69);
#else
  assert(420 == 69);
#endif
  exit(EXIT_SUCCESS);
}
