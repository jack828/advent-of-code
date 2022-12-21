#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#define TEST_MODE
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

blueprint_t *blueprints[BLUEPRINT_COUNT] = {0};
int blueprintIndex = 0;

typedef struct simulation_t {
  int ore;
  int clay;
  int obsidian;
  int geodes;
  int oreRobots;
  int clayRobots;
  int obsidianRobots;
  int geodeRobots;
  int minutes;
} simulation_t;

// Take an input string and put into <number> the first number in the string,
// and then return a pointer to the part after the number
char *tokeniser(char *input, int *number) {
  int length = strlen(input);
  int digitsStartIndex = -1;
  int digitsEndIndex = -1;
  for (int i = 0; i < length; i++) {
    if (digitsStartIndex == -1 && isdigit(input[i])) {
      digitsStartIndex = i;
    }
    if (digitsStartIndex != -1 && !isdigit(input[i])) {
      digitsEndIndex = i;
      break;
    }
  }

  char *str =
      strndup(input + digitsStartIndex, digitsEndIndex - digitsStartIndex);

  int numberFromString = atoi(str);
  (*number) = numberFromString;

  free(str);
  return input + digitsEndIndex;
}

void lineHandler(char *line) {
  fputs("line: ", stdout);
  fputs(line, stdout);
  blueprint_t *blueprint = malloc(sizeof(blueprint_t));
  cost_t *oreRobotCost = malloc(sizeof(cost_t));
  cost_t *clayRobotCost = malloc(sizeof(cost_t));
  cost_t *obsidianRobotCost = malloc(sizeof(cost_t));
  cost_t *geodeRobotCost = malloc(sizeof(cost_t));

  int number = 0;
  line = tokeniser(line, &number);
  blueprint->id = number;

  line = tokeniser(line, &number);
  oreRobotCost->ore = number;

  line = tokeniser(line, &number);
  clayRobotCost->ore = number;

  line = tokeniser(line, &number);
  obsidianRobotCost->ore = number;
  line = tokeniser(line, &number);
  obsidianRobotCost->clay = number;

  line = tokeniser(line, &number);
  geodeRobotCost->ore = number;
  line = tokeniser(line, &number);
  geodeRobotCost->obsidian = number;

  blueprint->oreRobotCost = oreRobotCost;
  blueprint->clayRobotCost = clayRobotCost;
  blueprint->obsidianRobotCost = obsidianRobotCost;
  blueprint->oreRobotCost = oreRobotCost;
  blueprints[blueprintIndex++] = blueprint;
}

void tick(simulation_t *simulation) {
  // increment everything as appropriate and minutes
  simulation->minutes++;
}

int run(int index) {
  simulation_t simulation = {.ore = 0,
                             .clay = 0,
                             .obsidian = 0,
                             .geodes = 0,
                             .oreRobots = 0,
                             .clayRobots = 0,
                             .obsidianRobots = 0,
                             .geodeRobots = 0,
                             .minutes = 0};

  do {
    // guess which to buy i guess?
    tick(&simulation);
  } while (simulation.minutes != MAX_TIME);
}

int main() {
  readInput(__FILE__, lineHandler);

  for (int i = 0; i < blueprintIndex; i++) {
    run(blueprintIndex);
  }

  fprintf(stdout, "Part one: %d\n", 69);
#ifdef TEST_MODE
  assert(69 == 420);
#else
  assert(69 == 420);
#endif

  fprintf(stdout, "Part two: %d\n", 420);
#ifdef TEST_MODE
  assert(420 == 69);
#else
  assert(420 == 69);
#endif
}
