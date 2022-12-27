#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
// #define TEST_MODE
#include "../../utils.h"

#ifdef TEST_MODE
#define GRID_SIZE 50
#else
#define GRID_SIZE 10000
#endif

typedef struct vec_t {
  int x;
  int y;
  int dir;
} vec_t;

enum dir_t { NONE, NORTH, SOUTH, WEST, EAST };

vec_t **elves = NULL;
int elfCount = 0;

#define mapco(coord) ((GRID_SIZE / 2) + coord)
#define unmapco(coord) (coord - (GRID_SIZE / 2))
char grid[GRID_SIZE][GRID_SIZE] = {0}; // lazy

typedef struct move_check_t {
  bool (*check)(vec_t *elf);
  struct move_check_t *next;
  int dir;
} move_check_t;

move_check_t *firstMove;

void fileHandler(int lines) {
  fprintf(stdout, "lines: %d\n", lines);
  elves = calloc(lines * lines, sizeof(vec_t));
  // grid = calloc(lines * lines, sizeof(char));
}

int inputY = 0;
void lineHandler(char *line) {
  fprintf(stdout, "line: %s\n", line);

  int length = strlen(line);
  for (int i = 0; i < length; i++) {
    if (line[i] == '#') {
      vec_t *elf = malloc(sizeof(vec_t));
      elf->y = mapco(inputY);
      elf->x = mapco(i);
      elves[elfCount++] = elf;
    }
  }
  inputY++;
}

vec_t *findElf(int y, int x) {
  for (int i = 0; i < elfCount; i++) {
    vec_t *elf = elves[i];
    if (elf->y == y && elf->x == x) {
      return elf;
    }
  }
  return NULL;
}

bool northCheck(vec_t *elf) {
  vec_t *nElf = findElf(elf->y - 1, elf->x);
  vec_t *neElf = findElf(elf->y - 1, elf->x + 1);
  vec_t *nwElf = findElf(elf->y - 1, elf->x - 1);
  // If there is no Elf in the N, NE, or NW adjacent positions, the Elf
  // proposes moving north one step.
  if (!nElf && !neElf && !nwElf) {
    // north
    grid[elf->y - 1][elf->x]++;
    elf->dir = NORTH;
    // fprintf(stdout, " NORTH\n");
    return true;
  }
  return false;
}

bool southCheck(vec_t *elf) {
  vec_t *seElf = findElf(elf->y + 1, elf->x + 1);
  vec_t *sElf = findElf(elf->y + 1, elf->x);
  vec_t *swElf = findElf(elf->y + 1, elf->x - 1);

  // If there is no Elf in the S, SE, or SW adjacent positions, the Elf
  // proposes moving south one step.
  if (!sElf && !seElf && !swElf) {
    // south
    grid[elf->y + 1][elf->x]++;
    elf->dir = SOUTH;
    // fprintf(stdout, " SOUTH\n");
    return true;
  }
  return false;
}

bool westCheck(vec_t *elf) {
  vec_t *swElf = findElf(elf->y + 1, elf->x - 1);
  vec_t *wElf = findElf(elf->y, elf->x - 1);
  vec_t *nwElf = findElf(elf->y - 1, elf->x - 1);

  // If there is no Elf in the W, NW, or SW adjacent positions, the Elf
  // proposes moving west one step.
  if (!wElf && !nwElf && !swElf) {
    // west
    grid[elf->y][elf->x - 1]++;
    elf->dir = WEST;
    // fprintf(stdout, " WEST\n");
    return true;
  }
  return false;
}

bool eastCheck(vec_t *elf) {
  vec_t *neElf = findElf(elf->y - 1, elf->x + 1);
  vec_t *eElf = findElf(elf->y, elf->x + 1);
  vec_t *seElf = findElf(elf->y + 1, elf->x + 1);

  // If there is no Elf in the E, NE, or SE adjacent positions, the Elf
  // proposes moving east one step.
  if (!eElf && !neElf && !seElf) {
    // east
    grid[elf->y][elf->x + 1]++;
    elf->dir = EAST;
    // fprintf(stdout, " EAST\n");
    return true;
  }
  return false;
}

bool hasNeighbours(vec_t *elf) {
  vec_t *swElf = findElf(elf->y + 1, elf->x - 1);
  vec_t *wElf = findElf(elf->y, elf->x - 1);
  vec_t *nwElf = findElf(elf->y - 1, elf->x - 1);
  vec_t *neElf = findElf(elf->y - 1, elf->x + 1);
  vec_t *eElf = findElf(elf->y, elf->x + 1);
  vec_t *seElf = findElf(elf->y + 1, elf->x + 1);
  vec_t *nElf = findElf(elf->y - 1, elf->x);
  vec_t *sElf = findElf(elf->y + 1, elf->x);

  return nElf || neElf || eElf || seElf || sElf || swElf || wElf || nwElf;
}

