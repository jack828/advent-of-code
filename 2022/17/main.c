#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#define TEST_MODE
#include "../../utils.h"

typedef struct move_t {
  int direction;
  int index;
  struct move_t *next;
} move_t;

move_t *moves = NULL;
int numberOfMoves = 0;

typedef struct rock_t {
  char *shape;
  int height;
  int width;
  char character;
  struct rock_t *next;
  int index;
} rock_t;

rock_t *rock1 = NULL;
rock_t *rock2 = NULL;
rock_t *rock3 = NULL;
rock_t *rock4 = NULL;
rock_t *rock5 = NULL;

#define HEIGHT 40000000
#define WIDTH 7
char grid[HEIGHT][WIDTH] = {0};
int highestRock = 0;
int numberOfRocks = 5;
move_t *currentMove = NULL;
rock_t *currentRock = NULL;

void lineHandler(char *line) {
  fputs("line: ", stdout);
  fputs(line, stdout);

  int length = strlen(line) - 1;

  moves = malloc(sizeof(move_t));
  moves->direction = line[0] == '>' ? 1 : -1;
  moves->index = 0;

  move_t *current = moves;
  for (int i = 1; i < length; i++) {
    move_t *next = malloc(sizeof(move_t));
    next->direction = line[i] == '>' ? 1 : -1;
    next->index = i;
    current->next = next;
    current = next;
  }
  current->next = moves;
  numberOfMoves = length;
  fputs("\n", stdout);
}

void printMoves() {
  move_t *move = moves;
  do {
    fputc(move->direction == 1 ? '>' : '<', stdout);
    if (move->index == numberOfMoves - 1) {
      break;
    }
  } while ((move = move->next) != NULL);
  fputc('\n', stdout);
}

void setupRocks() {
  // ####
  rock1 = malloc(sizeof(rock_t));
  rock1->index = 0;
  rock1->character = '1';
  rock1->shape = calloc(4, sizeof(char));
  rock1->shape[0] = 1;
  rock1->shape[1] = 1;
  rock1->shape[2] = 1;
  rock1->shape[3] = 1;
  rock1->height = 1;
  rock1->width = 4;

  // .#.
  // ###
  // .#.
  rock2 = malloc(sizeof(rock_t));
  rock2->index = 1;
  rock2->character = '2';
  rock2->shape = calloc(9, sizeof(char));
  rock2->shape[0] = 0;
  rock2->shape[1] = 1;
  rock2->shape[2] = 0;
  rock2->shape[3] = 1;
  rock2->shape[4] = 1;
  rock2->shape[5] = 1;
  rock2->shape[6] = 0;
  rock2->shape[7] = 1;
  rock2->shape[8] = 0;
  rock2->height = 3;
  rock2->width = 3;

  // ..#
  // ..#
  // ###
  rock3 = malloc(sizeof(rock_t));
  rock3->index = 2;
  rock3->character = '3';
  rock3->shape = calloc(9, sizeof(char));
  rock3->shape[0] = 1;
  rock3->shape[1] = 1;
  rock3->shape[2] = 1;
  rock3->shape[3] = 0;
  rock3->shape[4] = 0;
  rock3->shape[5] = 1;
  rock3->shape[6] = 0;
  rock3->shape[7] = 0;
  rock3->shape[8] = 1;
  rock3->height = 3;
  rock3->width = 3;

  // #
  // #
  // #
  // #
  rock4 = malloc(sizeof(rock_t));
  rock4->index = 3;
  rock4->character = '4';
  rock4->shape = calloc(12, sizeof(char));
  rock4->shape[0] = 1;
  rock4->shape[4] = 1;
  rock4->shape[8] = 1;
  rock4->shape[12] = 1;
  rock4->height = 4;
  rock4->width = 1;

  // ##
  // ##
  rock5 = malloc(sizeof(rock_t));
  rock5->index = 4;
  rock5->character = '5';
  rock5->shape = calloc(4, sizeof(char));
  rock5->shape[0] = 1;
  rock5->shape[1] = 1;
  rock5->shape[2] = 1;
  rock5->shape[3] = 1;
  rock5->height = 2;
  rock5->width = 2;

  // circulaaar
  rock1->next = rock2;
  rock2->next = rock3;
  rock3->next = rock4;
  rock4->next = rock5;
  rock5->next = rock1;
  currentRock = rock1;
}

