#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
// #define TEST_MODE
#include "../../lib/queue.h"
#include "../../utils.h"

#ifdef TEST_MODE
#define BLUEPRINT_COUNT 2
#else
#define BLUEPRINT_COUNT 30
#endif

#define MAX_TIME 24

typedef struct cost_t {
  int ore;
  int clay;
  int obsidian;
} cost_t;

typedef struct blueprint_t {
  int id;
  cost_t *oreRobotCost;
  cost_t *clayRobotCost;
  cost_t *obsidianRobotCost;
  cost_t *geodeRobotCost;
} blueprint_t;

blueprint_t **blueprints;
int blueprintIndex = 0;

typedef struct simulation_t {
  int id;
  int ore;
  int clay;
  int obsidian;
  int geode;
  int oreRobots;
  int clayRobots;
  int obsidianRobots;
  int geodeRobots;
  int minute;
} simulation_t;

void fileHandler(int lines) {
  blueprints = malloc(sizeof(blueprint_t *) * lines);
}

void lineHandler(char *line) {
  fprintf(stdout, "line: %s\n", line);
  blueprint_t *blueprint = malloc(sizeof(blueprint_t));
  cost_t *oreRobotCost = malloc(sizeof(cost_t));
  cost_t *clayRobotCost = malloc(sizeof(cost_t));
  cost_t *obsidianRobotCost = malloc(sizeof(cost_t));
  cost_t *geodeRobotCost = malloc(sizeof(cost_t));

  oreRobotCost->ore = 0;
  oreRobotCost->clay = 0;
  oreRobotCost->obsidian = 0;
  clayRobotCost->ore = 0;
  clayRobotCost->clay = 0;
  clayRobotCost->obsidian = 0;
  obsidianRobotCost->ore = 0;
  obsidianRobotCost->clay = 0;
  obsidianRobotCost->obsidian = 0;
  geodeRobotCost->ore = 0;
  geodeRobotCost->clay = 0;
  geodeRobotCost->obsidian = 0;
  sscanf(line,
         "Blueprint %d: Each ore robot costs %d ore. Each clay robot costs %d "
         "ore. Each obsidian robot costs %d ore and %d clay. Each geode robot "
         "costs %d ore and %d obsidian.",
         &blueprint->id, &oreRobotCost->ore, &clayRobotCost->ore,
         &obsidianRobotCost->ore, &obsidianRobotCost->clay,
         &geodeRobotCost->ore, &geodeRobotCost->obsidian);

  blueprint->oreRobotCost = oreRobotCost;
  blueprint->clayRobotCost = clayRobotCost;
  blueprint->obsidianRobotCost = obsidianRobotCost;
  blueprint->geodeRobotCost = geodeRobotCost;
  blueprints[blueprintIndex++] = blueprint;
}

// increment everything
void tick(simulation_t *sim) {
  sim->ore += sim->oreRobots;
  sim->clay += sim->clayRobots;
  sim->obsidian += sim->obsidianRobots;
  sim->geode += sim->geodeRobots;
  sim->minute++;
}

bool canAfford(simulation_t *sim, cost_t *cost) {
  return sim->ore >= cost->ore && sim->clay >= cost->clay &&
         sim->obsidian >= cost->obsidian;
}

void buyRobot(simulation_t *sim, cost_t *cost) {
  sim->ore -= cost->ore;
  sim->clay -= cost->clay;
  sim->obsidian -= cost->obsidian;
}

simulation_t *cloneSim(simulation_t *srcSim) {
  simulation_t *newSim = malloc(sizeof(simulation_t));
  newSim->id = rand();
  newSim->ore = srcSim->ore;
  newSim->clay = srcSim->clay;
  newSim->obsidian = srcSim->obsidian;
  newSim->geode = srcSim->geode;
  newSim->oreRobots = srcSim->oreRobots;
  newSim->clayRobots = srcSim->clayRobots;
  newSim->obsidianRobots = srcSim->obsidianRobots;
  newSim->geodeRobots = srcSim->geodeRobots;
  newSim->minute = srcSim->minute;
  return newSim;
}

int max4(int a, int b, int c, int d) { return max(a, max(b, max(c, d))); }

