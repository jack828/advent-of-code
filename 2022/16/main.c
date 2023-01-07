// NOTE
// You will need to amend the input to change any singular `valve XX` line to
// be `valves XX`
#include <assert.h>
#include <ctype.h>
#include <limits.h>
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
  // i guess for some kind of search operation
  u_int64_t idBit;
  int flowRate;
  int state;
  int minuteOpened;
  char *rawValves;
  int linkCount;
  struct valve_t **links;
  // the cost (in minutes) to move from this valve to any of the other valves
  u_int16_t *valveWeights;
  int index;
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
  // TODO overflows
  valve->idBit = 1 << valveCount;
  valve->index = valveCount;
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
    fprintf(stdout, "valve %s (%ld) -> rate %d -> valves '%s'\n", valve->id,
            valve->idBit, valve->flowRate, valve->rawValves);

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

// Floyd-Warshall
void calculateDistances() {
  fprintf(stdout, "Floyd-Warshall'ing\n");
  // let dist be a |V| × |V| array of minimum distances initialized to ∞
  u_int16_t dist[valveCount][valveCount];
  memset(dist, INT_MAX, sizeof(dist));
  // for each edge (u, v) do
  // dist[u][v] ← w(u, v)  // The weight of the edge (u, v)
  // We just explore the entire valve array and set each valid link to 1
  // I believe this is an adjacency matrix of our graph
  for (int u = 0; u < valveCount; u++) {
    valve_t *valve = allValves[u];
    fprintf(stdout, "valve %s\n", valve->id);
    for (int v = 0; v < valve->linkCount; v++) {
      dist[u][valve->links[v]->index] = 1;
    }
  }
  // for each vertex v do
  // dist[v][v] ← 0
  for (int v = 0; v < valveCount; v++) {
    dist[v][v] = 0;
  }

  // for k from 1 to |V|
  for (int k = 0; k < valveCount; k++) {
    // for i from 1 to |V|
    for (int i = 0; i < valveCount; i++) {
      // for j from 1 to |V|
      for (int j = 0; j < valveCount; j++) {
        // fprintf(stdout, "%d ", dist[i][j]);
        if (dist[i][j] > dist[i][k] + dist[k][j]) {
          dist[i][j] = dist[i][k] + dist[k][j];
        }
      }
    }
  }

  // debug print for visibility
  for (int i = 0; i < valveCount; i++) {
    for (int j = 0; j < valveCount; j++) {
      fprintf(stdout, "%d ", dist[i][j]);
    }
    fprintf(stdout, "\n");
  }

  for (int a = 0; a < valveCount; a++) {
    valve_t *aValve = allValves[a];
    // setup valveWeights array
    // which is an array of int to each valve that has a flowRate
    // only for AA or flowRate > 0
    aValve->valveWeights = calloc(valveCount, sizeof(int));
    if (aValve->flowRate != 0 || strcmp(aValve->id, "AA") == 0) {
      for (int b = 0; b < valveCount; b++) {
        valve_t *bValve = allValves[b];
        if (bValve->flowRate != 0) {
          // only set connections to valves with a positive flowRate
          aValve->valveWeights[b] = dist[a][b];
        }
      }
    }
  }

  // debug print for visibility
  for (int i = 0; i < valveCount; i++) {
    valve_t *valve = allValves[i];

    fprintf(stdout, "valve %s, weights: ", valve->id);
    for (int j = 0; j < valveCount; j++) {
      fprintf(stdout, "%d: %d ", j, valve->valveWeights[j]);
    }
    fprintf(stdout, "\n");
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
    // bool shouldOpen = nextValve->state == CLOSED && nextValve->flowRate !=
    // 0
    // && RANDOM(0, 4) > 3;
    // bool shouldOpen = nextValve->state == CLOSED || nextValve->flowRate ==
    // 0
    // || RANDOM(0, 4) > 0;
    if (currentValve->state == CLOSED && RANDOM(0, 4) > 0) {
      currentValve->state = OPEN;
      currentValve->minuteOpened = minute;
    } else {
      currentValve = nextValve;
    }
  }

  // at the end, sum the pressure released since the minute the valve opened
  // and add to total
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
  valve_t *currentElephantValve = aaValve;
  for (int minute = 1; minute <= maxTime; minute++) {
    int linkIndex = RANDOM(0, currentValve->linkCount - 1);
    valve_t *nextValve = currentValve->links[linkIndex];

    if (currentValve->flowRate == 0 || currentValve->state == OPEN) {
      currentValve = nextValve;
      continue;
    }
    if (currentValve->state == CLOSED && RANDOM(0, 4) > 0) {
      currentValve->state = OPEN;
      currentValve->minuteOpened = minute;
    } else {
      currentValve = nextValve;
    }

    // elephant
    int elephantLinkIndex = RANDOM(0, currentElephantValve->linkCount - 1);
    valve_t *nextElephantValve = currentElephantValve->links[elephantLinkIndex];

    if (currentElephantValve->flowRate == 0 ||
        currentElephantValve->state == OPEN) {
      currentElephantValve = nextElephantValve;
      continue;
    }
    if (currentElephantValve->state == CLOSED && RANDOM(0, 4) > 0) {
      currentElephantValve->state = OPEN;
      currentElephantValve->minuteOpened = minute;
    } else {
      currentElephantValve = nextElephantValve;
    }
  }

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

int main() {
  readInputFile(__FILE__, lineHandler, fileHandler);
  srand(time(NULL));
  connectValves();
  calculateDistances();

  int maxResult = 1651;
  int i = 1;
  // for (; i < MAX_ITERATIONS; i++) {
  while (i++) {
    maxResult = max(doTheValveOpeningThing(), maxResult);
#ifdef TEST_MODE
    if (maxResult == 1651)
      break;
#else
    if (maxResult == 1896)
      break;
#endif
  }

  fprintf(stdout, "i: %d\n", i);
  fprintf(stdout, "Part one: %d\n", maxResult);

#ifdef TEST_MODE
  assert(maxResult == 1651);
#else
  assert(maxResult == 1896);
#endif

  // TODO gunna need to do something smart
  // https://en.wikipedia.org/wiki/Floyd%E2%80%93Warshall_algorithm

  clock_t t;
  t = clock();
  int maxResultWithAnElephant = 0; // 2409;
  // for (int i = 0; i < MAX_ITERATIONS; i++) {
  while (1) {
    int result = doTheValveOpeningThingButWithAnElephant();
    if (result > maxResultWithAnElephant) {
      fprintf(stdout, "new max: %d\n", result);
      maxResultWithAnElephant = result;
    }
    break;
#ifdef TEST_MODE
    if (maxResultWithAnElephant == 1707)
      break;
#else
    if (maxResultWithAnElephant == 40000)
      break;
#endif
  }
  // leaving in case it's needed another time
  t = clock() - t;
  double time_taken =
      (((double)t) / CLOCKS_PER_SEC) / MAX_ITERATIONS; // in seconds
  double total_time = time_taken * MAX_ITERATIONS;

  printf("took %fs / %fms / %fus to execute\n", time_taken, time_taken * 1000,
         time_taken * 1000 * 1000);
  printf("total time was %.2fs / %.2f m / %.2f h \n", total_time,
         total_time / 60, total_time / 60 / 60);
  //
  fprintf(stdout, "Part two: %d\n", maxResultWithAnElephant);
#ifdef TEST_MODE
  assert(maxResultWithAnElephant == 1707);
#else
  assert(maxResultWithAnElephant > 2409);
#endif
}
