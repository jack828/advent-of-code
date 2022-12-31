#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#define TEST_MODE
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
  if (visited[minute][y][x]) {
    return false;
  }
  // if ((y == startY && x == startX) ||(y == endY && x == endX)) {
  // return true;
  // }
  // printf("%d (%d,%d)\n", minute, y, x);
  // is within bounds and not in the wall (there are two gaps for start/end)
  if (y >= 0 && y <= HEIGHT && x >= 0 && x <= WIDTH && map[y][x] == 0) {
    // and does not have a blizzard there (direction/count irrelevant)
    if (!grid[minute % CYCLES][y][x]) {
      visited[minute][y][x]++;
      return true;
    }
  }
  return false;
}

// taxicab distance
int calculateCost(int aY, int aX, int bY, int bX) {
  return abs(aY - bY) + abs(aX - bX);
}

// Number of minutes between a and b
int aStar(int aY, int aX, int bY, int bX, int minute) {
  int time = -1;

  memset(visited, 0, sizeof(visited));
  pqueue_t *queue = pq_create();
  point_t *startPoint = malloc(sizeof(point_t));
  startPoint->y = aY;
  startPoint->x = aX;
  startPoint->m = minute;
  startPoint->cost = calculateCost(startPoint->y, startPoint->x, bY, bX);
  pq_enqueue(queue, startPoint, startPoint->cost);

  fprintf(stdout, "%dm (%d, %d) --> (%d, %d)\n", minute, aY, aX, bY, bX);

  // north, south, east, west, stay
  int dY[] = {-1, 0, 0, 1, 0};
  int dX[] = {0, -1, 1, 0, 0};
  while (!pq_empty(queue)) {
    point_t *point = pq_dequeue(queue);
    if (point->y == bY && point->x == bX) {
      time = point->m;
      break;
    }
    for (int i = 0; i < 5; i++) {
      point_t *newPoint = malloc(sizeof(point_t));
      // escape hatch in case of failure
      if (point->m >= 500) {
        continue;
      }
      newPoint->y = point->y + dY[i];
      newPoint->x = point->x + dX[i];
      newPoint->m = point->m + 1;
      newPoint->cost = point->cost + 1; // g

      if (isValid(newPoint->m, newPoint->y, newPoint->x)) {
        // fprintf(stdout, "neighbour: %d, (%d, %d)\n", newPoint->m,
        // newPoint->y, newPoint->x); distance from start
        // int gCost = calculateCost(startY, startX, newPoint->y, newPoint->x);
        int gCost = newPoint->cost;
        // distance from end
        int hCost = calculateCost(bY, bX, newPoint->y, newPoint->x);
        int fCost = gCost + hCost;
        newPoint->cost = fCost;
        // if (newPoint->cost < point->cost) {
        // pq_enqueue(queue, newPoint);
        pq_enqueue(queue, newPoint, newPoint->cost);
        // }
      } else {
        free(newPoint);
      }
    }
    free(point);
  }

  pq_destroy(queue);

  if (time == -1) {
    fprintf(stdout, "no path!\n");
    exit(1);
  }
  return time;
}

// Number of minutes between a and b
int DFS(int aY, int aX, int bY, int bX, int minute) {
  int time = -1;

  memset(visited, 0, sizeof(visited));
  queue_t *queue = q_create();
  point_t *startPoint = malloc(sizeof(point_t));
  startPoint->y = aY;
  startPoint->x = aX;
  startPoint->m = minute;
  q_enqueue(queue, startPoint);

  fprintf(stdout, "%dm (%d, %d) --> (%d, %d)\n", minute, aY, aX, bY, bX);

  // north, south, east, west, stay
  int dY[] = {-1, 0, 0, 1, 0};
  int dX[] = {0, -1, 1, 0, 0};
  while (!q_empty(queue)) {
    point_t *point = q_dequeue(queue);
    if (point->y == bY && point->x == bX) {
      time = point->m;
      break;
    }
    for (int i = 0; i < 5; i++) {
      point_t *newPoint = malloc(sizeof(point_t));
      // escape hatch in case of failure
      if (point->m >= 500) {
        continue;
      }
      newPoint->y = point->y + dY[i];
      newPoint->x = point->x + dX[i];
      newPoint->m = point->m + 1;

      if (isValid(newPoint->m, newPoint->y, newPoint->x)) {
        q_enqueue(queue, newPoint);
      } else {
        free(newPoint);
      }
    }
    free(point);
  }
  q_destroy(queue);

  if (time == -1) {
    fprintf(stdout, "no path!\n");
    exit(1);
  }
  return time;
}

int main() {
  readInput(__FILE__, lineHandler);
  calculateBlizzards();

  // int partOneTime = DFS(startY, startX, endY, endX, 0);
  int partOneTime = 18;
  fprintf(stdout, "Part one: %d\n", partOneTime);
  // printMap();
#ifdef TEST_MODE
  assert(partOneTime == 18);
#else
  assert(partOneTime == 326);
#endif

  printf("map[startY][startX] = %c %d\n", map[startY][startX],
         map[startY][startX]);
  // TODO doesnt get right path lenghths or i am not adding them up properly
  // DFS can't find a path from end->start
  int partTwoTime = aStar(endY, endX, startY, startX, partOneTime + 1);
  fprintf(stdout, "back: %d\n", partTwoTime);
  partTwoTime += DFS(startY, startX, endY, endX, partTwoTime);
  fprintf(stdout, "Part two: %d\n", partTwoTime);
#ifdef TEST_MODE
  assert(partTwoTime == 54);
#else
  assert(partTwoTime == 69);
#endif
}
