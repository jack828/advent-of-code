#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
// #define TEST_MODE
#include "../../lib/pqueue.h"
#include "../../lib/queue.h"
#include "../../utils.h"

#ifdef TEST_MODE
#define HEIGHT 6
#define WIDTH 8
#define CYCLES 12
#else
#define HEIGHT 22
#define WIDTH 152
#define CYCLES 300 // lcm(HEIGHT-2, WIDTH-2) - excl walls
#endif

char grid[CYCLES][HEIGHT][WIDTH] = {0};
char map[HEIGHT][WIDTH] = {0};
char visited[CYCLES * CYCLES][HEIGHT][WIDTH] = {0};

typedef struct {
  int y;
  int x;
  int m;
  int cost;
} point_t;

typedef struct {
  int y;
  int x;
  char c;
} blizzard_t;

int startY = 0;
int startX = 1;
int endY = HEIGHT - 1;
int endX = WIDTH - 2;

blizzard_t *blizzards[HEIGHT * WIDTH] = {0};
int blizzardCount = 0;

int lineY = 0;
void lineHandler(char *line) {
  fprintf(stdout, "line: %s\n", line);

  int length = strlen(line);
  for (int i = 0; i < length; i++) {
    char c = line[i];
    switch (c) {
    case '^':
    case '>':
    case 'v':
    case '<': {
      // blizzard
      blizzard_t *blizzard = malloc(sizeof(blizzard_t));
      blizzard->y = lineY;
      blizzard->x = i;
      blizzard->c = c;
      blizzards[blizzardCount++] = blizzard;
      grid[0][lineY][i] = line[i];
      break;
    }
    case '.':
      // space
      grid[0][lineY][i] = 0;
      map[lineY][i] = 0;
      break;
    case '#':
      // wall
      grid[0][lineY][i] = line[i];
      map[lineY][i] = line[i];
      break;
    }
  }
  lineY++;
}

void tick() {
  // move blizzards
  for (int i = 0; i < blizzardCount; i++) {
    blizzard_t *blizzard = blizzards[i];
    int dY = 0;
    int dX = 0;
    switch (blizzard->c) {
    case '^':
      dY = -1;
      break;
    case '>':
      dX = 1;
      break;
    case 'v':
      dY = 1;
      break;
    case '<':
      dX = -1;
      break;
    }

    int newY = blizzard->y + dY;
    int newX = blizzard->x + dX;
    if (newY >= HEIGHT - 1) {
      newY = 1;
    } else if (newY < 1) {
      newY = HEIGHT - 2;
    }
    if (newX >= WIDTH - 1) {
      newX = 1;
    } else if (newX < 1) {
      newX = WIDTH - 2;
    }
    blizzard->y = newY;
    blizzard->x = newX;
  }
}

void calculateBlizzards() {
  for (int cycle = 0; cycle < CYCLES; cycle++) {
    for (int i = 0; i < blizzardCount; i++) {
      blizzard_t *blizzard = blizzards[i];
      grid[cycle][blizzard->y][blizzard->x] = blizzard->c;
    }
    tick();
  }
}

void printGrid(int minute) {
  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {
      if (startY == y && startX == x) {
        fputc('S', stdout);
        continue;
      }
      fputc(grid[minute][y][x] ? grid[minute][y][x] : '.', stdout);
    }
    fputs("\n", stdout);
  }
  fputs("\n", stdout);
}

void printMap() {
  for (int y = 0; y < HEIGHT; y++) {
    if (y == endY) {
      fputc('E', stdout);
    } else {
      fputc(' ', stdout);
    }
    for (int x = 0; x < WIDTH; x++) {
      if (y == endY && x == endX) {
        fputs("E", stdout);

      } else {
        fputc(map[y][x] ? map[y][x] : '.', stdout);
      }
    }
    fputs("\n", stdout);
  }
  fputs("\n", stdout);
}

bool isValid(int minute, int y, int x) {
  // fprintf(stdout, "isvalid %d, (%d, %d) = \n", minute, y, x);
  if (visited[minute][y][x]) {
    return false;
  }
  if (y == endY && x == endX) {
    // fprintf(stdout, "exit found4!\n\n");
    // fprintf(stdout, "y >= 0\t\t %d\n", y >= 0);
    // fprintf(stdout, "y <= HEIGHT\t %d\n", y <= HEIGHT);
    // fprintf(stdout, "x >= 0\t\t %d\n", x >= 0);
    // fprintf(stdout, "x <= WIDTH\t %d\n", x <= WIDTH);
    // fprintf(stdout, "map[y][x]\t %c\n", map[y][x]);
    // return true;
    // exit(1);
  }
  // is within bounds and not in the wall (there are two gaps for start/end)
  if (y >= 0 && y <= HEIGHT && x >= 0 && x <= WIDTH && map[y][x] == 0) {
    // and does not have a blizzard there (direction/count irrelevant)
    // fprintf(stdout, "in bound, grid: %c\n", grid[minute][y][x]);
    if (!grid[minute % CYCLES][y][x]) {
      // && visited[0][y][x] < 4) {
      visited[minute][y][x]++;
      // fprintf(stdout, "TRUE %c %d\n", grid[minute + 1 % CYCLES][y][x],
      return true;
    }
  }
  // fprintf(stdout, "false %c %d\n", grid[minute + 1 % CYCLES][y][x],
  // grid[minute + 1 % CYCLES][y][x]);
  // fprintf(stdout, "FALSE\n");
  return false;
}

