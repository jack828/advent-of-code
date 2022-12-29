#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#define TEST_MODE
#include "../../utils.h"

#ifdef TEST_MODE
#define HEIGHT 6
#define WIDTH 8
#else
#define HEIGHT 22
#define WIDTH 152
#endif

char grid[HEIGHT][WIDTH] = {0};

typedef struct {
  int y;
  int x;
  char c;
} blizzard_t;

typedef struct path_t {
  int y;
  int x;
  int minute;
  struct path_t *prev;
  bool processed;
} path_t;

int startY = 0;
int startX = 1;
int endY = HEIGHT;
int endX = WIDTH - 1;

int lineY = 0;
// probably will not need this many but cba to deal with dynamic-ness
path_t *paths[HEIGHT * WIDTH * 2] = {0};
int pathCount = 0;
int shortestPath = 0;
blizzard_t *blizzards[HEIGHT * WIDTH] = {0};
int blizzardCount = 0;

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

      line[i] = '.';
      break;
    }
    case '.':
      // space
      break;
    case '#':
      // wall
      break;
    }
  }
  strcat(grid[lineY++], line);
}

blizzard_t *getBlizzard(int y, int x) {
  for (int i = 0; i < blizzardCount; i++) {
    blizzard_t *blizzard = blizzards[i];
    if (blizzard->y == y && blizzard->x == x) {
      return blizzard;
    }
  }
  return NULL;
}

int countBlizzards(int y, int x) {
  int count = 0;
  for (int i = 0; i < blizzardCount; i++) {
    blizzard_t *blizzard = blizzards[i];
    if (blizzard->y == y && blizzard->x == x) {
      count++;
    }
  }
  return count;
}

void printGrid(path_t *path) {
  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {
      if (startY == y && startX == x) {
        fputc('S', stdout);
        continue;
      }
      if (path->y == y && path->x == x) {
        fputc('x', stdout);
        continue;
      }
      int numBlizzards = countBlizzards(y, x);
      if (numBlizzards == 1) {
        blizzard_t *blizzard = getBlizzard(y, x);
        fputc(blizzard->c, stdout);
      } else if (numBlizzards > 1) {
        fputc(numBlizzards + 48, stdout);
      } else {
        fputc(grid[y][x], stdout);
      }
    }
    fputs("\n", stdout);
  }
  fputs("\n", stdout);
}

