#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
// #define TEST_MODE
#include "../../lib/pqueue.h"
#include "../utils.h"
#define HASHMAP_SIZE (65535000)

// Future me, if you ever come back to this, do what this vid has to optimise
// https://youtu.be/99Mjs1i0JxU?t=610
// hashmap : untranslated values
// coords: translated to map for wall detect ect
//
// translate fn -> mod to wrap to map size
// mod_coord starts off but i couldnt figure it out :c
//
#ifdef TEST_MODE
#define MAX_STEPS 6
#else
#define MAX_STEPS 64
#endif

int grid_multiplier = 101;
char **grid;
// any point visited in an even number of steps
char **end_positions;
int height = -1;
int width = 0;
int grid_size;
char hm[HASHMAP_SIZE] = {0};

typedef struct {
  int y;
  int x;
  int s;
} point_t;

void noopFileHandler(int lines) {}

void fileHandler(int lines) {
  printf("lines: %d\n", lines);
  grid_size = lines;
  grid = calloc(lines * grid_multiplier, sizeof(char *));
  end_positions = calloc(lines * grid_multiplier, sizeof(char *));
}

void lineHandler(char *line, int length) {
  // printf("line (%d): %s\n", length, line);

  if (width == 0) {
    width = (length * grid_multiplier) - (1 + grid_multiplier);
  }

  grid[++height] = calloc(width, sizeof(char));
  for (int i = 0; i < grid_multiplier; i++) {
    strncat(grid[height], line, length);
  }
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

uint64_t hash_point(point_t *point) {
  uint64_t hash = 0;
  hash |= (uint64_t)(point->y);
  hash |= (uint64_t)(point->x) << 32;
  return hash % HASHMAP_SIZE;
}

void add_point(pqueue_t *queue, int y, int x, int s) {
  point_t *new_point = malloc(sizeof(point_t));
  new_point->y = y;
  new_point->x = x;
  new_point->s = s;
  pq_enqueue(queue, new_point, s);
}

int mod_coord(int coord) {
  return coord;
  int range_max = grid_size;
  int id = abs(coord) % (grid_size);

  if (coord >= 0) {
    return id;
  } else {
    return range_max - id;
  }
}

// or maybe DFS...i dont know!
int BFS(int start_y, int start_x, int max_steps) {
  printf("start (%d, %d) max %d\n", start_y, start_x, max_steps);

  int odd_or_even_steps = max_steps % 2;
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
    int y = mod_coord(point->y);
    int x = mod_coord(point->x);
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

    unsigned long hash_key = hash_point(point);

    // as in, we can reach here, valid end, etc
    int is_odd_or_even_steps = s % 2 == odd_or_even_steps;
    // stop if we can visit this already

    if (hm[hash_key]) {
      if (is_odd_or_even_steps) {
        free(point);
        continue;
      }
    }
    // if steps is odd_or_even_steps, this is a valid end point
    if (is_odd_or_even_steps) {
      hm[hash_key] = s;
    }

    if (s == max_steps) {
      free(point);
      continue;
    }

    int dY[] = {-1, 1, 0, 0};
    int dX[] = {0, 0, 1, -1};
    for (int i = 0; i < 4; i++) {
      add_point(queue, point->y + dY[i], point->x + dX[i], s + 1);
    }
    free(point);
  }
  pq_destroy(queue);

  printf("end\n");
  return 0;
}

int countAndResetEndPositions() {
  int count = 0;
  for (int i = 0; i < HASHMAP_SIZE; i++) {
    if (hm[i]) {
      count++;
      hm[i] = 0;
    }
  }
  return count;
}

int main() {
  init();
  readInputFile(__FILE__, lineHandler, fileHandler);

  for (int i = 0; i < grid_multiplier - 1; i++) {
    readInputFile(__FILE__, lineHandler, noopFileHandler);
  }

  // print_grid(grid);

  printf("centre: %c\n", grid[height / 2][width / 2]);
  printf("grid size: %d\n", grid_size);
  BFS(height / 2, width / 2, MAX_STEPS);

  int part_one = countAndResetEndPositions();
  printf("Part one: %d\n", part_one);
#ifdef TEST_MODE
  assert(part_one == 34);
#else
  assert(part_one == 3764);
#endif
// I'm not gunna lie here
// I'm just throwing stuff I don't understand from reddit together
// Maths is not my strong suit, this one makes me sad
// something something quadratic polynomial
/* TODO this could definitely do it all in one go, and just spit out
 * the counts at the intervals required, i.e. MAX_STEPS, input etc
 */
#ifdef TEST_MODE
  // TEST CASES, SAMPLE INPUT ONLY //
  BFS(height / 2, width / 2, 7);
  int out1 = countAndResetEndPositions();
  printf("7 steps = %d\n", out1);
  assert(out1 == 52);
  BFS(height / 2, width / 2, 8);
  int out2 = countAndResetEndPositions();
  printf("8 steps = %d\n", out2);
  assert(out2 == 68);
  BFS(height / 2, width / 2, 25);
  int out3 = countAndResetEndPositions();
  printf("25 steps = %d\n", out3);
  assert(out3 == 576);
  BFS(height / 2, width / 2, 42);
  int out4 = countAndResetEndPositions();
  printf("42 steps = %d\n", out4);
  assert(out4 == 1576);
  BFS(height / 2, width / 2, 59);
  int out5 = countAndResetEndPositions();
  printf("59 steps = %d\n", out5);
  assert(out5 == 3068);
  BFS(height / 2, width / 2, 100);
  int out6 = countAndResetEndPositions();
  printf("100 steps = %d\n", out6);
  assert(out6 == 8702);
#endif

  int half_grid_size = grid_size / 2;
  BFS(height / 2, width / 2, half_grid_size);
  int x_0 = countAndResetEndPositions();
  BFS(height / 2, width / 2, half_grid_size + grid_size);
  int x_1 = countAndResetEndPositions();
  BFS(height / 2, width / 2, half_grid_size + (grid_size * 2));
  int x_2 = countAndResetEndPositions();
  printf("x0,x1,x2 = %d,%d,%d\n", x_0, x_1, x_2);

  // "3 point formula"
  int a = (x_2 - (2 * x_1) + x_0) / 2;
  int b = x_1 - x_0 - a;
  int c = x_0;
  printf("a,b,c = %d,%d,%d\n", a, b, c);
#ifdef TEST_MODE
  int steps = 1180148;
#else
  int steps = 26501365;
#endif
  unsigned long long x = ((steps - half_grid_size) / grid_size);
  printf("x = %llu\n", x);
  unsigned long long part_two = (a * (x * x)) + (b * x) + c;
  printf("Part two: %llu\n", part_two);
#ifdef TEST_MODE
  // modified input from
  // https://old.reddit.com/r/adventofcode/comments/18o1071/2023_day_21_a_better_example_input_mild_part_2/
  // and help from https://github.com/derailed-dash/Advent-of-Code/blob/master/src/AoC_2023/Dazbo's_Advent_of_Code_2023.ipynb
  // and also https://www.youtube.com/watch?v=99Mjs1i0JxU&lc=UgxwS0greNfsrFXYPA14AaABAg.9ydjxdKIlaC9ydpDNgSwyb
  // many tanks!
  assert(part_two == 1185525742508llu);
#else
  assert(part_two == 622926941971282);
#endif
  exit(EXIT_SUCCESS);
}
