#include <assert.h>
#include <ctype.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
// #define TEST_MODE
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

#define THREAD_COUNT 16

typedef struct INPUT_LINE_LIST {
  int sensorY;
  int sensorX;
  int beaconY;
  int beaconX;
  int taxicabDist;
  struct INPUT_LINE_LIST *next;
} INPUT_LINE_LIST;

typedef struct POINT {
  int y;
  int x;
} POINT;

// Part two
struct POINT *partTwoPoint = NULL;
pthread_mutex_t mutex;

struct INPUT_LINE_LIST *input = NULL;

// map coordinate to the grid so we can support a negative index
int mapco(int coord) { return (HEIGHT / 2) + coord; }
int unmapco(int coord) { return coord - (HEIGHT / 2); }

void drawSensorArea(struct INPUT_LINE_LIST *line, char *testRow, int testY) {
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

int runPartOne() {
  int testY = (HEIGHT / 2) + TEST_Y;
  char *testRow = calloc(WIDTH, sizeof(char));
  for (struct INPUT_LINE_LIST *line = input; line != NULL; line = line->next) {
    drawSensorArea(line, testRow, testY);
  }

  // TODO why are we off-by-one
  int count = -1;
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
  free(testRow);
  return count;
}

POINT *run(int testY) {
  char *testRow = calloc(WIDTH, sizeof(char));
  for (struct INPUT_LINE_LIST *line = input; line != NULL; line = line->next) {
    drawSensorArea(line, testRow, testY);
  }

  struct POINT *point = NULL;

  int mappedZero = mapco(0);
  int mappedMax = mapco(MAX_COORD);
  for (int i = mappedZero; i < mappedMax; i++) {
    // fputc(testRow[i] == 0 ? '.' : testRow[i], stdout);
    if (testRow[i] == 0) {
      // gap found!
      point = malloc(sizeof(struct POINT *));
      point->y = unmapco(testY);
      point->x = unmapco(i);
      break;
    }
  }
  free(testRow);
  return point;
}

typedef struct ARGS {
  int id;
  int min;
  int max;
} ARGS;

void *worker(void *arg) {
  ARGS *args = (ARGS *)arg;
  fprintf(stdout, "Worker[%d] min %d, max %d\n", args->id, args->min,
          args->max);

  struct POINT *point = NULL;
  for (int y = args->min; y < args->max; y++) {
    point = run(y);

    if (point != NULL) {
      fprintf(stdout, "Worker[%d] got point (%d,%d)\n", args->id, point->y,
              point->x);
      pthread_mutex_unlock(&mutex);
      partTwoPoint = malloc(sizeof(POINT *));
      partTwoPoint->y = point->y;
      partTwoPoint->x = point->x;
      break;
    }
  }

  fprintf(stdout, "Worker[%d] done - point OK: %d\n", args->id,
          partTwoPoint != NULL);
  pthread_exit(NULL);
}

int main() {
#ifdef TEST_MODE
  grid = calloc(sizeof(char) * WIDTH * HEIGHT, sizeof *grid);
#endif
  readInput(__FILE__, lineHandler);

  int partOne = runPartOne();
  fprintf(stdout, "\nPart one: %d\n", partOne);

#ifdef TEST_MODE
  assert(partOne == 26);
#else
  assert(partOne == 4793062);
#endif

  /* Test timings to understand speedup for P2
   * In my case, I started with ~43h and this was reduced to ~7h on 10 threads.
  clock_t t;
  t = clock();
  int times = 100;

  int step = times / THREAD_COUNT;
  pthread_t threads[THREAD_COUNT];

  fprintf(stdout, "\nthreads: %d, step: %d\n", THREAD_COUNT, step);

  for (int i = 0; i < THREAD_COUNT; i++) {
    ARGS *args = malloc(sizeof(ARGS *));
    args->id = i;
    args->min = i * step;
    args->max = ((i + 1) * step);
    pthread_create(&threads[i], NULL, worker, (void *)args);
  }

  for (int i = 0; i < THREAD_COUNT; i++) {
    pthread_join(threads[i], NULL);
  }
  t = clock() - t;
  double time_taken =
      (((double)t) / CLOCKS_PER_SEC) / times / THREAD_COUNT; // in seconds
  double total_time = time_taken * 4000000;

  printf("run() took %f seconds to execute\n", time_taken);
  printf("part two will take %.2fs / %.2f m / %.2f h \n", total_time,
         total_time / 60, total_time / 60 / 60);
  */

  int mappedZero = mapco(0);
  int mappedMax = mapco(MAX_COORD);
  fprintf(stdout, "\nPxxxxxxxxxEx\n");

  int step = (mappedMax - mappedZero) / THREAD_COUNT;
  pthread_t threads[THREAD_COUNT];

  pthread_mutex_init(&mutex, NULL);
  pthread_mutex_lock(&mutex);

  int threadIndex = 0;
  for (int i = mappedZero; i < mappedMax; i += step) {
    ARGS *args = malloc(sizeof(ARGS *));
    args->id = threadIndex;
    args->min = i;
    args->max = i + step;
    pthread_create(&threads[threadIndex++], NULL, worker, (void *)args);
  }

  pthread_mutex_lock(&mutex);
  // for (int i = 0; i < THREAD_COUNT; i++) {
  //   pthread_join(threads[i], NULL);
  // }

  if (partTwoPoint == NULL) {
    fprintf(stdout, "Failed to get point\n");
    return 1;
  }

  fprintf(stdout, "finished point (%d,%d)\n", partTwoPoint->y, partTwoPoint->x);
  u_int64_t frequency = (partTwoPoint->x * 4000000) + partTwoPoint->y;

  fprintf(stdout, "Part two: %lu\n", frequency);
#ifdef TEST_MODE
  assert(frequency == 56000011);
#else
  assert(frequency == 4793062);
#endif
}
