#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
// #define TEST_MODE
#include "../../lib/queue.h"
#include "../utils.h"

typedef enum axis_t { X = 0, Y = 1, Z = 2 } axis_t;

typedef struct block_t {
  int letter;
  // start
  int x_s;
  int y_s;
  int z_s;
  // end
  int x_e;
  int y_e;
  int z_e;
  bool settled;
  // which dir the block is facing
  axis_t axis;
  int length;

  // blocks that this block supports
  struct block_t **supports;
  int support_count;

  // blocks that are supported by this one
  struct block_t **supported_by;
  int supported_by_count;
} block_t;

block_t **blocks;
int block_count = 0;
int x_max = 0;
int y_max = 0;
int z_max = 0;

char ***grid;

void fileHandler(int lines) {
  printf("lines: %d\n", lines);
  blocks = calloc(lines, sizeof(block_t *));
}

void lineHandler(char *line, int length) {
  printf("line (%d): %s\n", length, line);
  int x_s, y_s, z_s, x_e, y_e, z_e;

  sscanf(line, "%d,%d,%d~%d,%d,%d", &x_s, &y_s, &z_s, &x_e, &y_e, &z_e);

  block_t *block = malloc(sizeof(block_t));
  block->letter = 'a' + block_count;
  block->x_s = x_s;
  block->y_s = y_s;
  block->z_s = z_s;
  block->x_e = x_e;
  block->y_e = y_e;
  block->z_e = z_e;
  block->settled = false;
  block->axis = x_s != x_e ? X : (y_s != y_e ? Y : Z);
  switch (block->axis) {
  case X:
    block->length = x_e - x_s;
    break;
  case Y:
    block->length = y_e - y_s;
    break;
  case Z:
    block->length = z_e - z_s;
    break;
  }
  block->supports =
      calloc(block->length + 1, // can support maximum one per point
             sizeof(block_t *));
  block->support_count = 0;
  block->supported_by =
      calloc(block->length + 1, // can support maximum one per point
             sizeof(block_t *));
  block->supported_by_count = 0;
  blocks[block_count++] = block;

  // printf(">>> %d,%d,%d~%d,%d,%d (%d)\n\n", x_s, y_s, z_s, x_e, y_e, z_e,
  // block->axis);
  x_max = max(x_max, max(x_s, x_e));
  y_max = max(y_max, max(y_s, y_e));
  z_max = max(z_max, max(z_s, z_e));
}

void printBlock(char *tag, block_t *block) {
#ifdef TEST_MODE
#define LETTER "%c"
#else
#define LETTER "%d"
#endif
  printf("%s '" LETTER "' (%d,%d,%d) -> (%d,%d,%d) [%d] ", tag, block->letter,
         block->x_s, block->y_s, block->z_s, block->x_e, block->y_e, block->z_e,
         block->length + 1);
  switch (block->axis) {
  case X:
    printf("X");
    break;
  case Y:
    printf("Y");
    break;
  case Z:
    printf("Z");
    break;
  }
  if (block->support_count) {
    printf("\n\tSupports:     ");
    for (int i = 0; i < block->support_count; i++) {
      block_t *support_block = block->supports[i];
      printf(LETTER ",", support_block->letter);
    }
  }
  if (block->supported_by_count) {
    printf("\n\tSupported by: ");
    for (int i = 0; i < block->supported_by_count; i++) {
      block_t *supported_by_block = block->supported_by[i];
      printf(LETTER ",", supported_by_block->letter);
    }
  }
  printf("\n\n");
}

bool pointInBlock(int x, int y, int z, block_t *block) {
  if (block == NULL) {
    return false;
  }
  switch (block->axis) {
  case X: // extends in X axis, check y,z equal and x in range
    return (y == block->y_s) && (z == block->z_s) &&
           (x >= block->x_s && x <= block->x_e);
  case Y: // extends in Y axis, check x,z equal and y in range
    return (x == block->x_s) && (z == block->z_s) &&
           (y >= block->y_s && y <= block->y_e);
  case Z: // extends in Z axis, check x,y equal and z in range
    return (x == block->x_s) && (y == block->y_s) &&
           (z >= block->z_s && z <= block->z_e);
  default:
    return false;
  }
}

// does this (x,y,z) occur within a block?
// Which block?
block_t *blockIntersects(int x, int y, int z) {
  // printf("intersects (%d,%d,%d)\n", x, y, z);
  for (int i = 0; i < block_count; i++) {
    block_t *block = blocks[i];
    bool intersects = pointInBlock(x, y, z, block);
    if (intersects) {
      return block;
    }
  }
  return NULL;
}

