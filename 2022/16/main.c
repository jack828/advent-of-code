// NOTE
// You will need to amend the input to change any singular `valve XX` line to
// be `valves XX`
#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
// #define TEST_MODE
#include "../../lib/queue.h"
#include "../../utils.h"

#define MAX_TIME 30

typedef struct valve_t {
  // string id "AA"
  char *id;
  // bit field, only set on positive valves
  u_int64_t idBit;
  // obvious
  int flowRate;
  // the raw input string for later parsing
  char *rawValves;
  // pointers to other valves, dynamically allocated array
  struct valve_t **links;
  // number of pointers above
  int linkCount;
  // the cost (in minutes) to move from this valve to any of the other valves
  u_int16_t *valveWeights;
  // the index of the valve in the allValves array
  int index;
} valve_t;

valve_t **allValves = NULL;
int valveCount = 0;
valve_t *aaValve = NULL;
u_int64_t idBit = 1;
u_int64_t positiveValveIdBits = 0;
u_int64_t positiveValveCount = 0;

typedef struct graph_t {
  // bit field of visited (positive only) valves
  u_int64_t ids;
  // the index in allValves of the most recently opened valve
  int currentIndex;
  // number of visited valves (P2 only)
  int visited;
  // the current minute
  int minute;
  // total released pressure
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
  valve->rawValves = calloc(20, sizeof(char));
  strcpy(valve->rawValves, valvesStr);

  if (valve->flowRate > 0) {
    valve->idBit = idBit;
    positiveValveIdBits |= idBit;
    positiveValveCount++;
    idBit <<= 1;
  }

  // fprintf(stdout, "valve %s -> rate %d -> valves '%s'\n", valve->id,
  // valve->flowRate, valve->rawValves);
  allValves[valveCount++] = valve;

  fputs("\n", stdout);
}

void connectValves() {
  for (int i = 0; i < valveCount; i++) {
    valve_t *valve = allValves[i];
    // fprintf(stdout, "valve %s (%ld) -> rate %d -> valves '%s'\n", valve->id,
    // valve->idBit, valve->flowRate, valve->rawValves);

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
      for (int j = 0; j < valveCount; j++) {
        if (strcmp(allValves[j]->id, valveToken) == 0) {
          valve_t *connectedValve = allValves[j];
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
        if (dist[i][j] > dist[i][k] + dist[k][j]) {
          dist[i][j] = dist[i][k] + dist[k][j];
        }
      }
    }
  }

  /* debug print for visibility
  for (int i = 0; i < valveCount; i++) {
    for (int j = 0; j < valveCount; j++) {
      fprintf(stdout, "%d ", dist[i][j]);
    }
    fprintf(stdout, "\n");
  }
  */

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

  /* debug print for visibility
  for (int i = 0; i < valveCount; i++) {
    valve_t *valve = allValves[i];

    fprintf(stdout, "valve %s, weights: ", valve->id);
    for (int j = 0; j < valveCount; j++) {
      fprintf(stdout, "%d: %d ", j, valve->valveWeights[j]);
    }
    fprintf(stdout, "\n");
  }
  */
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
    // break if all are visited or if we have reached the time limit
    if (graph->ids == positiveValveIdBits || graph->minute == maxTime) {
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
            (valve->flowRate * (maxTime - graph->minute - valveMoveTime));
        q_enqueue(queue, newGraph);
        canMove = true;
      }
    }
    // reached a configuration that can't progress in time limit
    if (!canMove) {
      q_enqueue(endQueue, graph);
      continue;
    }
    free(graph);
  }
  q_destroy(queue);

  while (!q_empty(endQueue)) {
    graph_t *graph = q_dequeue(endQueue);
    if (graph->pressure > maxPressure) {
      maxPressure = graph->pressure;
    }
    free(graph);
  }
  q_destroy(endQueue);

  return maxPressure;
}

