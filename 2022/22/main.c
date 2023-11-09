#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#define _AOC_LINE_MAX 8192
#define TEST_MODE
#include "../../utils.h"

char **grid = NULL;
int gridHeight = 0;
int gridWidth = 0;
// maybe store cube in 3d array of each face
// then when we go out of bounds we can warp """"""easier""""""
char ***cube = NULL; // pointer to a pointer to a pointer to a char wtaf
int cubeHeight = 0;
int cubeWidth = 0;
int FACES = 6;

// input parsing only
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

instruction_t **instructions = NULL;
int instructionCount = 0;

void fileHandler(int lines) {
  fprintf(stdout, "lines: %d\n", lines);
  // one line for movements, one newline separation
  gridHeight = lines - 2;
  cubeHeight = gridHeight / 4;
}

void lineHandler(char *line) {
  fprintf(stdout, "line: %s\n", line);
  int length = strlen(line);
  if (grid == NULL) {
    // allocate the grid
    gridWidth = length;
    grid = malloc(gridHeight * sizeof(char *));
    for (int i = 0; i < gridHeight; i++) {
      grid[i] = calloc(gridWidth, sizeof(char));
    }
    cubeWidth = gridWidth / 3;
    cube = malloc(FACES * sizeof(char **));
    for (int i = 0; i < FACES; i++) {
      cube[i] = malloc(cubeHeight * sizeof(char *));
      for (int j = 0; j < cubeHeight; j++) {
        cube[i][j] = calloc(cubeWidth, sizeof(char));
      }
    }
  }

  if (lineY > gridHeight && length > 0) {
    instructions = malloc(length * sizeof(instruction_t *));
    // instructions line
    char token[4] = {0};

    for (int i = 0; i <= length; i++) {
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
        instructions[instructionCount++] = moveInstruction;

        if (line[i] != '\0') {
          instruction_t *turnInstruction = malloc(sizeof(instruction_t));
          turnInstruction->type = TURN;
          turnInstruction->direction = line[i] == 'L' ? L : R;
          instructions[instructionCount++] = turnInstruction;
        }
      }
    }
  } else if (length > 0) {
    // grid line
    for (int i = 0; i <= length; i++) {
      if (line[i] == '.' || line[i] == '#') {
        grid[lineY][i] = line[i];

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

void parseCube () {

}

void printCube() {
  fprintf(stdout, "cube: %dx%d (%d)\n", cubeHeight, cubeWidth,
          cubeHeight * cubeWidth);

  for (int i = 0; i < FACES; i++) {
  fprintf(stdout, "face: %d\n",i);
  for (int j = 0; j < gridHeight; j++) {
    for (int k = 0; k < gridWidth; k++) {
      if (j == startY && i == startX) {
        fputc('s', stdout);
      } else if (j == currY && k == currX) {
        fputc('e', stdout);
      } else if (cube[i][j][k]) {
        fputc(cube[i][j][k], stdout);
      } else {
        fputc(' ', stdout);
      }
    }
    fputs("\n", stdout);
  }
  }
  fputs("\n", stdout);
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
      } else if (grid[i][j]) {
        fputc(grid[i][j], stdout);
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
    instruction_t *instruction = instructions[i];

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
    instruction_t *instruction = instructions[instructionIndex];

    if (instruction->type == MOVE) {
      fprintf(stdout, "inst: MOVE, steps: %d\n", instruction->steps);
      // move incrementally and wrap if needed, stopping at a wall
      for (int i = 0; i < instruction->steps; i++) {
        fprintf(stdout, "inst: step: %d (%d,%d)\n", i, currY, currX);
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
          dY = -1;
          break;
        }
        int newY = currY;
        int newX = currX;
        do {
          newY += dY;
          newX += dX;
          // check for out of bounds or wrapping before attempting move
          // TODO part two probably here
          // fprintf(stdout, "inst: while: (%d,%d), g h: %d, g w: %d\n", newY,
          // newX, gridHeight, gridWidth);
          if (newY >= gridHeight) {
            newY = 0;
          } else if (newY < 0) {
            newY = gridHeight - 1;
          }
          if (newX >= gridWidth) {
            newX = 0;
          } else if (newX < 0) {
            newX = gridWidth - 1;
          }
          // fprintf(stdout, "y: %d, x: %d - '%c'\n", newY, newX,
          // grid[newY][newX]);
          // if (grid[newY][newX] == '.' ||
          //     grid[newY][newX] == '#' ||
          //     grid[newY][newX] == 'E') {
          //   break;
          // }
        } while (grid[newY][newX] == 0);

        if (grid[newY][newX] == '#') {
          break;
        } else {
          // if (grid[newY][newX] == '.' ||
          // grid[newY][newX] == 'E') {
          grid[newY][newX] = 'E';
          currY = newY;
          currX = newX;
        }
      }

    } else if (instruction->type == TURN) {
      fprintf(stdout, "inst: TURN, dir %c - now facing: ",
              instruction->direction == L ? 'L' : 'R');
      switch (instruction->direction) {
      case L: // anticlockwise
        switch (dir) {
        case RIGHT:
          dir = UP;
          fprintf(stdout, "U");
          break;
        case DOWN:
          dir = RIGHT;
          fprintf(stdout, "R");
          break;
        case LEFT:
          dir = DOWN;
          fprintf(stdout, "D");
          break;
        case UP:
          dir = LEFT;
          fprintf(stdout, "L");
          break;
        }
        break;
      case R: // clockwise
        switch (dir) {
        case RIGHT:
          dir = DOWN;
          fprintf(stdout, "D");
          break;
        case DOWN:
          dir = LEFT;
          fprintf(stdout, "L");
          break;
        case LEFT:
          dir = UP;
          fprintf(stdout, "U");
          break;
        case UP:
          dir = RIGHT;
          fprintf(stdout, "R");
          break;
        }
        break;
      }
      fprintf(stdout, "\n");
    }
  }
}

int calculatePassword() {
  int score = 0;
  // Facing is 0 for right (>), 1 for down (v), 2 for left (<), and 3 for up
  // (^).
  // The final password is the sum of 1000 times the row, 4 times the column,
  // and the facing.
  fprintf(stdout, "final: ");
  switch (dir) {
  case RIGHT:
    fprintf(stdout, "R");
    score = 0;
    break;
  case DOWN:
    fprintf(stdout, "D");
    score = 1;
    break;
  case LEFT:
    score = 2;
    fprintf(stdout, "L");
    break;
  case UP:
    score = 3;
    fprintf(stdout, "U");
    break;
  }

  fprintf(stdout, " (%d,%d)\n", currY, currX);
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
  // modified input
  // https://www.reddit.com/r/adventofcode/comments/zst7z3/2022_day_22_part_2_improved_example_input_working/
  assert(password == 10012);
#else
  assert(password == 3590);
#endif

    parseCube();
    printCube();
  fprintf(stdout, "Part two: %d\n", 420);
#ifdef TEST_MODE
  assert(420 == 69);
#else
  assert(420 == 69);
#endif
}