void settleBlocks() {
  // for each z axis,
  for (int z = 1; z <= z_max; z++) {
    for (int x = 0; x <= x_max; x++) {
      for (int y = 0; y <= y_max; y++) {
        // examine each x,y
        block_t *block = blockIntersects(x, y, z);
        if (block == NULL) {
          continue;
        }
        // printf("\n\ncoord (%d,%d,%d)\n", x, y, z);
        // printBlock("settling", block);

        // if there is a block, (try to) move it down
        int new_x = x;
        int new_y = y;
        int new_z = z - 1;
        while (!block->settled) {
          bool can_move_down = true;
          // a block can move if ALL spaces below it are free

          // if axis is not z, then need to look across x,y

          switch (block->axis) {
          case X: {
            // look below each point on the x axis of the block
            for (int dX = 0; dX <= block->length; dX++) {
              if (new_z < 1) {
                // printf("bottomed (%d,%d,%d)\n", new_x + dX, new_y, new_z);
                can_move_down = false;
                break;
              }
              block_t *touches_block =
                  blockIntersects(new_x + dX, new_y, new_z);
              if (touches_block != NULL) {
                // printf("touches (%d,%d,%d) [%c]\n", new_x + dX, new_y, new_z,
                // touches_block->letter);
                can_move_down = false;
                break;
              }
            }
            break;
          }
          case Y: {
            // look below each point on the x axis of the block
            for (int dY = 0; dY <= block->length; dY++) {
              if (new_z < 1) {
                // printf("bottomed (%d,%d,%d)\n", new_x, new_y + dY, new_z);
                can_move_down = false;
                break;
              }
              block_t *touches_block =
                  blockIntersects(new_x, new_y + dY, new_z);
              if (touches_block != NULL) {
                // printf("touches (%d,%d,%d) [%c]\n", new_x, new_y + dY, new_z,
                // touches_block->letter);
                can_move_down = false;
                break;
              }
            }
            break;
          }
          case Z: {
            // look below the start point of the block
            if (new_z < 1) {
              // printf("bottomed (%d,%d,%d)\n", new_x, new_y, new_z);
              can_move_down = false;
              break;
            }
            block_t *touches_block = blockIntersects(new_x, new_y, new_z);
            if (touches_block != NULL) {
              // printf("touches (%d,%d,%d) [%c]\n", new_x, new_y, new_z,
              // touches_block->letter);
              can_move_down = false;
              break;
            }
            break;
          }
          }

          // we cannot move any more
          if (can_move_down == false) {
            block->settled = true;
            break;
          } else {
            // update position and keep going
            block->z_s -= 1;
            block->z_e -= 1;
          }
          new_z--;
        }
      }
    }
  }
}

// block A supports block B, without it, B would fall
void addBlockSupport(block_t *a, block_t *b) {
  // first check it is not already included
  for (int i = 0; i < a->support_count; i++) {
    block_t *block = a->supports[i];
    if (block->letter == b->letter) {
      // already know about this one!
      return;
    }
  }
  // add it
  a->supports[a->support_count++] = b;
}

// block B is supported by block A, without it, B would fall
void addBlockSupportedBy(block_t *a, block_t *b) {
  // first check it is not already included
  for (int i = 0; i < a->supported_by_count; i++) {
    block_t *block = a->supported_by[i];
    if (block->letter == b->letter) {
      // already know about this one!
      return;
    }
  }
  // add it
  a->supported_by[a->supported_by_count++] = b;
}

void calculateSupportingBlocks() {
  // for each block
  // check each point above it
  // if it is a block
  // add to supports[]
  // if not already present
  for (int i = 0; i < block_count; i++) {
    block_t *block = blocks[i];

    // printBlock("supports", block);
    int new_z = block->z_s + 1;
    switch (block->axis) {
    case X: {
      // look below each point on the x axis of the block
      for (int dX = 0; dX <= block->length; dX++) {
        block_t *touches_block =
            blockIntersects(block->x_s + dX, block->y_s, new_z);
        if (touches_block != NULL) {
          // printf("touches (%d,%d,%d) [%c]\n", block->x_s + dX, block->y_s,
          // new_z, touches_block->letter);
          addBlockSupport(block, touches_block);
          addBlockSupportedBy(touches_block, block);
        }
      }
      break;
    }
    case Y: {
      // look below each point on the x axis of the block
      for (int dY = 0; dY <= block->length; dY++) {
        block_t *touches_block =
            blockIntersects(block->x_s, block->y_s + dY, new_z);
        if (touches_block != NULL) {
          // printf("touches (%d,%d,%d) [%c]\n", block->x_s, block->y_s + dY,
          // new_z, touches_block->letter);
          addBlockSupport(block, touches_block);
          addBlockSupportedBy(touches_block, block);
        }
      }
      break;
    }
    case Z: {
      // look below the start point of the block
      new_z = block->z_e + 1;
      block_t *touches_block = blockIntersects(block->x_s, block->y_s, new_z);
      if (touches_block != NULL) {
        // printf("touches (%d,%d,%d) [%c]\n", block->x_s, block->y_s, new_z,
        // touches_block->letter);
        addBlockSupport(block, touches_block);
        addBlockSupportedBy(touches_block, block);
      }
    } break;
    }
  }
}