void dropRock() {
  // Each rock appears so that its left edge is two units away from the left
  // wall and its bottom edge is three units above the highest rock in the room
  // (or the floor, if there isn't one).
  rock_t *rock = currentRock;
  currentRock = currentRock->next;
  int rockY = highestRock + 3;
  int rockX = 2;
  bool stopped = false;

  do {
    // After a rock appears, it alternates between being pushed by a jet of hot
    // gas one unit and then falling one unit down.

    // fprintf(stdout, "rock %c moving (%d,%d), height=%d, width=%d\n",
    // rock->character, rockY, rockX, rock->height, rock->width);
    int direction = currentMove->direction;
    currentMove = currentMove->next;

    // fprintf(stdout, "rock %c pushed (%s)\n", rock->character,
    // direction == 1 ? "right" : "left");
    // can it be pushed?
    bool canPush = false;
    if (direction == 1) {
      // right
      if (rockX + 1 + rock->width <= WIDTH) {
        // fprintf(stdout, "rock %c moving RIGHT\n", rock->character);
        canPush = true;
      }
    } else {
      // left
      if (rockX - 1 >= 0) {
        // fprintf(stdout, "rock %c moving LEFT\n", rock->character);
        canPush = true;
      }
    }

    if (canPush) {
      for (int y = 0; y < rock->height; y++) {
        for (int x = 0; x < rock->width; x++) {
          int pieceY = rockY + y;
          int pieceX = rockX + direction + x;

          if (rock->shape[(y * rock->height) + x]) {
            if (grid[pieceY][pieceX] != 0) {
              // fprintf(stdout, "rock %c hit piece (%d,%d)\n", rock->character,
              // pieceY, pieceX);
              canPush = false;
              break;
            }
          }
        }
        if (!canPush) {
          break;
        }
      }
    }
    if (canPush) {
      rockX += direction;
    }

    // can it go down?

    // check each row in the rock
    for (int y = 0; y < rock->height; y++) {
      for (int x = 0; x < rock->width; x++) {
        int pieceY = rockY - 1 + y;
        int pieceX = rockX + x;

        // fprintf(stdout, "rock %c down check (%d,%d)\n", rock->character,
        // pieceY, pieceX);

        if (pieceY < 0) {
          // fprintf(stdout, "rock %c hit bottom\n", rock->character);
          stopped = true;
          break;
        }
        if (rock->shape[(y * rock->height) + x]) {
          if (grid[pieceY][pieceX] != 0) {
            // fprintf(stdout, "rock %c hit piece\n", rock->character);
            stopped = true;
            break;
          }
        }
      }
      if (stopped) {
        break;
      }
    }
    if (!stopped) {
      // fprintf(stdout, "rock %c not stopped\n", rock->character);
      rockY--;
    }
  } while (!stopped);

  // fprintf(stdout, "rock %c stopped at (%d,%d)\n", rock->character, rockY,
  // rockX);
  // Place the rock in the grid at rockY,rockX

  for (int y = 0; y < rock->height; y++) {
    for (int x = 0; x < rock->width; x++) {
      int pieceY = rockY + y;
      int pieceX = rockX + x;
      // fprintf(stdout, "rock %c index (%d) = %c\n", rock->character,
      // (y * rock->height) + x, rock->shape[(y * rock->height) + x]);
      if (rock->shape[(y * rock->height) + x]) {
        grid[pieceY][pieceX] = rock->character;
        // fprintf(stdout, "rock %c point (%d,%d)\n", rock->character, pieceY,
        // pieceX);
      }

      highestRock = max(pieceY + 1, highestRock);
    }
  }
}

void printGrid() {
  fprintf(stdout, "printGrid highestRock (%d)\n\n", highestRock);
  for (int y = highestRock; y >= 0; y--) {
    fputs("|", stdout);
    for (int x = 0; x < WIDTH; x++) {
      fputc(grid[y][x] != 0 ? grid[y][x] : '.', stdout);
    }
    fputs("|\n", stdout);
  }
  fputs("+-------+\n", stdout);
}

