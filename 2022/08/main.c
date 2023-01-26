#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
// #define TEST_MODE
#include "../utils.h"

#define LINE_MAX 1024

int trees[LINE_MAX][LINE_MAX];
int treesDebug[LINE_MAX][LINE_MAX];
int treesScores[LINE_MAX][LINE_MAX];

u_int64_t rowCount = 0;
u_int64_t colCount = 0;

bool isVisible(u_int64_t row, u_int64_t col) {
  // leftwards
  bool left = true;
  for (int64_t i = col - 1; i >= 0; i--) {
    if (trees[row][i] >= trees[row][col]) {
      left = false;
      break;
    }
  }
  // rightwards
  bool right = true;
  for (int64_t i = col + 1; i <= colCount; i++) {
    if (trees[row][i] >= trees[row][col]) {
      right = false;
      break;
    }
  }
  // upwards
  bool up = true;
  for (int64_t i = row - 1; i >= 0; i--) {
    if (trees[i][col] >= trees[row][col]) {
      up = false;
      break;
    }
  }
  // downwards
  bool down = true;

  for (int64_t i = row + 1; i <= rowCount; i++) {
    if (trees[i][col] >= trees[row][col]) {
      down = false;
      break;
    }
  }
  return left || right || up || down;
}

u_int64_t scenicScore(u_int64_t row, u_int64_t col) {
  // leftwards
  u_int64_t left = 0;
  for (int64_t i = col - 1; i >= 0; i--) {
    left++;
    if (trees[row][i] >= trees[row][col]) {
      break;
    }
  }
  // rightwards
  u_int64_t right = 0;
  for (int64_t i = col + 1; i < colCount; i++) {
    right++;
    if (trees[row][i] >= trees[row][col]) {
      break;
    }
  }
  // upwards
  u_int64_t up = 0;
  for (int64_t i = row - 1; i >= 0; i--) {
    up++;
    if (trees[i][col] >= trees[row][col]) {
      break;
    }
  }
  // downwards
  u_int64_t down = 0;

  for (int64_t i = row + 1; i < rowCount; i++) {
    down++;
    if (trees[i][col] >= trees[row][col]) {
      break;
    }
  }
  return left * right * up * down;
}

void fileHandler(int lines) { fprintf(stdout, "lines: %d\n", lines); }

void lineHandler(char *line) {
  fprintf(stdout, "line: %s\n", line);
  if (!colCount) {
    colCount = strlen(line);
  }
  for (u_int64_t i = 0; i < colCount; i++) {
    trees[rowCount][i] = line[i] - 48;
    treesDebug[rowCount][i] = line[i] - 48;
  }

  rowCount++;
}

int main() {
  init();
  readInputFile(__FILE__, lineHandler, fileHandler);

  u_int64_t visibleCount = 0;

  for (u_int64_t i = 0; i < rowCount; i++) {
    for (u_int64_t j = 0; j < colCount; j++) {

      if (i == 0 || j == 0 || i == rowCount - 1 || j == colCount - 1) {
        // edges are always visible
        visibleCount++;
        continue;
      }
      if (isVisible(i, j)) {
        visibleCount++;
      } else {
        treesDebug[i][j] = 0;
      }
    }
  }

  fprintf(stdout, "Part one: %lu\n", visibleCount);
#ifdef TEST_MODE
  assert(visibleCount == 21);
#else
  assert(visibleCount == 1798);
#endif

  u_int64_t highScore = 0;

  for (u_int64_t i = 0; i < rowCount; i++) {
    for (u_int64_t j = 0; j < colCount; j++) {

      if (i == 0 || j == 0 || i == rowCount - 1 || j == colCount - 1) {
        // edges are always 0
        continue;
      }
      u_int64_t score = scenicScore(i, j);
      highScore = score > highScore ? score : highScore;
    }
  }
  fprintf(stdout, "Part two: %lu\n", highScore);
#ifdef TEST_MODE
  assert(highScore == 8);
#else
  assert(highScore == 259308);
#endif
  exit(EXIT_SUCCESS);
}
