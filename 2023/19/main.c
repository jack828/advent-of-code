#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
// #define TEST_MODE
#include "../../lib/queue.h"
#include "../utils.h"

typedef struct workflow_t workflow_t;
typedef struct rule_t rule_t;
typedef enum rule_type_t { ACCEPT, REJECT, WORKFLOW } rule_type_t;

struct workflow_t {
  char *id;
  rule_t **rules;
  int rule_count;
};

struct rule_t {
  char field;     // x m a s ... nice one, Eric.
  char condition; // > or <
  int value;      // value of rule
  bool end;       // if true, there is no condition
  rule_type_t accept_type;
  char *workflow_str;   // only set if type == WORKFLOW
  workflow_t *workflow; // only set if type == WORKFLOW
};

typedef struct part_t {
  int x;
  int m;
  int a;
  int s;
} part_t;

workflow_t **workflows;
int workflow_count;

part_t **parts;
int part_count;

bool parse_workflow = true;

void fileHandler(int lines) {
  printf("lines: %d\n", lines);
  workflows = calloc(lines, sizeof(workflow_t));
  parts = calloc(lines, sizeof(part_t));
}

void lineHandler(char *line, int length) {
  printf("line (%d): %s\n", length, line);
  if (length == 1) {
    parse_workflow = false;
    return;
  }

  if (parse_workflow) {
    workflow_t *workflow = malloc(sizeof(workflow_t));
    workflow->rules = calloc(10, sizeof(rule_t));
    workflow->rule_count = 0;

    char id[8] = {0};
    int id_length = 0;
    do {
      // consume characters from `line` until it is `{`
      id[id_length++] = line[0];
      // probably the most "C" code I've ever written...
    } while ((++line)[0] != '{');

    workflow->id = strdup(id);
    ++line;
    // cut the end `}` off
    line[length - id_length - 3] = '\0';

    char *end;
    char *delim = ",";
    char *token = strtok_r(line, delim, &end);
    do {
      // printf("rule token: '%s'\n", token);
      rule_t *rule = malloc(sizeof(rule_t));
      char *condition_char = strchr(token, ':');
      if (condition_char == NULL) {
        // No condition, end state rule
        rule->end = true;
        rule->accept_type = token[0] == 'A'   ? ACCEPT
                            : token[0] == 'R' ? REJECT
                                              : WORKFLOW;
        rule->workflow_str = strdup(token);
      } else {
        rule->end = false;
        rule->field = (token++)[0];
        rule->condition = (token++)[0];
        rule->value = strtol(token, &condition_char, 10);
        // printf("rule: %c %c %d\n", rule->field, rule->condition,
        // rule->value); Now it becomes the end of the rule - what to do if
        // condition passes
        ++condition_char;
        if (condition_char[0] == 'A') {
          rule->accept_type = ACCEPT;
        } else if (condition_char[0] == 'R') {
          rule->accept_type = REJECT;
        } else {
          rule->accept_type = WORKFLOW;
          // to be linked later
          rule->workflow_str = strdup(condition_char);
        }
      }

      workflow->rules[workflow->rule_count++] = rule;
    } while ((token = strtok_r(NULL, delim, &end)) != NULL);

    workflows[workflow_count++] = workflow;
  } else {
    // parse part
    part_t *part = malloc(sizeof(part_t));

    line[length - 2] = '\0'; // remove end `}`
    ++line;                  // remove start `{`
    char *end;
    char *delim = ",";
    char *token = strtok_r(line, delim, &end);
    do {
      char type = token[0];
      token += 2;
      int value = strtol(token, NULL, 10);
      if (type == 'x') {
        part->x = value;
      } else if (type == 'm') {
        part->m = value;
      } else if (type == 'a') {
        part->a = value;
      } else if (type == 's') {
        part->s = value;
      }
    } while ((token = strtok_r(NULL, delim, &end)) != NULL);

    parts[part_count++] = part;
  }
}

void print_workflows() {
  for (int i = 0; i < workflow_count; i++) {
    workflow_t *workflow = workflows[i];
    printf("%s{", workflow->id);
    for (int j = 0; j < workflow->rule_count; j++) {
      if (j != 0) {
        printf(",");
      }
      rule_t *rule = workflow->rules[j];
      if (!rule->end) {
        // there is a condition
        printf("%c%c%d:", rule->field, rule->condition, rule->value);
      }
      switch (rule->accept_type) {
      case ACCEPT:
        printf("A");
        break;
      case REJECT:
        printf("R");
        break;
      case WORKFLOW:
        printf("%s", rule->workflow_str);
        break;
      }
    }
    printf("}\n");
  }
}

void print_parts() {
  for (int i = 0; i < part_count; i++) {
    part_t *part = parts[i];
    printf("{x=%d,m=%d,a=%d,s=%d}\n", part->x, part->m, part->a, part->s);
  }
}

workflow_t *find_workflow(char *target) {
  for (int i = 0; i < workflow_count; i++) {
    workflow_t *workflow = workflows[i];
    if (strcmp(workflow->id, target) == 0) {
      return workflow;
    }
  }
  return NULL;
}

// Convert the rule->workflow_str to a pointer in rule->workflow
void link_workflows() {
  for (int i = 0; i < workflow_count; i++) {
    workflow_t *workflow = workflows[i];
    for (int j = 0; j < workflow->rule_count; j++) {
      rule_t *rule = workflow->rules[j];
      if (rule->accept_type == WORKFLOW) {
        // printf("%s\n", rule->workflow_str);
        rule->workflow = find_workflow(rule->workflow_str);
      }
    }
  }
}

int get_part_value(part_t *part, char field) {
  switch (field) {
  case 'x':
    return part->x;
  case 'm':
    return part->m;
  case 'a':
    return part->a;
  case 's':
    return part->s;
  }
  return 0;
}

