#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
// #define TEST_MODE
#include "../utils.h"

int fullyContainedGroups = 0;
int overlappingGroups = 0;

bool contains(int aLeft, int aRight, int bLeft, int bRight) {
  return aLeft >= bLeft && aRight <= bRight;
}

bool overlaps(int aLeft, int aRight, int bLeft, int bRight) {
  return max(aLeft, aRight) >= min(bLeft, bRight);
}

void fileHandler(int lines) {
  // do nothing here
}

void lineHandler(char *line) {
  fprintf(stdout, "line: %s\n", line);
  int aLeft;
  int aRight;
  int bLeft;
  int bRight;

  sscanf(line, "%d-%d,%d-%d", &aLeft, &aRight, &bLeft, &bRight);
  // fprintf(stdout, "%d-%d,%d-%d\n", aLeft, aRight, bLeft, bRight);
  if (contains(aLeft, aRight, bLeft, bRight) ||
      contains(bLeft, bRight, aLeft, aRight)) {
    fullyContainedGroups++;
  }
  if (overlaps(aLeft, aRight, bLeft, bRight) &&
      overlaps(bLeft, bRight, aLeft, aRight)) {
    overlappingGroups++;
  }
}

int main() {
  init();
  readInputFile(__FILE__, lineHandler, fileHandler);

  fprintf(stdout, "Part one: %d\n", fullyContainedGroups);
#ifdef TEST_MODE
  assert(fullyContainedGroups == 2);
#else
  assert(fullyContainedGroups == 651);
#endif

  fprintf(stdout, "Part two: %d\n", overlappingGroups);
#ifdef TEST_MODE
  assert(overlappingGroups == 4);
#else
  assert(overlappingGroups == 956);
#endif
  exit(EXIT_SUCCESS);
}
