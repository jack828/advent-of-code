#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
// #define TEST_MODE
#include "../utils.h"

char **map;
char **visited;
int height = 0;
int width = 0;
int line_i = 0;
int start_y = 0;
int start_x = 0;

typedef enum dir_t { NORTH = 0, SOUTH, EAST, WEST } dir_t;
int dXY[][2] = {
    {-1, 0}, // N
    {1, 0},  // S
    {0, 1},  // E
    {0, -1}  // W
};
dir_t dDIR[4] = {
    EAST,  // NORTH -> 90 -> EAST
    WEST,  // SOUTH -> 90 -> WEST
    SOUTH, // EAST -> 90 -> SOUTH
    NORTH, // WEST -> 90 -> NORTH
};

void fileHandler(int lines) {
  printf("lines: %d\n", lines);
  map = calloc(lines, sizeof(char *));
  visited = calloc(lines, sizeof(char *));
  height = lines;
}

void lineHandler(char *line, int length) {
  printf("line (%d): %s\n", length, line);

  if (width == 0) {
    width = length - 1;
    for (int i = 0; i < height; i++) {
      map[i] = calloc(width, sizeof(char));
      visited[i] = calloc(width, sizeof(char));
    }
  }

  strncpy(map[line_i++], line, width);
  char *pos = strstr(line, "^");
  if (pos) {
    start_y = line_i - 1;
    start_x = pos - line;
  }
}

void print_map(char **map_to_print) {
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      int c = map_to_print[y][x];
      if (c > 10) {
        printf("%c", c);
      } else {
        printf("%c", c ? 'X' : '.');
      }
    }
    printf("\n");
  }
}

int main() {
  init();
  readInputFile(__FILE__, lineHandler, fileHandler);

  print_map(map);
  printf("start (%d,%d) - size %d x %d\n", start_y, start_x, height, width);

  dir_t dir = NORTH;
  int y = start_y;
  int x = start_x;
  visited[y][x] = 1;
  for (;;) {
    // printf("(%d,%d)\n", y, x);
    int *d = dXY[dir];
    int new_y = y + d[0];
    int new_x = x + d[1];

    if (new_y < 0 || new_y > height - 1 || new_x < 0 || new_x > width) {
      break;
    }

    if (map[new_y][new_x] == '#') {
      dir = dDIR[dir];
    } else {
      y = new_y;
      x = new_x;
    }
    visited[y][x] = 1;
    // break;
  }

  int visited_positions = 0;
  for (int v_y = 0; v_y < height; v_y++) {
    for (int v_x = 0; v_x < width; v_x++) {
      int c = visited[v_y][v_x];
      if (c) {
        visited_positions++;
      }
    }
  }
  printf("Part one: %d\n", visited_positions);
  // print_map(visited);

#ifdef TEST_MODE
  assert(visited_positions == 41);
#else
  assert(visited_positions == 4826);
#endif

  char **loop_visited;
  loop_visited = calloc(height, sizeof *loop_visited);
  for (int i = 0; i < height; i++) {
    loop_visited[i] = calloc(width, sizeof(char));
  }

  // bit mask to store visited so we can track "have we been at this spot AND in
  // this direction" loop_visited[y][x] |= 1 << dir

  int loop_count = 0;
  // for every y and x, mark a temporary new obstacle in the map
  for (int obs_y = 0; obs_y < height; obs_y++) {
    for (int obs_x = 0; obs_x < width; obs_x++) {
      // if (map[obs_y][obs_x] != '.') { // || visited[obs_y][obs_x] != 1) {
      if (map[obs_y][obs_x] != '.' || visited[obs_y][obs_x] != 1) {
        // already obstacle or start pos
        continue;
      }
      // printf("(%d,%d)\n", obs_y, obs_x);
      map[obs_y][obs_x] = '#';

      // reset state
      dir = NORTH;
      y = start_y;
      x = start_x;
      for (int i = 0; i < height; i++) {
        memset(loop_visited[i], 0, width + 1);
      }
      loop_visited[y][x] = 1;

      bool has_loop = false;
      for (;;) {
        // printf("(%d,%d) = %d\n", y, x, loop_visited[y][x]);
        int *d = dXY[dir];
        int new_y = y + d[0];
        int new_x = x + d[1];

        if (new_y < 0 || new_y > height - 1 || new_x < 0 || new_x > width) {
          // printf("oob\n");
          break;
        }
        if (loop_visited[new_y][new_x] & (1 << dir)) {
          // printf("loop\n");
          has_loop = true;
          break;
        }

        if (map[new_y][new_x] == '#') {
          dir = dDIR[dir];
          // printf("cont\n");
          continue;
        } else {
          y = new_y;
          x = new_x;
        }
        loop_visited[y][x] |= 1 << dir;
      }
      if (has_loop) {
        loop_count++;
        // printf("loop: %d\n", has_loop);
        map[obs_y][obs_x] = 'O';
        // print_map(map);
      }
      map[obs_y][obs_x] = 'O';
      // printf("\n--\n");
      // print_map(map);
      // printf("\n>\n");
      // print_map(loop_visited);
      // printf("\n=-=\n");
      map[obs_y][obs_x] = '.';
    }
  }

  printf("Part two: %d\n", loop_count);
#ifdef TEST_MODE
  assert(loop_count == 6);
#else
  assert(loop_count == 1721);
#endif
  exit(EXIT_SUCCESS);
}
