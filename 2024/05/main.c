#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
// #define TEST_MODE
#include "../utils.h"

int **rules;
int rule_count = 0;

typedef struct page_t {
  int size;
  int *pages;
} page_t;

page_t **pages;
int page_count = 0;
page_t **incorrect_pages;
int incorrect_page_count = 0;

void fileHandler(int lines) {
  printf("lines: %d\n", lines);
  rules = calloc(lines, sizeof *rules);
  pages = calloc(lines, sizeof *pages);
  incorrect_pages = calloc(lines, sizeof *incorrect_pages);
}

bool rules_input = true;

void lineHandler(char *line, int length) {
  printf("line (%d): %s\n", length, line);
  if (length == 1) {
    rules_input = false;
    return;
  }

  if (rules_input) {
    int *rule = calloc(2, sizeof(int));
    sscanf(line, "%d|%d", &rule[0], &rule[1]);
    rules[rule_count++] = rule;
  } else {
    page_t *page = malloc(sizeof(page_t));
    page->pages = calloc(32, sizeof(int));

    char *token = strtok(line, ",");
    do {
      page->pages[page->size++] = atoi(token);
    } while ((token = strtok(NULL, ",")) != NULL);
    pages[page_count++] = page;
  }
}

void printRules() {
  for (int i = 0; i < rule_count; i++) {
    int *rule = rules[i];
    printf("[%d]: %d|%d\n", i, rule[0], rule[1]);
  }
}

void printPages(page_t **pages_to_print, int count) {
  for (int i = 0; i < count; i++) {
    page_t *page = pages_to_print[i];
    printf("[%d]: (%d)\n", i, page->size);
    for (int j = 0; j < page->size; j++) {
      printf("%d,", page->pages[j]);
    }
    printf("\n");
  }
}

int findIndex(int *array, int size, int target) {

  int index = -1;

  for (int i = 0; i < size; i++) {
    if (array[i] == target) {
      return i;
    }
  }

  return index;
}

bool isOrderedCorrectly(page_t *page) {
  bool is_correct = true;
  // for every rule
  for (int r = 0; r < rule_count; r++) {
    int *rule = rules[r];
    // find index of page_one in page->pages
    int page_one_index = findIndex(page->pages, page->size, rule[0]);
    // find index of page_two in page->pages
    int page_two_index = findIndex(page->pages, page->size, rule[1]);

    // if both indexes !== -1
    if (page_one_index != -1 && page_two_index != -1) {

      if (page_one_index > page_two_index) {
        // validate page_one_index < page_two_index
        // if it fails, the pages are bad, ignore
        is_correct = false;
        break;
      }
    }
  }
  return is_correct;
}

int getIncorrectRuleIndex(page_t *page) {
  int index = -1;
  // for every rule
  for (int r = 0; r < rule_count; r++) {
    int *rule = rules[r];
    // find index of page_one in page->pages
    int page_one_index = findIndex(page->pages, page->size, rule[0]);
    // find index of page_two in page->pages
    int page_two_index = findIndex(page->pages, page->size, rule[1]);

    // if both indexes !== -1
    if (page_one_index != -1 && page_two_index != -1) {

      if (page_one_index > page_two_index) {
        // validate page_one_index < page_two_index
        // if it fails, the pages are bad, ignore
        index = r;
        break;
      }
    }
  }
  return index;
}

int main() {
  init();
  readInputFile(__FILE__, lineHandler, fileHandler);

  // printRules();
  // printPages(pages, page_count);

  int part_one = 0;

  for (int i = 0; i < page_count; i++) {
    page_t *page = pages[i];

    bool is_correct = isOrderedCorrectly(page);
    if (is_correct) {
      part_one += page->pages[page->size / 2];
    } else {
      incorrect_pages[incorrect_page_count++] = page;
    }
  }

  printf("Part one: %d\n", part_one);
#ifdef TEST_MODE
  assert(part_one == 143);
#else
  assert(part_one == 5713);
#endif

  int part_two = 0;
  for (int i = 0; i < incorrect_page_count; i++) {
    page_t *page = incorrect_pages[i];
    int incorrect_rule_index = getIncorrectRuleIndex(page);

    while (incorrect_rule_index != -1) {
      int *rule = rules[incorrect_rule_index];

      // find index of page_one in page->pages
      int page_one_index = findIndex(page->pages, page->size, rule[0]);
      // find index of page_two in page->pages
      int page_two_index = findIndex(page->pages, page->size, rule[1]);

      // swap em
      int a = page->pages[page_one_index];
      int b = page->pages[page_two_index];
      page->pages[page_one_index] = b;
      page->pages[page_two_index] = a;

      // try again (or it is sorted correctly and we exit loop)
      incorrect_rule_index = getIncorrectRuleIndex(page);
    }

    // now it is correct
    part_two += page->pages[page->size / 2];
  }

  // printPages(incorrect_pages, incorrect_page_count);
  printf("Part two: %d\n", part_two);
#ifdef TEST_MODE
  assert(part_two == 123);
#else
  assert(part_two == 5180);
#endif
  exit(EXIT_SUCCESS);
}
