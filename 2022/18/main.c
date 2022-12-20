#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
// #define TEST_MODE
#include "../../utils.h"

#define MAX_CUBES 2030
#define MAX_X 100
#define MAX_Y 100
#define MAX_Z 100
char grid[MAX_X][MAX_Y][MAX_Z] = {0};
char visited[MAX_X][MAX_Y][MAX_Z] = {0};

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

  grid[x][y][z] = 1;
  cube_t *cube = malloc(sizeof(cube_t));
  cube->x = x;
  cube->y = y;
  cube->z = z;
  cubes[cubeIndex++] = cube;

  fprintf(stdout, "cubes[%d] (%d,%d,%d)\n", cubeIndex - 1, cube->x, cube->y,
          cube->z);
  fputs("\n", stdout);
}

void seek(int x, int y, int z) {}

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

  fprintf(stdout, "Part two: %d\n", 420);
#ifdef TEST_MODE
  assert(420 == 69);
#else
  assert(420 == 69);
#endif
}