int main() {
  readInput_n(__FILE__, lineHandler, 11000);
  // printMoves();

  currentMove = moves;
  setupRocks();
        fprintf(stdout, "number of moves: %d, number of rocks: %d\n", numberOfMoves, numberOfRocks);

  // int (*arr)[numberOfRocks] =
  // malloc(sizeof(int[numberOfMoves][numberOfRocks])); arr is pointer to
  // int[numberOfRocks]. use like arr[numberofMoves][numberOfRocks-1]; and
  // free(arr);
  //
  // int *movesMade = calloc(numberOfRocks * numberOfMoves, sizeof(int));
  int(*movesMade)[numberOfRocks] = calloc(
      numberOfRocks * numberOfMoves, sizeof(int[numberOfMoves][numberOfRocks]));

  u_int64_t trillionRocks = 1000000000000llu;
  int times = trillionRocks;

  // the highest rock at the first cycle
  int firstCycleHeight = 0;
  // amount of rocks dropped per cycle
  int rocksPerCycle = 0;
  // amount the height increases per cycle
  int heightPerCycle = 0;

  // the number of cycles skipped over using the pattern
  u_int64_t skippedCycles = 0;
  // total count of rocks dropped
  u_int64_t rocksDropped = 1;
  // amount of rocks dropped in the last cycle
  u_int64_t finalCycleCount = 0;
  u_int64_t finalCycleHeightMarker = -1;
  for (; rocksDropped <= times; rocksDropped++) {
    dropRock();
    // if (rocksDropped > numberOfMoves) {
    if ((movesMade[currentMove->index][currentRock->index])++ == 2) {
      // if(rocksDropped % (numberOfRocks * numberOfMoves* 345) == 0){
      // fprintf(stdout, "cycle: %d,\t\th: %d,\t\tdH: %d,\tdR: %d\n",
      // rocksDropped, highestRock, diffHeight, diffRocks);
      //
      if (firstCycleHeight == 0) {
        fprintf(stdout, "found cycle: %lu, h: %d\n", rocksDropped, highestRock);
        firstCycleHeight = highestRock;
        rocksPerCycle = rocksDropped;
      }
      }
    // }

    // this is the first cycle _after_ finding the cycle
    if (rocksPerCycle != 0 && heightPerCycle == 0) {
      if (rocksDropped > rocksPerCycle && rocksDropped % rocksPerCycle == 0 &&
          firstCycleHeight != 0) {
        heightPerCycle = highestRock - firstCycleHeight;
        fprintf(stdout, "second cycle: %lu, h: %d, height cycle: %d\n",
                rocksDropped, highestRock, heightPerCycle);

        // now we know that every rocksPerCycle we increase our height by
        // heightPerCycle
        //
        // so we can augment our rocksDropped now:
        //
        // firstly, how many rocks do we still need to drop?
        u_int64_t remainingRocks = trillionRocks - rocksDropped;
        // how many cycles is that? (ROUNDED DOWN)
        u_int64_t remainingCycles = remainingRocks / rocksPerCycle;
        fprintf(stdout, "remaining - rocks: %lu, cycles: %lu\n", remainingRocks,
                remainingCycles);
        skippedCycles = remainingCycles;
      }
    }

    if (rocksPerCycle && heightPerCycle) {
      finalCycleHeightMarker = highestRock < finalCycleHeightMarker
                                   ? highestRock
                                   : finalCycleHeightMarker;
      // fprintf(stdout, "check: %lu\n",
      // rocksDropped + (skippedCycles * rocksPerCycle));
      if (rocksDropped + (skippedCycles * rocksPerCycle) == trillionRocks) {
        fprintf(stdout, "ONE TRILLION\n");
        break;
      }
    }
  }

  // printGrid();
  fprintf(stdout, "Part one: %d\n", highestRock);

#ifdef TEST_MODE
  // assert(highestRock == 3068);
#else
  // assert(highestRock == 3202);
#endif

  u_int64_t finalCycleHeight = highestRock - finalCycleHeightMarker;
  fprintf(stdout, "finalCycleHeight: %lu\n", finalCycleHeight);
  fprintf(stdout, "finalCycleHeightMarker: %lu\n", finalCycleHeightMarker);
  u_int64_t trillionRockHeight =
      firstCycleHeight + (heightPerCycle * skippedCycles) + finalCycleHeight;
  // (((1000000000000llu / diffRocks) - 1) * (diffHeight)) + 2749;
  // I don't know why, but this does not work in test mode
  fprintf(stdout, "Part two: %lu\n", trillionRockHeight);

#ifdef TEST_MODE
  assert(trillionRockHeight == 1591977075756);
#else
  // min 1591977078534
  // max 1591977075756
  assert(trillionRockHeight > 1591977078534);
  assert(trillionRockHeight < 1591977075756);
#endif
}
