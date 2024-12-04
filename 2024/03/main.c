#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#define _AOC_LINE_MAX 65535
// #define TEST_MODE
#include "../utils.h"
#define PCRE2_CODE_UNIT_WIDTH 8
#include "pcre2.h"

char *program;

void fileHandler(int lines) { printf("lines: %d\n", lines); }

void lineHandler(char *line, int length) {
  printf("line (%d): %s\n", length, line);
  program = strdup(line);
}

int part_one = 0;
int part_two = 0;
int a = 0;

void match_part_one(void *data) {
  int val = atoi((char *)data);
  if (!a) {
    a = val;
  } else {
    part_one += a * val;
    a = 0;
  }
  return;
}

bool flag = true;
void match_part_two(void *group) {
  char *data = group;

  if (strcmp(data, "do()") == 0) {
    flag = true;
    return;
  } else if (strcmp(data, "don't()") == 0) {
    flag = false;
    return;
  }

  if (!flag) {
    return;
  }

  int val = atoi(data);
  if (!a) {
    a = val;
  } else {
    part_two += a * val;
    a = 0;
  }
  return;
}

/**
 * Execute a regex on a string.
 *
 * on_match function will be called with the capture group contents.
 *
 * for on_match to trigger, the regex must contain capture groups!
 */
int regex_exec(char *subject, char *pattern, void (*on_match)(void *)) {
  pcre2_code *regex;
  PCRE2_SPTR subject_ptr = (PCRE2_SPTR)subject;
  PCRE2_SPTR pattern_ptr = (PCRE2_SPTR)pattern;

  int error_number;
  PCRE2_SIZE error_offset;

  // Compile the regex pattern
  regex = pcre2_compile(pattern_ptr, PCRE2_ZERO_TERMINATED, 0, &error_number,
                        &error_offset, NULL);
  if (regex == NULL) {
    PCRE2_UCHAR buffer[256];
    pcre2_get_error_message(error_number, buffer, sizeof(buffer));
    printf("PCRE2 compilation failed at offset %zu: %s\n", error_offset,
           buffer);
    return -1;
  }

  // Create match data block
  pcre2_match_data *match_data =
      pcre2_match_data_create_from_pattern(regex, NULL);

  // Match context
  pcre2_match_context *match_context = pcre2_match_context_create(NULL);

  // Matching variables
  PCRE2_SIZE *ovector;
  PCRE2_SIZE start_offset = 0;
  int rc;

  // Multiple match loop
  while ((rc = pcre2_match(regex, subject_ptr, strlen(program), start_offset, 0,
                           match_data, match_context)) >= 0) {
    // Get the output vector for match locations
    ovector = pcre2_get_ovector_pointer(match_data);

    for (int i = 1; i < rc; i++) {
      PCRE2_SIZE start = ovector[2 * i];
      PCRE2_SIZE end = ovector[2 * i + 1];

      if (start != PCRE2_UNSET) {
        char str[end - start];

        sprintf(str, "%.*s", (int)(end - start), (char *)(program + start));
        on_match(str);
      }
    }

    // Update start offset to continue searching
    start_offset = ovector[1];

    // Break if no more matches or at end of string
    if (start_offset == strlen(program)) {
      break;
    }
  }

  // Cleanup
  pcre2_match_data_free(match_data);
  pcre2_match_context_free(match_context);
  pcre2_code_free(regex);
  return 0;
}

int main() {
  init();
  readInputFile(__FILE__, lineHandler, fileHandler);

  regex_exec(program, "mul\\((\\d{1,3}),(\\d{1,3})\\)", match_part_one);

  printf("Part one: %d\n", part_one);
#ifdef TEST_MODE
  assert(part_one == 161);
#else
  assert(part_one == 161289189);
#endif

  regex_exec(program, "mul\\((\\d{1,3}),(\\d{1,3})\\)|(do\\(\\))|(don't\\(\\))",
             match_part_two);
  printf("Part two: %d\n", part_two);
#ifdef TEST_MODE
  assert(part_two == 48);
#else
  assert(part_two == 83595109);
#endif
  exit(EXIT_SUCCESS);
}
