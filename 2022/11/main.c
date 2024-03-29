#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
// #define TEST_MODE
#include "../utils.h"

#define MAX_ITEMS 4096
typedef struct monkey_t {
  int number;
  int falseMonkeyIndex;
  int trueMonkeyIndex;
  int testCase;
  u_int64_t items[MAX_ITEMS];
  u_int64_t itemCount;
  u_int64_t originalItems[MAX_ITEMS];
  u_int64_t originalItemCount;
  char operation;
  int operationNum;
  u_int64_t inspectionCount;
} monkey_t;

u_int64_t supermod = 0;

#define MAX_MONKEYS 8
monkey_t *monkeys[MAX_MONKEYS];
int monkeyIndex = -1;

void fileHandler(int lines) { fprintf(stdout, "lines: %d\n", lines); }

void lineHandler(char *line) {
  fprintf(stdout, "line: %s\n", line);

  if (strncmp(line, "Monkey", 6) == 0) {
    // new monkey
    monkeyIndex++;
    monkey_t *monkey = malloc(sizeof(monkey_t));
    monkey->number = monkeyIndex;
    monkey->itemCount = 0;
    monkey->originalItemCount = 0;
    monkey->inspectionCount = 0;
    monkeys[monkeyIndex] = monkey;
  } else if (strncmp(line, "  Starting", 10) == 0) {
    monkey_t *monkey = monkeys[monkeyIndex];
    // parse item array
    char *token = strtok(line, ":"); // discard start
    token = strtok(NULL, ":");       // now token is the item list

    char *itemToken = strtok(token, ",");
    do {
      monkey->items[monkey->itemCount++] = atoi(itemToken);

      itemToken = strtok(NULL, ",");
    } while (itemToken != NULL);

    memcpy(monkey->originalItems, monkey->items, monkey->itemCount);
    monkey->originalItemCount = monkey->itemCount;
  } else if (strncmp(line, "  Operation", 11) == 0) {
    // parse operation
    char *operation;
    char *operationValue;
    strtok(line, " ");                  // Operation:
    strtok(NULL, " ");                  // new
    strtok(NULL, " ");                  // =
    strtok(NULL, " ");                  // old
    operation = strtok(NULL, " ");      // <operation>
    operationValue = strtok(NULL, " "); // <operationValue>
    // Annoying edge case - change operation to ^ and we'll pretend it means
    // old*old
    if (strncmp(operation, "*", 1) == 0 &&
        strncmp(operationValue, "old", 3) == 0) {
      monkeys[monkeyIndex]->operation = '^';
    } else {
      monkeys[monkeyIndex]->operation = operation[0];
      monkeys[monkeyIndex]->operationNum = atoi(operationValue);
    }
  } else if (strncmp(line, "  Test", 6) == 0) {
    // parse test case
    char *token = strtok(line, " "); // Test:
    token = strtok(NULL, " ");       // divisible
    token = strtok(NULL, " ");       // by
    token = strtok(NULL, " ");       // x
    monkeys[monkeyIndex]->testCase = atoi(token);
  } else if (strncmp(line, "    If true", 11) == 0) {
    // parse true monkey
    char *token = strtok(line, " "); // If
    token = strtok(NULL, " ");       // true:
    token = strtok(NULL, " ");       // throw
    token = strtok(NULL, " ");       // to
    token = strtok(NULL, " ");       // monkey
    token = strtok(NULL, " ");       // x
    monkeys[monkeyIndex]->trueMonkeyIndex = atoi(token);
  } else if (strncmp(line, "    If false", 12) == 0) {
    // parse false monkey
    char *token = strtok(line, " "); // If
    token = strtok(NULL, " ");       // false:
    token = strtok(NULL, " ");       // throw
    token = strtok(NULL, " ");       // to
    token = strtok(NULL, " ");       // monkey
    token = strtok(NULL, " ");       // x
    monkeys[monkeyIndex]->falseMonkeyIndex = atoi(token);
  }
}

void doTheMonkeyThrows(int limit, u_int64_t (*worryModifier)(u_int64_t)) {
  for (int round = 0; round < limit; round++) {
    // test every monkey in series
    for (int testMonkey = 0; testMonkey <= monkeyIndex; testMonkey++) {
      monkey_t *monkey = monkeys[testMonkey];

      // DEBUG: list items
      // for (int itemIndex = 0; itemIndex < monkey->itemCount; itemIndex++) {
      // fprintf(stdout, "%lu,", monkey->items[itemIndex]);
      // }
      // test each item the monkey holds
      for (int itemIndex = 0; itemIndex < monkey->itemCount; itemIndex++) {
        // inspect item = item <operation> <operationNum>
        monkey->inspectionCount++;
        switch (monkey->operation) {
        case '^': // exponential
          monkey->items[itemIndex] *= monkey->items[itemIndex];
          break;
        case '*':
          monkey->items[itemIndex] *= monkey->operationNum;
          break;
        case '+':
          monkey->items[itemIndex] += monkey->operationNum;
          break;
        default:
          exit(EXIT_FAILURE);
          break;
        }

        monkey->items[itemIndex] = worryModifier(monkey->items[itemIndex]);

        // then perform test
        // throw to new monkey
        monkey_t *nextMonkey = NULL;

        if (monkey->items[itemIndex] % monkey->testCase == 0) {
          // true monkey
          nextMonkey = monkeys[monkey->trueMonkeyIndex];
        } else {
          // false monkey
          nextMonkey = monkeys[monkey->falseMonkeyIndex];
        }
        nextMonkey->items[nextMonkey->itemCount] = monkey->items[itemIndex];
        nextMonkey->itemCount++;
      }
      // monkey has no more items now
      monkey->itemCount = 0;
    }
  }
}

