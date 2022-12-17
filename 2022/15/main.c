#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#define TEST_MODE
#include <time.h>
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
  int taxicabDist;
  struct INPUT_LINE_LIST *next;
} INPUT_LINE_LIST;

struct INPUT_LINE_LIST *input = NULL;

// map coordinate to the grid so we can support a negative index
int mapco(int coord) { return (HEIGHT / 2) + coord; }
int unmapco(int coord) { return coord - (HEIGHT / 2); }

void drawSensorArea(struct INPUT_LINE_LIST *line, int testY) {
  if (line->sensorY + line->taxicabDist > testY) {
    for (int y = line->taxicabDist; y >= 0; y--) {
      int minX = line->sensorX - line->taxicabDist + y;
      int maxX = line->sensorX + line->taxicabDist - y;
      if (line->sensorY + y != testY) {
        continue;
      }
      for (int x = minX; x <= maxX; x++) {
        testRow[x] = '#';
#ifdef TEST_MODE
        if (grid[line->sensorY + y][x] == 0) {
          grid[line->sensorY + y][x] = '#';
        }
#endif
      }
    }
  }

  if (line->sensorY - line->taxicabDist < testY) {
    for (int y = line->taxicabDist; y >= 0; y--) {
      int minX = line->sensorX - line->taxicabDist + y;
      int maxX = line->sensorX + line->taxicabDist - y;
      if (line->sensorY - y != testY) {
        continue;
      }
      for (int x = minX; x <= maxX; x++) {
        testRow[x] = '#';
#ifdef TEST_MODE
        if (grid[line->sensorY - y][x] == 0) {
          grid[line->sensorY - y][x] = '#';
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

  struct INPUT_LINE_LIST *lineItem = malloc(sizeof(struct INPUT_LINE_LIST));

  lineItem->sensorY = mapco(sensorY);
  lineItem->sensorX = mapco(sensorX);
  lineItem->beaconY = mapco(beaconY);
  lineItem->beaconX = mapco(beaconX);
  lineItem->taxicabDist = abs(lineItem->sensorY - lineItem->beaconY) +
                          abs(lineItem->sensorX - lineItem->beaconX);
  lineItem->next = input;

  input = lineItem;
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

void run(int testY) {
  for (struct INPUT_LINE_LIST *line = input; line != NULL; line = line->next) {
    drawSensorArea(line, testY);
  }
}

int main() {
#ifdef TEST_MODE
  grid = calloc(sizeof(char) * WIDTH * HEIGHT, sizeof *grid);
#endif
  readInput(__FILE__, lineHandler);
  // TODO why are we off-by-one
  int count = -1;

  int testY = (HEIGHT / 2) + TEST_Y;

  run(testY);

#ifdef TEST_MODE
  grid[testY][0] = '#';
  grid[testY][WIDTH - 1] = '#';
  // printGrid();
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
  int mappedZero = mapco(0);
  int mappedMax = mapco(MAX_COORD);
  fprintf(stdout, "\nPxxxxxxxxxEx\n");
  for (int y = mappedZero; y < mappedMax; y++) {
    memset(testRow, 0, sizeof testRow);
    run(y);

    // testRow[mappedMax] = '\0';
    // fputs(testRow, stdout);
    // char *gap = strchr(testRow + mappedZero, 0);
    // fprintf(stdout, "gap: %c\n", *gap);
    // int dist = ((char *)gap) - ((char *)testRow);
    // if (gap != NULL && dist <= mappedMax) {
    // gap found!
    // beaconY = unmapco(y);
    // beaconX = unmapco(dist);
    // fprintf(stdout, "gap: %c, dist %d, x %d\n", *gap, dist, beaconX);
    // goto end;
    // break;
    // }
    for (int i = mappedZero; i < mappedMax; i++) {
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
    fputs("\n", stdout);
#endif
  }
end:
  u_int64_t frequency = (beaconX * 4000000) + beaconY;

  fprintf(stdout, "\nPxxxxxxxxxx\n");
  clock_t t;
  t = clock();
  int times = 100;
  for (int i = 0; i < times; i++) {
    run(i);
  }
  t = clock() - t;
  double time_taken = (((double)t) / CLOCKS_PER_SEC) / times; // in seconds
  double total_time = time_taken * 4000000;

  printf("run() took %f seconds to execute\n", time_taken);
  printf("part two will take %.2fs / %.2f m / %.2f h \n", total_time,
         total_time / 60, total_time / 60 / 60);

  fprintf(stdout, "\nPart one: %d\n", count);
  fprintf(stdout, "Part two: %lu\n", frequency);
}
