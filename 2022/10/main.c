#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
// #define TEST_MODE
#include "../utils.h"

int cycleCount = 0;
int xReg = 1;
int signalSum = 0;

int addSignalCycle = 20;

#define CRT_WIDTH 40
#define CRT_HEIGHT 6
char CRT[CRT_HEIGHT][CRT_WIDTH];

int spriteMin = 0;
int spriteMax = 2;

void runCycle() {
  cycleCount++;
  if (cycleCount == addSignalCycle) {
    addSignalCycle += 40;
    signalSum += cycleCount * xReg;
  }

  int y = (cycleCount - 1) / 40;
  int x = (cycleCount - 1) % 40;
  if (x >= (xReg - 1) && x <= (xReg + 1)) {
    CRT[y][x] = '#';
  } else {
    CRT[y][x] = '.';
  }
}

void fileHandler(int lines) { fprintf(stdout, "lines: %d\n", lines); }

void lineHandler(char *line) {
  fprintf(stdout, "line: %s\n", line);
  if (strncmp(line, "noop", 4) == 0) {
    // one cycle
    runCycle();
  } else if (strncmp(line, "addx", 4) == 0) {
    char *token;
    strtok(line, " ");
    token = strtok(NULL, " ");
    int V = atoi(token);
    // two cycles
    runCycle();
    runCycle();
    // after, add V to xReg
    xReg += V;
  }
}

int main() {
  init();
  readInputFile(__FILE__, lineHandler, fileHandler);
  fprintf(stdout, "cycleCount: %d, xReg: %d\n", cycleCount, xReg);
  fprintf(stdout, "Part one: %d\n", signalSum);
#ifdef TEST_MODE
  assert(signalSum == 13140);
#else
  assert(signalSum == 15140);
#endif

  fprintf(stdout, "Part two: \n");

  for (int i = 0; i < CRT_HEIGHT; i++) {
    for (int j = 0; j < CRT_WIDTH - 1; j++) {
      fputc(CRT[i][j], stdout);
    }
    fputc('\n', stdout);
  }
  exit(EXIT_SUCCESS);
}