// taxicab distance
int calculateCost(int aY, int aX, int bY, int bX) {
  return abs(aY - bY) + abs(aX - bX);
}

int main() {
  readInput(__FILE__, lineHandler);
  calculateBlizzards();

  queue_t *queue = q_create();
  point_t *startPoint = malloc(sizeof(point_t));
  startPoint->y = startY;
  startPoint->x = startX;
  startPoint->m = 0;
  // startPoint->y = 5;
  // startPoint->x = 6;
  // startPoint->m = 2;
  startPoint->cost = calculateCost(endY, endX, startPoint->y, startPoint->x);
  q_enqueue(queue, startPoint);

  point_t *startPoint2 = malloc(sizeof(point_t));
  startPoint2->y = 20;
  startPoint2->x = 151;
  startPoint2->m = 1;
  // q_enqueue(queue, startPoint2);
  // q_enqueue(yQueue, 5);
  // q_enqueue(xQueue, 6);
  // q_enqueue(mQueue, 2);

  fprintf(stdout, "end is at (%d, %d)\n", endY, endX);

  // north, south, east, west, stay
  int dY[] = {-1, 0, 0, 1, 0};
  int dX[] = {0, -1, 1, 0, 0};
  int time = 0;
  int loop = 0;
  while (!q_empty(queue)) {
    point_t *point = q_dequeue(queue);
    // fprintf(stdout, "loop: %d - %d, (%d, %d)\n", loop++, point->m, point->y,
    // point->x);
    // if (loop > 100) break;
    // fprintf(stdout, "loop %d, (%d, %d)\n", m, y, x);
    // dont ask my why there's two
    // i think it's because i couldn't get the bounds check quite right
    if (point->y == endY && point->x == endX) {
      fprintf(stdout, "exit found1!\n\n");
      time = point->m;
      break;
    }
    // if (point->y + 1 == endY && point->x == endX) {
    //   fprintf(stdout, "exit found2!\n\n");
    //   time = point->m - 1;
    //   break;
    // }
    for (int i = 0; i < 5; i++) {
      point_t *newPoint = malloc(sizeof(point_t));
      if (point->m >= 500) {
        continue;
      }
      newPoint->y = point->y + dY[i];
      newPoint->x = point->x + dX[i];
      newPoint->m = point->m + 1;
      newPoint->cost = point->cost + 1; // g

      if (isValid(newPoint->m, newPoint->y, newPoint->x)) {
        // fprintf(stdout, "neighbour: %d, (%d, %d)\n", newPoint->m,
        // newPoint->y, newPoint->x); distance from start int gCost =
        // calculateCost(startY, startX, newPoint->y, newPoint->x);
        // int gCost = newPoint->cost;
        // distance from end
        // int hCost = calculateCost(endY, endX, newPoint->y, newPoint->x);
        // int fCost = gCost + hCost;
        // newPoint->cost = hCost;
        /* fprintf(stdout, "old m %d, c: %d\n", point->m, point->cost);
        fprintf(stdout,
                "m %d, c: %d, gCost: %d, hCost: %d, fCost: "
                "%d\n",
                newPoint->m, newPoint->cost, gCost, hCost, fCost); */
        // if (newPoint->cost < point->cost) {
        // fprintf(stdout, "cost ok\n");
        q_enqueue(queue, newPoint);
        // }
      } else {
        free(newPoint);
      }
    }
    free(point);
  }

  // printGrid(2);
  /*
  for (int cycle = 0; cycle < CYCLES; cycle++) {
    fprintf(stdout, "cycle: %d\n", cycle);
    for (int y = 0; y < HEIGHT; y++) {
      for (int x = 0; x < WIDTH; x++) {
        fputc(grid[cycle][y][x] ? grid[cycle][y][x] : '.', stdout);
      }
      fputc('\n', stdout);
    }
    fputc('\n', stdout);
  }
  */

  fprintf(stdout, "end is at (%d, %d) - %c %d\n", endY, endX, map[endY][endX],
          map[endY][endX]);
  fprintf(stdout, "Part one: %d\n", time);
  // printMap();
#ifdef TEST_MODE
  assert(time == 18);
#else
  assert(time == 326);
#endif

  fprintf(stdout, "Part two: %d\n", 420);
#ifdef TEST_MODE
  assert(420 == 69);
#else
  // assert(420 == 69);
#endif
}
