#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
// #define TEST_MODE
#include "../../utils.h"

#define MAX_CUBES 2030
#define MAX_X 30
#define MAX_Y 30
#define MAX_Z 30
char grid[MAX_X][MAX_Y][MAX_Z] = {0};
char visited[MAX_X][MAX_Y][MAX_Z] = {0};

enum { EMPTY = 0, ROCK = 1, WATER = 2 };

typedef struct cube_t {
  int x;
  int y;
  int z;
} cube_t;

cube_t *cubes[MAX_CUBES];
int cubeIndex = 0;

void lineHandler(char *line) {
  fputs("line: ", stdout);
  fputs(line, stdout);

  int x = atoi(strtok(line, ","));
  int y = atoi(strtok(NULL, ","));
  int z = atoi(strtok(NULL, ","));

  grid[x][y][z] = ROCK;
  cube_t *cube = malloc(sizeof(cube_t));
  cube->x = x;
  cube->y = y;
  cube->z = z;
  cubes[cubeIndex++] = cube;
}

void flood(int x, int y, int z) {
  // stop if we hit the edge of the grid
  if (x < 0 || x == MAX_X) {
    return;
  }
  if (y < 0 || y == MAX_Y) {
    return;
  }
  if (z < 0 || z == MAX_Z) {
    return;
  }
  // stop if we've been here before, or if there is a cube there
  if (grid[x][y][z] != EMPTY) {
    return;
  }

  // mark that we have been here
  grid[x][y][z] = WATER;

  // spread out
  flood(x + 1, y, z);
  flood(x - 1, y, z);
  flood(x, y + 1, z);
  flood(x, y - 1, z);
  flood(x, y, z + 1);
  flood(x, y, z - 1);
}

int main() {
  readInput(__FILE__, lineHandler);

  int totalSides = cubeIndex * 6;
  int connectedSides = 0;

  for (int i = 0; i < cubeIndex; i++) {
    cube_t *cube = cubes[i];

    if (grid[cube->x + 1][cube->y][cube->z]) {
      connectedSides++;
    }
    if (grid[cube->x - 1][cube->y][cube->z]) {
      connectedSides++;
    }

    if (grid[cube->x][cube->y + 1][cube->z]) {
      connectedSides++;
    }
    if (grid[cube->x][cube->y - 1][cube->z]) {
      connectedSides++;
    }

    if (grid[cube->x][cube->y][cube->z + 1]) {
      connectedSides++;
    }
    if (grid[cube->x][cube->y][cube->z - 1]) {
      connectedSides++;
    }
  }

  int unconnectedSides = totalSides - connectedSides;
  fprintf(stdout, "Part one: %d\n", unconnectedSides);

#ifdef TEST_MODE
  assert(unconnectedSides == 64);
#else
  assert(unconnectedSides == 3576);
#endif

  flood(0, 0, 0);

  int waterSides = 0;

  for (int i = 0; i < cubeIndex; i++) {
    cube_t *cube = cubes[i];

    if (grid[cube->x + 1][cube->y][cube->z] == WATER) {
      waterSides++;
    }
    if (cube->x == 0 || grid[cube->x - 1][cube->y][cube->z] == WATER) {
      waterSides++;
    }

    if (grid[cube->x][cube->y + 1][cube->z] == WATER) {
      waterSides++;
    }
    if (cube->y == 0 || grid[cube->x][cube->y - 1][cube->z] == WATER) {
      waterSides++;
    }

    if (grid[cube->x][cube->y][cube->z + 1] == WATER) {
      waterSides++;
    }
    if (cube->z == 0 || grid[cube->x][cube->y][cube->z - 1] == WATER) {
      waterSides++;
    }
  }

  fprintf(stdout, "Part two: %d\n", waterSides);
#ifdef TEST_MODE
  assert(waterSides == 58);
#else
  assert(waterSides == 2066);
#endif
}