// true if accepted
// false if rejected
bool sort_part(workflow_t *start, part_t *part) {
  workflow_t *workflow = start;
  while (1) {
    // FIXME could certainly remove this somehow...
  START:
    for (int j = 0; j < workflow->rule_count; j++) {
      rule_t *rule = workflow->rules[j];
      if (rule->end) {
        switch (rule->accept_type) {
        case ACCEPT:
          return true;
        case REJECT:
          return false;
        case WORKFLOW:
          workflow = rule->workflow;
          goto START; // hate me cos u aint me
        }
      } else {
        // evaluate condition
        int part_value = get_part_value(part, rule->field);
        bool satisfies;
        if (rule->condition == '>') {
          satisfies = part_value > rule->value;
        } else {
          // rule->condition == '<'
          satisfies = part_value < rule->value;
        }
        if (satisfies) {
          switch (rule->accept_type) {
          case ACCEPT:
            return true;
          case REJECT:
            return false;
          case WORKFLOW:
            workflow = rule->workflow;
            goto START; // hate me cos u aint me
          }
        } else {
          // just continue to the next rule
        }
      }
    }
    break;
  }
  return false;
}

int get_field_index(char field) {
  switch (field) {
  case 'x':
    return 0;
  case 'm':
    return 1;
  case 'a':
    return 2;
  case 's':
    return 3;
  }
  return 0;
}

unsigned long long calc_range(int **range) {
  // bleedin 'eck
  return ((unsigned long long)(range[0][1] - range[0][0] + 1)) *
         ((unsigned long long)(range[1][1] - range[1][0] + 1)) *
         ((unsigned long long)(range[2][1] - range[2][0] + 1)) *
         ((unsigned long long)(range[3][1] - range[3][0] + 1));
}

int **clone_range(int **range) {
  // FIXME probably a nicer way...for sure...
  int **r = calloc(4, sizeof(int *));
  r[0] = calloc(2, sizeof(int));
  r[0][0] = range[0][0];
  r[0][1] = range[0][1];
  r[1] = calloc(2, sizeof(int));
  r[1][0] = range[1][0];
  r[1][1] = range[1][1];
  r[2] = calloc(2, sizeof(int));
  r[2][0] = range[2][0];
  r[2][1] = range[2][1];
  r[3] = calloc(2, sizeof(int));
  r[3][0] = range[3][0];
  r[3][1] = range[3][1];
  return r;
}

void get_ranges(queue_t *q, workflow_t *workflow, int **range) {
  for (int i = 0; i < workflow->rule_count; i++) {
    rule_t *rule = workflow->rules[i];
    if (rule->end) {
      switch (rule->accept_type) {
      case ACCEPT:
        q_enqueue(q, clone_range(range));
        break;
      case REJECT:
        break;
      case WORKFLOW:
        get_ranges(q, rule->workflow, clone_range(range));
        break;
      }
    }

    if (rule->condition == '>') {
      int **new_range = clone_range(range);
      new_range[get_field_index(rule->field)][0] = rule->value + 1;

      range[get_field_index(rule->field)][1] = rule->value;

      switch (rule->accept_type) {
      case ACCEPT:
        q_enqueue(q, clone_range(new_range));
        break;
      case REJECT:
        break;
      case WORKFLOW:
        get_ranges(q, rule->workflow, clone_range(new_range));
        break;
      }
    }

    if (rule->condition == '<') {
      int **new_range = clone_range(range);
      new_range[get_field_index(rule->field)][1] = rule->value - 1;

      range[get_field_index(rule->field)][0] = rule->value;

      switch (rule->accept_type) {
      case ACCEPT:
        q_enqueue(q, clone_range(new_range));
        break;
      case REJECT:
        break;
      case WORKFLOW:
        get_ranges(q, rule->workflow, clone_range(new_range));
        break;
      }
    }
  }
}

int main() {
  init();
  readInputFile(__FILE__, lineHandler, fileHandler);

  // print_workflows();
  // print_parts();
  link_workflows();
  workflow_t *start = find_workflow("in");
  /* if (start) {
    printf("start: '%s'\n", start->id);
  } */

  // printf("in -> px: '%s'\n", start->rules[0]->workflow->id);

  int part_rating_sum = 0;
  for (int i = 0; i < part_count; i++) {
    part_t *part = parts[i];
    if (sort_part(start, part)) {
      part_rating_sum += part->x + part->m + part->a + part->s;
    }
  }

  printf("Part one: %d\n", part_rating_sum);
#ifdef TEST_MODE
  assert(part_rating_sum == 19114);
#else
  assert(part_rating_sum == 456651);
#endif

  int **range = calloc(4, sizeof(int *));
  range[0] = calloc(2, sizeof(int));
  range[0][0] = 1;
  range[0][1] = 4000;
  range[1] = calloc(2, sizeof(int));
  range[1][0] = 1;
  range[1][1] = 4000;
  range[2] = calloc(2, sizeof(int));
  range[2][0] = 1;
  range[2][1] = 4000;
  range[3] = calloc(2, sizeof(int));
  range[3][0] = 1;
  range[3][1] = 4000;

  unsigned long long rating_combinations = 0;

  // I guess that's one way to do a dynamic array
  queue_t *queue = q_create();
  get_ranges(queue, start, range);

  while (!q_empty(queue)) {
    int **r = q_dequeue(queue);
    rating_combinations += calc_range(r);
  }

  printf("Part two: %llu\n", rating_combinations);
#ifdef TEST_MODE
  assert(rating_combinations == 167409079868000llu);
#else
  assert(rating_combinations == 131899818301477llu);
#endif
  exit(EXIT_SUCCESS);
}
