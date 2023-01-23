#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
// I broke test mode oops
// #define TEST_MODE
#include "../../utils.h"

#ifdef TEST_MODE
#define HEIGHT 120
#define WIDTH 120
#else
#define HEIGHT 4096
#define WIDTH 4096
#endif

char grid[HEIGHT][WIDTH];
int visited1[HEIGHT][WIDTH];
int visited2[HEIGHT][WIDTH];

typedef struct knot_t {
  int x;
  int y;
  struct knot_t *tail;
  bool isTailKnot;
  char indicator;
} knot_t;

// PART ONE

knot_t tail = {.isTailKnot = true,
               .x = HEIGHT / 2,
               .y = WIDTH / 2,
               .tail = NULL,
               .indicator = 'T'};
knot_t head1 = {.isTailKnot = false,
                .x = HEIGHT / 2,
                .y = WIDTH / 2,
                .tail = &tail,
                .indicator = 'H'};

// PART TWO
knot_t tail9 = {.isTailKnot = true,
                .x = HEIGHT / 2,
                .y = WIDTH / 2,
                .tail = NULL,
                .indicator = '9'};
knot_t tail8 = {.isTailKnot = false,
                .x = HEIGHT / 2,
                .y = WIDTH / 2,
                .tail = &tail9,
                .indicator = '8'};
knot_t tail7 = {.isTailKnot = false,
                .x = HEIGHT / 2,
                .y = WIDTH / 2,
                .tail = &tail8,
                .indicator = '7'};
knot_t tail6 = {.isTailKnot = false,
                .x = HEIGHT / 2,
                .y = WIDTH / 2,
                .tail = &tail7,
                .indicator = '6'};
knot_t tail5 = {.isTailKnot = false,
                .x = HEIGHT / 2,
                .y = WIDTH / 2,
                .tail = &tail6,
                .indicator = '5'};
knot_t tail4 = {.isTailKnot = false,
                .x = HEIGHT / 2,
                .y = WIDTH / 2,
                .tail = &tail5,
                .indicator = '4'};
knot_t tail3 = {.isTailKnot = false,
                .x = HEIGHT / 2,
                .y = WIDTH / 2,
                .tail = &tail4,
                .indicator = '3'};
knot_t tail2 = {.isTailKnot = false,
                .x = HEIGHT / 2,
                .y = WIDTH / 2,
                .tail = &tail3,
                .indicator = '2'};
knot_t tail1 = {.isTailKnot = false,
                .x = HEIGHT / 2,
                .y = WIDTH / 2,
                .tail = &tail2,
                .indicator = '1'};
knot_t head2 = {.isTailKnot = false,
                .x = HEIGHT / 2,
                .y = WIDTH / 2,
                .tail = &tail1,
                .indicator = 'H'};

void printVisited(int visited[HEIGHT][WIDTH]) {
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

void moveRope(knot_t *head, int visited[HEIGHT][WIDTH], char direction) {
#ifdef TEST_MODE
  fprintf(stdout, "MOVE: dir %c, knot (%d,%d), tail (%d,%d)[%c]\n", direction,
          head->x, head->y, head->tail->x, head->tail->y,
          head->tail->indicator);
#endif
  int dX = 0;
  int dY = 0;
  switch (direction) {
  case 'R':
    dX = 1;
    dY = 0;
    break;
  case 'U':
    dX = 0;
    dY = -1;
    break;
  case 'L':
    dX = -1;
    dY = 0;
    break;
  case 'D':
    dX = 0;
    dY = 1;
    break;
  }
  int newHeadX = head->x + dX;
  int newHeadY = head->y + dY;
// set new position
#ifdef TEST_MODE
  grid[head->y][head->x] = ' ';
#endif
  head->x = newHeadX;
  head->y = newHeadY;
#ifdef TEST_MODE
  grid[head->y][head->x] = head->indicator;
#endif

  struct knot_t *headPtr = head;
  struct knot_t *tailPtr = head->tail;

  do {
#ifdef TEST_MODE
    fprintf(stdout, "MOVE HEAD: head (%d,%d) - %c\n", headPtr->x, headPtr->y,
            headPtr->indicator);
    fprintf(stdout, "MOVE TAIL: tail (%d,%d) - %c\n", tailPtr->x, tailPtr->y,
            tailPtr->indicator);
#endif

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
#ifdef TEST_MODE
    grid[tailPtr->y][tailPtr->x] = tailPtr->indicator;
#endif
    if (tailPtr->isTailKnot) {
      visited[tailPtr->y][tailPtr->x] = 1;
    }
    headPtr = tailPtr;
    tailPtr = tailPtr->tail;
  } while (tailPtr != NULL);
}

void lineHandler(char *line) {
  fprintf(stdout, "line: %s\n", line);

  char *direction;
  int steps;
  direction = strtok(line, " ");
  steps = atoi(strtok(NULL, " "));

#ifdef TEST_MODE
  fprintf(stdout, "MOVE: dir %s, steps %d\n", direction, steps);
#endif

  for (int i = 0; i < steps; i++) {
    // TODO could probably be optimised into a single loop with a check on the
    // second rope knot and treat it as the end of the part one rope
    moveRope(&head1, visited1, direction[0]);
    moveRope(&head2, visited2, direction[0]);
  }
}

int main() {
  grid[HEIGHT / 2][WIDTH / 2] = 'H';
  readInput(__FILE__, lineHandler);
#ifdef TEST_MODE
  printGrid();
  printVisited(visited1);
  printVisited(visited2);
#endif

  int visitedCountP1 = 0;
  for (int i = 0; i < HEIGHT; i++) {
    for (int j = 0; j < HEIGHT; j++) {
      if (visited1[i][j] == 1) {
        visitedCountP1++;
      }
    }
  }

  fprintf(stdout, "Part one: %d\n", visitedCountP1);

  int visitedCountP2 = 0;
  for (int i = 0; i < HEIGHT; i++) {
    for (int j = 0; j < HEIGHT; j++) {
      if (visited2[i][j] == 1) {
        visitedCountP2++;
      }
    }
  }

  fprintf(stdout, "Part two: %d\n", visitedCountP2);
#ifdef TEST_MODE
  assert(visitedCountP1 == 13);
#else
  assert(visitedCountP1 == 5874);
#endif

#ifdef TEST_MODE
  assert(visitedCountP2 == 36);
#else
  assert(visitedCountP2 == 2467);
#endif
}
