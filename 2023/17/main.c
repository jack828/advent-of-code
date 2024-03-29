#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
// #define TEST_MODE
#define HASHMAP_SIZE (60000000)
#include "../../lib/hashmap.h"
#include "../../lib/pqueue.h"
#include "../utils.h"

char **grid;
int height = 0;
int width = 0;

int startY = 0;
int startX = 0;

typedef enum { NORTH = 1, SOUTH = 2, EAST = 4, WEST = 8 } dir_t;

typedef struct {
  int y;
  int x;
  dir_t dir;
  int straight_count;
  int heat_loss;
} point_t;

void fileHandler(int lines) {
  printf("lines: %d\n", lines);
  grid = calloc(lines + 1, sizeof(char *));
}

void lineHandler(char *line, int length) {
  printf("line (%d): %s\n", length, line);

  if (width == 0) {
    width = length - 1;
  }

  grid[height] = calloc(width, sizeof(char));
  for (int x = 0; x < width; x++) {
    grid[height][x] = line[x] - '0';
  }
  height++;
}

void print_grid(char **grid_to_print) {
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      char c = grid_to_print[y][x];
      // printf("(%d, %d) = %d", y, x, c);
      printf("%d", c);
    }
    printf("\n");
  }
}

char *dir_str(dir_t dir) {
  switch (dir) {
  case NORTH:
    return "NORTH";
  case SOUTH:
    return "SOUTH";
  case EAST:
    return "EAST";
  case WEST:
    return "WEST";
  default:
    return "NONE";
  }
}

// FIXME this hash method is certainly not great
// not is the hashmap implementation
unsigned long hash_point(point_t *point) {
  unsigned long hash = 0;
  hash |= (unsigned long)(point->y);
  hash |= (unsigned long)(point->x) << 8;
  hash |= (unsigned long)(point->straight_count) << 16;
  hash |= (unsigned long)(point->dir) << 24;
  // unsigned long hash = 5381;
  //
  // int c;
  //
  // c = point->y;
  // hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
  // c = point->x;
  // hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
  // c = point->dir;
  // hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
  // c = point->straight_count;
  // hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
  //
  return hash % HASHMAP_SIZE;
}

int dijkstra(int aY, int aX, int bY, int bX) {
  int min_heat_loss = -1;

  hashmap_t *hashmap = hm_create();
  pqueue_t *queue = pq_create();
  point_t *start = malloc(sizeof(point_t));
  start->y = aY;
  start->x = aX;
  start->straight_count = 1;
  start->heat_loss = 0;
  pq_enqueue(queue, start, 0);

  printf("(%d, %d) --> (%d, %d)\n", aY, aX, bY, bX);

  int *s = malloc(sizeof(int));
  *s = 1;
  hm_set(hashmap, hash_point(start), s);

  // north, south, east, west
  int dY[] = {-1, 1, 0, 0};
  int dX[] = {0, 0, 1, -1};
  int dD[] = {NORTH, SOUTH, EAST, WEST};
  while (!pq_empty(queue)) {
    point_t *point = pq_dequeue(queue);
    // printf("\n(%d, %d) %s - %d\n", point->y, point->x, dir_str(point->dir),
    // point->heat_loss);
    if (point->y == bY && point->x == bX) {
      min_heat_loss = point->heat_loss;
      break;
    }
    // printf("> hash %ld\n", hash_point(point));
    for (int i = 0; i < 4; i++) {
      point_t *new_point = malloc(sizeof(point_t));
      new_point->y = point->y + dY[i];
      new_point->x = point->x + dX[i];
      new_point->dir = dD[i];
      new_point->straight_count = point->straight_count;

      // out of bounds
      if (new_point->y < 0 || new_point->y >= height || new_point->x < 0 ||
          new_point->x >= width) {
        // printf("    oob (%d, %d) %s\n", new_point->y, new_point->x,
        // dir_str(new_point->dir));
        free(new_point);
        continue;
      }

      // cannot go back on self
      if ((point->dir == NORTH && new_point->dir == SOUTH) ||
          (point->dir == SOUTH && new_point->dir == NORTH) ||
          (point->dir == WEST && new_point->dir == EAST) ||
          (point->dir == EAST && new_point->dir == WEST)) {
        free(new_point);
        continue;
      }

      // reached straight line limit, cannot continue in same dir
      if (new_point->dir == point->dir) {
        // can only do this thrice
        if (point->straight_count == 3) {
          free(new_point);
          continue;
        } else {
          new_point->straight_count++;
        }
      } else {
        new_point->straight_count = 1;
      }

      new_point->heat_loss =
          point->heat_loss + grid[new_point->y][new_point->x];

      // printf("    new (%d, %d) %s h: %d->%d\n", new_point->y, new_point->x,
      // dir_str(new_point->dir), point->heat_loss, new_point->heat_loss);
      unsigned long hash = hash_point(new_point);
      int *pvalue = hm_get(hashmap, hash);
      if (pvalue != NULL) {
        free(new_point);
        continue;
      } else {
        int *v = malloc(sizeof(int));
        *v = 1;
        hm_set(hashmap, hash, v);
      }

      // printf("> (%d, %d) %s - loss %d, str %d\n", new_point->y, new_point->x,
      //        dir_str(new_point->dir), new_point->heat_loss,
      //        new_point->straight_count);
      pq_enqueue(queue, new_point, new_point->heat_loss);
    }
    free(point);
  }

  pq_destroy(queue);
  hm_destroy(hashmap);

  return min_heat_loss;
}

