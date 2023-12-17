# Snippets

A collection of snippets that I find come in handy when doing the puzzles.

Also because I CBA to generalise and put in a lib folder.

Also may contain just links to a year/day where a particular algorithm is used.

## Lib Folder

I lied, there is a lib folder, for generic stuff.

It contains:

 - Priority Queue
 - Queue

# Common bits

## Dynamically allocated 2D array

```c
char **grid;
int height = 0;
int width = 0;
int line_i = 0;

void fileHandler(int lines) {
  printf("lines: %d\n", lines);
  grid = calloc(lines, sizeof(char *));
  height = lines;
}

void lineHandler(char *line, int length) {
  printf("line (%d): %s\n", length, line);

  if (width == 0) {
    width = length;
    for (int i = 0; i < height; i++) {
      grid[i] = calloc(width, sizeof(char));
    }
  }

  strncpy(grid[line_i++], line, length);
}

void print_grid(char **grid_to_print) {
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      int c = grid_to_print[y][x];
      printf("%c", c == 0 ? '.' : c);
    }
    printf("\n");
  }
}
```

## 2D Grid Parsed

```c
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#define TEST_MODE
#include "../utils.h"

char **map;
int height = 0;
int width = 0;
int line_i= 0;

void fileHandler(int lines) {
  printf("lines: %d\n", lines);
  map = calloc(lines, sizeof(char *));
  height = lines;
}

void lineHandler(char *line, int length) {
  printf("line (%d): %s\n", length, line);

  if (width == 0) {
    width = length - 1;
    for (int i = 0; i < height; i++) {
      map[i] = calloc(width, sizeof(char));
    }
  }

  strncpy(map[line_i++], line, width);
}

void print_map() {
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      int c = map[y][x];
      printf("%c", c);
    }
    printf("\n");
  }
}

int main() {
  init();
  readInputFile(__FILE__, lineHandler, fileHandler);

  print_map();

  printf("Part one: %d\n", 69);
#ifdef TEST_MODE
  assert(69 == 420);
#else
  assert(69 == 420);
#endif

  printf("Part two: %d\n", 420);
#ifdef TEST_MODE
  assert(420 == 69);
#else
  assert(420 == 69);
#endif
  exit(EXIT_SUCCESS);
}
```

# Algorithms

## Flood Fill

 - 3D [2022 Day 18](./2022/18/main.c)
 - 2D [2023 Day 10](./2023/10/main.c)

## A*

 - [2022 Day 24](2022/24/main.c)

## DFS (Depth First Search)

 - [2022 Day 24](2022/24/main.c)
 - [2023 Day 10](2023/10/main.c)

