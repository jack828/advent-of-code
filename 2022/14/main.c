#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
// #define TEST_MODE
#include "../../utils.h"

#define PART_TWO

#ifdef PART_TWO
#define WIDTH 5000
#define HEIGHT 1000
#else
#define WIDTH 1000
#define HEIGHT 1000
#endif

char grid[HEIGHT][WIDTH] = {0};
// Only used for debug print
int lowestX = INT_MAX;
int highestX = 0;
// sand that ends up at this level falls forever
int highestY = 0;

int sandSrc[2] = {0, 500};

int sign(int x) { return x >= 0 ? 1 : -1; }

void drawLine(int from[], int to[]) {
  fprintf(stdout, "draw line (%d,%d) -> (%d,%d)\n", from[0], from[1], to[0],
          to[1]);
  if (from[0] == to[0]) {
    // Y same, so line is in X axis
    int fromX = min(from[1], to[1]);
    int toX = max(from[1], to[1]);

    for (int x = fromX; x <= toX; x++) {
      // fprintf(stdout, "rock: %d,%d\n", from[0], x);
      grid[from[0]][x] = '#';
    }
  }
  if (from[1] == to[1]) {
    // X same, line is in Y axis
    int fromY = min(from[0], to[0]);
    int toY = max(from[0], to[0]);

    for (int y = fromY; y <= toY; y++) {
      // fprintf(stdout, "rock: %d,%d\n", y, from[1]);
      grid[y][from[1]] = '#';
    }
  }

  // we will need this to determine if sand is falling to infinity
  highestY = max(from[0], highestY);
  highestY = max(to[0], highestY);
  // Just for the print grid debugging
  highestX = max(from[1], highestX);
  highestX = max(to[1], highestX);
  lowestX = min(from[1], lowestX);
  lowestX = min(to[1], lowestX);
}

void lineHandler(char *line) {
  fputs("line: ", stdout);
  fputs(line, stdout);

  int prev[2] = {-1, -1};
  int next[2] = {-1, -1};

  char *lineEnd;
  char *token;
  token = strtok_r(line, " ", &lineEnd);
  do {
    if (strcmp(token, "->") == 0) {
      prev[0] = next[0];
      prev[1] = next[1];
    } else {
      // new set of coords
      char *tokenEnd;
      int x = atoi(strtok_r(token, ",", &tokenEnd));
      int y = atoi(strtok_r(NULL, ",", &tokenEnd));
      next[0] = y;
      next[1] = x;

      if (prev[0] != -1 && prev[1] != -1) {
        // draw a rock line from prev -> next
        drawLine(prev, next);
      }
    }
  } while ((token = strtok_r(NULL, " ", &lineEnd)) != NULL);

  fputs("\n", stdout);
}

void printGrid() {
  grid[0][500] = '+';
  for (int y = 0; y <= highestY + 1; y++) {
    for (int x = lowestX - 1; x <= highestX + 1; x++) {
      char out = grid[y][x];
      fputc(out == 0 ? '.' : out, stdout);
    }
    fputc('\n', stdout);
  }
}

bool dropSand() {
  // assume sand starting point is always clear
  int sandY = sandSrc[0];
  int sandX = sandSrc[1];
  // A unit of sand always falls down one step if possible.
  // If the tile immediately below is blocked (by rock or sand), the unit of
  // sand attempts to instead move diagonally one step down and to the left.
  // If that tile is blocked, the unit of sand attempts to instead move
  // diagonally one step down and to the right.
  //
  // Sand keeps moving as long as it is able to do so, at each step trying to
  // move down, then down-left, then down-right. If all three possible
  // destinations are blocked, the unit of sand comes to rest and no longer
  // moves, at which point the next unit of sand is created back at the source.

  bool downBlocked = false;
  bool leftBlocked = false;
  bool rightBlocked = false;

  do {
    if (sandY >= highestY) {
      return true;
    }
    downBlocked = false;
    leftBlocked = false;
    rightBlocked = false;
    if (grid[sandY + 1][sandX] != 0) {
      downBlocked = true;
    } else {
      sandY++;
      continue;
    }
    if (grid[sandY + 1][sandX - 1] != 0) {
      leftBlocked = true;
    } else {
      // leftBlocked = false;
      sandY++;
      sandX--;
      continue;
    }
    if (grid[sandY + 1][sandX + 1] != 0) {
      rightBlocked = true;
    } else {
      // rightBlocked = false;
      sandY++;
      sandX++;
      continue;
    }

  } while (!downBlocked || !leftBlocked || !rightBlocked);

  grid[sandY][sandX] = 'o';

#ifdef PART_TWO
  if (sandY == sandSrc[0] && sandX == sandSrc[1]) {
    return true;
  }
#endif
  return false;
}

int main() {
  readInput(__FILE__, lineHandler);
#ifdef PART_TWO
  char *line = malloc(sizeof(char) * 20);
  sprintf(line, "0,%d -> %d,%d", highestY + 2, WIDTH, highestY + 2);
  lineHandler(line);
#endif
  // printGrid();

  int sand = 0;

  for (;;) {
    // fprintf(stdout, "sand: %d\n", sand);
    bool sandFellToAbyss = dropSand();
    if (sandFellToAbyss) {
      break;
    }
    sand++;
    // printGrid();
  }
#ifndef PART_TWO
  fprintf(stdout, "Part one: %d\n", sand);
#else
  fprintf(stdout, "Part two: %d\n", sand+1);
#endif
}
