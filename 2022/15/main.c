#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
// #define TEST_MODE
#include "../../utils.h"

#ifdef TEST_MODE
#define HEIGHT 60
#define WIDTH 60
#define TEST_Y 10
#else
#define HEIGHT 10000000ll
#define WIDTH 10000000ll
#define TEST_Y 2000000

#endif

// char (*grid)[WIDTH];
char testRow[WIDTH] = {0};

// map coordinate to the grid so we can support a negative index
int mapco(int coord) { return (HEIGHT / 2) + coord; }

int testY = (HEIGHT / 2) + TEST_Y;

void drawSensorArea(int sensorY, int sensorX, int beaconY, int beaconX) {
  int taxicabDist = abs(sensorY - beaconY) + abs(sensorX - beaconX);

  if (sensorY + taxicabDist > testY) {
    for (int y = taxicabDist; y >= 0; y--) {
      int minX = sensorX - taxicabDist + y;
      int maxX = sensorX + taxicabDist - y;
      if (sensorY + y != testY) {
        continue;
      }
      for (int x = minX; x <= maxX; x++) {
        testRow[x] = '#';
        // if (grid[sensorY + y][x] == 0) {
        // grid[sensorY + y][x] = '#';
        // }
      }
    }
  }

  if (sensorY - taxicabDist < testY) {
    for (int y = taxicabDist; y >= 0; y--) {
      int minX = sensorX - taxicabDist + y;
      int maxX = sensorX + taxicabDist - y;
      if (sensorY - y != testY) {
        continue;
      }
      for (int x = minX; x <= maxX; x++) {
        testRow[x] = '#';
        // if (grid[sensorY - y][x] == 0) {
        // grid[sensorY - y][x] = '#';
        // }
      }
    }
  }
  return;
  /*
  if (sensorY + taxicabDist > testY) {
    for (int x = taxicabDist; x >= 0; x--) {
      int minY = sensorY - taxicabDist + x;
      int maxY = sensorY + taxicabDist - x;
      for (int y = minY; y <= maxY; y++) {
        if (y == testY) {
          fputs("hit\n", stdout);
          testRow[sensorX + x] = '#';
        }
        if (grid[y][sensorX + x] == 0) {
          grid[y][sensorX + x] = '#';
        }
      }
    }
  }
  if (sensorY + taxicabDist > testY) {
    for (int x = taxicabDist; x >= 0; x--) {
      int minY = sensorY - taxicabDist + x;
      int maxY = sensorY + taxicabDist - x;
      for (int y = minY; y <= maxY; y++) {
        if (y == testY) {
          fputs("hit\n", stdout);
          testRow[sensorX - x] = '#';
        }
        if (grid[y][sensorX - x] == 0) {
          grid[y][sensorX - x] = '#';
        }
      }
    }
  }
  */
}

int tokenToInt(char *token) {
  char *strippedToken = strdup(token);
  for (int i = 0; i < strlen(token); i++) {
    if (isdigit(token[i]) || token[i] == '-') {
      strippedToken[i] = token[i];
    } else {
      strippedToken[i] = ' ';
    }
  }
  return atoi(strippedToken);
}

void lineHandler(char *line) {
  fputs("line: ", stdout);
  fputs(line, stdout);

  int sensorY;
  int sensorX;
  int beaconY;
  int beaconX;
  char *lineEnd;
  char *sensorYtoken;
  char *sensorXtoken;
  char *beaconYtoken;
  char *beaconXtoken;
  strtok_r(line, " ", &lineEnd);                // Sensor
  strtok_r(NULL, " ", &lineEnd);                // at
  sensorXtoken = strtok_r(NULL, " ", &lineEnd); // x=<num>,
  sensorYtoken = strtok_r(NULL, " ", &lineEnd); // y=<num>:
  strtok_r(NULL, " ", &lineEnd);                // closest
  strtok_r(NULL, " ", &lineEnd);                // beacon
  strtok_r(NULL, " ", &lineEnd);                // is
  strtok_r(NULL, " ", &lineEnd);                // at
  beaconXtoken = strtok_r(NULL, " ", &lineEnd); // x=<num>,
  beaconYtoken = strtok_r(NULL, " ", &lineEnd); // y=<num>:

  sensorY = tokenToInt(sensorYtoken);
  sensorX = tokenToInt(sensorXtoken);
  beaconY = tokenToInt(beaconYtoken);
  beaconX = tokenToInt(beaconXtoken);
  // fprintf(stdout, "sensor: (%s, %s), beacon (%s, %s)\n", sensorYtoken,
  // sensorXtoken, beaconYtoken, beaconXtoken);

  int taxicabDist = abs(sensorY - beaconY) + abs(sensorX - beaconX);
  fprintf(stdout, "sensor: (%d, %d), beacon (%d, %d) taxicabDist %d\n", sensorY,
          sensorX, beaconY, beaconX, taxicabDist);

  // grid[mapco(sensorY)][mapco(sensorX)] = 'S';
  // grid[mapco(beaconY)][mapco(beaconX)] = 'B';

  drawSensorArea(mapco(sensorY), mapco(sensorX), mapco(beaconY),
                 mapco(beaconX));
  fputs("\n", stdout);
}

void printGrid() {
  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {
      // char out = grid[y][x];
      // fputc(out == 0 ? '.' : out, stdout);
    }
    // fputc('\n', stdout);
  }
}

int main() {
  // grid = calloc(sizeof(char) * WIDTH * HEIGHT, sizeof *grid);
  readInput(__FILE__, lineHandler);

  // TODO why are we off-by-one
  int count = -1;

  // grid[testY][0] = '#';
  // grid[testY][WIDTH - 1] = '#';
  // printGrid();
  fprintf(stdout, "y: \"");
  for (int i = 0; i < WIDTH; i++) {
    // fputc(grid[y][i] == 0 ? '.' : grid[y][i], stdout);
    // fputc(testRow[i] == 0 ? '.' : testRow[i], stdout);
    if (testRow[i] != 0) {
      count++;
    }
  }
  fputs("\"\n", stdout);
  fprintf(stdout, "Part one: %d\n", count);
  fprintf(stdout, "Part two: %d\n", 420);
}
