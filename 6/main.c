#include "input.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#define LINE_MAX 4096

u_int64_t findUniqueIndex(char *string, u_int64_t length) {
  u_int64_t index = 0;
  u_int64_t stringLength = strlen(string);
  for (; index <= stringLength - length; index++) {
    char substring[length];
    strncpy(substring, &string[index], length);
    /* fprintf(stdout, "substring: %s, index: %lu\n", substring, index); */

    bool allUnique = true;

    for (u_int64_t i = 0; i < length; i++) {
      for (u_int64_t j = 0; j < length; j++) {
        if (i == j) {
          continue;
        }

        allUnique = substring[i] != substring[j];

        /* fprintf(stdout, "unique: i: %lu - %c, j: %lu - %c, au: %d\n", i,
         * substring[i], j, substring[j], allUnique); */
        if (!allUnique) {
          goto substringLoop;
        }
      }
    }
  substringLoop:
    if (allUnique) {
      // start of X marker complete after index number of characters PLUS
      // the length of the string being searched
      return index + length;
    }
  }

  return index; // ???
}

int main() {
  /* FILE *fp = fopen("./6/input.txt", "r");
  if (fp == NULL) {
    perror("Unable to open file!");
    exit(1);
  }

  char chunk[LINE_MAX];

  while (fgets(chunk, sizeof(chunk), fp) != NULL) {
    fputs(chunk, stdout);
    fputs("e\n", stdout);
  }
  fclose(fp);
  */
  /*
  u_int64_t idx1 = findUniqueIndex("mjqjpqmgbljsphdztnvjfqwrcgsmlb", 4);
  fprintf(stdout, "Test 4: %lu - %d\n", idx1, idx1 == 7);
  u_int64_t idx2 = findUniqueIndex("mjqjpqmgbljsphdztnvjfqwrcgsmlb", 14);
  fprintf(stdout, "Test 14: %lu - %d\n", idx2, idx2 == 19);
  fprintf(stdout, "Case 1: %lu\n",
          findUniqueIndex("mjqjpqmgbljsphdztnvjfqwrcgsmlb", 4)); // 7
  fprintf(stdout, "Case 2: %lu\n",
          findUniqueIndex("bvwbjplbgvbhsrlpgdmjqwftvncz", 4)); // 5
  fprintf(stdout, "Case 3: %lu\n",
          findUniqueIndex("nppdvjthqldpwncqszvftbrmjlhg", 4)); // 6
  fprintf(stdout, "Case 4: %lu\n",
          findUniqueIndex("nznrnfrfntjfmvfwmzdfjlvtqnbhcprsg", 4)); // 10
  fprintf(stdout, "Case 5: %lu\n",
          findUniqueIndex("zcfzfwzzqfrljwzlrfnpqdbhtmscgvjw", 4)); // 11
  */

  // ----part one----
  fprintf(stdout, "Part one: %lu\n", findUniqueIndex(INPUT, 4));

  /*
  fprintf(stdout, "Case 2: %lu\n",
          findUniqueIndex("bvwbjplbgvbhsrlpgdmjqwftvncz", 14)); // 23
  fprintf(stdout, "Case 3: %lu\n",
          findUniqueIndex("nppdvjthqldpwncqszvftbrmjlhg", 14)); // 23
  fprintf(stdout, "Case 4: %lu\n",
          findUniqueIndex("nznrnfrfntjfmvfwmzdfjlvtqnbhcprsg", 14)); // 29
  fprintf(stdout, "Case 5: %lu\n",
          findUniqueIndex("zcfzfwzzqfrljwzlrfnpqdbhtmscgvjw", 14)); // 26
  */

  // ----part two----
  fprintf(stdout, "Part two: %lu\n", findUniqueIndex(INPUT, 14));
}