int compare(const void *a, const void *b) {
  u_int64_t int_a = *((u_int64_t *)a);
  u_int64_t int_b = *((u_int64_t *)b);

  if (int_a == int_b)
    return 0;
  else if (int_a < int_b)
    return -1;
  else
    return 1;
}

u_int64_t GCD(u_int64_t a, u_int64_t b) {
  if (b == 0) {
    return a;
  }
  return GCD(b, a % b);
}

void calculateSupermod() {
  int lcm = monkeys[0]->testCase;
  int gcd = monkeys[0]->testCase;

  // Loop through the array and find GCD
  // use GCD to find the LCM
  for (int i = 0; i <= monkeyIndex; i++) {
    gcd = GCD(monkeys[i]->testCase, lcm);
    lcm = (lcm * monkeys[i]->testCase) / gcd;
  }
  supermod = lcm;
}

u_int64_t divideByThree(u_int64_t worryLevel) { return worryLevel / 3; }

u_int64_t modSuperMod(u_int64_t worryLevel) { return worryLevel % supermod; }

void resetMonkeys() {
  for (int i = 0; i <= monkeyIndex; i++) {
    monkey_t *monkey = monkeys[i];
    monkey->inspectionCount = 0;
    monkey->itemCount = monkey->originalItemCount;
    memcpy(monkey->items, monkey->originalItems, monkey->originalItemCount);
  }
}

int main() {
  init();
  readInputFile(__FILE__, lineHandler, fileHandler);

  calculateSupermod();
  doTheMonkeyThrows(20, divideByThree);

  fprintf(stdout, "\nAfter rounds:\n");
  for (int testMonkey = 0; testMonkey <= monkeyIndex; testMonkey++) {
    monkey_t *monkey = monkeys[testMonkey];
    fprintf(stdout, "monkey %d: inspected %lu\n", monkey->number,
            monkey->inspectionCount);
  }

  u_int64_t inspectionCountsP1[MAX_MONKEYS];
  int index = 0;
  for (int testMonkey = 0; testMonkey <= monkeyIndex; testMonkey++) {
    monkey_t *monkey = monkeys[testMonkey];
    inspectionCountsP1[index++] = monkey->inspectionCount;
  }

  qsort(inspectionCountsP1, index, sizeof(u_int64_t), compare);
  u_int64_t partOneSum =
      inspectionCountsP1[monkeyIndex - 1] * inspectionCountsP1[monkeyIndex];

  fprintf(stdout, "Part one: %lu\n", partOneSum);
#ifdef TEST_MODE
  assert(partOneSum == 10605);
#else
  assert(partOneSum == 117624);
#endif

  // TODO maybe actually resetting is faster idk
  monkeyIndex = -1;
  readInputFile(__FILE__, lineHandler, fileHandler);
  doTheMonkeyThrows(10000, modSuperMod);

  fprintf(stdout, "\nAfter rounds:\n");
  for (int testMonkey = 0; testMonkey <= monkeyIndex; testMonkey++) {
    monkey_t *monkey = monkeys[testMonkey];
    fprintf(stdout, "monkey %d: inspected %lu\n", monkey->number,
            monkey->inspectionCount);
  }
  u_int64_t inspectionCountsP2[MAX_MONKEYS];
  index = 0;
  for (int testMonkey = 0; testMonkey <= monkeyIndex; testMonkey++) {
    monkey_t *monkey = monkeys[testMonkey];
    inspectionCountsP2[index++] = monkey->inspectionCount;
  }

  qsort(inspectionCountsP2, index, sizeof(u_int64_t), compare);
  u_int64_t partTwoSum =
      inspectionCountsP2[monkeyIndex - 1] * inspectionCountsP2[monkeyIndex];
  fprintf(stdout, "Part two: %lu\n", partTwoSum);
#ifdef TEST_MODE
  assert(partTwoSum == 2713310158);
#else
  assert(partTwoSum == 16792940265);
#endif

  exit(EXIT_SUCCESS);
}
