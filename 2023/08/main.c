#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
// #define TEST_MODE
#include "../utils.h"

typedef struct node_t node_t;

struct node_t {
  bool is_start;
  bool is_end;
  node_t *left;
  node_t *right;
  char *id;
  char *left_str;
  char *right_str;
};

node_t **nodes;
node_t *current_node;
int node_count = 0;
char instructions[_AOC_LINE_MAX];
int instructions_length = 0;

void fileHandler(int lines) {
  printf("lines: %d\n", lines);
  nodes = calloc(lines - 2, sizeof(node_t));
}

void lineHandler(char *line, int length) {
  printf("line (%d): %s\n", length, line);
  if (length == 1) {
    return;
  }
  if (instructions[0] == 0) {
    strcpy(instructions, line);
    instructions_length = length - 1;
    return;
  }

  char id[4];
  char left[4];
  char right[4];
  sscanf(line, "%3s = (%3s, %3s)", id, left, right);

  node_t *node = calloc(1, sizeof(node_t));
  node->is_start = strcmp("AAA", id) == 0;
  node->is_end = strcmp("ZZZ", id) == 0;
  node->id = strdup(id);
  node->left_str = strdup(left);
  node->right_str = strdup(right);
  nodes[node_count++] = node;
  if (node->is_start) {
    current_node = node;
  }
}

void print_nodes() {
  for (int i = 0; i < node_count; i++) {
    node_t *node = nodes[i];
    printf("node %s = (%s, %s) - is_start %c, is_end %c\n", node->id,
           node->left_str, node->right_str, node->is_start ? 'Y' : 'N',
           node->is_end ? 'Y' : 'N');
  }
}

node_t *find_node(char *node_id) {
  for (int i = 0; i < node_count; i++) {
    node_t *node = nodes[i];
    if (strcmp(node->id, node_id) == 0) {
      return node;
    }
  }
  printf("COULD NOT FIND NODE!!\n");
  return NULL;
}

void connect_nodes() {
  for (int i = 0; i < node_count; i++) {
    node_t *node = nodes[i];
    printf("node %s = (%s, %s)\n", node->id, node->left_str, node->right_str);
    node->left = find_node(node->left_str);
    node->right = find_node(node->right_str);
  }
}

int main() {
  init();
  readInputFile(__FILE__, lineHandler, fileHandler);

  print_nodes();
  // Now that we have parsed them we are able to traverse and link together
  connect_nodes();

  int steps = 0;
  while (!current_node->is_end) {
    char dir = instructions[steps % instructions_length];
    // printf("at node %s, step %d -> %c\n", current_node->id, steps, dir);
    if (dir == 'L') {
      current_node = current_node->left;
    } else {
      current_node = current_node->right;
    }
    steps++;
  }

  printf("Part one: %d\n", steps);
#ifdef TEST_MODE
  // Unfortunately, this day's example doesn't work for both
  // assert(steps == 2);
#else
  assert(steps == 20777);
#endif

  printf("Part two: %d\n", 420);
#ifdef TEST_MODE
  assert(420 == 69);
#else
  assert(420 == 69);
#endif
  exit(EXIT_SUCCESS);
}
