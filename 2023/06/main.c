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
#define RACE_COUNT 3
#else
#define RACE_COUNT 4
#endif

int times[RACE_COUNT];
int distances[RACE_COUNT];

u_int64_t p2_time = 0;
u_int64_t p2_distance = 0;

void fileHandler(int lines) { printf("lines: %d\n", lines); }

void lineHandler(char *line, int length) {
  printf("line (%d): %s\n", length, line);

#ifdef TEST_MODE
  int num1, num2, num3;

  int *dst;
  if (line[0] == 'T') {
    dst = (int *)&times;
    sscanf(line, "Time: %d %d %d", &num1, &num2, &num3);
  } else {
    dst = (int *)&distances;
    sscanf(line, "Distance: %d %d %d", &num1, &num2, &num3);
  }

  printf("values: %d %d %d\n", num1, num2, num3);
  int nums[RACE_COUNT] = {num1, num2, num3};
#else
  int num1, num2, num3, num4;

  int *dst;
  if (line[0] == 'T') {
    dst = (int *)&times;
    sscanf(line, "Time: %d %d %d %d", &num1, &num2, &num3, &num4);
  } else {
    dst = (int *)&distances;
    sscanf(line, "Distance: %d %d %d %d", &num1, &num2, &num3, &num4);
  }

  printf("values: %d %d %d %d\n", num1, num2, num3, num4);
  int nums[RACE_COUNT] = {num1, num2, num3, num4};
#endif
  memcpy(dst, nums, RACE_COUNT * sizeof(int));

  char digits[length];
  int digit_index = 0;
  for (int i = 0; i < length; i++) {
    if (isdigit(line[i])) {
      digits[digit_index++] = line[i];
    }
  }
  digits[digit_index] = '\0';
  // printf("digits: %s\n", digits);
  u_int64_t value = strtoul(digits, NULL, 10);

  if (line[0] == 'T') {
    p2_time = value;
  } else {
    p2_distance = value;
  }
}

int calc_dist(int hold_time, int max_time) {
  int velocity = hold_time;
  int dist = velocity * (max_time - hold_time);
  return dist;
}

int main() {
  init();
  readInputFile(__FILE__, lineHandler, fileHandler);

  int part_one = 1;

  for (int i = 0; i < RACE_COUNT; i++) {
    int max_time = times[i];
    int best_distance = distances[i];
    // printf("Race %d: %dms -> %dmm\n", i, max_time, best_distance);
    int ways_to_win = 0;
    for (int time = 1; time < max_time; time++) {
      int distance = calc_dist(time, max_time);
      // printf("Hold %dms = %dmm distance\n", time, distance);
      if (distance > best_distance) {
        ways_to_win++;
      }
    }

    part_one *= ways_to_win;
  }

  printf("Part one: %d\n", part_one);
#ifdef TEST_MODE
  assert(part_one == 288);
#else
  assert(part_one == 140220);
#endif

  int part_two = 0;

  // printf("Race: %lums -> %lumm\n", p2_time, p2_distance);
  for (u_int64_t time = 1; time < p2_time; time++) {
    u_int64_t distance = time * (p2_time - time);
    // printf("Hold %dms = %dmm distance\n", time, distance);
    // TODO optimisation
    // if part_two is above 0, and this evaluates to false,
    // it will never evaluate to true again
    // so we can exit
    // alternatively, maths :shrug:
    if (distance > p2_distance) {
      part_two++;
    }
  }

  printf("Part two: %d\n", part_two);
#ifdef TEST_MODE
  assert(part_two == 71503);
#else
  assert(part_two == 69);
  assert(part_two == 39570185);
#endif
  exit(EXIT_SUCCESS);
}
