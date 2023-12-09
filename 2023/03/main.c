#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
// #define TEST_MODE
#include "../utils.h"

#ifdef TEST_MODE
#define HEIGHT 10
#define WIDTH 10
#else
#define HEIGHT 140
#define WIDTH 140
#endif

char schematic[HEIGHT][WIDTH] = {0};
struct gear_t {
  int *part_numbers;
  int part_count;
} gear_t;
struct gear_t *gears[HEIGHT][WIDTH] = {0};

void fileHandler(int lines) { printf("lines: %d\n", lines); }

int currentHeight = 0;

void lineHandler(char *line, int length) {
  printf("line (%d): %s\n", length, line);

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

void printGears() {
  for (int i = 0; i < HEIGHT; i++) {
    for (int j = 0; j < WIDTH; j++) {
      printf("%c", gears[i][j] != NULL ? gears[i][j]->part_count + '0' : '.');
    }
    printf("\n");
  }
  printf("\n");
}

bool issymbol(char c) {
  return c == '*' || c == '#' || c == '-' || c == '+' || c == '@' || c == '%' ||
         c == '&' || c == '=' || c == '$' || c == '/';
}
bool isgear(char c) { return c == '*'; }

void addToGearMap(int y, int x, int num) {
  // printf("add gear: (%d,%d) - %d\n", x, y, num);
  if (gears[y][x] == NULL) {
    gears[y][x] = malloc(sizeof(struct gears_t *));
    gears[y][x]->part_numbers = calloc(16, sizeof(int));
    gears[y][x]->part_count = 0;
  }
  // if (gears[y][x]->part_numbers[gears[y][x]->part_count - 1] == num) {
  // already added
  // return;
  // }
  gears[y][x]->part_numbers[gears[y][x]->part_count++] = num;
}

int main() {
  init();
  readInputFile(__FILE__, lineHandler, fileHandler);

  // printGrid();

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

      int num = strtol(num_str, NULL, 10);
      // printf("num %s len %d at (%d,%d)\n", num_str, num_len, y, x);

      int num_positions = 3 + (num_len * 2) + 3;
      int positions[num_positions][2];
      int pos_i = 0;

      // NW of digit 1
      positions[pos_i][0] = y - 1;
      positions[pos_i][1] = x - 1;
      pos_i++;

      // W of digit 1
      positions[pos_i][0] = y;
      positions[pos_i][1] = x - 1;
      pos_i++;

      // SW of digit 1
      positions[pos_i][0] = y + 1;
      positions[pos_i][1] = x - 1;
      pos_i++;

      // for the length of the number,
      // add the surrounding coords to `positions`
      // excluding the coords that the digit occupies
      // this ensures we only check for a surrounding symbol ONCE
      for (int str_index = 0; str_index < num_len; str_index++) {
        // N of digit N
        positions[pos_i][0] = y - 1;
        positions[pos_i][1] = x + str_index;
        pos_i++;

        /* Ignore digit in middle */

        // S of digit N
        positions[pos_i][0] = y + 1;
        positions[pos_i][1] = x + str_index;
        pos_i++;
      }

      // NE of last digit
      positions[pos_i][0] = y - 1;
      positions[pos_i][1] = x + num_len;
      pos_i++;

      // E of last digit
      positions[pos_i][0] = y;
      positions[pos_i][1] = x + num_len;
      pos_i++;

      // SE of last digit
      positions[pos_i][0] = y + 1;
      positions[pos_i][1] = x + num_len;
      pos_i++;
      for (int i = 0; i < pos_i; i++) {
        int *pos = positions[i];
        int yPos = pos[0];
        int xPos = pos[1];

        if (yPos < 0 && yPos >= HEIGHT)
          continue;

        if (xPos < 0 && xPos >= WIDTH)
          continue;

        if (issymbol(schematic[yPos][xPos])) {
          isValidPartNo = true;
          if (isgear(schematic[yPos][xPos])) {
            addToGearMap(yPos, xPos, num);
          }
        }
      }

      if (isValidPartNo) {
        partOne += num;
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

  // printGears();

  uint64_t partTwo = 0;
  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {
      struct gear_t *gear = gears[y][x];
      if (gear == NULL) {
        continue;
      }
      if (gear->part_count == 2) {
        partTwo +=
            gear->part_numbers[0] * gear->part_numbers[gear->part_count - 1];
      }
    }
  }
  printf("Part two: %ld\n", partTwo);
#ifdef TEST_MODE
  assert(partTwo == 467835);
#else
  assert(partTwo == 87605697);
#endif
  exit(EXIT_SUCCESS);
}
