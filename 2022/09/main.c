#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
// #define TEST_MODE
#include "../../utils.h"

#ifdef TEST_MODE
#define HEIGHT 120
#define WIDTH 120
#else
#define HEIGHT 4096
#define WIDTH 4096
#endif

// you'll have to manually swap this out to test part one and two, lol soz
// #define PART_ONE

char grid[HEIGHT][WIDTH];
int visited[HEIGHT][WIDTH];

typedef struct KNOT {
  int x;
  int y;
  struct KNOT *tail;
  bool isTailKnot;
  char indicator;
} KNOT;

#ifdef PART_ONE
KNOT tail = {.isTailKnot = true,
             .x = HEIGHT / 2,
             .y = WIDTH / 2,
             .tail = NULL,
             .indicator = 'T'};

#else
KNOT tail9 = {.isTailKnot = true,
              .x = HEIGHT / 2,
              .y = WIDTH / 2,
              .tail = NULL,
              .indicator = '9'};
KNOT tail8 = {.isTailKnot = false,
              .x = HEIGHT / 2,
              .y = WIDTH / 2,
              .tail = &tail9,
              .indicator = '8'};
KNOT tail7 = {.isTailKnot = false,
              .x = HEIGHT / 2,
              .y = WIDTH / 2,
              .tail = &tail8,
              .indicator = '7'};
KNOT tail6 = {.isTailKnot = false,
              .x = HEIGHT / 2,
              .y = WIDTH / 2,
              .tail = &tail7,
              .indicator = '6'};
KNOT tail5 = {.isTailKnot = false,
              .x = HEIGHT / 2,
              .y = WIDTH / 2,
              .tail = &tail6,
              .indicator = '5'};
KNOT tail4 = {.isTailKnot = false,
              .x = HEIGHT / 2,
              .y = WIDTH / 2,
              .tail = &tail5,
              .indicator = '4'};
KNOT tail3 = {.isTailKnot = false,
              .x = HEIGHT / 2,
              .y = WIDTH / 2,
              .tail = &tail4,
              .indicator = '3'};
KNOT tail2 = {.isTailKnot = false,
              .x = HEIGHT / 2,
              .y = WIDTH / 2,
              .tail = &tail3,
              .indicator = '2'};
KNOT tail1 = {.isTailKnot = false,
              .x = HEIGHT / 2,
              .y = WIDTH / 2,
              .tail = &tail2,
              .indicator = '1'};

#endif

KNOT head = {.isTailKnot = false,
             .x = HEIGHT / 2,
             .y = WIDTH / 2,
#ifdef PART_ONE
             .tail = &tail,
#else
             .tail = &tail1,
#endif
             .indicator = 'H'};

void printVisited() {
  for (int i = 0; i < HEIGHT; i++) {
    for (int j = 0; j < WIDTH; j++) {
      fputs(visited[i][j] ? "1" : "0", stdout);
    }
    fputs("\n", stdout);
  }
}

void printGrid() {
  for (int i = 0; i < HEIGHT; i++) {
    for (int j = 0; j < WIDTH; j++) {
      if ((grid[i][j] >= '1' && grid[i][j] <= '9') || grid[i][j] == 'H' ||
          grid[i][j] == 'T') {
        fputc(grid[i][j], stdout);
      } else {
        fputc('x', stdout);
      }
    }
    fputs("\n", stdout);
  }
}

void doMove(char direction) {
  fprintf(stdout, "MOVE: dir %c, knot (%d,%d), tail (%d,%d)[%c]\n", direction,
          head.x, head.y, head.tail->x, head.tail->y, head.tail->indicator);
  int changeX = 0;
  int changeY = 0;
  switch (direction) {
  case 'R':
    changeX = 1;
    changeY = 0;
    break;
  case 'U':
    changeX = 0;
    changeY = -1;
    break;
  case 'L':
    changeX = -1;
    changeY = 0;
    break;
  case 'D':
    changeX = 0;
    changeY = 1;
    break;
  }
  int newHeadX = head.x + changeX;
  int newHeadY = head.y + changeY;
  // set new position
  grid[head.y][head.x] = ' ';
  head.x = newHeadX;
  head.y = newHeadY;
  grid[head.y][head.x] = head.indicator;

  struct KNOT *headPtr = &head;
  struct KNOT *tailPtr = head.tail;

  do {
    fprintf(stdout, "MOVE HEAD: head (%d,%d) - %c\n", headPtr->x, headPtr->y,
            headPtr->indicator);
    fprintf(stdout, "MOVE TAIL: tail (%d,%d) - %c\n", tailPtr->x, tailPtr->y,
            tailPtr->indicator);

    // Confession: i had to look for help for this block
    // If the head is right next to or directly on top of the tail then we do
    // nothing
    int diffInX = abs(tailPtr->x - headPtr->x);
    int diffInY = abs(tailPtr->y - headPtr->y);
    if (diffInY <= 1 && diffInX <= 1) {
      // all good
    } else {
      int xDirection = 1;
      int yDirection = 1;
      if (headPtr->y < tailPtr->y) {
        yDirection = -1;
      }
      if (headPtr->x < tailPtr->x) {
        xDirection = -1;
      }
      if (headPtr->x == tailPtr->x || headPtr->y == tailPtr->y) {

        if (headPtr->x == tailPtr->x) {
          tailPtr->y += 1 * yDirection;
        }
        if (headPtr->y == tailPtr->y) {
          tailPtr->x += 1 * xDirection;
        }
      } else {
        // Diagonal
        tailPtr->x += 1 * xDirection;
        tailPtr->y += 1 * yDirection;
      }
    }
    // tail
    grid[tailPtr->y][tailPtr->x] = tailPtr->indicator;
    if (tailPtr->isTailKnot) {
      visited[tailPtr->y][tailPtr->x] = 1;
    }
    headPtr = tailPtr;
    tailPtr = tailPtr->tail;
  } while (tailPtr != NULL);
}

void lineHandler(char *line) {
  fputs("line: ", stdout);
  fputs(line, stdout);

  char *direction;
  int steps;
  direction = strtok(line, " ");
  steps = atoi(strtok(NULL, " "));

  fprintf(stdout, "MOVE: dir %s, steps %d\n", direction, steps);

  for (int i = 0; i < steps; i++) {
    doMove(direction[0]);
  }

  fputs("\n", stdout);
}

int main() {

  grid[HEIGHT / 2][WIDTH / 2] = 'H';
  readInput(__FILE__, lineHandler);
  printGrid();
  printVisited();
  int visitedCount = 0;
  for (int i = 0; i < HEIGHT; i++) {
    for (int j = 0; j < HEIGHT; j++) {
      if (visited[i][j] == 1) {
        visitedCount++;
      }
    }
  }

  fprintf(stdout, "Result: %d\n", visitedCount);
#ifdef PART_ONE
#ifdef TEST_MODE
  assert(visitedCount == 13);
#else
  assert(visitedCount == 5874);
#endif
#else
#ifdef TEST_MODE
  assert(visitedCount == 36);
#else
  assert(visitedCount == 2467);
#endif
#endif
}
