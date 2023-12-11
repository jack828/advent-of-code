#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
// #define TEST_MODE
#include "../../lib/pqueue.h"
#include "../utils.h"

char **map;
int height = 0;
int width = 0;
int line_i = 0;
int startX = 0;
int startY = 0;
// to track distance
int **visited;

typedef struct {
  int y;
  int x;
  int d;
} point_t;

void fileHandler(int lines) {
  printf("lines: %d\n", lines);
  map = calloc(lines, sizeof(char *));
  visited = calloc(lines, sizeof(int *));
  height = lines;
}

void lineHandler(char *line, int length) {
  printf("line (%d): %s\n", length, line);

  if (width == 0) {
    width = length - 1;
    for (int i = 0; i < height; i++) {
      map[i] = calloc(width, sizeof(char));
      visited[i] = calloc(width, sizeof(int));
    }
  }

  strncpy(map[line_i++], line, width);

  if (startX == 0 && startY == 0) {
    for (int i = 0; i < length; i++) {
      if (line[i] == 'S') {
        startY = line_i - 1;
        startX = i;
      }
    }
  }
}

void print_map() {
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      int c = map[y][x];
      if (c == '|') {
        printf("│");
      } else if (c == '-') {
        printf("─");
      } else if (c == 'L') {
        printf("└");
      } else if (c == 'J') {
        printf("┘");
      } else if (c == '7') {
        printf("┐");
      } else if (c == 'F') {
        printf("┌");
      } else if (c == '.') {
        printf(".");
      } else if (c == 'S') {
        printf("x");
      } else {
        printf("%c", c);
      }
    }
    printf("\n");
  }
}

void print_visited() {
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      if (y == startY && x == startX) {
        printf("S");
      } else {

        int c = visited[y][x];
        printf("%c", c + '0');
        // printf("%d ", c);
      }
    }
    printf("\n");
  }
}

void add_point(pqueue_t *queue, int y, int x, int d) {
  point_t *new_point = malloc(sizeof(point_t));
  new_point->y = y;
  new_point->x = x;
  new_point->d = d;
  pq_enqueue(queue, new_point, d);
}

void DFS(int start_y, int start_x) {
  // printf("start (%d, %d)\n", start_y, start_x);

  pqueue_t *queue = pq_create();
  point_t *start_point = calloc(1, sizeof(point_t));
  start_point->y = start_y;
  start_point->x = start_x;
  start_point->d = 0;

  pq_enqueue(queue, start_point, start_point->d);
  // only attempt to start there if it is possible to have moved there
  // from an unknown piece
  if (start_y - 1 >= 0 &&
      (map[start_y - 1][start_x] == '|' || map[start_y - 1][start_x] == 'F' ||
       map[start_y - 1][start_x] == '7')) {
    add_point(queue, start_y - 1, start_x, start_point->d + 1); // N
  }
  if (start_y + 1 < height &&
      (map[start_y + 1][start_x] == '|' || map[start_y + 1][start_x] == 'L' ||
       map[start_y + 1][start_x] == 'J')) {
    add_point(queue, start_y + 1, start_x, start_point->d + 1); // S
  }
  if (start_x - 1 >= 0 &&
      (map[start_y][start_x - 1] == '-' || map[start_y][start_x - 1] == 'F' ||
       map[start_y][start_x - 1] == '7')) {
    add_point(queue, start_y, start_x - 1, start_point->d + 1); // E
  }
  if (start_x + 1 < width &&
      (map[start_y][start_x + 1] == '-' || map[start_y][start_x + 1] == '7' ||
       map[start_y][start_x + 1] == 'J')) {
    add_point(queue, start_y, start_x + 1, start_point->d + 1); // W
  }

  while (!pq_empty(queue)) {
    point_t *point = pq_dequeue(queue);
    int y = point->y;
    int x = point->x;
    int d = point->d;

    // stop if we hit the edge of the grid
    if (y < 0 || y > height) {
      continue;
    }
    if (x < 0 || x > width) {
      continue;
    }
    // stop if we've been here before
    if (visited[y][x] != 0) {
      continue;
    }
    // stop if we're back to the start
    if (y == startY && x == startX) {
      continue;
    }
    // printf("point [%c] (%d, %d) - %d\n", map[y][x], y, x, d);
    visited[y][x] = d;

    // if (d > 3)
    // continue;
    char c = map[y][x];
    // TODO need to check that it's allowed to go
    if (c == '|') {
      // N and S
      add_point(queue, y - 1, x, d + 1); // N
      add_point(queue, y + 1, x, d + 1); // S
    } else if (c == '-') {
      // E and W
      add_point(queue, y, x + 1, d + 1); // E
      add_point(queue, y, x - 1, d + 1); // W
    } else if (c == 'L') {
      // N and E
      add_point(queue, y - 1, x, d + 1); // N
      add_point(queue, y, x + 1, d + 1); // E
    } else if (c == 'J') {
      // N and W
      add_point(queue, y - 1, x, d + 1); // N
      add_point(queue, y, x - 1, d + 1); // W
    } else if (c == '7') {
      // S and W
      add_point(queue, y + 1, x, d + 1); // S
      add_point(queue, y, x - 1, d + 1); // W
    } else if (c == 'F') {
      // S and E
      add_point(queue, y + 1, x, d + 1); // S
      add_point(queue, y, x + 1, d + 1); // E
    } else if (c == '.') {
      // no move
      visited[y][x] = 0;
    }

    free(point);
  }
  pq_destroy(queue);
}

