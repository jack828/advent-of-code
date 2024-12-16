#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
// #define TEST_MODE
#include "../utils.h"

typedef enum dir_t { UP = '^', DOWN = 'v', RIGHT = '>', LEFT = '<' } dir_t;
typedef enum block_t {
  WALL = '#',
  EMPTY = '.',
  CRATE = 'O',
  ROBOT = '@'
} block_t;

int width = 0;
int height = 0;
char *instructions;
char **map = NULL;
int line_i = 0;
int robot_x = 0;
int robot_y = 0;

void fileHandler(int lines) { printf("lines: %d\n", lines); }

bool map_input = true;

void lineHandler(char *line, int length) {
  printf("line (%d): %s\n", length, line);
  if (length == 1) {
    map_input = false;
    return;
  }

  if (map_input) {
    if (map == NULL) {
      width = length - 1;
      height = length - 1;
      map = calloc(length, sizeof *map);
      // for (int y = 0; y < length; y++) {
      // map[y] = calloc(length, sizeof **map);
      // }
    }
    map[line_i++] = strndup(line, length - 1);
    char *idx = strchr(line, ROBOT);
    if (idx) {
      robot_y = line_i - 1;
      robot_x = idx - line;
    }
  } else {
    // instructions
    if (instructions == NULL) {
      instructions = calloc(length * 50, sizeof *instructions);
    }
    strncat(instructions, line, length - 1);
  }
}

void printMap(char **map_to_print) {
  printf("(%dx%d)\n", height, width);
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      // printf("(%d,%d)\n", y, x);
      int c = map_to_print[y][x];
      printf("%c", c);
    }
    printf("\n");
  }
}

int instToIndex(dir_t dir) {
  switch (dir) {
  case UP:
    return 0;
  case DOWN:
    return 1;
  case RIGHT:
    return 2;
  case LEFT:
    return 3;
  }
}

int getGPSCoords() {
  int sum = 0;

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      int c = map[y][x];
      if (c != CRATE) {
        continue;
      }
      sum += (100 * y) +x;
    }
  }
  return sum;
}

int main() {
  init();
  readInputFile(__FILE__, lineHandler, fileHandler);

  printf("robot: (%d,%d)\n", robot_y, robot_x);
  printMap(map);
  printf("instructions: %s\n", instructions);

  // up, down, right, left
  int dY[] = {-1, 1, 0, 0};
  int dX[] = {0, 0, 1, -1};
  do {
    char inst = *instructions;
    int idx = instToIndex(inst);
    int new_y = robot_y + dY[idx];
    int new_x = robot_x + dX[idx];
    printf("%c -> (%d,%d)\n", inst, new_y, new_x);

    char pos = map[new_y][new_x];
    if (pos == WALL) {
      printf("wall\n");
      continue;
    } else if (pos == EMPTY) {
      printf("empty\n");
      map[robot_y][robot_x] = EMPTY;
      robot_y = new_y;
      robot_x = new_x;
      map[robot_y][robot_x] = ROBOT;
    } else {
      // pos == CRATE
      // need to find each crate in the line we are facing
      // until there is a gap or wall
      char crates[width][2];
      int crate_count = 0;
      int y = new_y;
      int x = new_x;
      while (map[y][x] == CRATE) {
        printf("crates: #%d (%d,%d) = %c\n", crate_count, y, x, map[y][x]);
        crates[crate_count][0] = y;
        crates[crate_count][1] = x;
        crate_count++;
        y += dY[idx];
        x += dX[idx];
      }
      printf("crates end: %d crates, end pos (%d,%d) = %c\n", crate_count, y, x,
             map[y][x]);
      // if it ends in a wall, do nothing, cannot move
      if (map[y][x] == WALL) {
        continue;
      }
      // if it ends in a gap, shuffle all by 1
      // for (int i = crate_count; i >=0; i --) {
      for (int i = 0; i < crate_count; i++) {
        int crate_y = crates[i][0];
        int crate_x = crates[i][1];
        printf("mov crates: #%d (%d,%d) => (%d, %d)\n", i, crate_y, crate_x,
               crate_y + dY[idx], crate_x + dX[idx]);
        // no need to clear it, the robot will do this
        // map[crate_y][crate_x] = EMPTY;
        map[crate_y + dY[idx]][crate_x + dX[idx]] = CRATE;
      }

      printf("mov robot: (%d,%d) => (%d, %d)\n", robot_y, robot_x, new_y,
             new_x);
      map[robot_y][robot_x] = EMPTY;
      robot_y = new_y;
      robot_x = new_x;
      map[robot_y][robot_x] = ROBOT;
    }
    // printf("new: (%d,%d) = %c\n", new_y, new_x, pos);

    printMap(map);
  } while (*(++instructions));
  printMap(map);

  int part_one = getGPSCoords();
  printf("Part one: %d\n", part_one);
#ifdef TEST_MODE
  assert(part_one == 10092);
#else
  assert(part_one == 1509074);
#endif

  printf("Part two: %d\n", 420);
#ifdef TEST_MODE
  assert(420 == 69);
#else
  assert(420 == 69);
#endif
  exit(EXIT_SUCCESS);
}
