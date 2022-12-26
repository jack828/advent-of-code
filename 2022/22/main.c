#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#define _AOC_LINE_MAX 8192
// #define TEST_MODE
#include "../../utils.h"

char *grid = NULL;
int gridHeight = 12;
int gridWidth = 0;
int lineY = 0;

enum FACING { RIGHT = 0, DOWN, LEFT, UP };

int startY = 0;
int startX = 0;
int currY = 0;
int currX = 0;
int dir = RIGHT;

enum ACTION { MOVE, TURN };
enum DIRECTION { L, R };

typedef struct instruction_t {
  enum ACTION type;
  enum DIRECTION direction;
  int steps;
} instruction_t;

instruction_t *instructions;
int instructionCount = 0;

void fileHandler(int lines) {
  fprintf(stdout, "lines: %d\n", lines);
  // one line for movements, one newline separation
  gridHeight = lines - 2;
}

void lineHandler(char *line) {
  fprintf(stdout, "line: %s\n", line);
  int length = strlen(line);
  if (grid == NULL) {
    // allocate the grid
    gridWidth = length - 1;
    grid = calloc(gridHeight * gridWidth, sizeof(char));
  }

  if (lineY > gridHeight && length > 0) {
    instructions = calloc(length - 1, sizeof(instruction_t));
    // instructions line
    char token[4] = {0};

    for (int i = 0; i < length; i++) {
      if (isdigit(line[i])) {
        // digits for a number of steps
        strncat(token, line + i, 1);
      } else {
        // onto an action
        // first parse and append the movements
        int steps = atoi(token);
        token[0] = '\0';
        instruction_t *moveInstruction = malloc(sizeof(instruction_t));
        moveInstruction->type = MOVE;
        moveInstruction->steps = steps;
        instructions[instructionCount++] = *moveInstruction;

        if (line[i] != '\n') {
          instruction_t *turnInstruction = malloc(sizeof(instruction_t));
          turnInstruction->type = TURN;
          turnInstruction->direction = line[i] == 'L' ? L : R;
          instructions[instructionCount++] = *turnInstruction;
        }
      }
    }
  } else if (length > 0) {
    // grid line
    for (int i = 0; i < length; i++) {
      if (line[i] == '.' || line[i] == '#') {
        grid[(lineY * gridWidth) + i] = line[i];

        if (lineY == 0 && startX == 0 && line[i] == '.') {
          // start position
          startX = i;
          currX = i;
        }
      }
    }
  }
  lineY++;
}

void printGrid() {
  fprintf(stdout, "size: %dx%d (%d)\n", gridHeight, gridWidth,
          gridHeight * gridWidth);
  for (int i = 0; i < gridHeight; i++) {
    for (int j = 0; j < gridWidth; j++) {
      if (i == startY && j == startX) {
        fputc('s', stdout);
      } else if (i == currY && j == currX) {
        fputc('e', stdout);
      } else if (grid[(i * gridWidth) + j]) {
        fputc(grid[(i * gridWidth) + j], stdout);
      } else {
        fputc(' ', stdout);
      }
    }
    fputs("\n", stdout);
  }
  fputs("\n", stdout);
}

void printInstructions() {
  for (int i = 0; i < instructionCount; i++) {
    instruction_t *instruction = &instructions[i];

    if (instruction->type == MOVE) {
      fprintf(stdout, "inst: MOVE, steps: %d\n", instruction->steps);
    } else if (instruction->type == TURN) {
      fprintf(stdout, "inst: TURN, dir %c\n",
              instruction->direction == L ? 'L' : 'R');
    }
  }
}

void doTheMoves() {
  for (int instructionIndex = 0; instructionIndex < instructionCount;
       instructionIndex++) {
    instruction_t *instruction = &instructions[instructionIndex];

    if (instruction->type == MOVE) {
      fprintf(stdout, "inst: MOVE, steps: %d\n", instruction->steps);
      // move incrementally and wrap if needed, stopping at a wall
      for (int i = 0; i < instruction->steps; i++) {
        fprintf(stdout, "inst: step: %d\n", i);
        int dY = 0;
        int dX = 0;
        switch (dir) {
        case RIGHT:
          dX = 1;
          break;
        case DOWN:
          dY = 1;
          break;
        case LEFT:
          dX = -1;
          break;
        case UP:
          dX = -1;
          break;
        }
        int newY = currY;
        int newX = currX;
        do {
          newY += dY;
          newX += dX;
          // check for out of bounds or wrapping before attempting move
          // TODO part two probably here
          if (newY >= gridHeight) {
            newY = 0;
          } else if (newY < 0) {
            newY = gridHeight;
          }
          if (newX >= gridWidth) {
            newX = 0;
          } else if (newX < 0) {
            newX = gridWidth;
          }
          // fprintf(stdout, "y: %d, x: %d - '%c'\n", newY, newX,
                  // grid[(newY * gridWidth) + newX]);
          // if (grid[(newY * gridWidth) + newX] == '.' ||
          //     grid[(newY * gridWidth) + newX] == '#' ||
          //     grid[(newY * gridWidth) + newX] == 'E') {
          //   break;
          // }
        } while (grid[(newY * gridWidth) + newX] == 0);

        if (grid[(newY * gridWidth) + newX] == '#') {
          break;
        } else {
          // if (grid[(newY * gridWidth) + newX] == '.' ||
          // grid[(newY * gridWidth) + newX] == 'E') {
          grid[(newY * gridWidth) + newX] = 'E';
          currY = newY;
          currX = newX;
        }
      }

    } else if (instruction->type == TURN) {
      fprintf(stdout, "inst: TURN, dir %c\n",
              instruction->direction == L ? 'L' : 'R');
      switch (instruction->direction) {
      case L: // anticlockwise
        switch (dir) {
        case RIGHT:
          dir = UP;
          break;
        case DOWN:
          dir = RIGHT;
          break;
        case LEFT:
          dir = DOWN;
          break;
        case UP:
          dir = LEFT;
          break;
        }
        break;
      case R: // clockwise
        switch (dir) {
        case RIGHT:
          dir = DOWN;
          break;
        case DOWN:
          dir = LEFT;
          break;
        case LEFT:
          dir = UP;
          break;
        case UP:
          dir = RIGHT;
          break;
        }
        break;
      }
    }
  }
}

int calculatePassword() {
  int score = 0;
  // Facing is 0 for right (>), 1 for down (v), 2 for left (<), and 3 for up
  // (^).
  // The final password is the sum of 1000 times the row, 4 times the column,
  // and the facing.
  switch (dir) {
  case RIGHT:
    score = 0;
    break;
  case DOWN:
    score = 1;
    break;
  case LEFT:
    score = 2;
    break;
  case UP:
    score = 3;
    break;
  }

  // zero indexed to 1 indexed
  score += 1000 * (currY + 1);
  score += 4 * (currX + 1);

  return score;
}

int main() {
  readInputFile(__FILE__, lineHandler, fileHandler);

  printGrid();
  // printInstructions();

  doTheMoves();
  printGrid();

  int password = calculatePassword();
  fprintf(stdout, "Part one: %d\n", password);
#ifdef TEST_MODE
  assert(password == 6032);
#else
// TODO It doesn't work and i can't figure out why :c
  assert(password < 26446);
  assert(password == 420);
#endif

  fprintf(stdout, "Part two: %d\n", 420);
#ifdef TEST_MODE
  assert(420 == 69);
#else
  assert(420 == 69);
#endif
}