// qsort passes pointers to the array elements to compare even if they are
// already pointers
int compare(const void *a, const void *b) {
  const graph_t *graph_a = *(const graph_t **)a;
  const graph_t *graph_b = *(const graph_t **)b;

  if (graph_a->pressure == graph_b->pressure)
    return 0;
  else if (graph_a->pressure > graph_b->pressure)
    return -1;
  else
    return 1;
}

// same as above, but elephant must visit a different valve than the human
int exploreValvesWithElephant() {
  int maxPressure = -1;

  int maxTime = MAX_TIME - 4;
  int maxValves = floor(positiveValveCount / 2.0);
  queue_t *queue = q_create();
  graph_t *graphStart = malloc(sizeof(graph_t));
  graphStart->visited = 0;
  graphStart->ids = aaValve->idBit;
  graphStart->currentIndex = aaValve->index;
  graphStart->minute = 1;
  graphStart->pressure = 0;
  q_enqueue(queue, graphStart);

  queue_t *endQueue = q_create();

  while (!q_empty(queue)) {
    graph_t *graph = q_dequeue(queue);
    // break if all are visited
    // or if we have reached the time limit
    // or if we have reached maxValves
    if (graph->ids == positiveValveIdBits || graph->minute == maxTime ||
        graph->visited == maxValves) {
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
        newGraph->visited = graph->visited + 1;
        newGraph->ids = graph->ids;
        newGraph->ids |= valve->idBit;
        newGraph->currentIndex = valve->index;
        newGraph->minute = graph->minute + valveMoveTime + 1;
        newGraph->pressure =
            graph->pressure +
            (valve->flowRate * (maxTime - graph->minute - valveMoveTime));
        q_enqueue(queue, newGraph);
        canMove = true;
      }
    }
    // reached a configuration that can't progress in time limit
    if (!canMove) {
      q_enqueue(endQueue, graph);
      continue;
    }
    free(graph);
  }
  q_destroy(queue);

  // now our endQueue needs to go into an array
  graph_t **endArray = calloc(endQueue->size, sizeof(graph_t *));
  int endIndex = 0;
  while (!q_empty(endQueue)) {
    graph_t *graph = q_dequeue(endQueue);
    endArray[endIndex++] = graph;
  }

  qsort(endArray, endIndex, sizeof(graph_t *), compare);

  // TODO room for improvement here, how do we prune?
  for (int a = 0; a < endIndex / 2; a++) {
    // find the highest disjointed combination of nodes
    graph_t *aGraph = endArray[a];

    for (int b = 0; b < endIndex / 2; b++) {
      graph_t *bGraph = endArray[b];
      // if each set of ids is different then we will get zero
      // 0010 & 0100 == 0000
      // 0011 & 0010 == 0010
      if ((aGraph->ids & bGraph->ids) == 0) {
        int total = aGraph->pressure + bGraph->pressure;
        if (total > maxPressure) {
          maxPressure = total;
        }
      }
    }
  }
  for (int i = 0; i < endIndex; i++) {
    free(endArray[i]);
  }
  free(endArray);
  q_destroy(endQueue);

  return maxPressure;
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

  // leaving in case it's needed another time
  int iterTimes = 2;
  clock_t t;
  t = clock();

  for (int i = 0; i < iterTimes; i++) {
    exploreValvesWithElephant();
  }
  t = clock() - t;
  double time_taken = (((double)t) / CLOCKS_PER_SEC) / iterTimes; // in seconds
  double total_time = time_taken * iterTimes;

  printf("took %fs / %fms / %fus to execute\n", time_taken, time_taken * 1000,
         time_taken * 1000 * 1000);
  printf("total time was %.2fs / %.2f m / %.2f h \n", total_time,
         total_time / 60, total_time / 60 / 60);
  //

  int partTwo = exploreValvesWithElephant();

  fprintf(stdout, "Part two: %d\n", partTwo);
#ifdef TEST_MODE
  assert(partTwo == 1707);
#else
  assert(partTwo == 2576);
#endif
}
