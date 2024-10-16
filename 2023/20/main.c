#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
// #define TEST_MODE
#include "../../lib/queue.h"
#include "../utils.h"

typedef enum pulse_t { HIGH = 1, LOW = 0 } pulse_t;
typedef enum state_t { ON = 1, OFF = 0 } state_t;
typedef enum type_t {
  BROADCASTER = 'B',
  FLIP_FLOP = '%',
  CONJUNCTION = '&',
  RX = 'R'
} type_t;

typedef struct node_t {
  uint32_t id; // requires modifying example `inv` => `in`
  type_t type; // %, &, B, R
  state_t state;

  uint32_t *input_ids;
  state_t *input_memory;
  int input_id_count;

  // struct node_t *outputs; // probably faster
  uint32_t *output_ids;
  int output_id_count;

  // for part two
  uint32_t pulsed_high_at;
} node_t;

typedef struct action_t {
  node_t *input;
  node_t *output;
  pulse_t pulse;
} action_t;

node_t **nodes;
int nodeCount = 0;
node_t *broadcaster;
node_t *rx;
node_t *rx_input;
int button_presses = 0;

// TODO lib file!
char *split(char *str, const char *delim) {
  char *p = strstr(str, delim);

  if (p == NULL) {
    return NULL; // delimiter not found
  }

  *p = '\0';                // terminate string after head
  return p + strlen(delim); // return tail substring
}

uint32_t str_to_node_id(char *str) {
  uint32_t id = 0;
  id += str[0] << 8;
  id += str[1];
  return id;
}

void fileHandler(int lines) {
  printf("lines: %d\n", lines);
  nodes = calloc(lines, sizeof(node_t *));
}

void lineHandler(char *line, int length) {
  printf("line (%d): %s\n", length, line);
  char *dst;
  dst = split(line, " -> ");
  printf("src: %s\n", line);
  printf("dst: %s\n", dst);
  node_t *node = calloc(1, sizeof(node_t));

  if (strcmp("broadcaster", line) == 0) {
    printf("broadcaster\n");
    node->id = 0;
    node->type = BROADCASTER;
    broadcaster = node;
  } else if (line[0] == '%') {
    // flippen floppen
    char *token = strtok(line, "%");
    printf("flipflop '%s' '%d' \n", token, str_to_node_id(token));
    node->id = str_to_node_id(token);
    node->type = FLIP_FLOP;
    node->state = OFF;
  } else if (line[0] == '&') {
    // conjunction
    char *token = strtok(line, "&");
    printf("conjunction '%s' '%d' \n", token, str_to_node_id(token));
    node->id = str_to_node_id(token);
    node->type = CONJUNCTION;

    node->input_ids = calloc(32, sizeof(uint32_t));
    node->input_memory = calloc(32, sizeof(state_t));
    node->input_id_count = 0;
    node->pulsed_high_at = 0;
  }
  node->output_ids =
      calloc(strlen(dst), sizeof(uint32_t)); // oversized but ohwell
  node->output_id_count = 0;
  char *token = strtok(dst, ", ");
  do {
    // add id stuff
    node->output_ids[node->output_id_count++] = str_to_node_id(token);
    printf("dst tok '%s' '%d' \n", token, str_to_node_id(token));
  } while ((token = strtok(NULL, ", ")) != NULL);

  nodes[nodeCount++] = node;
}

/*
// // MODULES
// flip_flop %
// state = OFF // initial
// on high pulse, do nothing
// on low pulse
//  toggle state
//  emit pulse as per new state - ON=HIGH OFF=LOW
//
// conjunction &
// nodes [ { memory = HIGH/LOW (LOW initially) } ] for each input node
// for each pulse, update memory for input
// then check input node memory - if all HIGH => send LOW
//                              - otherwise HIGH
//
// broadcaster
//  - repeat pulse to all output nodes
//
// button
//  - send LOW pulse to broadcaster module
 */

node_t *findNodeById(uint32_t id) {
  for (int i = 0; i < nodeCount; i++) {
    node_t *node = nodes[i];

    if (node->id == id) {
      return node;
    }
  }
  return NULL;
}
// TODO this could do the mapping to output_node_pointers
// also sets `rx` as a valid node
void findConjunctionInputs() {
  // printf("findConjunctionInputs\n");
  for (int i = 0; i < nodeCount; i++) {
    node_t *node = nodes[i];
    // printf("Node: %d, %c\n", node->id, node->type);

    for (int j = 0; j < node->output_id_count; j++) {
      node_t *output_node = findNodeById(node->output_ids[j]);
      if (output_node == NULL) {
        // printf("rx node %d [j: %d]\n", node->output_ids[j], j);

        rx = calloc(1, sizeof(node_t));
        rx->id = str_to_node_id("rx");
        rx->type = RX;
        rx->input_ids = calloc(1, sizeof(uint32_t));
        rx->input_ids[rx->input_id_count++] = node->id;
        nodes[nodeCount++] = rx;
        continue;
      }
      // printf("output node: %d, %c\n", output_node->id, output_node->type);
      if (output_node->type == CONJUNCTION) {
        output_node->input_ids[output_node->input_id_count++] = node->id;
      }
    }
  }
}

