#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
// #define TEST_MODE
#include "../utils.h"

#ifdef TEST_MODE
uint64_t test_area_min = 7;
uint64_t test_area_max = 27;
#else
uint64_t test_area_min = 200000000000000;
uint64_t test_area_max = 400000000000000;
#endif

typedef enum dir_t { X = 0, Y = 1, Z = 2 } dir_t;

typedef struct vec_t {
  uint64_t x;
  uint64_t y;
  uint64_t z;
  int dx;
  int dy;
  int dz;
} vec_t;

typedef struct hailstone_t {
  int index;
  int64_t x;
  int64_t y;
  int64_t z;
  int dx;
  int dy;
  int dz;
  double m;
  long double b;
} hailstone_t;

hailstone_t **hailstones;
int hailstone_count = 0;
int max_v = 0;
int64_t **shared_vxs;
int shared_vxs_count = 0;
int64_t **shared_vys;
int shared_vys_count = 0;
int64_t **shared_vzs;
int shared_vzs_count = 0;

void fileHandler(int lines) {
  printf("lines: %d\n", lines);

  hailstones = calloc(lines, sizeof(hailstone_t *));
  shared_vxs = calloc(500, sizeof(int64_t *));
  shared_vys = calloc(500, sizeof(int64_t *));
  shared_vzs = calloc(500, sizeof(int64_t *));
}