int main() {
  init();
  readInputFile(__FILE__, lineHandler, fileHandler);

  // print_map();
  DFS(startY, startX);

  // print_visited();

  // the maximum distance along the loop from start
  int part_one = 0;
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      int c = visited[y][x];
      part_one = max(c, part_one);
    }
  }
  printf("Part one: %d\n", part_one);
#ifdef TEST_MODE
  assert(part_one == 23);
#else
  assert(part_one == 6828); // This is great (see my github username)
#endif

  // remove all pipe segments not visited
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      int v = visited[y][x];
      if (v == 0) {
        map[y][x] = '.';
      }
    }
  }
  map[startY][startX] = 'S';

  int points_in_loop = 0;
  // use the scanline method to determine points within loop
  for (int y = 0; y < height; y++) {
    // always outside the loop at the beginning
    bool in_loop = false;
    char last_corner = 0;
    for (int x = 0; x < width; x++) {
      char c = map[y][x];
      if (c == '|') {
        // pass over the loop boundary
        in_loop = !in_loop;
      } else if (c == '-') {
        // Do nothing
      } else if (c == '.') {
        // increment if in loop
        if (in_loop) {
          map[y][x] = '!';
          points_in_loop++;
        }
      } else {
        // must be a corner piece
        switch (last_corner) {
        case 'F': {
          if (c == '7') {
            // U, ignore
          } else {
            in_loop = !in_loop;
          }
          last_corner = 0;
          break;
        }
        case '7': {
          if (c == 'F') {
            // U, ignore
          } else {
            in_loop = !in_loop;
          }
          last_corner = 0;
          break;
        }
        case 'L': {
          if (c == 'J') {
            // U, ignore
          } else {
            in_loop = !in_loop;
          }
          last_corner = 0;
          break;
        }
        case 'J': {
          if (c == 'L') {
            // U, ignore
          } else {
            in_loop = !in_loop;
          }
          last_corner = 0;
          break;
        }
        case 0:
          last_corner = c;
          break;
        }
      }
    }
  }

  // print_map();
  printf("Part two: %d\n", points_in_loop);
#ifdef TEST_MODE
  assert(points_in_loop == 4);
#else
  assert(points_in_loop == 459);
#endif
  exit(EXIT_SUCCESS);
}
