#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
// #define TEST_MODE
#define _AOC_LINE_MAX 23000
#include "../utils.h"

int part_one = 0;
typedef struct lens_t lens_t;

struct lens_t {
  lens_t *next_lens;
  char *label;
  int power;
};

typedef struct box_t {
  lens_t *lens;
} box_t;

box_t **boxes;

void fileHandler(int lines) {
  printf("lines: %d\n", lines);
  boxes = calloc(256, sizeof(box_t *));

  for (int i = 0; i < 256; i++) {
    boxes[i] = calloc(1, sizeof(box_t));
  }
}

void print_boxes() {
  for (int i = 0; i < 256; i++) {
    box_t *box = boxes[i];

    if (box->lens == NULL) {
      continue;
    }

    printf("Box %d: ", i);

    lens_t *lens = box->lens;
    while (lens != NULL) {
      printf("[%s %d] ", lens->label, lens->power);
      lens = lens->next_lens;
    }
    printf("\n");
  }
}

void lineHandler(char *line, int length) {
  printf("line (%d): %s\n", length, line);

  char *ptr;
  char *token = strtok_r(line, ",", &ptr);

  while (token != NULL) {
    int tok_length = strlen(token);
    int hash = 0;
    char operation = 0;
    char *label;
    int power = 0;

    // TODO maybe refactor this one day
    for (int i = 0; i < tok_length; i++) {
      char c = token[i];
      hash += c;
      hash *= 17;
      hash %= 256;

      if (!isalnum(c)) {
        operation = c;
      }
    }
    part_one += hash;

    // Part 2
    if (operation == '=') {
      label = strtok(token, "=");
      power = strtol(strtok(NULL, "="), NULL, 10);
    } else if (operation == '-') {
      label = strtok(token, "-");
    }
    int label_hash = 0;
    int label_length = strlen(label);

    for (int i = 0; i < label_length; i++) {
      char c = label[i];
      label_hash += c;
      label_hash *= 17;
      label_hash %= 256;
    }
    // printf("step: hash=%d, label=%s, operation='%c', power=%d\n", label_hash,
           // label, operation, power);

    // go to the relevant box
    box_t *box = boxes[label_hash];

    lens_t *lens = box->lens;
    lens_t *prev_lens = box->lens;
    while (lens != NULL) {
      if (strcmp(lens->label, label) == 0) {
        break;
      }
      prev_lens = lens;
      lens = lens->next_lens;
    }

    // If the operation character is a dash (-),
    if (operation == '-') {
      // and remove the lens with the given label
      // if it is present in the box.
      if (lens != NULL) {                // i've only just wrote this
        if (lens == prev_lens) {         // and
          if (lens->next_lens != NULL) { // i've already forgot
            box->lens = lens->next_lens;
          } else {
            box->lens = NULL;
          }
        } else {
          prev_lens->next_lens = lens->next_lens;
        }
      }
    } else if (operation == '=') {
      if (lens != NULL) {
        // If there is already a lens in the box with the same label,
        // replace the old lens with the new lens:
        // remove the old lens and put the new lens in its place,
        // not moving any other lenses in the box.
        lens->power = power;
      } else {
        // If there is not already a lens in the box with the same label,
        // add the lens to the box immediately behind any lenses already
        // in the box. Don't move any of the other lenses when you do this.
        // If there aren't any lenses in the box, the new lens goes all the way
        // to the front of the box.
        if (prev_lens == NULL) {
          // box empty
          box->lens = malloc(sizeof(lens_t));
          box->lens->label = strdup(label);
          box->lens->power = power;
          box->lens->next_lens = NULL;
        } else {
          // add to the end
          prev_lens->next_lens = malloc(sizeof(lens_t));
          prev_lens->next_lens->label = strdup(label);
          prev_lens->next_lens->power = power;
          prev_lens->next_lens->next_lens = NULL;
        }
      }
    }
    token = strtok_r(NULL, ",", &ptr);
  }
}

int main() {
  init();
  readInputFile(__FILE__, lineHandler, fileHandler);
  // printf("---\n");
  // print_boxes();

  printf("Part one: %d\n", part_one);
#ifdef TEST_MODE
  assert(part_one == 1320);
#else
  assert(part_one == 511215);
#endif

  int total_power = 0;
  for (int i = 0; i < 256; i++) {
    box_t *box = boxes[i];

    if (box->lens == NULL) {
      continue;
    }

    int slot = 1;
    lens_t *lens = box->lens;
    while (lens != NULL) {
      int lens_power = (i + 1) * slot++ * lens->power;
      total_power+=lens_power;
      lens = lens->next_lens;
    }
  }

  printf("Part two: %d\n", total_power);
#ifdef TEST_MODE
  assert(total_power == 145);
#else
  assert(total_power == 236057);
#endif
  exit(EXIT_SUCCESS);
}
