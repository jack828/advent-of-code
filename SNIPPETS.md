# Snippets

A collection of snippets that I find come in handy when doing the puzzles.

Also because I CBA to generalise and put in a lib folder.

Also may contain just links to a year/day where a particular algorithm is used.

## Lib Folder

I lied, there is a lib folder, for generic stuff.

It contains:

 - Priority Queue
 - Queue
 - Hashmap (bring-your-own-hasher)

# Common bits

## Dynamically allocated 2D array

```c
char **grid;
char **visited;
int height = -1;
int width = 0;

typedef struct {
  int y;
  int x;
  int s;
} point_t;

void fileHandler(int lines) {
  printf("lines: %d\n", lines);
  grid = calloc(lines, sizeof(char *));
}

void lineHandler(char *line, int length) {
  printf("line (%d): %s\n", length, line);

  if (width == 0) {
    width = length - 2;
  }

  grid[++height] = calloc(width, sizeof(char));
  strncpy(grid[height], line, length);
}

void print_grid(char **grid_to_print) {
  for (int y = 0; y <= height; y++) {
    for (int x = 0; x <= width; x++) {
      char c = grid_to_print[y][x];
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

## 3D Array

```c

char ***grid;
grid = calloc(x_max + 1, sizeof(char **));
for (int x = 0; x <= x_max; x++) {
  printf("x: %d\n", x);
  grid[x] = calloc(y_max + 1, sizeof(char *));
  for (int y = 0; y <= y_max; y++) {
  printf("y: %d\n", y);
    grid[x][y] = calloc(z_max + 1, sizeof(char));
  }
}

// perspective?
void printGrid() {
  for (int x = 0; x <= x_max; x++) {
    printf("x: %d\n", x);
    for (int y = 0; y <= y_max; y++) {
      for (int z = 0; z <= z_max; z++) {
        if (grid[x][y][z]) {
          printf("%d", grid[x][y][z]);
        } else {
          printf(".");
        }
      }
      printf("\n");
    }
  }
  printf("\n");
}
```

## strtok_r while loop

```c
  char *end;
  char *delim = " ";
  char *token = strtok_r(line, delim, &end);
  do {
    // stuff
  } while ((token = strtok_r(NULL, delim, &end)) != NULL);
```

# Data Structures

## Hashmap

 - [2023 Day 14](2023/14/main.c)
   - Not using `lib.h`
   - But does use djb2 hash algorithm
 - [2023 Day 17](2023/17/main.c)

# Algorithms

## Flood Fill

 - 3D [2022 Day 18](./2022/18/main.c)
 - 2D [2023 Day 10](./2023/10/main.c)
 - 2D [2023 Day 18](2023/18/main.c)

## Dijkstra's

 - [2022 Day 24](2022/24/main.c)
 - [2023 Day 17](2023/17/main.c)

## DFS (Depth First Search)

 - [2022 Day 24](2022/24/main.c)
 - [2023 Day 10](2023/10/main.c)
 - [2023 Day 21](2023/21/main.c)

## Shoelace Formula

Area of an irregular non-intersecting polygon.
Also Pick's Theorem.

 - [2023 Day 18](2023/18/main.c)