void lineHandler(char *line, int length) {
  printf("line (%d): %s\n", length, line);
  hailstone_t *hailstone = malloc(sizeof(hailstone_t));

  sscanf(line, "%ld, %ld, %ld @ %d, %d, %d", &hailstone->x, &hailstone->y,
         &hailstone->z, &hailstone->dx, &hailstone->dy, &hailstone->dz);

  hailstone->index = hailstone_count;
  hailstone->m = ((double)hailstone->dy) / ((double)hailstone->dx);
  hailstone->b = (long double)hailstone->y - (hailstone->m * hailstone->x);
  hailstones[hailstone_count++] = hailstone;
  max_v = max(max_v, max(abs(hailstone->dx),
                         max(abs(hailstone->dy), abs(hailstone->dz))));
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

int64_t abs64(int64_t v) { return v < 0 ? -v : v; }

int64_t getHailstonePos(hailstone_t *a, dir_t dir) {
  return dir == X ? a->x : dir == Y ? a->y : a->z;
}
int64_t getHailstoneVel(hailstone_t *a, dir_t dir) {
  return dir == X ? a->dx : dir == Y ? a->dy : a->dz;
}

// Following functions are a translation of
// https://github.com/johnbeech/advent-of-code-2023/blob/main/solutions/day24/solution.js
// because i cant
int64_t *intersect(hailstone_t *a, hailstone_t *b, int shift0, int shift1,
                   dir_t coords0, dir_t coords1) {
  int64_t apos0 = getHailstonePos(a, coords0);
  int64_t apos1 = getHailstonePos(a, coords1);
  int64_t avel0 = getHailstoneVel(a, coords0);
  int64_t avel1 = getHailstoneVel(a, coords1);
  int64_t bpos0 = getHailstonePos(b, coords0);
  int64_t bpos1 = getHailstonePos(b, coords1);
  int64_t bvel0 = getHailstoneVel(b, coords0);
  int64_t bvel1 = getHailstoneVel(b, coords1);
  int64_t x1 = apos0;
  int64_t y1 = apos1;
  int64_t x2 = apos0 + avel0 - shift0;
  int64_t y2 = apos1 + avel1 - shift1;
  int64_t x3 = bpos0;
  int64_t y3 = bpos1;
  int64_t x4 = bpos0 + bvel0 - shift0;
  int64_t y4 = bpos1 + bvel1 - shift1;

  int64_t ua;

  int64_t denom = (y4 - y3) * (x2 - x1) - (x4 - x3) * (y2 - y1);
  if (denom == 0) {
    return NULL;
  }
  ua = ((x4 - x3) * (y1 - y3) - (y4 - y3) * (x1 - x3)) / denom;
  int64_t *res = calloc(2, sizeof(int64_t));
  res[0] = x1 + ua * (x2 - x1);
  res[1] = y1 + ua * (y2 - y1);
  return res;
}

int64_t getTime(hailstone_t *a, int64_t v, int64_t shift, dir_t coord) {
  int64_t pos = getHailstonePos(a, coord);
  int64_t numerator = v - pos;
  int64_t vel = getHailstoneVel(a, coord);
  int64_t denominator = vel - shift;
  if (numerator == 0 || denominator == 0) {
    return 0;
  }
  return numerator / denominator;
}

int64_t get_starting_coordinate_sum() {
  int64_t result = 0;
  for (int vz = -300; vz <= 300; vz++) {
    if (abs(vz) % 10 == 0) {
      printf("testing velocity z %d\n", vz);
    }
    // you could lower these to make it faster
    // but i don't know a general way of reasoning why they SHOULD be lower
    for (int vx = -300; vx <= 300; vx++) {
      for (int vy = -300; vy <= 300; vy++) {
        // [vx, vy, vz] is the velocity of our rock
        // we subtract rock velocity from each sandstone's velocity and look for
        // common intersection
        bool skip = false;
        int cnt = 0;

        for (int i = 0; i < hailstone_count; i++) {
          hailstone_t *A = hailstones[i];
          for (int j = i + 1; j < hailstone_count; j++) {
            hailstone_t *B = hailstones[j];
            int64_t *intXY = intersect(A, B, vx, vy, X, Y);
            int64_t *intYZ = intersect(A, B, vy, vz, Y, Z);
            int64_t *intXZ = intersect(A, B, vx, vz, X, Z);

            int parallelCount = 0;
            if (intXY == NULL) {
              parallelCount++;
            }
            if (intYZ == NULL) {
              parallelCount++;
            }
            if (intXZ == NULL) {
              parallelCount++;
            }

            if (parallelCount > 2) {
              skip = true;
              if (intXY) {
                free(intXY);
              }
              if (intYZ) {
                free(intYZ);
              }
              if (intXZ) {
                free(intXZ);
              }
              break;
            }

            int matches = 0;
            int64_t t = 0;
            int64_t tj = 0;

            if (intXY != NULL) {
              int64_t t1 = getTime(A, intXY[0], vx, X);
              if (t1 == 0)
                t1 = getTime(A, intXY[1], vy, 1);

              int64_t t1j = getTime(B, intXY[0], vx, X);
              if (t1j == 0)
                t1j = getTime(B, intXY[1], vy, Y);

              if (t == 0)
                t = t1;
              if (tj == 0)
                tj = t1j;

              if (t1 > 0 && t1j > 0 && t == t1 && tj == t1j)
                matches++;
            }

            if (intYZ != NULL) {
              int64_t t2 = getTime(A, intYZ[0], vy, Y);
              if (t2 == 0)
                t2 = getTime(A, intYZ[1], vz, Z);

              int64_t t2j = getTime(B, intYZ[0], vy, Y);
              if (t2j == 0)
                t2j = getTime(B, intYZ[1], vz, Z);

              if (t == 0)
                t = t2;
              if (tj == 0)
                tj = t2j;

              if (t2 > 0 && t2j > 0 && t == t2 && tj == t2j)
                matches++;
            }

            if (intXZ != NULL) {
              int64_t t3 = getTime(A, intXZ[0], vx, X);
              if (t3 == 0)
                t3 = getTime(A, intXZ[1], vz, Z);

              int64_t t3j = getTime(B, intXZ[0], vx, Z);
              if (t3j == 0)
                t3j = getTime(B, intXZ[1], vz, Z);

              if (t == 0)
                t = t3;
              if (tj == 0)
                tj = t3j;

              if (t3 > 0 && t3j > 0 && t == t3 && tj == t3j)
                matches++;
            }

            if (matches >= 2)
              cnt++;
            else {
              skip = true;
              if (intXY) {
                free(intXY);
              }
              if (intYZ) {
                free(intYZ);
              }
              if (intXZ) {
                free(intXZ);
              }
              break;
            }

            if (cnt >= 20 && parallelCount == 0) {
              printf("solution: (%ld,%ld,%ld) == %ld\n", intXY[0], intXY[1],
                     intYZ[1], intXY[0] + intXY[1] + intYZ[1]);

              result = intXY[0] + intXY[1] + intYZ[1];
              free(intXY);
              free(intYZ);
              free(intXZ);
              goto return_statement;
            }
            if (intXY) {
              free(intXY);
            }
            if (intYZ) {
              free(intYZ);
            }
            if (intXZ) {
              free(intXZ);
            }
          }
          if (skip)
            break;
        }
      }
    }
  }
return_statement:
  return result;
}

int main() {
  init();
  readInputFile(__FILE__, lineHandler, fileHandler);

  // printHailstones();

  int intersections = 0;
  for (int i = 0; i < hailstone_count; i++) {
    hailstone_t *A = hailstones[i];
    for (int j = i + 1; j < hailstone_count; j++) {
      hailstone_t *B = hailstones[j];
      // printHailstone("\nA ", A);
      // printHailstone("B ", B);
      // printf("%d + %d: ", A->index, B->index);
      long double x = (B->b - A->b) / (A->m - B->m);
      long double y = (A->m * (x)) + A->b;
      long double t_a = (x - A->x) / A->dx;
      long double t_b = (x - B->x) / B->dx;
      if (isinf(x) || isinf(y)) {
        // printf("parallel\n");
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

  // I freaking tried but maths like this takes me back and not in a good way :c
  int64_t starting_sum = get_starting_coordinate_sum();

  printf("Part two: %ld\n", starting_sum);
#ifdef TEST_MODE
  assert(starting_sum == 47);
#else
  assert(starting_sum == 849377770236905);
#endif
  exit(EXIT_SUCCESS);
}