int dijkstra_ultra(int aY, int aX, int bY, int bX) {
  int min_heat_loss = -1;
  int min_in_dir = 4;
  int max_in_dir = 10;

  hashmap_t *hashmap = hm_create();
  pqueue_t *queue = pq_create();
  point_t *start_s = malloc(sizeof(point_t));
  start_s->y = aY;
  start_s->x = aX;
  start_s->dir = SOUTH;
  start_s->straight_count = 1;
  start_s->heat_loss = 0;
  pq_enqueue(queue, start_s, 0);

  point_t *start_e = malloc(sizeof(point_t));
  start_e->y = aY;
  start_e->x = aX;
  start_e->dir = EAST;
  start_e->straight_count = 1;
  start_e->heat_loss = 0;
  pq_enqueue(queue, start_e, 0);

  printf("(%d, %d) --> (%d, %d)\n", aY, aX, bY, bX);

  int *s = malloc(sizeof(int));
  *s = 1;
  hm_set(hashmap, hash_point(start_s), s);
  hm_set(hashmap, hash_point(start_e), s);

  // north, south, east, west
  int dY[] = {-1, 1, 0, 0};
  int dX[] = {0, 0, 1, -1};
  int dD[] = {NORTH, SOUTH, EAST, WEST};
  while (!pq_empty(queue)) {
    point_t *point = pq_dequeue(queue);
    // cannot finish unless we've moved at least min_in_dir steps
    if (point->y == bY && point->x == bX && point->straight_count >= min_in_dir) {
      min_heat_loss = point->heat_loss;
      break;
    }
    // printf("> hash %ld\n", hash_point(point));
    for (int i = 0; i < 4; i++) {
      point_t *new_point = malloc(sizeof(point_t));
      new_point->y = point->y + dY[i];
      new_point->x = point->x + dX[i];
      new_point->dir = dD[i];
      new_point->straight_count = point->straight_count;

      // out of bounds
      if (new_point->y < 0 || new_point->y >= height || new_point->x < 0 ||
          new_point->x >= width) {
        // printf("    oob (%d, %d) %s\n", new_point->y, new_point->x,
        // dir_str(new_point->dir));
        free(new_point);
        continue;
      }

      // cannot go back on self
      if ((point->dir == NORTH && new_point->dir == SOUTH) ||
          (point->dir == SOUTH && new_point->dir == NORTH) ||
          (point->dir == WEST && new_point->dir == EAST) ||
          (point->dir == EAST && new_point->dir == WEST)) {
        free(new_point);
        continue;
      }

      // if going in a straight line
      if (new_point->dir == point->dir) {
        // make sure we don't go over limit
        if (point->straight_count >= max_in_dir) {
          free(new_point);
          continue;
        } else {
          new_point->straight_count++;
        }
      } else {
        // if trying to turn
        // only do so if we have been 4 or more spaces
        if (new_point->straight_count < min_in_dir) {
          free(new_point);
          continue;
        }
        new_point->straight_count = 1;
      }

      // cannot turn, not gone far enough
      if (new_point->straight_count < 4 && point->dir != new_point->dir) {
      }

      new_point->heat_loss =
          point->heat_loss + grid[new_point->y][new_point->x];

      // printf("    new (%d, %d) %s h: %d->%d\n", new_point->y, new_point->x,
      // dir_str(new_point->dir), point->heat_loss, new_point->heat_loss);
      unsigned long hash = hash_point(new_point);
      int *pvalue = hm_get(hashmap, hash);
      if (pvalue != NULL) {
        free(new_point);
        continue;
      } else {
        int *v = malloc(sizeof(int));
        *v = 1;
        hm_set(hashmap, hash, v);
      }

      // printf("> (%d, %d) %s - loss %d, str %d\n", new_point->y, new_point->x,
      // dir_str(new_point->dir), new_point->heat_loss,
      // new_point->straight_count);
      pq_enqueue(queue, new_point, new_point->heat_loss);
    }
    free(point);
  }

  pq_destroy(queue);
  hm_destroy(hashmap);

  return min_heat_loss;
}

int main() {
  init();
  readInputFile(__FILE__, lineHandler, fileHandler);

  print_grid(grid);

  int min_heat_loss = dijkstra(0, 0, height - 1, width - 1);
  // print_seen();

  printf("Part one: %d\n", min_heat_loss);
#ifdef TEST_MODE
  assert(min_heat_loss == 102);
#else
  assert(min_heat_loss == 785);
#endif

  int min_heat_loss_ultra_crucible =
      dijkstra_ultra(0, 0, height - 1, width - 1);

  printf("Part two: %d\n", min_heat_loss_ultra_crucible);
#ifdef TEST_MODE
  assert(min_heat_loss_ultra_crucible == 94);
#else
  assert(min_heat_loss_ultra_crucible == 922);
#endif
  exit(EXIT_SUCCESS);
}
