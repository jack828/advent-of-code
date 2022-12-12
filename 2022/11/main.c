#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
// #define TEST_MODE
#include "../../utils.h"

// change manually
// #define PART_ONE

#define MAX_ITEMS 4096
typedef struct MONKEY {
  int number;
  int falseMonkeyIndex;
  int trueMonkeyIndex;
  int testCase;
  u_int64_t items[MAX_ITEMS];
  u_int64_t itemCount;
  char operation;
  int operationNum;
  u_int64_t inspectionCount;
} MONKEY;

u_int64_t supermod = 0;

#define MAX_MONKEYS 7
struct MONKEY *monkeys[MAX_MONKEYS];
int monkeyIndex = -1;

void lineHandler(char *line) {
  fputs("line: ", stdout);
  fputs(line, stdout);

  if (strncmp(line, "Monkey", 6) == 0) {
    // new monkey
    monkeyIndex++;
    struct MONKEY *monkey = malloc(sizeof(MONKEY));
    monkey->number = monkeyIndex;
    monkey->itemCount = 0;
    monkey->inspectionCount = 0;
    monkeys[monkeyIndex] = monkey;
  } else if (strncmp(line, "  Starting", 10) == 0) {
    // parse item array
    char *token = strtok(line, ":"); // discard start
    token = strtok(NULL, ":");       // now token is the item list

    char *itemToken = strtok(token, ",");
    do {
      monkeys[monkeyIndex]->items[monkeys[monkeyIndex]->itemCount++] =
          atoi(itemToken);

      itemToken = strtok(NULL, ",");
    } while (itemToken != NULL);
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

void doTheMonkeyThrows() {
  // for 20 rounds

#ifdef PART_ONE
  for (int round = 0; round < 20; round++) {
#else
  for (int round = 0; round < 10000; round++) {

    if (round == 20) {

      fprintf(stdout, "\nAfter round 20:\n");
      for (int testMonkey = 0; testMonkey <= monkeyIndex; testMonkey++) {
        struct MONKEY *monkey = monkeys[testMonkey];
        fprintf(stdout, "monkey %d: inspected %lu\n", monkey->number,
                monkey->inspectionCount);
      }
    }
#endif
    // fprintf(stdout, "round: %d\n", round);

    // test every monkey in series
    for (int testMonkey = 0; testMonkey <= monkeyIndex; testMonkey++) {
      struct MONKEY *monkey = monkeys[testMonkey];
      // fprintf(stdout, "monkey #%d, operation: %c, opNum: %d, items: %d\n",
      //         monkey->number, monkey->operation, monkey->operationNum,
      //         monkey->itemCount);

      // DEBUG: list items
      for (int itemIndex = 0; itemIndex < monkey->itemCount; itemIndex++) {
        // fprintf(stdout, "%lu,", monkey->items[itemIndex]);
      }
      // fputc('\n', stdout);
      // test each item the monkey holds
      for (int itemIndex = 0; itemIndex < monkey->itemCount; itemIndex++) {
        // fprintf(stdout, "Start %lu,", monkey->items[itemIndex]);
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
          // TODO handle if this happens i guess
          // fprintf(stdout, "unknown op: %c\n", monkey->operation);
          exit(1);
          break;
        }
        // fprintf(stdout, "inspected %lu,", monkey->items[itemIndex]);

#ifdef PART_ONE
        // divide by 3 and floor result
        monkey->items[itemIndex] = monkey->items[itemIndex] / 3;
        // fprintf(stdout, "bored %lu,", monkey->items[itemIndex]);
#else
        monkey->items[itemIndex] = monkey->items[itemIndex] % supermod;
#endif

        // then perform test
        // throw to new monkey
        struct MONKEY *nextMonkey = NULL;

        // fprintf(stdout, "test %lu,",
        // monkey->items[itemIndex] % monkey->testCase);

        if (monkey->items[itemIndex] % monkey->testCase == 0) {
          // true monkey
          nextMonkey = monkeys[monkey->trueMonkeyIndex];
        } else {
          // false monkey
          nextMonkey = monkeys[monkey->falseMonkeyIndex];
        }
        // fprintf(stdout, "next monkey %d (new # items %lu)",
        // nextMonkey->number, nextMonkey->itemCount);
        nextMonkey->items[nextMonkey->itemCount] = monkey->items[itemIndex];
        nextMonkey->itemCount++;
        // fputc('\n', stdout);
      }
      // monkey has no more items now
      monkey->itemCount = 0;
      // fputc('\n', stdout);
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

int main() {
  readInput(__FILE__, lineHandler);

  calculateSupermod();
  doTheMonkeyThrows();

  fprintf(stdout, "\nAfter rounds:\n");
  for (int testMonkey = 0; testMonkey <= monkeyIndex; testMonkey++) {
    struct MONKEY *monkey = monkeys[testMonkey];
    fprintf(stdout, "monkey %d: inspected %lu\n", monkey->number,
            monkey->inspectionCount);
  }

  u_int64_t inspectionCounts[MAX_MONKEYS];
  int index = 0;
  for (int testMonkey = 0; testMonkey <= monkeyIndex; testMonkey++) {
    struct MONKEY *monkey = monkeys[testMonkey];
    inspectionCounts[index++] = monkey->inspectionCount;
  }

  qsort(inspectionCounts, index, sizeof(u_int64_t), compare);
  u_int64_t sum =
      inspectionCounts[monkeyIndex - 1] * inspectionCounts[monkeyIndex];

#ifdef PART_ONE
  fprintf(stdout, "Part one: %lu\n", sum);
#else
  fprintf(stdout, "Part two: %lu\n", sum);
#ifdef TEST_MODE
  assert(sum == 2713310158);
#else
  assert(sum == 16792940265);
#endif

#endif
}
