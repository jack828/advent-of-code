#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
// #define TEST_MODE
#include "../../lib/pqueue.h"
#include "../utils.h"
#define HASHMAP_SIZE (UINT16_MAX)

typedef struct node_t node_t;
typedef struct edge_t edge_t;

struct edge_t {
  node_t *node;
  int dist;
};

struct node_t {
  int index;
  int y;
  int x;
  edge_t **edges;
  int edge_count;
};

typedef struct point_t {
  int y;
  int x;
  int s; // steps
  char *hashmap;
  // part two only
  node_t *from_node;

  node_t *node; // for the final DFS
} point_t;

char **map;
int height = 0;
int width = 0;
int line_i = 0;
node_t **nodes;
int node_count = 0;

void fileHandler(int lines) {
  printf("lines: %d\n", lines);
  map = calloc(lines, sizeof(char *));
  height = lines;
  nodes = calloc(height, sizeof(node_t *));
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

void printMap() {
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      int c = map[y][x];
      printf("%c", c);
    }
    printf("\n");
  }
}

uint64_t hash_point(point_t *point) {
  uint64_t hash = 0;
  hash |= (uint64_t)(point->y);
  hash |= (uint64_t)(point->x) << 8;
  return hash % HASHMAP_SIZE;
}

void add_point(pqueue_t *queue, int y, int x, int s, char *hashmap) {
  point_t *new_point = malloc(sizeof(point_t));
  new_point->y = y;
  new_point->x = x;
  new_point->s = s;
  new_point->hashmap = calloc(HASHMAP_SIZE, sizeof(char));
  memcpy(new_point->hashmap, hashmap, HASHMAP_SIZE);
  pq_enqueue(queue, new_point, -s);
}

void destroy_point(point_t *point) {
  if (point->hashmap != NULL) {
    free(point->hashmap);
  }
  free(point);
}

bool isInvalidPoint(int y, int x, char *hashmap) {
  bool invalid =
      y < 0 || x < 0 || y >= height || x >= width || map[y][x] == '#';
  if (invalid) {
    return invalid;
  }

  if (hashmap == NULL) {
    return false;
  }

  uint64_t key = 0;
  key |= (uint64_t)(y);
  key |= (uint64_t)(x) << 8;

  return hashmap[key] == 1;
}

// or bfs
int DFS(int start_x, int start_y, int end_y, int end_x) {
  printf("DFS: (%d,%d) -> (%d,%d)\n", start_x, start_y, end_y, end_x);

  pqueue_t *output = pq_create();
  pqueue_t *queue = pq_create();

  point_t *new_point = malloc(sizeof(point_t));
  new_point->y = start_y;
  new_point->x = start_x;
  new_point->s = 1;
  new_point->hashmap = calloc(HASHMAP_SIZE, sizeof(char));
  pq_enqueue(queue, new_point, new_point->s);

  while (!pq_empty(queue)) {
    point_t *point = pq_dequeue(queue);

    if (point->y == end_y && point->x == end_x) {
      // end reached!
      // printf("END: (%d,%d) -> [%d]\n", point->x, point->y, point->s);
      pq_enqueue(output, point, point->s);
      // free'd later
      continue;
    }

    uint64_t key = hash_point(point);

    point->hashmap[key] = 1;

    // printf("point: (%d,%d) -> [%d]\n", point->x, point->y, point->s);

    char pos = map[point->y][point->x];
    // are we forced to make a move?
    if (pos == '>') {
      if (!isInvalidPoint(point->y, point->x + 1, point->hashmap)) {
        add_point(queue, point->y, point->x + 1, point->s + 1, point->hashmap);
      }
    } else if (pos == '<') {
      if (!isInvalidPoint(point->y, point->x - 1, point->hashmap)) {
        add_point(queue, point->y, point->x - 1, point->s + 1, point->hashmap);
      }
    } else if (pos == 'v') {
      if (!isInvalidPoint(point->y + 1, point->x, point->hashmap)) {
        add_point(queue, point->y + 1, point->x, point->s + 1, point->hashmap);
      }
    } else {
      // otherwise go everywhere valid
      int dY[] = {-1, 1, 0, 0};
      int dX[] = {0, 0, 1, -1};
      for (int i = 0; i < 4; i++) {
        if (!isInvalidPoint(point->y + dY[i], point->x + dX[i],
                            point->hashmap)) {
          add_point(queue, point->y + dY[i], point->x + dX[i], point->s + 1,
                    point->hashmap);
        }
      }
    }

    destroy_point(point);
  }
  pq_destroy(queue);

  int max_steps = 0;
  while (!pq_empty(output)) {
    point_t *point = pq_dequeue(output);
    // printf("output: %d\n", point->s);
    max_steps = point->s + 1; // we skip a step to make it easier
    destroy_point(point);
  }
  pq_destroy(output);

  return max_steps;
}

