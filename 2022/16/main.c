// NOTE
// You will need to amend the input to change any singular `valve XX` line to
// be `valves XX`
#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#define TEST_MODE
#include "../../utils.h"

#define MAX_TIME 30
#define MAX_ITERATIONS 10000000
// #define RANDOM(x) (rand() / ((RAND_MAX + 1u) / x))
#define RANDOM(min, max) ((rand() % (max - min + 1)) + min)

enum state { CLOSED = 0, OPEN = 1 };

typedef struct valve_t {
  char *id;
  int flowRate;
  int state;
  int minuteOpened;
  char *rawValves;
  int linkCount;
  struct valve_t **links;
} valve_t;

valve_t **allValves = NULL;
int valveCount = 0;
valve_t *aaValve = NULL;

void fileHandler(int lines) { allValves = malloc(sizeof(valve_t *) * lines); }

void lineHandler(char *line) {
  fprintf(stdout, "line: %s\n", line);

  char id[3];
  int flowRate = 0;
  char *valvesStr = strrchr(line, 's') + 2;
  sscanf(line, "Valve %s has flow rate=%d", id, &flowRate);

  valve_t *valve = malloc(sizeof(valve_t));
  valve->id = calloc(3, sizeof(char));
  strcpy(valve->id, id);
  valve->flowRate = flowRate;
  valve->linkCount = 0;
  valve->state = CLOSED;
  valve->minuteOpened = 0;
  valve->rawValves = calloc(20, sizeof(char));
  strcpy(valve->rawValves, valvesStr);

  fprintf(stdout, "valve %s -> rate %d -> valves '%s'\n", valve->id,
          valve->flowRate, valve->rawValves);
  allValves[valveCount++] = valve;

  fputs("\n", stdout);
}

void connectValves() {
  for (int i = 0; i < valveCount; i++) {
    valve_t *valve = allValves[i];
    fprintf(stdout, "valve %s -> rate %d -> valves '%s'\n", valve->id,
            valve->flowRate, valve->rawValves);

    int linkCount = 1;

    for (char *c = valve->rawValves; *c; c++) {
      if (*c == ',') {
        linkCount++;
      }
    }

    valve->links = malloc(sizeof(valve_t *) * linkCount);
    char *valveEnd;
    char *valveToken = strtok_r(valve->rawValves, ", ", &valveEnd);
    do {
      // fprintf(stdout, "tok: %s\n", valveToken);
      for (int j = 0; j < valveCount; j++) {
        if (strcmp(allValves[j]->id, valveToken) == 0) {
          valve_t *connectedValve = allValves[j];
          // fprintf(stdout, "connected: %s\n", connectedValve->id);
          valve->links[valve->linkCount++] = connectedValve;
          break;
        }
      }
    } while ((valveToken = strtok_r(NULL, ", ", &valveEnd)) != NULL);
  }

  for (int i = 0; i < valveCount; i++) {
    if (strcmp(allValves[i]->id, "AA") == 0) {
      aaValve = allValves[i];
    }
  }
}

int doTheValveOpeningThing() {
  int pressure = 0;
  int maxTime = MAX_TIME;
  valve_t *currentValve = aaValve;
  for (int minute = 1; minute <= maxTime; minute++) {
    // can either move or open a valve

    valve_t *nextValve =
        currentValve->links[RANDOM(0, currentValve->linkCount - 1)];

    // if current valve flow rate is 0 or we are already open, move
    if (currentValve->flowRate == 0 || currentValve->state == OPEN) {
      currentValve = nextValve;
      continue;
    }
    // if current valve flow rate ISNT 0 and ISNT open, open it
    // TODO only decide to skip if next valve is more than double current and
    // is closed or something
    // bool shouldOpen = nextValve->state == CLOSED && nextValve->flowRate != 0 &&
                      // RANDOM(0, 4) > 3;
    // bool shouldOpen = nextValve->state == CLOSED || nextValve->flowRate == 0 ||
                      // RANDOM(0, 4) > 0;
    if (currentValve->state == CLOSED && RANDOM(0, 4) > 0) {
      currentValve->state = OPEN;
      currentValve->minuteOpened = minute;
    } else {
      currentValve = nextValve;
    }
  }

  // at the end, sum the pressure released since the minute the valve opened and
  // add to total
  for (int i = 0; i < valveCount; i++) {
    valve_t *valve = allValves[i];
    if (valve->minuteOpened) {
      pressure += valve->flowRate * (maxTime - valve->minuteOpened);
    }
    // reset valves after counting for next run
    valve->state = CLOSED;
    valve->minuteOpened = 0;
  }
  return pressure;
}

