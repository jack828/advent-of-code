#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#define TEST_MODE
#include "../utils.h"

typedef struct input_t {
  char *line;
  int line_length;
  int *groups;
  int group_count;
} input_t;

input_t **inputs;
int input_count = 0;

void fileHandler(int lines) {
  printf("lines: %d\n", lines);
  inputs = calloc(lines, sizeof(input_t *));
}

void lineHandler(char *line, int length) {
  printf("line (%d): %s\n", length, line);

  input_t *input = malloc(sizeof(input_t));
  char *input_line = strtok(line, " ");
  char *groups = strtok(NULL, " ");
  input->line = strdup(input_line);
  input->line_length = strlen(input->line);

  input->groups = calloc(strlen(groups), sizeof(int));
  input->group_count = 0;
  char *token = strtok(groups, ",");
  do {
    input->groups[input->group_count++] = atoi(token);
  } while ((token = strtok(NULL, ",")) != NULL);
  inputs[input_count++] = input;
}

input_t *clone(input_t *in) {
  input_t *out = malloc(sizeof(input_t));
  out->line = strdup(in->line);
  out->line_length = in->line_length;

  out->groups = calloc(in->group_count, sizeof(int));
  memcpy(out->groups, in->groups, in->group_count * sizeof(*in->groups));
  out->group_count = in->group_count;
  return out;
}

void freeInput(input_t *input) {
  free(input->line);
  free(input->groups);
  free(input);
}

int countArrangements(input_t *input) {
  printf("count: '%s', len %d, group_count %d\n", input->line,
         input->line_length, input->group_count);
  // ran out of input line
  if (input->line_length == 0) {
    // if we don't have groups, this is GOOD solution
    if (input->group_count == 0) {
      printf("1\n\n");
      return 1;
    }
    // otherwise, we still had groups to process, BAD solution
    printf("0 - groups left\n\n");
    return 0;
  }

  /* if (input->group_count == 0) {
    for (int i = 0; i < input->line_length; i++) {
      if (input->line[i] == '#') {
        // it isnt, return early
        printf("0 - no fit\n");
        return 0;
      }
    }
    return 1;
  } */

  // if it starts with a ., discard the . and recursively check again.
  if (input->line[0] == '.') {
    printf("dot recurse\n");
    input_t *new_input = clone(input);
    memmove(new_input->line, input->line + 1,
            new_input->line_length * sizeof(*new_input->line));
    new_input->line_length--;
    int count = countArrangements(new_input);
    // freeInput(input);
    return count;
  }

  // if it starts with a ?, replace the ? with a . and recursively check again,
  // AND replace it with a # and recursively check again.
  if (input->line[0] == '?') {
    printf("? recurse\n");
    input_t *input_a = clone(input);
    input_t *input_b = clone(input);

    // A path gets .
    input_a->line[0] = '.';
    // B path gets #
    input_b->line[0] = '#';
    int a_count = countArrangements(input_a);
    int b_count = countArrangements(input_b);
    // freeInput(input_a);
    // freeInput(input_b);
    // return a_count;
    return a_count + b_count;
  }

  // it it starts with a #,
  // check if it is long enough for the first group,
  // check if all characters in the first [grouplength] characters are not '.',
  // and then remove the first [grouplength] chars and the first group number,
  // recursively check again.
  if (input->line[0] == '#') {
    // check if it is long enough for the first group,
    for (int i = 0; i < input->groups[0]; i++) {
      if (input->line[i] == '.') {
        // it isnt, return early
        printf("0 - no fit\n\n");
        return 0;
      }
    }
    if (input->line[input->groups[0]] == '#') {
      printf("0 - no fit?\n\n");
      return 0;
    }
    
    // and then remove the first [grouplength] chars and the first group
    // number,
    input_t *new_input = clone(input);
    memmove(new_input->line, new_input->line + new_input->groups[0],
            new_input->line_length * sizeof(*new_input->line));
    new_input->line_length -= new_input->groups[0];

    memmove(new_input->groups, new_input->groups + 1,
            new_input->group_count * sizeof(*new_input->groups));
    new_input->group_count--;
    printf("# recurse\n");
    // recursively check again.
    int count = countArrangements(new_input);
    // freeInput(new_input);
    return count;
  }
  printf("BAD\n");
  return 0;
}

int main() {
  init();
  readInputFile(__FILE__, lineHandler, fileHandler);
  int total = 0;
  for (int i = 0; i < input_count; i++) {
    input_t *input = inputs[i];
    int count = countArrangements(input);
    printf("count %d\n", count);
    total += count;
  }

  printf("Part one: %d\n", total);
#ifdef TEST_MODE
  assert(total == 1);
#else
  assert(total == 420);
#endif

  printf("Part two: %d\n", 420);
#ifdef TEST_MODE
  assert(420 == 69);
#else
  assert(420 == 69);
#endif
  exit(EXIT_SUCCESS);
}
