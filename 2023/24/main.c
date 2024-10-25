#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#define TEST_MODE
#include "../utils.h"

#ifdef TEST_MODE
uint64_t test_area_min = 7;
uint64_t test_area_max = 27;
#else
uint64_t test_area_min = 200000000000000;
uint64_t test_area_max = 400000000000000;
#endif

typedef struct hailstone_t {
  int index;
  uint64_t x;
  uint64_t y;
  uint64_t z;
  int dx;
  int dy;
  int dz;
  double m;
  long double b;
} hailstone_t;

hailstone_t **hailstones;
int hailstone_count = 0;

void fileHandler(int lines) {
  printf("lines: %d\n", lines);

  hailstones = calloc(lines, sizeof(hailstone_t *));
}

void lineHandler(char *line, int length) {
  printf("line (%d): %s\n", length, line);
  hailstone_t *hailstone = malloc(sizeof(hailstone_t));

  sscanf(line, "%lu, %lu, %lu @ %d, %d, %d", &hailstone->x, &hailstone->y,
         &hailstone->z, &hailstone->dx, &hailstone->dy, &hailstone->dz);

  hailstone->index = hailstone_count;
  hailstone->m = ((double)hailstone->dy) / ((double)hailstone->dx);
  hailstone->b = (long double)hailstone->y - (hailstone->m * hailstone->x);
  hailstones[hailstone_count++] = hailstone;
}

void printHailstone(char *tag, hailstone_t *h) {
  printf("%s%lu, %lu, %lu @ %d, %d, %d\n\tm = %f, b = %Lf\n", tag, h->x, h->y,
         h->z, h->dx, h->dy, h->dz, h->m, h->b);
}
void printHailstones() {
  for (int i = 0; i < hailstone_count; i++) {
    hailstone_t *h = hailstones[i];
    printHailstone("", h);
  }
}

int main() {
  init();
  readInputFile(__FILE__, lineHandler, fileHandler);

  printHailstones();

  printf("\n\n---\n\n");

  int intersections = 0;
  for (int i = 0; i < hailstone_count; i++) {
    hailstone_t *A = hailstones[i];
    for (int j = i + 1; j < hailstone_count; j++) {
      hailstone_t *B = hailstones[j];
      // if (seenOrSamePair(A, B)) {
      // continue;
      // }
      // printHailstone("\nA ", A);
      // printHailstone("B ", B);
      // printf("%d + %d: ", A->index, B->index);
      long double x = (B->b - A->b) / (A->m - B->m);
      long double y = (A->m * (x)) + A->b;
      long double t_a = (x - A->x) / A->dx;
      long double t_b = (x - B->x) / B->dx;
      if (isinf(x) || isinf(y)) {
        printf("parallel\n");
      } else if (t_a < 0) {
        // printf("in past for A\n");
      } else if (t_b < 0) {
        // printf("in past for B\n");
      } else if (x > test_area_min && x < test_area_max && y > test_area_min &&
                 y < test_area_max) {
        // printf("intersects x = %Lf, y = %Lf\n", x, y);
        intersections++;
      } else {
        // printf("outside area\n");
      }
    }
  }

  printf("Part one: %d\n", intersections);
#ifdef TEST_MODE
  assert(intersections == 2);
#else
  assert(intersections == 19976);
#endif

  printf("Part two: %d\n", 420);
#ifdef TEST_MODE
  assert(420 == 69);
#else
  assert(420 == 69);
#endif
  exit(EXIT_SUCCESS);
}
