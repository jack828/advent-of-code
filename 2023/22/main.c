#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
// #define TEST_MODE
#include "../utils.h"

typedef enum axis_t { X = 0, Y = 1, Z = 2 } axis_t;

typedef struct block_t {
  char letter;
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
  blocks[block_count++] = block;

  printf(">>> %d,%d,%d~%d,%d,%d (%d)\n\n", x_s, y_s, z_s, x_e, y_e, z_e,
         block->axis);
  x_max = max(x_max, max(x_s, x_e));
  y_max = max(y_max, max(y_s, y_e));
  z_max = max(z_max, max(z_s, z_e));
}

void printBlock(char *tag, block_t *block) {
  printf("%s '%c' (%d,%d,%d) -> (%d,%d,%d) [%d] ", tag, block->letter,
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

bool settleBlocks(bool quit_if_moves) {
  // for each z axis,
  bool has_moves = false;
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
            // printf("settled\n");
            block->settled = true;
            break;
          } else {
            if (quit_if_moves) {
              // printBlock("moves", block);
              return true;
            } else {
              // update position and keep going
              // printf("moving!\n");
              // printBlock("moved b", block);
              block->z_s -= 1;
              block->z_e -= 1;
              // printBlock("moved a", block);
              has_moves = true;
            }
          }
          new_z--;
          // and set settled = true
          // break;
        } // while (true);
      }
    }
  }
  return has_moves;
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

void resetSettled() {
  for (int i = 0; i < block_count; i++) {
    block_t *block = blocks[i];
    block->settled = false;
  }
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

int main() {
  init();
  readInputFile(__FILE__, lineHandler, fileHandler);

  // input needs to be sorted in ascending Z index
  // so the lowest one is dropped first
  printBlock("before sort", blocks[0]);
  qsort(blocks, block_count, sizeof(block_t *), compare);

  printBlock(" after sort", blocks[0]);
  /* grid = calloc(x_max + 1, sizeof(char **));
  for (int x = 0; x <= x_max; x++) {
    printf("x: %d\n", x);
    grid[x] = calloc(y_max + 1, sizeof(char *));
    for (int y = 0; y <= y_max; y++) {
      printf("y: %d\n", y);
      grid[x][y] = calloc(z_max + 1, sizeof(char));
    }
  } */

  // assert(pointInBlock(1, 0, 1, blocks[0]) == true);
  // assert(pointInBlock(1, 1, 1, blocks[0]) == true);
  // assert(pointInBlock(1, 2, 1, blocks[0]) == true);
  // assert(pointInBlock(1, 2, 0, blocks[0]) == false);

  printf("max %d,%d,%d\n\n", x_max, y_max, z_max);

  // while(settleBlocks(false));
  settleBlocks(false);

  printBlockTower();

  int can_disintegrate = 0;
  // for (int i = 0; i < 1; i++) {
  for (int i = 0; i < block_count; i++) {
    block_t *block = blocks[i];

    // "delete" block ref in blocks[]
    // run settleBlocks

    resetSettled();
    blocks[i] = NULL;
    printBlock("can destroy?", block);

    bool quit_if_moves = true;
    bool has_changes = settleBlocks(quit_if_moves);
    if (!has_changes) {
      can_disintegrate++;
      printBlock("\tdestroy", block);
    } else {
      printBlock("\t  NO", block);
    }

    blocks[i] = block;
  }
  printf("Part one: %d\n", can_disintegrate);
#ifdef TEST_MODE
  assert(can_disintegrate == 5);
#else
  assert(can_disintegrate == 421);
#endif

  printf("Part two: %d\n", 420);
#ifdef TEST_MODE
  assert(420 == 69);
#else
  assert(420 == 69);
#endif
  exit(EXIT_SUCCESS);
}