bool tick() {
  bool hasMove = false;
  memset(grid, 0, GRID_SIZE * GRID_SIZE);
  // on each check tick go through the elves
  //  check where if could move to
  //  increment the space counter by 1
  for (int i = 0; i < elfCount; i++) {
    vec_t *elf = elves[i];
    // fprintf(stdout, "\nelf %d (%d,%d): dir %d", i, elf->y, elf->x, elf->dir);

    move_check_t *move = firstMove;
    for (int j = 0; j < 4; j++) {
      if (!hasNeighbours(elf)) {
        break;
      }

      if (move->check(elf)) {
        break;
      }
      move = move->next;
    }
  }
  // rotate to next move after all elves checked
  firstMove = firstMove->next;

  // on each move tick go through the elves
  //  check their desired space
  //  if counter is 1 then move there
  for (int i = 0; i < elfCount; i++) {
    vec_t *elf = elves[i];

    int dY = 0;
    int dX = 0;
    switch (elf->dir) {
    case NORTH:
      dY = -1;
      break;
    case SOUTH:
      dY = 1;
      break;
    case WEST:
      dX = -1;
      break;
    case EAST:
      dX = 1;
      break;
    case NONE:
      // elf does not want to move
      continue;
      break;
    }

    int space = grid[elf->y + dY][elf->x + dX];
    // fprintf(stdout, "elf %d (%d,%d): dir %d - %d\n", i, elf->y, elf->x,
    // elf->dir, space);
    if (space == 1) {
      hasMove = true;
      elf->y += dY;
      elf->x += dX;
    }
    elf->dir = NONE;
  }

  return hasMove;
}

void printGrid() {
  int minY = INT_MAX;
  int maxY = 0;
  int minX = INT_MAX;
  int maxX = 0;
  for (int i = 0; i < elfCount; i++) {
    vec_t *elf = elves[i];
    minY = min(minY, elf->y);
    maxY = max(maxY, elf->y);
    minX = min(minX, elf->x);

    maxX = max(maxX, elf->x);
  }

  minY = unmapco(minY);
  maxY = unmapco(maxY);
  minX = unmapco(minX);
  maxX = unmapco(maxX);

  fprintf(stdout, "minY %d, maxY %d, minX %d, maxX %d\n", minY, maxY, minX,
          maxX);

  for (int y = minY; y <= maxY; y++) {
    for (int x = minX; x <= maxX; x++) {
      vec_t *elf = findElf(mapco(y), mapco(x));
      if (elf != NULL) {
        fputc('#', stdout);
      } else {
        fputc('.', stdout);
      }
    }
    fputc('\n', stdout);
  }
}

void setupChecks() {
  move_check_t *north = malloc(sizeof(move_check_t));
  move_check_t *south = malloc(sizeof(move_check_t));
  move_check_t *west = malloc(sizeof(move_check_t));
  move_check_t *east = malloc(sizeof(move_check_t));

  north->dir = NORTH;
  south->dir = SOUTH;
  west->dir = WEST;
  east->dir = EAST;

  north->check = northCheck;
  south->check = southCheck;
  west->check = westCheck;
  east->check = eastCheck;

  north->next = south;
  south->next = west;
  west->next = east;
  east->next = north;

  firstMove = north;
}

int countEmpty() {
  int minY = INT_MAX;
  int maxY = 0;
  int minX = INT_MAX;
  int maxX = 0;
  for (int i = 0; i < elfCount; i++) {
    vec_t *elf = elves[i];
    minY = min(minY, elf->y);
    maxY = max(maxY, elf->y);
    minX = min(minX, elf->x);

    maxX = max(maxX, elf->x);
  }

  minY = unmapco(minY);
  maxY = unmapco(maxY);
  minX = unmapco(minX);
  maxX = unmapco(maxX);

  fprintf(stdout, "minY %d, maxY %d, minX %d, maxX %d\n", minY, maxY, minX,
          maxX);

  int totalSpaces = ((maxY - minY) + 1) * ((maxX - minX) + 1);
  fprintf(stdout, "height %d, width %d, total %d\n", maxY - minY, maxX - minX,
          totalSpaces);
  return totalSpaces - elfCount;
}

int main() {
  readInputFile(__FILE__, lineHandler, fileHandler);

  setupChecks();

  memset(grid, 0, GRID_SIZE * GRID_SIZE);
  // printGrid();

  int rounds = 0;
  for (; rounds < 10; rounds++) {
    tick();
    // printGrid();
  }
  // printGrid();

  int spaces = countEmpty();
  fprintf(stdout, "Part one: %d\n", spaces);
#ifdef TEST_MODE
  assert(spaces == 110);
#else
  assert(spaces == 4109);
#endif

  while (rounds++) {
    bool hasMove = tick();
    if (!hasMove) {
      break;
    }
  }

  fprintf(stdout, "Part two: %d\n", rounds);
#ifdef TEST_MODE
  assert(rounds == 20);
#else
  assert(rounds == 1055);
#endif
}
