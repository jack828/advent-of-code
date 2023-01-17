#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
// #define TEST_MODE
#include "../../utils.h"

typedef struct rucksack_t {
  char *contents;
  int length;
  char *left;
  char *right;
} rucksack_t;

rucksack_t **rucksacks;
int rucksackCount = 0;

void fileHandler(int lines) {
  fprintf(stdout, "lines: %d\n", lines);
  rucksacks = malloc(lines * sizeof(rucksack_t *));
}

void lineHandler(char *line) {
  fprintf(stdout, "line: %s\n", line);
  int length = strlen(line);
  rucksack_t *rucksack = malloc(sizeof(rucksack_t));
  rucksack->contents = strdup(line);
  rucksack->length = length;
  rucksack->left = strndup(line, length / 2);
  rucksack->right = strdup(line + (length / 2));
  rucksacks[rucksackCount++] = rucksack;
}

void printRucksacks() {
  for (int i = 0; i < rucksackCount; i++) {
    rucksack_t *rucksack = rucksacks[i];
    fprintf(stdout, "Rucksack %d: contents: %s, left: %s, right: %s\n", i,
            rucksack->contents, rucksack->left, rucksack->right);
  }
}

int priority(char c) {

  if (c >= 'a' && c <= 'z') {
    return c - 'a' + 1;
  }
  if (c >= 'A' && c <= 'Z') {
    return c - 'A' + 27;
  }

  return 9001;
}

int main() {
  readInputFile(__FILE__, lineHandler, fileHandler);

  // printRucksacks();

  int prioritySum = 0;

  for (int i = 0; i < rucksackCount; i++) {
    rucksack_t *rucksack = rucksacks[i];
    char *duplicateChar = strpbrk(rucksack->left, rucksack->right);
    // fprintf(stdout, "%d: %c\n", i, *duplicateChar);
    prioritySum += priority(*duplicateChar);
  }

  fprintf(stdout, "Part one: %d\n", prioritySum);
#ifdef TEST_MODE
  assert(prioritySum == 157);
#else
  assert(prioritySum == 8298);
#endif

  int groupPrioritySum = 0;

  for (int i = 0; i < rucksackCount; i += 3) {
    rucksack_t *rucksack1 = rucksacks[i];
    rucksack_t *rucksack2 = rucksacks[i + 1];
    rucksack_t *rucksack3 = rucksacks[i + 2];

    char duplicateChar = 0;
    for (int j = 0; j < rucksack1->length; j++) {
      char c = rucksack1->contents[j];
      if (strchr(rucksack2->contents, c) && strchr(rucksack3->contents, c)) {
        duplicateChar = c;
        break;
      }
    }
    // fprintf(stdout, "%d: %c\n", i, duplicateChar);
    groupPrioritySum += priority(duplicateChar);
  }
  fprintf(stdout, "Part two: %d\n", groupPrioritySum);
#ifdef TEST_MODE
  assert(groupPrioritySum == 70);
#else
  assert(groupPrioritySum == 2708);
#endif
}
