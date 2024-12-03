#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
// #define TEST_MODE
#include "../utils.h"

typedef enum dir_t { INC = 1, DEC = -1 } dir_t;
typedef struct report_t {
  int size;
  int *data;
} report_t;

report_t **reports;
int report_count = 0;

void fileHandler(int lines) {
  printf("lines: %d\n", lines);
  reports = calloc(lines, sizeof *reports);
}

void lineHandler(char *line, int length) {
  printf("line (%d): %s\n", length, line);
  report_t *report = malloc(sizeof *report);
  *report =
      (report_t){.data = calloc(20 /* arbitrary */, sizeof *report), .size = 0};

  char *lineEnd;
  char *token;
  token = strtok_r(line, " ", &lineEnd);
  do {
    report->data[report->size++] = atoi(token);
  } while ((token = strtok_r(NULL, " ", &lineEnd)) != NULL);
  reports[report_count++] = report;
}

void printReports() {
  for (int i = 0; i < report_count; i++) {
    report_t *report = reports[i];

    printf("\nReport: %d\n", i);
    for (int j = 0; j < report->size; j++) {
      printf("%d,", report->data[j]);
    }
    printf("\n");
  }
}

bool isSafe(report_t *report, int ignored_index) {
  // as in, increasing or decreasing
  // naming is hard
  // -1 decreasing
  //  1 increasing
  //  0 start
  dir_t direction = 0;
  int last = 0;
  bool safe = true;
  for (int j = 0; j < report->size; j++) {
    if (j == ignored_index) {
      continue;
    }
    int current = report->data[j];
    // printf("%d,", report->data[j]);
    if (direction == 0) {
      if (last == 0) {

        last = current;
        continue;
      }

      // determine direction
      direction = last > current ? DEC : INC;
    }

    // direction must all be the same
    dir_t dir = last > current ? DEC : INC;
    if (direction != dir) {
      // printf("unsafe dir\n");
      safe = false;
      break;
    }

    int diff = abs(last - current);
    if (diff > 3 || diff == 0) {
      // printf("unsafe diff\n");
      safe = false;
      break;
    }

    last = current;
  }
  return safe;
}

int main() {
  init();
  readInputFile(__FILE__, lineHandler, fileHandler);

  // printReports();

  int safe_report_count = 0;

  for (int i = 0; i < report_count; i++) {
    report_t *report = reports[i];

    // printf("\nReport: %d\n", i);
    bool safe = isSafe(report, -1);
    if (safe) {
      safe_report_count++;
    }
  }

  printf("Part one: %d\n", safe_report_count);
#ifdef TEST_MODE
  assert(safe_report_count == 2);
#else
  assert(safe_report_count == 299);
#endif

  int very_safe_report_count = 0;

  for (int i = 0; i < report_count; i++) {
    report_t *report = reports[i];

    // printf("\nReport: %d\n", i);
    bool safe = isSafe(report, -1);
    if (safe) {
      very_safe_report_count++;
    } else {
      // find the safer one
      for (int j = 0; j < report->size; j++) {

        bool very_safe = isSafe(report, j);

        if (very_safe) {
          very_safe_report_count++;
          break;
        }
      }
    }
  }

  printf("Part two: %d\n", very_safe_report_count);
#ifdef TEST_MODE
  assert(very_safe_report_count == 4);
#else
  assert(very_safe_report_count == 364);
#endif
  exit(EXIT_SUCCESS);
}
