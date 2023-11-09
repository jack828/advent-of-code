#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#define TEST_MODE
#include "../../utils.h"

#ifdef TEST_MODE
#define WIDTH 5
#define HEIGHT 7
#else

#define WIDTH 500
#define HEIGHT 700
#endif

char grid[HEIGHT][WIDTH] = {0};
int inputY = 0;

int startPos[2] = {0};
int endPos[2] = {0};
int startPoints[1024][2] = {0};
int startPointIndex = 0;

void lineHandler(char *line) {
  fputs("line: ", stdout);
  fputs(line, stdout);

  for (int i = 0; i < strlen(line) - 1; i++) {
    if (line[i] == 'S') {
      grid[inputY][i] = 'a';
      startPos[0] = inputY;
      startPos[1] = i;
    } else if (line[i] == 'E') {
      grid[inputY][i] = 'z';
      endPos[0] = inputY;
      endPos[1] = i;
    } else {
      grid[inputY][i] = line[i];
    }

    if (grid[inputY][i] == 'a') {
      startPoints[startPointIndex][0] = inputY;
      startPoints[startPointIndex++][0] = i;
    }
  }
  inputY++;
  fputs("\n", stdout);
}

int main() {
  readInput(__FILE__, lineHandler);
  fprintf(stdout, "Part one: %d\n", startPointIndex);
  fprintf(stdout, "Part two: %d\n", 420);
}