// returns true if we haven't visited this place at most <limit> moves ago
// and there isn't a better path already there
bool shouldVisit(path_t *path, int y, int x) {
  bool recentlyVisited = false;
  int limit = 0;
  path_t *movement = path->prev;
  while (limit && movement != NULL) {

    // fprintf(stdout, "Path minute: %d visited (%d,%d) == (%d,%d)\n",
    // movement->minute, movement->y, movement->x, y, x);
    if (movement->y == y && movement->x == x) {
      recentlyVisited = true;
      break;
    }
    movement = movement->prev;
    limit--;
  }
  bool hasBetterPath = false;
  int count = pathCount;
  for (int i = 0; i < count; i++) {
    path_t *path = paths[i];
    if (path->y == y && path->x == x) {
      recentlyVisited = true;
      break;
    }
  }
  return !recentlyVisited || !hasBetterPath;
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

void findPath(path_t *path) {
  if (path->processed) {
    // ignore all but the head of a path
    return;
  }
  fprintf(stdout, "Path minute: %d\n", path->minute);
  // check for new available paths
  // fork for each one and increment minute
  // ...isnt each step in the path a minute increment?

  // check if we can move to the end
  if (path->y + 1 == endY && path->x == endX) {
    fprintf(stdout, "exit found!\n\n");
    shortestPath = path->minute + 1;
    return;
  }
  // check each available place
  // north
  if (path->y - 1 > 0) {
    if (!getBlizzard(path->y - 1, path->x) &&
        shouldVisit(path, path->y - 1, path->x)) {
      fprintf(stdout, "Path minute: %d - north\n", path->minute);
      // move there!
      path_t *newPath = malloc(sizeof(path_t));
      newPath->processed = false;
      newPath->y = path->y - 1;
      newPath->x = path->x;
      newPath->prev = path;
      newPath->minute = path->minute + 1;
      paths[pathCount++] = newPath;
      // findPath(newPath);
    }
  }
  // south
  if (path->y + 1 < HEIGHT - 1) {
    if (!getBlizzard(path->y + 1, path->x) &&
        shouldVisit(path, path->y + 1, path->x)) {
      fprintf(stdout, "Path minute: %d - south\n", path->minute);
      // move there!
      path_t *newPath = malloc(sizeof(path_t));
      newPath->processed = false;
      newPath->y = path->y + 1;
      newPath->x = path->x;
      newPath->prev = path;
      newPath->minute = path->minute + 1;
      paths[pathCount++] = newPath;
      // findPath(newPath);
    }
  }
  // west
  if (path->y > 0 && path->x - 1 > 0) {
    if (!getBlizzard(path->y, path->x - 1) &&
        shouldVisit(path, path->y, path->x - 1)) {
      fprintf(stdout, "Path minute: %d - west\n", path->minute);
      // move there!
      path_t *newPath = malloc(sizeof(path_t));
      newPath->processed = false;
      newPath->y = path->y;
      newPath->x = path->x - 1;
      newPath->prev = path;
      newPath->minute = path->minute + 1;
      paths[pathCount++] = newPath;
      // findPath(newPath);
    }
  }
  // east
  if (path->y > 0 && path->x + 1 < WIDTH - 1) {
    if (!getBlizzard(path->y, path->x + 1) &&
        shouldVisit(path, path->y, path->x + 1)) {
      fprintf(stdout, "Path minute: %d - east\n", path->minute);
      // move there!
      path_t *newPath = malloc(sizeof(path_t));
      newPath->processed = false;
      newPath->y = path->y;
      newPath->x = path->x + 1;
      newPath->prev = path;
      newPath->minute = path->minute + 1;
      paths[pathCount++] = newPath;
      // findPath(newPath);
    }
  }
  // stay
  if (path->y > 0 && !getBlizzard(path->y, path->x) &&
      shouldVisit(path, path->y, path->x)) {
    fprintf(stdout, "Path minute: %d - stay\n", path->minute);
    // but wait
    path_t *newPath = malloc(sizeof(path_t));
    newPath->processed = false;
    newPath->y = path->y;
    newPath->x = path->x;
    newPath->prev = path;
    newPath->minute = path->minute + 1;
    paths[pathCount++] = newPath;
    // findPath(newPath);
  }
  path->processed = true;
}

int main() {
  readInput(__FILE__, lineHandler);

  path_t *path = malloc(sizeof(path_t));
  path->x = startX;
  path->y = startY;
  path->minute = 1;
  paths[pathCount++] = path;
  printGrid(path);

  fprintf(stdout, "paths: %d\n", pathCount);
  int steps = 0;
  while (shortestPath == 0) {
    tick();
    int count = pathCount;
    for (int i = 0; i < count; i++) {
      path_t *path = paths[i];
      findPath(path);
    }
    if (steps > 17) {
    break;
    }
    steps++;
    fprintf(stdout, "step: %d, paths: %d\n", steps, pathCount);
    printGrid(paths[pathCount - 1]);
  }
  fprintf(stdout, "paths: %d\n", pathCount);
  printGrid(paths[pathCount - 6]);
  // tick();

  fprintf(stdout, "Part one: %d\n", 69);
#ifdef TEST_MODE
  assert(69 == 420);
#else
  assert(69 == 420);
#endif

  fprintf(stdout, "Part two: %d\n", 420);
#ifdef TEST_MODE
  assert(420 == 69);
#else
  assert(420 == 69);
#endif
}
