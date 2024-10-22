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

typedef struct point_t {
  int y;
  int x;
  int s;
  char *hashmap;
} point_t;

char **map;
int height = 0;
int width = 0;
int line_i = 0;
#define HASHMAP_SIZE (UINT16_MAX)
char hm[HASHMAP_SIZE] = {0};

void fileHandler(int lines) {
  printf("lines: %d\n", lines);
  map = calloc(lines, sizeof(char *));
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

void print_map() {
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      int c = map[y][x];
      printf("%c", c);
    }
    printf("\n");
  }
}

uint64_t hash_point(point_t *point) {
  uint64_t hash = 0;
  hash |= (uint64_t)(point->y);
  hash |= (uint64_t)(point->x) << 8;
  return hash % HASHMAP_SIZE;
}

void add_point(pqueue_t *queue, int y, int x, int s, char *hashmap) {
  point_t *new_point = malloc(sizeof(point_t));
  new_point->y = y;
  new_point->x = x;
  new_point->s = s;
  new_point->hashmap = calloc(HASHMAP_SIZE, sizeof(char));
  memcpy(new_point->hashmap, hashmap, HASHMAP_SIZE);
  pq_enqueue(queue, new_point, s);
}

void destroy_point(point_t *point) {
  free(point->hashmap);
  free(point);
}

bool isInvalidPoint(int y, int x, char *hashmap) {
  bool invalid =
      y < 0 || x < 0 || y >= height || x >= width || map[y][x] == '#';
  if (invalid) {
    return invalid;
  }

  uint64_t key = 0;
  key |= (uint64_t)(y);
  key |= (uint64_t)(x) << 8;

  return hashmap[key] == 1;
}

// or bfs
int DFS(int start_x, int start_y, int end_y, int end_x) {
  printf("DFS: (%d,%d) -> (%d,%d)\n", start_x, start_y, end_y, end_x);

  pqueue_t *output = pq_create();
  pqueue_t *queue = pq_create();

  point_t *new_point = malloc(sizeof(point_t));
  new_point->y = start_y;
  new_point->x = start_x;
  new_point->s = 1;
  new_point->hashmap = calloc(HASHMAP_SIZE, sizeof(char));
  pq_enqueue(queue, new_point, new_point->s);

  while (!pq_empty(queue)) {
    point_t *point = pq_dequeue(queue);

    if (point->y == end_y && point->x == end_x) {
      // end reached!
      // printf("END: (%d,%d) -> [%d]\n", point->x, point->y, point->s);
      pq_enqueue(output, point, point->s);
      // free'd later
      continue;
    }

    uint64_t key = hash_point(point);

    point->hashmap[key] = 1;

    // printf("point: (%d,%d) -> [%d]\n", point->x, point->y, point->s);

    char pos = map[point->y][point->x];
    // are we forced to make a move?
    if (pos == '>') {
      if (!isInvalidPoint(point->y, point->x + 1, point->hashmap)) {
        add_point(queue, point->y, point->x + 1, point->s + 1, point->hashmap);
      }
    } else if (pos == '<') {
      if (!isInvalidPoint(point->y, point->x - 1, point->hashmap)) {
        add_point(queue, point->y, point->x - 1, point->s + 1, point->hashmap);
      }
    } else if (pos == 'v') {
      if (!isInvalidPoint(point->y + 1, point->x, point->hashmap)) {
        add_point(queue, point->y + 1, point->x, point->s + 1, point->hashmap);
      }
    } else {
      // otherwise go everywhere, dequeue will handle walls etc
      int dY[] = {-1, 1, 0, 0};
      int dX[] = {0, 0, 1, -1};
      for (int i = 0; i < 4; i++) {
        if (!isInvalidPoint(point->y + dY[i], point->x + dX[i],
                            point->hashmap)) {
          add_point(queue, point->y + dY[i], point->x + dX[i], point->s + 1,
                    point->hashmap);
        }
      }
    }

    destroy_point(point);
  }
  pq_destroy(queue);

  int max_steps = 0;
  while (!pq_empty(output)) {
    point_t *point = pq_dequeue(output);
    // printf("output: %d\n", point->s);
    max_steps = point->s + 1; // we skip a step to make it easier
    destroy_point(point);
  }
  pq_destroy(output);

  return max_steps;
}

int main() {
  init();
  readInputFile(__FILE__, lineHandler, fileHandler);

  map[0][1] = '#';
  map[1][1] = 'S';
  map[height - 2][width - 2] = 'E';
  // print_map();

  int max_steps = DFS(1, 1, height - 2, width - 2);

  printf("Part one: %d\n", max_steps);
#ifdef TEST_MODE
  assert(max_steps == 94);
#else
  assert(max_steps == 2314);
#endif

  printf("Part two: %d\n", 420);
#ifdef TEST_MODE
  assert(420 == 69);
#else
  assert(420 == 69);
#endif
  exit(EXIT_SUCCESS);
}
