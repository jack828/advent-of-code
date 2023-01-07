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
#include "../../lib/queue.h"
#include "../../utils.h"

#define MAX_TIME 30
#define MAX_ITERATIONS 10000000
#define RANDOM(min, max) ((rand() % (max - min + 1)) + min)

enum state { CLOSED = 0, OPEN = 1 };

typedef struct valve_t {
  char *id;
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
u_int64_t idBit = 1;
u_int64_t positiveValveIdBits = 0;

typedef struct graph_t {
  u_int64_t ids;
  int currentIndex;
  int minute;
  int pressure;
} graph_t;

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
  valve->idBit = 0;
  valve->index = valveCount;
  valve->flowRate = flowRate;
  valve->linkCount = 0;
  valve->state = CLOSED;
  valve->minuteOpened = 0;
  valve->rawValves = calloc(20, sizeof(char));
  strcpy(valve->rawValves, valvesStr);

  if (valve->flowRate > 0) {
    valve->idBit = idBit;
    positiveValveIdBits |= idBit;
    idBit <<= 1;
  }

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

// DFS (or maybe BFS, i have no idea) search of all possible ways to visit each
// positive flowRate valve in the allowed time
int exploreValves() {
  int maxPressure = -1;

  int maxTime = MAX_TIME;
  queue_t *queue = q_create();
  graph_t *graphStart = malloc(sizeof(graph_t));
  graphStart->ids = aaValve->idBit;
  graphStart->currentIndex = aaValve->index;
  graphStart->minute = 1;
  graphStart->pressure = 0;
  q_enqueue(queue, graphStart);

  queue_t *endQueue = q_create();

  while (!q_empty(queue)) {
    graph_t *graph = q_dequeue(queue);
    // fprintf(stdout, "graph %lu, minute %d, pressure %d\n", graph->ids,
    // graph->minute, graph->pressure);
    // break if all are visited or if we have reached the time limit
    if (graph->ids == positiveValveIdBits || graph->minute == maxTime) {
      // fprintf(stdout, "end graph %lu, minute %d, pressure %d\n", graph->ids,
      // graph->minute, graph->pressure);
      q_enqueue(endQueue, graph);
      continue;
    }

    bool canMove = false;
    for (int i = 0; i < valveCount; i++) {
      valve_t *valve = allValves[i];
      int valveMoveTime = allValves[graph->currentIndex]->valveWeights[i];

      // for every positive valve
      // that we have not yet visited
      // that we can move and open in the time limit
      if (valve->flowRate > 0 && (graph->ids & valve->idBit) == 0 &&
          graph->minute + valveMoveTime + 1 < maxTime) {
        graph_t *newGraph = malloc(sizeof(graph_t));
        newGraph->ids = graph->ids;
        newGraph->ids |= valve->idBit;
        newGraph->currentIndex = valve->index;
        newGraph->minute = graph->minute + valveMoveTime + 1;
        newGraph->pressure =
            graph->pressure +
            (valve->flowRate * (maxTime - graph->minute - valveMoveTime - 0));
        q_enqueue(queue, newGraph);
        canMove = true;
      }
    }
    // reached a configuration that can't progress in time limit
    if (!canMove) {
      // fprintf(stdout, "cant move graph %lu, minute %d, pressure %d\n",
      // graph->ids, graph->minute, graph->pressure);
      q_enqueue(endQueue, graph);
      continue;
    }
    free(graph);
  }
  q_destroy(queue);

  while (!q_empty(endQueue)) {
    graph_t *graph = q_dequeue(endQueue);
    if (graph->pressure > maxPressure) {
      // fprintf(stdout, "new max: %d\n", graph->pressure);
      maxPressure = graph->pressure;
    }
    free(graph);
  }
  q_destroy(endQueue);

  return maxPressure;
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

  int partOne = exploreValves();

  fprintf(stdout, "Part one: %d\n", partOne);

#ifdef TEST_MODE
  assert(partOne == 1651);
#else
  assert(partOne == 1896);
#endif

  /* leaving in case it's needed another time
  int iterTimes = 1000;
  clock_t t;
  t = clock();

  for (int i = 0; i < iterTimes; i++) {
    exploreValves();
  }
  t = clock() - t;
  double time_taken =
      (((double)t) / CLOCKS_PER_SEC) / MAX_ITERATIONS; // in seconds
  double total_time = time_taken * MAX_ITERATIONS;

  printf("took %fs / %fms / %fus to execute\n", time_taken, time_taken * 1000,
         time_taken * 1000 * 1000);
  printf("total time was %.2fs / %.2f m / %.2f h \n", total_time,
         total_time / 60, total_time / 60 / 60);
  */

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

  fprintf(stdout, "Part two: %d\n", maxResultWithAnElephant);
#ifdef TEST_MODE
  assert(maxResultWithAnElephant == 1707);
#else
  assert(maxResultWithAnElephant > 2409);
#endif
}
