#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
// #define TEST_MODE

#define _AOC_LINE_MAX 20001
#include "../utils.h"

#define EMPTY -1
int *disk;
int disk_size = 0;

void fileHandler(int lines) { printf("lines: %d\n", lines); }

void lineHandler(char *line, int length) {
  printf("line (%d): %s\n", length, line);

  // needs to be bigger to accommodate expansion
  disk = disk ? disk : calloc(_AOC_LINE_MAX * 10, sizeof *disk);
  int *ptr = disk;
  int id = 0;
  bool is_empty_space = false;
  do {
    int size = *line - '0';
    // printf("%c\n", *line);
    for (int i = size; i > 0; i--) {
      if (is_empty_space) {
        *ptr = EMPTY;
      } else {
        *ptr = id;
      }
      ptr++;
    }
    id += is_empty_space;
    is_empty_space = !is_empty_space;
  } while (*(++line));
  disk_size = ptr - disk;
}

int *moveFwdToNextFree(int *ptr) {
  while (*ptr != EMPTY) {
    ptr++;
  }
  return ptr;
}

int *moveBackToNextBlock(int *ptr) {
  while (*ptr == EMPTY) {
    ptr--;
  }
  return ptr;
}

void printDisk() {
  for (int i = 0; i < disk_size; i++) {
    int val = disk[i];
    if (val == EMPTY) {
      printf(".");
    } else {
      printf("%d", disk[i]);
    }
  }
  printf("\n");
}

int getSize(int *ptr) {
  int *cur = ptr;
  int inc = *ptr == EMPTY ? 1 : -1;

  int size = 0;
  do {
    size++;
    cur += inc;
  } while (*cur == *ptr);

  return size;
}
// The final step of this file-compacting process is to update the filesystem
// checksum. To calculate the checksum, add up the result of multiplying each of
// these blocks' position with the file ID number it contains. The leftmost
// block is in position 0. If a block contains free space, skip it instead.
//
// Continuing the first example, the first few blocks' position multiplied by
// its file ID number are 0 * 0 = 0, 1 * 0 = 0, 2 * 9 = 18, 3 * 9 = 27, 4 * 8 =
// 32, and so on. In this example, the checksum is the sum of these, 1928.
long getChecksum() {
  int *ptr = disk;
  long checksum = 0;
  for (int i = 0; i < disk_size; i++, ptr++) {
    if (*ptr == EMPTY) {
      continue;
    }
    checksum += i * (*ptr);
    // printf("%d, ", *ptr);
  }
  // do {
  //   // printf("pos: %ld\nval: %c [%d]\n", ptr - disk, *ptr,  (*ptr) - '0');
  //   checksum += (ptr - disk) * (*ptr);
  //   ptr++;
  // } while (count--);
  return checksum;
}

int main() {
  init();
  readInputFile(__FILE__, lineHandler, fileHandler);

  // printDisk();

  // compress the disk
  // starting at the end, move each block to the first available free space
  // until the next available free space is past the block_to_move pointer
  int *head = moveFwdToNextFree(disk);
  int *tail = moveBackToNextBlock(disk + disk_size - 1);
  while (head < tail) {
    // printf("h: %d\n", *head);
    // printf("t: %d\n", *tail);
    *head = *tail;
    *tail = EMPTY;

    head = moveFwdToNextFree(head);
    tail = moveBackToNextBlock(tail);
  }

  // printDisk();
  long checksum_part_one = getChecksum();

  printf("Part one: %ld\n", checksum_part_one);
#ifdef TEST_MODE
  assert(checksum_part_one == 1928);
#else
  assert(checksum_part_one == 6283170117911L);
#endif

  readInputFile(__FILE__, lineHandler, fileHandler);
  // printDisk();

  int *disk_copy = calloc(disk_size, sizeof *disk_copy);
  memcpy(disk_copy, disk, disk_size + 1);

  tail = moveBackToNextBlock(disk + disk_size - 1);
  while (1) {
    int block_size = getSize(tail);

    int *headptr = moveFwdToNextFree(disk);
    if (headptr > tail) {
      break;
    }
    bool fits = false;
    while (headptr < tail) {
      int gap_size = getSize(headptr);

      if (block_size <= gap_size) {
        fits = true; // then sits
        break;
      }
      headptr = moveFwdToNextFree(headptr + gap_size);
    }
    // need to find a gap to the left of the block that can fit
    // otherwise skip block and reset head to start

    // if the block fits in the gap, move it all
    if (fits) {
      while (block_size--) {
        *headptr = *tail;
        *tail = EMPTY;
        headptr++;
        tail--;
      }
    } else {
      // skip the block
      tail -= block_size;
    }
    tail = moveBackToNextBlock(tail);
  }
  // printDisk();

  long checksum_part_two = getChecksum();
  printf("Part two: %ld\n", checksum_part_two);
#ifdef TEST_MODE
  assert(checksum_part_two == 2858);
#else
  assert(checksum_part_two == 6307653242596L);
#endif
  exit(EXIT_SUCCESS);
}
