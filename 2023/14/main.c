#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
// #define TEST_MODE
#include "../utils.h"

char **grid;
int height = 0;
int width = 0;
int line_i = 0;

typedef enum dir_t { NORTH = 0, SOUTH, EAST, WEST } dir_t;

int movements[][2] = {
    {-1, 0}, // N
    {1, 0},  // S
    {0, 1},  // E
    {0, -1}  // W
};

void fileHandler(int lines) {
  printf("lines: %d\n", lines);
  grid = calloc(lines, sizeof(char *));
  height = lines;
}

void lineHandler(char *line, int length) {
  printf("line (%d): %s\n", length, line);

  if (width == 0) {
    width = length - 1;
    for (int i = 0; i < height; i++) {
      grid[i] = calloc(width, sizeof(char));
    }
  }

  strncpy(grid[line_i++], line, length);
}

void print_grid(char **grid_to_print) {
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      char c = grid_to_print[y][x];
      printf("%c", c == 0 ? '.' : c);
    }
    printf("\n");
  }
}

void tilt(dir_t dir) {
  int start_y;
  int start_x;
  int op_y;
  int op_x;
  switch (dir) {
  case NORTH:
    start_y = 0;
    start_x = 0;
    op_y = 1;
    op_x = 1;
    break;
  case SOUTH:
    start_y = height - 1;
    start_x = 0;
    op_y = -1;
    op_x = 1;
    break;
  case EAST:
    start_y = 0;
    start_x = width;
    op_y = 1;
    op_x = -1;
    break;
  case WEST:
    start_y = 0;
    start_x = 0;
    op_y = 1;
    op_x = 1;
    break;
  }

  for (int y = start_y;; y += op_y) {
    if (dir == SOUTH) {
      if (y < 0) {
        break;
      }
    } else if (y >= height) {
      break;
    }
    for (int x = start_x;; x += op_x) {
      if (dir == EAST) {
        if (x < 0) {
          break;
        }
      } else if (x > width) {
        break;
      }
      // printf("tilt (%d, %d)\n", y, x);
      char c = grid[y][x];
      if (c != 'O') {
        continue;
      }
      // printf("rock (%d,%d)\n", y, x);
      int *mov = movements[dir];
      int newY = y + mov[0];
      int newX = x + mov[1];

      // printf("new (%d,%d)\n", newY, newX);
      while ((newY >= 0 && newY < height) && (newX >= 0 && newX < width) &&
             grid[newY][newX] == '.') {
        // printf("mov (%d,%d)\n", newY, newX);
        grid[newY - mov[0]][newX - mov[1]] = '.';
        grid[newY][newX] = 'O';
        newY = newY + mov[0];
        newX = newX + mov[1];
      }
    }
  }
}

int calculate_load() {
  int load = 0;
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      char c = grid[y][x];
      if (c == 'O') {
        load += height - y;
      }
    }
  }
  return load;
}
#define HASHMAP_SIZE 65535

// structure for the linked list in each bucket
typedef struct node_t {
  unsigned long key; // a hash
  int value;
  struct node_t *next;
} node_t;

typedef struct {
  node_t *head;
} bucket_t;

typedef struct {
  bucket_t buckets[HASHMAP_SIZE];
} hash_map_t;

unsigned long djb2_hash_grid() {
  unsigned long hash = 5381;

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      char c = grid[y][x];
      hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
  }

  return hash %
         HASHMAP_SIZE;
}

hash_map_t *initializeHashMap() {
  hash_map_t *hashmap = malloc(sizeof(hash_map_t));
  memset(hashmap->buckets, 0, sizeof(hashmap->buckets));
  return hashmap;
}

void insert(hash_map_t *hashmap, unsigned long hash, int value) {

  node_t *newNode = malloc(sizeof(node_t));
  newNode->key = hash;
  newNode->value = value;
  newNode->next = NULL;

  // Insert at the beginning of the linked list (simplest collision resolution)
  newNode->next = hashmap->buckets[hash].head;
  hashmap->buckets[hash].head = newNode;
}

int get(hash_map_t *hashmap, unsigned long hash) {

  node_t *current = hashmap->buckets[hash].head;
  while (current != NULL) {
    if (current->key == hash) {
      return current->value;
    }
    current = current->next;
  }

  // not found
  return -1;
}

int main() {
  init();
  readInputFile(__FILE__, lineHandler, fileHandler);

  // print_grid(grid);

  tilt(NORTH);

  int load = calculate_load();
  printf("Part one: %d\n", load);
#ifdef TEST_MODE
  assert(load == 136);
#else
  assert(load == 110677);
#endif

  hash_map_t *hashmap = initializeHashMap();
  // finish the cycle
  tilt(WEST);
  tilt(SOUTH);
  tilt(EAST);
  // cycle 1, hash grid and store key as cycle number
  insert(hashmap, djb2_hash_grid(), 1);

  int cycle_length;
  int cycle = 1;

  while (cycle++) {
    tilt(NORTH);
    tilt(WEST);
    tilt(SOUTH);
    tilt(EAST);
    // for each cycle iteration
    //   hash grid and see if we have already stored the cycle number before
    unsigned long hash = djb2_hash_grid();
    int seen_cycle = get(hashmap, hash);
    if (seen_cycle != -1) {
      //   if we have, we have identified a loop
      cycle_length = cycle - seen_cycle;
      break;
    } else {
      //   otherwise, store that we've seen it
      insert(hashmap, hash, cycle);
    }
  }

  int remaining_cycles = (1000000000 - cycle) % cycle_length;

  while (remaining_cycles--) {
    tilt(NORTH);
    tilt(WEST);
    tilt(SOUTH);
    tilt(EAST);
  }
  load = calculate_load();
  printf("Part two: %d\n", load);
#ifdef TEST_MODE
  assert(load == 64);
#else
  assert(load == 90551);
#endif
  exit(EXIT_SUCCESS);
}