int doTheValveOpeningThingButWithAnElephant() {
  int pressure = 0;
  int maxTime = MAX_TIME - 4;
  valve_t *currentValve = aaValve;
  valve_t *elephantValve = aaValve;
  for (int minute = 1; minute <= maxTime; minute++) {
    char openedId[3];
    char openedIdElephant[3];
    bool hasOpened = false;
    bool hasOpenedElephant = false;
    // can either move or open a valve
    // if current valve flow rate is 0, move
    if (currentValve->flowRate == 0) {
      currentValve =
          currentValve->links[RANDOM(0, currentValve->linkCount - 1)];
      // fprintf(stdout, "moved to valve %s\n", currentValve->id);
    } else {
      // if current valve flow rate ISNT 0 and ISNT open, open it
      if (currentValve->state == CLOSED && RANDOM(0, 4) > 0) {
        currentValve->state = OPEN;
        // currentValve-> minuteOpened
        // fprintf(stdout, "opened valve %s\n", currentValve->id);
        hasOpened = true;
        strcpy(openedId, currentValve->id);
      } else {
        // otherwise, move
        currentValve =
            currentValve->links[RANDOM(0, currentValve->linkCount - 1)];
        // fprintf(stdout, "moved to valve %s\n", currentValve->id);
      }
    }

    // elephant
    if (elephantValve->flowRate == 0) {
      elephantValve =
          elephantValve->links[RANDOM(0, elephantValve->linkCount - 1)];
    } else {
      if (elephantValve->state == CLOSED && RANDOM(0, 4) > 0) {
        elephantValve->state = OPEN;
        hasOpenedElephant = true;
        strcpy(openedIdElephant, elephantValve->id);
      } else {
        elephantValve =
            elephantValve->links[RANDOM(0, elephantValve->linkCount - 1)];
      }
    }

    // at the end, count the pressure released in the minute and add to total
    int pressureReleased = 0;
    // fprintf(stdout, "open valves: ");
    for (int i = 0; i < valveCount; i++) {
      if (allValves[i]->state == OPEN) {
        // fprintf(stdout, "%s, ", allValves[i]->id);
        if (hasOpened || hasOpenedElephant) {
          if (strcmp(allValves[i]->id, openedId) != 0 &&
              strcmp(allValves[i]->id, openedIdElephant) != 0) {
            pressureReleased += allValves[i]->flowRate;
          }
        } else {
          pressureReleased += allValves[i]->flowRate;
        }
      }
    }
    // fprintf(stdout, "\n");
    pressure += pressureReleased;
  }
  // reset valves after
  for (int i = 0; i < valveCount; i++) {
    allValves[i]->state = CLOSED;
  }
  return pressure;
}

int main() {
  readInputFile(__FILE__, lineHandler, fileHandler);
  srand(time(NULL));
  connectValves();

  clock_t t;
  t = clock();

  int maxResult = 0;
  int i = 1;
  for (; i < MAX_ITERATIONS; i++) {
    // while (i++) {
    maxResult = max(doTheValveOpeningThing(), maxResult);
#ifdef TEST_MODE
    // if (maxResult == 1651)
    // break;
#else
    // if (maxResult == 1896)
    // break;
#endif
  }
  t = clock() - t;
  double time_taken =
      (((double)t) / CLOCKS_PER_SEC) / MAX_ITERATIONS; // in seconds
  double total_time = time_taken * MAX_ITERATIONS;

  printf("doTheValveOpeningThing() took %fs / %fms / %fus to execute\n",
         time_taken, time_taken * 1000, time_taken * 1000 * 1000);
  printf("total time was %.2fs / %.2f m / %.2f h \n", total_time,
         total_time / 60, total_time / 60 / 60);

  fprintf(stdout, "i: %d\n", i);
  fprintf(stdout, "Part one: %d\n", maxResult);

#ifdef TEST_MODE
  // I could never get it working for the example :c
  assert(maxResult == 1651);
#else
  // assert(maxResult == 1896);
#endif

  // TODO gunna need to do something smart
  // https://en.wikipedia.org/wiki/Floyd%E2%80%93Warshall_algorithm
  int maxResultWithAnElephant = 0; // 2409;
  // for (int i = 0; i < MAX_ITERATIONS*2; i++) {
  while (1) {
    int result = doTheValveOpeningThingButWithAnElephant();
    if (result > maxResultWithAnElephant) {
      fprintf(stdout, "new max: %d\n", result);
      maxResultWithAnElephant = result;
    }
    // if (maxResultWithAnElephant == 1896)
    break;
  }

  fprintf(stdout, "Part two: %d\n", maxResultWithAnElephant);
#ifdef TEST_MODE
  assert(maxResultWithAnElephant == 1707);
#else
  assert(maxResultWithAnElephant > 2409);
#endif
}