void printNodes() {
  printf("\n");
  for (int i = 0; i < nodeCount; i++) {
    node_t *node = nodes[i];
    printf("Node: %d, %c\n", node->id, node->type);
    for (int j = 0; j < node->output_id_count; j++) {
      node_t *output_node = findNodeById(node->output_ids[j]);
      if (output_node == NULL) {
        printf("rx node %d [j: %d]\n", node->output_ids[j], j);
        continue;
      }
      printf("\toutput: %d, %c\n", output_node->id, output_node->type);
    }
  }
}

void q_action(queue_t *queue, node_t *input, node_t *output, pulse_t pulse) {
  action_t *action = calloc(1, sizeof(action_t));
  action->input = input;
  action->output = output;
  action->pulse = pulse;
  q_enqueue(queue, action);
}

bool isInputNode(node_t *node) {
  for (int i = 0; i < rx_input->input_id_count; i++) {
    if (node->id == rx_input->input_ids[i]) {
      return true;
    }
  }
  return false;
}

int low_pulses = 0;
int high_pulses = 0;

void pushButton() {
  queue_t *queue = q_create();

  q_action(queue, NULL, broadcaster, LOW); // input button press

  while (!q_empty(queue)) {
    action_t *action = q_dequeue(queue);
    node_t *node = action->output;
    pulse_t pulse = action->pulse;
    if (button_presses < 1000) {
      if (pulse == HIGH) {
        high_pulses++;
      } else {
        low_pulses++;
      }
    }

    // printf("action!: %d, %c -> %d\n", node->id, node->type, pulse);
    if (node->type == BROADCASTER) {
      // low pulse to all output nodes
      // printf("broadcasting\n");
      for (int j = 0; j < node->output_id_count; j++) {
        node_t *output_node = findNodeById(node->output_ids[j]);
        // printf("\toutput: %d, %c\n", output_node->id, output_node->type);
        q_action(queue, node, output_node, LOW);
      }
    } else if (node->type == FLIP_FLOP) {
      // printf("flipflop state: %d\n", node->state);
      // flip_flop %
      // state = OFF // initial
      // on high pulse, do nothing
      if (pulse == HIGH) {
        free(action);
        continue;
      }
      // on low pulse
      //  toggle state
      node->state = !node->state;
      //  emit pulse as per new state - ON=HIGH OFF=LOW
      for (int j = 0; j < node->output_id_count; j++) {
        node_t *output_node = findNodeById(node->output_ids[j]);
        // printf("\toutput: %d, %c\n", output_node->id, output_node->type);
        q_action(queue, node, output_node, node->state == ON ? HIGH : LOW);
      }
    } else if (node->type == CONJUNCTION) {
      node_t *input = action->input;

      int input_id_index = -1;
      for (int j = 0; j < node->input_id_count; j++) {
        if (node->input_ids[j] == input->id) {
          input_id_index = j;
          break;
        }
      }
      // printf("conjunction memory: input index '%d'\n", input_id_index);
      node->input_memory[input_id_index] = pulse == HIGH ? ON : OFF;

      // nodes [ { memory = HIGH/LOW (LOW initially) } ] for each input node
      // for each pulse, update memory for input
      // then check input node memory - if all HIGH => send LOW
      //                              - otherwise HIGH
      bool all_high = true;
      for (int j = 0; j < node->input_id_count; j++) {
        if (node->input_memory[j] == OFF) {
          all_high = false;
          break;
        }
      }
      // printf("conjunction memory all_high %d\n", all_high);

      // if it is an INPUT to rx->input_ids[0]
      // AND
      // the output will be HIGH
      if (!all_high) {
        if (!node->pulsed_high_at && isInputNode(node)) {
          // not entirely sure why +1 is needed
          // maybe "the press after this would output the correct signal"
          node->pulsed_high_at = button_presses + 1;
        }
      }

      for (int j = 0; j < node->output_id_count; j++) {
        node_t *output_node = findNodeById(node->output_ids[j]);
        // printf("\toutput: %d, %c --> %d\n", output_node->id,
        // output_node->type, all_high ? LOW : HIGH);
        q_action(queue, node, output_node, all_high ? LOW : HIGH);
      }
    }
    free(action);
  }
  q_destroy(queue);
}

// time to beat, 14ms
int main() {
  init();
  readInputFile(__FILE__, lineHandler, fileHandler);

  findConjunctionInputs();

  // printNodes();

  rx_input = findNodeById(rx->input_ids[0]);

  // TODO definitely a "better" way, since this may not cover all inputs
  for (; button_presses < 5000; button_presses++) {
    pushButton();
  }

  unsigned long long part_two = 1;
  for (int i = 0; i < rx_input->input_id_count; i++) {
    node_t *rx_input_input = findNodeById(rx_input->input_ids[i]);
    // printf("rx input input: %d, %c == %d\n", rx_input_input->id,
    // rx_input_input->type, rx_input_input->pulsed_high_at);
    part_two *= rx_input_input->pulsed_high_at;
  }

  int part_one = low_pulses * high_pulses;
  printf("Part one: %d\n", part_one);
#ifdef TEST_MODE
  assert(part_one == 32000000);
#else
  assert(part_one == 743871576);
#endif

#ifdef TEST_MODE
  // NO DATA
#else
  printf("Part two: %llu\n", part_two);
  assert(part_two == 244151741342687);
#endif
  exit(EXIT_SUCCESS);
}