node_t *findOrCreateNode(int y, int x) {
  for (int i = 0; i < node_count; i++) {
    node_t *found_node = nodes[i];
    if (found_node->y == y && found_node->x == x) {
      // printf("found node\n");
      return found_node;
    }
  }

  // i make a new one
  // printf("new node\n");
  node_t *new_node = malloc(sizeof(node_t));
  new_node->y = y;
  new_node->x = x;
  new_node->index = node_count;
  new_node->edges = calloc(4, sizeof(edge_t *));
  new_node->edge_count = 0;
  nodes[node_count++] = new_node;
  return new_node;
}

void dfsToBuildGraph(int start_x, int start_y, int end_y, int end_x) {
  printf("DFS for graph: (%d,%d) -> (%d,%d)\n", start_x, start_y, end_y, end_x);

  pqueue_t *queue = pq_create();

  node_t *start_node = malloc(sizeof(node_t));
  start_node->y = start_y;
  start_node->x = start_x;
  start_node->index = node_count;
  start_node->edges = calloc(4, sizeof(edge_t *));
  start_node->edge_count = 0;
  nodes[node_count++] = start_node;

  point_t *start_point = malloc(sizeof(point_t));
  start_point->y = start_y;
  start_point->x = start_x;
  start_point->s = 1;
  start_point->from_node = start_node;

  char *hashmap = calloc(HASHMAP_SIZE, sizeof(char));
  pq_enqueue(queue, start_point, start_point->s);

  while (!pq_empty(queue)) {
    point_t *point = pq_dequeue(queue);

    if (point->y == end_y && point->x == end_x) {
      // end reached!
      // printf("END: (%d,%d) -> [%d]\n", point->y, point->x, point->s);

      node_t *src_node = point->from_node;
      node_t *dst_node = findOrCreateNode(point->y, point->x);
      edge_t *edge = malloc(sizeof(edge_t));
      edge->node = dst_node;
      edge->dist = point->s;
      // add a new edge/node reference for the point->from_node
      src_node->edges[src_node->edge_count++] = edge;
      // printf("final edge\t[#%d] -> [#%d] = %d\n", src_node->index,
      //        dst_node->index, point->s);
      // printf("\t\t(%d,%d) -> (%d,%d)\n", src_node->y, src_node->x,
      // dst_node->y,
      //        dst_node->x);
      free(point);
      continue;
    }

    uint64_t key = hash_point(point);

    // printf("\npoint: (%d,%d) -> %d\n", point->y, point->x, point->s);

    // map[point->y][point->x] = 'x';

    int dY[] = {-1, 1, 0, 0};
    int dX[] = {0, 0, 1, -1};
    int valid_moves = 0;
    for (int i = 0; i < 4; i++) {
      if (!isInvalidPoint(point->y + dY[i], point->x + dX[i], hashmap)) {
        valid_moves++;
      }
    }

    // printf("valid_moves (%d,%d) = %d\n", point->y, point->x, valid_moves);

    hashmap[key] = 1;
    if (valid_moves == 1) {
      for (int i = 0; i < 4; i++) {
        if (!isInvalidPoint(point->y + dY[i], point->x + dX[i], hashmap)) {
          point_t *new_point = malloc(sizeof(point_t));
          new_point->y = point->y + dY[i];
          new_point->x = point->x + dX[i];
          new_point->s = point->s + 1;
          new_point->from_node = point->from_node;
          pq_enqueue(queue, new_point, -point->s + 1);
        }
      }
    } else if (valid_moves > 1) {
      // oooooooo eeeeeeee a junction
      // map[point->y][point->x] = 'E';
      //
      node_t *src_node = point->from_node;
      node_t *dst_node = findOrCreateNode(point->y, point->x);
      edge_t *src_edge = malloc(sizeof(edge_t));
      src_edge->node = dst_node;
      src_edge->dist = point->s;
      // add a new edge/node reference for the point->from_node
      src_node->edges[src_node->edge_count++] = src_edge;

      edge_t *dst_edge = malloc(sizeof(edge_t));
      dst_edge->node = src_node;
      dst_edge->dist = point->s;
      dst_node->edges[dst_node->edge_count++] = dst_edge;

      // printf("edge\t[#%d] -> [#%d] = %d\n", src_node->index, dst_node->index,
      //        src_edge->dist);
      // printf("\t(%d,%d) -> (%d,%d)\n", src_node->y, src_node->x, dst_node->y,
      //        dst_node->x);

      for (int i = 0; i < 4; i++) {
        int y = point->y + dY[i];
        int x = point->x + dX[i];
        if (!isInvalidPoint(y, x, hashmap)) {

          // printf("move point: (%d,%d) - node [#%d]\n", y, x,
          // dst_node->index); before moving on, reset steps
          point_t *new_point = malloc(sizeof(point_t));
          new_point->y = y;
          new_point->x = x;
          new_point->s = 1;
          new_point->from_node = dst_node;
          pq_enqueue(queue, new_point, 1);
        }
      }
    } else if (valid_moves == 0 && point->s != 1) {
      // We must be at a point before a junction, but can't enter it and
      // continue, as the main path has already visited through it.
      // We can only build the edge
      for (int i = 0; i < 4; i++) {
        int y = point->y + dY[i];
        int x = point->x + dX[i];
        node_t *src_node = point->from_node;
        if (!isInvalidPoint(y, x, NULL)) {
          for (int j = 0; j < node_count; j++) {
            node_t *dst_node = nodes[j];
            if (dst_node->y == y && dst_node->x == x) {
              edge_t *src_edge = malloc(sizeof(edge_t));
              src_edge->node = dst_node;
              src_edge->dist = point->s + 1;
              // add a new edge/node reference for the point->from_node
              src_node->edges[src_node->edge_count++] = src_edge;

              edge_t *dst_edge = malloc(sizeof(edge_t));
              dst_edge->node = src_node;
              dst_edge->dist = point->s;
              dst_node->edges[dst_node->edge_count++] = dst_edge;
              // printf("side edge\t[#%d] -> [#%d] = %d\n", src_node->index,
              //        dst_node->index, src_edge->dist);
              // printf("\t\t(%d,%d) -> (%d,%d)\n", src_node->y, src_node->x,
              //        dst_node->y, dst_node->x);
            }
          }
        }
      }
    } else {
      // printf("ignored\n");
    }

    free(point);
  }
  pq_destroy(queue);
  free(hashmap);
}

