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
// #define TEST_MODE
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
  char *rawValves;
  int linkCount;
  struct valve_t *links[10];
} valve_t;

valve_t *allValves[100] = {0};
int valveCount = 0;
valve_t *aaValve = NULL;

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
  valve_t *currentValve = aaValve;
  for (int minute = 1; minute <= MAX_TIME; minute++) {
    char openedId[3];
    bool hasOpened = false;
    // fprintf(stdout, "minute %d\n", minute);
    // can either move or open a valve
    // if current valve flow rate is 0, move
    if (currentValve->flowRate == 0) {
      currentValve =
          currentValve->links[RANDOM(0, currentValve->linkCount - 1)];
      // fprintf(stdout, "moved to valve %s\n", currentValve->id);
    } else {
      // if current valve flow rate ISNT 0 and ISNT open, open it
      if (currentValve->state == CLOSED) {
        currentValve->state = OPEN;
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

    // at the end, count the pressure released in the minute and add to total
    int pressureReleased = 0;
    // fprintf(stdout, "open valves: ");
    for (int i = 0; i < valveCount; i++) {
      if (allValves[i]->state == OPEN) {
        // fprintf(stdout, "%s, ", allValves[i]->id);
        if (hasOpened) {
          if (strcmp(allValves[i]->id, openedId) != 0) {
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

int doTheValveOpeningThingButWithAnElephant() {
  int pressure = 0;
  valve_t *currentValve = aaValve;
  valve_t *elephantValve = aaValve;
  for (int minute = 1; minute <= MAX_TIME - 4; minute++) {
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
      if (currentValve->state == CLOSED) {
        currentValve->state = OPEN;
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
      if (elephantValve->state == CLOSED) {
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
  readInput(__FILE__, lineHandler);
  srand(time(NULL));
  connectValves();

  int maxResult = 0;
  for (int i = 0; i < MAX_ITERATIONS; i++) {
    // while (1) {
    // maxResult = max(doTheValveOpeningThing(), maxResult);
    if (maxResult == 1896)
      break;
  }

  fprintf(stdout, "Part one: %d\n", maxResult);

#ifdef TEST_MODE
  // I could never get it working for the example :c
  assert(maxResult == 1);
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
    // break;
  }

  fprintf(stdout, "Part two: %d\n", maxResultWithAnElephant);
#ifdef TEST_MODE
  assert(maxResultWithAnElephant == 1707);
#else
  assert(maxResultWithAnElephant > 2409);
#endif
}
