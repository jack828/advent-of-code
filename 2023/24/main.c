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
  double slope;
  long double intercept;
} hailstone_t;

hailstone_t **hailstones;
int hailstone_count = 0;
uint32_t *hashmap;
#define HASHMAP_SIZE (UINT32_MAX)

void fileHandler(int lines) {
  printf("lines: %d\n", lines);

  hailstones = calloc(lines, sizeof(hailstone_t *));
  hashmap = calloc(HASHMAP_SIZE, sizeof(uint32_t));
}

void lineHandler(char *line, int length) {
  printf("line (%d): %s\n", length, line);
  hailstone_t *hailstone = malloc(sizeof(hailstone_t));

  sscanf(line, "%lu, %lu, %lu @ %d, %d, %d", &hailstone->x, &hailstone->y,
         &hailstone->z, &hailstone->dx, &hailstone->dy, &hailstone->dz);

  hailstone->index = hailstone_count;
  hailstone->slope = ((double)hailstone->dy) / ((double)hailstone->dx);
  hailstone->intercept = (long double)hailstone->y - (hailstone->slope * hailstone->x);
  hailstones[hailstone_count++] = hailstone;
}

void printHailstone(char *tag, hailstone_t *h) {
  printf("%s%lu, %lu, %lu @ %d, %d, %d\n\tm = %f, b = %Lf\n", tag, h->x, h->y,
         h->z, h->dx, h->dy, h->dz, h->slope, h->intercept);
}
void printHailstones() {
  for (int i = 0; i < hailstone_count; i++) {
    hailstone_t *h = hailstones[i];
    printHailstone("", h);
  }
}

bool seenOrSamePair(hailstone_t *A, hailstone_t *B) {
  if (A->index == B->index) {
    return true;
  }
  uint16_t a = A->index;
  uint16_t b = B->index;
  if (b > a) {
    a = B->index;
    b = A->index;
  }
  uint32_t hash = 0;
  hash |= (uint32_t)(a);
  hash |= (uint32_t)(b) << 16;
  hash = hash % HASHMAP_SIZE;
  if (hashmap[hash]) {
    return true;
  }
  hashmap[hash] = 1;
  return false;
}

int main() {
  init();
  readInputFile(__FILE__, lineHandler, fileHandler);

  printHailstones();

  printf("\n\n---\n\n");
  /* hailstone_t *A = hailstones[1];
  hailstone_t *B = hailstones[4];
  printHailstone("\nA ", A);
  printHailstone("B ", B);
  double x = (B->intercept - A->intercept) / (A->slope - B->slope);
  double y = (A->slope * (x)) + A->intercept;
  if (isinf(x) || isinf(y)) {
    printf("parrallel\n");
  } else if ((A->dx < 0 && A->x < x) || (A->dy < 0 && A->y < y)) {
    printf("in past for A\n");
  } else if ((B->dx < 0 && B->x < x) || (B->dy < 0 && B->y < y)) {
    printf("in past for B\n");
  } else if (x > test_area_min && x < test_area_max && y > test_area_min &&
             y < test_area_max) {
    printf("intercept x = %f, y = %f\n", x, y);
  } */
  int intersections = 0;
  for (int i = 0; i < hailstone_count; i++) {
    hailstone_t *A = hailstones[i];
    for (int j = 0; j < hailstone_count; j++) {
      hailstone_t *B = hailstones[j];
      if (seenOrSamePair(A, B)) {
        continue;
      }
      // printHailstone("\nA ", A);
      // printHailstone("B ", B);
      // printf("%d + %d: ", A->index, B->index);
      long double x = (B->intercept - A->intercept) / (A->slope - B->slope);
      long double y = (A->slope * (x)) + A->intercept;
      if (isinf(x) || isinf(y)) {
        // printf("parallel\n");
      } else if ((A->dx < 0 && A->x < x) || (A->dy < 0 && A->y < y)) {
        // printf("in past for A\n");
      } else if ((B->dx < 0 && B->x < x) || (B->dy < 0 && B->y < y)) {
        // printf("in past for B\n");
      } else if (x > test_area_min && x < test_area_max && y > test_area_min &&
                 y < test_area_max) {
        // printf("intercept x = %f, y = %f\n", x, y);
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
  assert(intersections < 21852);
#endif

  printf("Part two: %d\n", 420);
#ifdef TEST_MODE
  assert(420 == 69);
#else
  assert(420 == 69);
#endif
  exit(EXIT_SUCCESS);
}
