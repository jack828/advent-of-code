#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
// #define TEST_MODE
#include "../utils.h"

int *listA;
int *listB;
int count = 0;

void fileHandler(int lines) {
  printf("lines: %d\n", lines);
  listA = calloc(lines, sizeof *listA);
  listB = calloc(lines, sizeof *listB);
}

void lineHandler(char *line, int length) {
  printf("line (%d): %s\n", length, line);
  int a = atoi(strtok(line, " "));
  int b = atoi(strtok(NULL, " "));

  listA[count] = a;
  listB[count] = b;
  count++;
}

int compare(const void *a, const void *b) {
  int int_a = *((int *)a);
  int int_b = *((int *)b);

  if (int_a == int_b) {
    return 0;
  } else if (int_a < int_b) {
    return -1;
  } else {
    return 1;
  }
}

void printList(int *list) {
  for (int i = 0; i < count; i++) {
    printf("[%d] = %d\n", i, list[i]);
  }
}

int main() {
  init();
  readInputFile(__FILE__, lineHandler, fileHandler);

  // printList(listA);
  qsort(listA, count, sizeof(int), compare);
  // printList(listA);
  qsort(listB, count, sizeof(int), compare);

  int dist = 0;
  for (int i = 0; i < count; i++) {
    int a = listA[i];
    int b = listB[i];
    dist += abs(a - b);
  }
  printf("Part one: %d\n", dist);
#ifdef TEST_MODE
  assert(dist == 11);
#else
  assert(dist == 2742123);
#endif

  int similarity = 0;
  for (int i = 0; i < count; i++) {
    int a = listA[i];
    int seen_times = 0;

    for (int j = 0; j < count; j++) {
      int b = listB[j];
      if (b > a) {
        // never going to be similar now
        break;
      }
      if (a == b) {
        seen_times++;
      }

    }
    similarity += a * seen_times;
  }
  printf("Part two: %d\n", similarity);
#ifdef TEST_MODE
  assert(similarity == 31);
#else
  assert(similarity == 21328497);
#endif
  exit(EXIT_SUCCESS);
}