int dfsToGetLongestPath(node_t *start_node, node_t *end_node) {
  printf("DFS for longest path: [%d] -> [%d]\n", start_node->index,
         end_node->index);

  pqueue_t *queue = pq_create();

  point_t *start_point = malloc(sizeof(point_t));
  start_point->node = start_node;
  start_point->s = 1;
  start_point->hashmap = calloc(node_count, sizeof(char));

  pq_enqueue(queue, start_point, start_point->s);

  int max_steps = 0;

  while (!pq_empty(queue)) {
    point_t *point = pq_dequeue(queue);

    if (point->node->index == end_node->index) {
      // end reached!

      if (point->s+1 > max_steps) {
        printf("END: (%d) -> [%d]\n", point->node->index, point->s + 1);
        max_steps = point->s + 1;
      }
      // pq_enqueue(output, point, point->s);
      destroy_point(point);
      continue;
    }

    uint64_t key = point->node->index;
    point->hashmap[key] = 1;

    // printf("\npoint: (%d) -> [%d]\n", point->node->index, point->s);

    for (int i = 0; i < point->node->edge_count; i++) {
      edge_t *edge = point->node->edges[i];
      node_t *next_node = edge->node;
      if (point->hashmap[next_node->index]) {
        // seen
        continue;
      }
      // otherwise, must be valid
      point_t *new_point = malloc(sizeof(point_t));
      new_point->node = next_node;
      new_point->s = point->s + edge->dist;
      new_point->hashmap = calloc(node_count, sizeof(char));
      memcpy(new_point->hashmap, point->hashmap, node_count);
      pq_enqueue(queue, new_point, -new_point->s);
    }

    destroy_point(point);
  }
  pq_destroy(queue);

  return max_steps;
}

int main() {
  init();
  readInputFile(__FILE__, lineHandler, fileHandler);

  map[0][1] = '#';
  map[1][1] = 'S';
  map[height - 2][width - 2] = 'E';
  // printMap();

  int part_one = DFS(1, 1, height - 2, width - 2);

  printf("Part one: %d\n", part_one);
#ifdef TEST_MODE
  assert(part_one == 94);
#else
  assert(part_one == 2314);
#endif

  // dfs 2 electric boogaloo
  dfsToBuildGraph(1, 1, height - 2, width - 2);
  // turn the map into an "acyclic digraph" apparently
  // which is just:
  //  - graph of points with the distance to each junction in the map
  //  then that can be bruteforced faster

  // printMap();

  /* printf("\n\n---\n\n");
  for (int j = 0; j < node_count; j++) {
    node_t *node = nodes[j];
    printf("\nnode [#%d](%d,%d) - %d edges\n", node->index, node->y, node->x,
           node->edge_count);
    for (int k = 0; k < node->edge_count; k++) {
      edge_t *edge = node->edges[k];
      printf("[#%d](%d,%d) = %d steps\n", edge->node->index, edge->node->y,
             edge->node->x, edge->dist);
    }
  } */

  // dfs 3 electric boogalee
  int part_two = dfsToGetLongestPath(nodes[0], nodes[node_count - 1]);
  printf("Part two: %d\n", part_two);
#ifdef TEST_MODE
  assert(part_two == 154);
#else
  assert(part_two > 6553);
  assert(part_two != 6869);
  assert(part_two != 6870);
  assert(part_two != 6903);
  assert(part_two < 7000);
#endif
  // I think this is the only one i actually bothered to properly free memory
  // cos who needs to do that when the program will close anyway amirite
  for (int i = 0; i < height; i++) {
    free(map[i]);
  }
  free(map);

  for (int j = 0; j < node_count; j++) {
    node_t *node = nodes[j];
    for (int k = 0; k < node->edge_count; k++) {
      free(node->edges[k]);
    }
    free(node->edges);
    free(node);
  }
  free(nodes);
  exit(EXIT_SUCCESS);
}