// runs the simulation and returns the highest number of geodes possible
int run(blueprint_t *blueprint) {
  simulation_t *simStart = malloc(sizeof(simulation_t));
  simStart->id = rand();
  simStart->ore = 0;
  simStart->clay = 0;
  simStart->obsidian = 0;
  simStart->geode = 0;
  simStart->oreRobots = 1;
  simStart->clayRobots = 0;
  simStart->obsidianRobots = 0;
  simStart->geodeRobots = 0;
  simStart->minute = 0;

  queue_t *queue = q_create();
  q_enqueue(queue, simStart);

  int maxOreCost =
      max4(blueprint->oreRobotCost->ore, blueprint->clayRobotCost->ore,
           blueprint->obsidianRobotCost->ore, blueprint->geodeRobotCost->ore);

  int maxClayCost =
      max4(blueprint->oreRobotCost->clay, blueprint->clayRobotCost->clay,
           blueprint->obsidianRobotCost->clay, blueprint->geodeRobotCost->clay);

  int maxObsidianCost = max4(blueprint->oreRobotCost->obsidian,
                             blueprint->clayRobotCost->obsidian,
                             blueprint->obsidianRobotCost->obsidian,
                             blueprint->geodeRobotCost->obsidian);

  fprintf(stdout, "max: %d ore %d clay %d obsidian\n", maxOreCost, maxClayCost,
          maxObsidianCost);

  int maxQueueSize = 0;
  int maxGeodes = 0;
  while (!q_empty(queue)) {
    // fprintf(stdout, "queue size %d\n", queue->size);
    if (queue->size > maxQueueSize) {
      maxQueueSize = queue->size;
    }
    simulation_t *sim = q_dequeue(queue);
    // fprintf(stdout, "sim: %d - %dm - %dg %do %dc %do, %dgr %dor %dcr %dor\n",
    //         sim->id, sim->minute, sim->geode, sim->obsidian, sim->clay,
    //         sim->ore, sim->geodeRobots, sim->obsidianRobots, sim->clayRobots,
    //         sim->oreRobots);

    if (sim->geode > maxGeodes) {
      maxGeodes = sim->geode;
    }

    // reached the end
    if (sim->minute == MAX_TIME) {
      free(sim);
      continue;
    }

    // do not continue if this path is worse than the current best
    if (sim->geode < maxGeodes) {
      free(sim);
      continue;
    }

    // can we make a geode robot
    // if we can, do only that
    if (canAfford(sim, blueprint->geodeRobotCost)) {
      simulation_t *newSim = cloneSim(sim);
      buyRobot(newSim, blueprint->geodeRobotCost);
      tick(newSim);
      newSim->geodeRobots++;
      q_enqueue(queue, newSim);
    } else {

      // can we make a obsidian robot, and is it worth building
      if (sim->obsidianRobots < maxObsidianCost &&
          canAfford(sim, blueprint->obsidianRobotCost)) {
        simulation_t *newSim = cloneSim(sim);
        buyRobot(newSim, blueprint->obsidianRobotCost);
        tick(newSim);
        newSim->obsidianRobots++;
        q_enqueue(queue, newSim);
      }

      // can we make a clay robot
      if (sim->clayRobots < maxClayCost &&
          canAfford(sim, blueprint->clayRobotCost)) {
        simulation_t *newSim = cloneSim(sim);
        buyRobot(newSim, blueprint->clayRobotCost);
        tick(newSim);
        newSim->clayRobots++;
        q_enqueue(queue, newSim);
      }
      // can we make an ore robot
      if (sim->oreRobots < maxOreCost &&
          canAfford(sim, blueprint->oreRobotCost)) {
        // fprintf(stdout, "sim: %d - buy ore robot\n", sim->id);
        simulation_t *newSim = cloneSim(sim);
        buyRobot(newSim, blueprint->oreRobotCost);
        tick(newSim);
        newSim->oreRobots++;
        q_enqueue(queue, newSim);
      }
      // also just wait
      tick(sim);
      q_enqueue(queue, sim);
    }
  }
  q_destroy(queue);

  fprintf(stdout, "\n\nmax queue size: %d\n", maxQueueSize);
  return maxGeodes;
}

int main() {
  readInputFile(__FILE__, lineHandler, fileHandler);
  for (int i = 0; i < blueprintIndex; i++) {
    blueprint_t *blueprint = blueprints[i];
    fprintf(stdout, "Blueprint: %d\n", blueprint->id);
    fprintf(stdout, "Ore robot cost: %do %dc %do\n",
            blueprint->oreRobotCost->ore, blueprint->oreRobotCost->clay,
            blueprint->oreRobotCost->obsidian);
    fprintf(stdout, "Clay robot cost: %do %dc %do\n",
            blueprint->clayRobotCost->ore, blueprint->clayRobotCost->clay,
            blueprint->clayRobotCost->obsidian);
    fprintf(stdout, "Obsidian robot cost: %do %dc %do\n",
            blueprint->obsidianRobotCost->ore,
            blueprint->obsidianRobotCost->clay,
            blueprint->obsidianRobotCost->obsidian);
    fprintf(stdout, "Geode robot cost: %do %dc %do\n",
            blueprint->geodeRobotCost->ore, blueprint->geodeRobotCost->clay,
            blueprint->geodeRobotCost->obsidian);
  }

  int qualityLevelSum = 0;
  for (int i = 0; i < blueprintIndex ; i++) {
    blueprint_t *blueprint = blueprints[i];
    int maxGeodes = run(blueprint);
    qualityLevelSum += blueprint->id * maxGeodes;
  }

  fprintf(stdout, "Part one: %d\n", qualityLevelSum);
#ifdef TEST_MODE
  assert(qualityLevelSum == 33);
#else
  assert(qualityLevelSum == 420);
#endif

  fprintf(stdout, "Part two: %d\n", 420);
#ifdef TEST_MODE
  assert(420 == 69);
#else
  assert(420 == 69);
#endif
}
