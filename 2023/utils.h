#ifndef __UTILS_H__
#define __UTILS_H__

#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#ifndef _AOC_LINE_MAX
#define _AOC_LINE_MAX 1024
#endif

void sigxcpu_handler(int signum) {
  exit(EXIT_FAILURE);
}

void init() { signal(SIGXCPU, sigxcpu_handler); }

void readInputFile(const char *file, void (*lineHandler)(char *),
                   void (*fileHandler)(int)) {
  char path[_AOC_LINE_MAX];
  path[0] = '\0';
  strcat(path, file);
  *strrchr(path, '/') = '\0';
#ifdef TEST_MODE
  strcat(path, "/example.txt");
#else
  strcat(path, "/input.txt");
#endif
  fprintf(stdout, "Reading input file: %s\n", path);

  FILE *fp = fopen(path, "r");
  if (fp == NULL) {
    perror("Unable to open file!");
    exit(EXIT_FAILURE);
  }

  // count number of lines
  int c;
  int newlineCount = 0;
  while ((c = fgetc(fp)) != EOF) {
    if (c == '\n')
      newlineCount++;
  }
  fileHandler(newlineCount);

  fseek(fp, 0, 0);

  char line[_AOC_LINE_MAX];
  while (fgets(line, sizeof(line), fp) != NULL) {
    // trim trailing newline
    if (strlen(line) > 1) {
      line[strcspn(line, "\n")] = '\0';
    }
    lineHandler(line);
  }
  fclose(fp);
}

int max(int a, int b) { return a > b ? a : b; }

int min(int a, int b) { return a > b ? b : a; }

#endif
