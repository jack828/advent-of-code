#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#define _AOC_LINE_MAX 1024

void readInput(const char *file, void (*lineHandler)(char *)) {
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
    exit(1);
  }
  char line[_AOC_LINE_MAX];
  while (fgets(line, sizeof(line), fp) != NULL) {
    lineHandler(line);
  }
  fclose(fp);
}

int max(int a, int b) {
  return a > b ? a : b;
}

int min(int a, int b) {
  return a > b ? b : a;
}

#endif
