#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#define TEST_MODE
#include "../../utils.h"

#ifdef TEST_MODE
#define HEIGHT 60
#define WIDTH 60
#define TEST_Y 10
#define MAX_COORD 20
char (*grid)[WIDTH];
#else
#define HEIGHT 10000000ll
#define WIDTH 10000000ll
#define TEST_Y 2000000
#define MAX_COORD 4000000
#endif

char testRow[WIDTH] = {0};

typedef struct INPUT_LINE_LIST {
  int sensorY;
  int sensorX;
  int beaconY;
  int beaconX;
  struct INPUT_LINE_LIST *next;
} INPUT_LINE_LIST;

struct INPUT_LINE_LIST input = {};

// map coordinate to the grid so we can support a negative index
int mapco(int coord) { return (HEIGHT / 2) + coord; }
int unmapco(int coord) { return coord - (HEIGHT / 2); }

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
#ifdef TEST_MODE
        if (grid[sensorY + y][x] == 0) {
          grid[sensorY + y][x] = '#';
        }
#endif
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
#ifdef TEST_MODE
        if (grid[sensorY - y][x] == 0) {
          grid[sensorY - y][x] = '#';
        }
#endif
      }
    }
  }
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
  // fputs("line: ", stdout);
  // fputs(line, stdout);

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

  // int taxicabDist = abs(sensorY - beaconY) + abs(sensorX - beaconX);
  // fprintf(stdout, "sensor: (%d, %d), beacon (%d, %d) taxicabDist %d\n",
  // sensorY, sensorX, beaconY, beaconX, taxicabDist);

#ifdef TEST_MODE
  grid[mapco(sensorY)][mapco(sensorX)] = 'S';
  grid[mapco(beaconY)][mapco(beaconX)] = 'B';
#endif

  drawSensorArea(mapco(sensorY), mapco(sensorX), mapco(beaconY),
                 mapco(beaconX));
  // fputs("\n", stdout);
}

#ifdef TEST_MODE
void printGrid() {
  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {
      char out = grid[y][x];
      fputc(out == 0 ? '.' : out, stdout);
    }
    fputc('\n', stdout);
  }
}
#endif

// void run () {
  // iterate the input lines and then draw drawSensorAreas
// }

int main() {
#ifdef TEST_MODE
  grid = calloc(sizeof(char) * WIDTH * HEIGHT, sizeof *grid);
#endif
  readInput(__FILE__, lineHandler);

  // run();

  // TODO why are we off-by-one
  int count = -1;

#ifdef TEST_MODE
  grid[testY][0] = '#';
  grid[testY][WIDTH - 1] = '#';
  printGrid();
  fprintf(stdout, "y: \"");
#endif
  for (int i = 0; i < WIDTH; i++) {
#ifdef TEST_MODE
    fputc(testRow[i] == 0 ? '.' : testRow[i], stdout);
#endif
    if (testRow[i] != 0) {
      count++;
    }
  }
#ifdef TEST_MODE
  fputs("\"\n", stdout);
#endif

  int beaconY = 0;
  int beaconX = 0;
  for (int y = mapco(0); y < mapco(MAX_COORD); y++) {
    testY = y;
    memset(testRow, 0, sizeof testRow);
    readInput(__FILE__, lineHandler);

    for (int i = mapco(0); i < mapco(MAX_COORD); i++) {
#ifdef TEST_MODE
      fputc(testRow[i] == 0 ? '.' : testRow[i], stdout);
#endif
      if (testRow[i] == 0) {
        // gap found!
        beaconY = unmapco(y);
        beaconX = unmapco(i);
        goto end;
      }
    }
#ifdef TEST_MODE
    fputs("\"\n", stdout);
#endif
  }
end:
  u_int64_t frequency = (beaconX * 4000000) + beaconY;

  fprintf(stdout, "\nPart one: %d\n", count);
  fprintf(stdout, "Part two: %llu\n", frequency);
}
