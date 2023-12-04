#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#define TEST_MODE
#include "../utils.h"

int height;
int width;

#ifdef TEST_MODE
#define HEIGHT 10
#define WIDTH 10
#else
#define HEIGHT 140
#define WIDTH 140
#endif

char schematic[HEIGHT][WIDTH] = {0};
struct gear_t {
  int * part_numbers;
  int part_count;
} gear_t;
struct gear_t* gears[HEIGHT][WIDTH] = {0};

void fileHandler(int lines) {
  printf("lines: %d\n", lines);
  height = lines;
}

int currentHeight = 0;

void lineHandler(char *line, int length) {
  printf("line (%d): %s\n", length, line);
  width = length;

  // TODO one day, figure out what the hell you need to dynamically allocate
  // this char(*A)[width] = malloc(sizeof(char[width][height])); schematic =
  // (char **)A;
  // (*schematic)[width] = malloc(sizeof *schematic * height);
  // T (*ap)[cols] = malloc(sizeof *ap * rows);
  // ...
  // ap[i][j] = x;
  for (int i = 0; i < length; i++) {
    schematic[currentHeight][i] = line[i];
  }

  currentHeight++;
}

void printGrid() {
  for (int i = 0; i < HEIGHT; i++) {
    for (int j = 0; j < WIDTH; j++) {
      printf("%c", schematic[i][j]);
    }
    printf("\n");
  }
  printf("\n");
}

bool issymbol(char c) {
  return c == '*' || c == '#' || c == '-' || c == '+' || c == '@' || c == '%' ||
         c == '&' || c == '=' || c == '$' || c == '/';
}

int main() {
  init();
  readInputFile(__FILE__, lineHandler, fileHandler);

  printGrid();

  int partOne = 0;

  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {
      bool isValidPartNo = false;
      char num_str[16] = {0};
      int num_len = 0;
      if (!isdigit(schematic[y][x])) {
        continue;
      }
      // found a potential part no
      // continue grabbing to get full number and length
      while (isdigit(schematic[y][x + num_len])) {
        num_str[num_len] = schematic[y][x + num_len];
        num_len++;
      }
      // printf("num %s len %d\n", num_str, num_len);
      // for the length of the number, check if it is adjacent to a symbol
      for (int str_index = 0; str_index < num_len; str_index++) {
        // This is definitely not optimal, rechecking stuff etc
        // but those elves wont mind
        int newX = x + str_index;
        int newY = y - 1;
        // N
        if ((y - 1 >= 0) && issymbol(schematic[y - 1][newX])) {
          isValidPartNo = true;
          // printf("N %c \n", schematic[y - 1][newX]);
        }
        // NE
        if ((y - 1 >= 0) && (newX + 1 < WIDTH) &&
            issymbol(schematic[y - 1][newX + 1])) {
          isValidPartNo = true;
          // printf("NE %c \n", schematic[y - 1][newX + 1]);
        }
        // E
        if ((newX + 1 < WIDTH) && issymbol(schematic[y][newX + 1])) {
          isValidPartNo = true;
          // printf("E %c \n", schematic[y][newX + 1]);
        }
        // SE
        if ((y + 1 < HEIGHT) && (newX + 1 < WIDTH) &&
            issymbol(schematic[y + 1][newX + 1])) {
          isValidPartNo = true;
          // printf("SE %c \n", schematic[y + 1][newX + 1]);
        }
        // S
        if ((y + 1 < HEIGHT) && issymbol(schematic[y + 1][newX])) {
          isValidPartNo = true;
          // printf("S %c \n", schematic[y + 1][newX]);
        }
        // SW
        if ((y + 1 < HEIGHT) && (newX - 1 >= 0) &&
            issymbol(schematic[y + 1][newX - 1])) {
          isValidPartNo = true;
          // printf("SW %c \n", schematic[y + 1][newX - 1]);
        }
        // W
        if ((newX - 1 >= 0) && issymbol(schematic[y][newX - 1])) {
          isValidPartNo = true;
          // printf("W %c \n", schematic[y][newX - 1]);
        }
        // NW
        if ((y - 1 >= 0) && (newX - 1 >= 0) &&
            issymbol(schematic[y - 1][newX - 1])) {
          isValidPartNo = true;
          // printf("NW %c \n", schematic[y - 1][newX - 1]);
        }
      }

      if (isValidPartNo) {
        partOne += strtol(num_str, NULL, 10);
      }
      // prevent re-capturing the number we already dealt with
      x += num_len - 1;
    }
  }

  printf("Part one: %d\n", partOne);
#ifdef TEST_MODE
  assert(partOne == 4361);
#else
  assert(partOne == 540212);
#endif

int partTwo = 0;
  printf("Part two: %d\n", partTwo);
#ifdef TEST_MODE
  assert(partTwo == 467835);
#else
  assert(partTwo == 69);
#endif
  exit(EXIT_SUCCESS);
}
