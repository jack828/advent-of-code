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
  bool is_a;
  bool is_z;
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
  node->is_a = id[2] == 'A';
  node->is_z = id[2] == 'Z';
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
    printf("node %s = (%s, %s) - is_start %c, is_end %c, is_a %c, is_z %c\n",
           node->id, node->left_str, node->right_str,
           node->is_start ? 'Y' : 'N', node->is_end ? 'Y' : 'N',
           node->is_a ? 'Y' : 'N', node->is_z ? 'Y' : 'N');
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
    // printf("node %s = (%s, %s)\n", node->id, node->left_str, node->right_str);
    node->left = find_node(node->left_str);
    node->right = find_node(node->right_str);
  }
}

u_int64_t GCD(u_int64_t a, u_int64_t b) {
  if (b == 0) {
    return a;
  }
  return GCD(b, a % b);
}

u_int64_t lcm_array(int nums[], int num_length) {
  u_int64_t lcm = nums[0];
  u_int64_t gcd = nums[0];

  // Loop through the array and find GCD
  // use GCD to find the LCM
  for (int i = 0; i < num_length; i++) {
    gcd = GCD(nums[i], lcm);
    lcm = (lcm * nums[i]) / gcd;
  }
  return lcm;
}

int main() {
  init();
  readInputFile(__FILE__, lineHandler, fileHandler);

  // print_nodes();
  // Now that we have parsed them we are able to traverse and link together
  connect_nodes();

#ifdef TEST_MODE
  u_int64_t steps = 0;
  // Unfortunately, this day's example doesn't work for both
  assert(steps == 0);
#else
  u_int64_t steps = 0;
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

  printf("Part one: %lu\n", steps);
  assert(steps == 20777);
#endif

  // Count the number of paths through the maze we have
  int a_node_count = 0;

  for (int i = 0; i < node_count; i++) {
    node_t *node = nodes[i];
    if (node->is_a) {
      a_node_count++;
    }
  }

  // build array of start nodes

  // TODO would re-allocation be faster? removes a loop
  node_t **current_nodes = calloc(a_node_count, sizeof(node_t *));

  int a_index = 0;
  for (int i = 0; i < node_count; i++) {
    node_t *node = nodes[i];
    if (node->is_a) {
      current_nodes[a_index++] = node;
    }
  }

  steps = -1;

  // find the step length of each loop
  int *loop_sizes = calloc(a_node_count, sizeof(int));
  // then find find the lowest common multiple

  int z_nodes = 0;
  while (z_nodes != a_node_count) {
    char dir = instructions[++steps % instructions_length];
    for (int i = 0; i < a_node_count; i++) {
      // do it to each node and check for is_z
      if (current_nodes[i]->is_z) {
        continue;
      }
      if (dir == 'L') {
        current_nodes[i] = current_nodes[i]->left;
      } else {
        current_nodes[i] = current_nodes[i]->right;
      }

      if (current_nodes[i]->is_z) {
        z_nodes++;
      }
      loop_sizes[i]++;
    }
  }

  u_int64_t lcm = lcm_array(loop_sizes, a_node_count);
  printf("Part two: %lu\n", lcm);
#ifdef TEST_MODE
  assert(lcm == 6);
#else
  assert(lcm == 13289612809129);
#endif
  exit(EXIT_SUCCESS);
}