void printBlockTower() {
  for (int z = z_max; z > 0; z--) {
    // printf("z: %d\n", z);
    for (int x = 0; x <= x_max; x++) {
      // int x = 0;
      for (int y = 0; y <= y_max; y++) {
        block_t *block = blockIntersects(x, y, z);
        if (block != NULL) {
          printf("%c", block->letter);
        } else {
          printf(".");
        }
      }
      printf("   ");
    }
    printf("\n");
  }
  printf("---   ---   ---\n");
}

int compare(const void *left, const void *right) {
  block_t *left_block = *(block_t **)left;
  block_t *right_block = *(block_t **)right;

  if (left_block->z_s < right_block->z_s) {
    return -1;
  } else {
    return 1;
  }
}

// DFS or BFS, whatevs
// BFS for each brick - keep a vector of "fallen" brick indexes, then for
// each supported brick, enqueue it if all of its "supported_by" bricks have
// fallen
int countDependantBlocks(block_t *startBlock) {
  // so we can index by `block->letter`
  char fallen_indexes[block_count + 'a'];
  memset(fallen_indexes, 0, (block_count + 'a') * sizeof(char));

  queue_t *queue = q_create();

  q_enqueue(queue, startBlock);

  while (!q_empty(queue)) {
    block_t *block = q_dequeue(queue);
    // this block has fallen
    fallen_indexes[block->letter] = 1;
    for (int i = 0; i < block->support_count; i++) {
      block_t *support_block = block->supports[i];

      bool add = true;
      for (int j = 0; j < support_block->supported_by_count; j++) {
        block_t *supported_block = support_block->supported_by[j];
        if (fallen_indexes[supported_block->letter] == 0) {
          // not eligible
          add = false;
          continue;
        }
      }
      if (add) {
        q_enqueue(queue, support_block);
      }
    }
  }
  q_destroy(queue);

  // -1, because the first block doesn't fall,
  // it disintegrates
  int fall_count = -1;
  for (int i = 0; i < block_count + 'a'; i++) {
    if (fallen_indexes[i]) {
      fall_count++;
    }
  }
  return fall_count;
}

int main() {
  init();
  readInputFile(__FILE__, lineHandler, fileHandler);

  // input needs to be sorted in ascending Z index
  // so the lowest one is dropped first
  qsort(blocks, block_count, sizeof(block_t *), compare);

  // tests
  // assert(pointInBlock(1, 0, 1, blocks[0]) == true);
  // assert(pointInBlock(1, 1, 1, blocks[0]) == true);
  // assert(pointInBlock(1, 2, 1, blocks[0]) == true);
  // assert(pointInBlock(1, 2, 0, blocks[0]) == false);

  // printf("max %d,%d,%d\n\n", x_max, y_max, z_max);

  settleBlocks();
  // printBlockTower();
  calculateSupportingBlocks();


  block_t **unsafe_blocks = calloc(block_count, sizeof(block_t *));
  int unsafe_block_count = 0;
  int can_disintegrate = 0;
  // can destroy a block if
  // the blocks it supports have > 1 supported_by_count
  for (int i = 0; i < block_count; i++) {
    block_t *block = blocks[i];

    // printBlock("disintegrate", block);

    bool safe = true;
    for (int j = 0; j < block->support_count; j++) {
      block_t *supported_block = block->supports[j];
      if (supported_block->supported_by_count == 1) {
        safe = false;
      }
    }
    // printf("safe: %c\n", safe ? 'Y' : 'N');
    if (safe) {
      can_disintegrate++;
    } else {
      unsafe_blocks[unsafe_block_count++] = block;
    }
  }
  printf("Part one: %d\n", can_disintegrate);
#ifdef TEST_MODE
  assert(can_disintegrate == 5);
#else
  assert(can_disintegrate == 421);
#endif

  // printf("unsafe: %d\n", unsafe_block_count);
  int fall_sum = 0;
  for (int i = 0; i < unsafe_block_count; i++) {
    block_t *block = unsafe_blocks[i];
    // printBlock("unsafe", block);
    fall_sum += countDependantBlocks(block);
  }
  printf("Part two: %d\n", fall_sum);
#ifdef TEST_MODE
  assert(fall_sum == 7);
#else
  assert(fall_sum == 39247);
#endif
  exit(EXIT_SUCCESS);
}
