#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "../utils.h"

#define LINE_MAX 1024

char cwd[LINE_MAX];

typedef enum { DIRECTORY_T, FILE_T } type_t;

typedef struct FS {
  char name[LINE_MAX];
  type_t type;
  u_int64_t size;
  struct FS *parent;
  struct FS *filesystem[LINE_MAX];
  u_int64_t filesystemIndex;
} FS;

struct FS filesystem;
struct FS *all[LINE_MAX];
u_int64_t allIndex = 0;

void printDir(struct FS *fs, int level) {
  for (int i = 0; i < level; i++) {
    fputs("  ", stdout);
  }
  fputs("- ", stdout);
  fprintf(stdout, "%s (", fs->name);
  switch (fs->type) {
  case DIRECTORY_T:
    fputs("dir", stdout);
    break;
  case FILE_T:
    fputs("file", stdout);
    break;
  }
  fprintf(stdout, ", size=%lu", fs->size);
  fputs(")\n", stdout);
  if (fs->filesystemIndex != 0) {
    for (int i = 0; i < fs->filesystemIndex; i++) {
      printDir(fs->filesystem[i], level + 1);
    }
  }
}

int compare(const void *a, const void *b) {
  u_int64_t int_a = *((u_int64_t *)a);
  u_int64_t int_b = *((u_int64_t *)b);

  if (int_a == int_b)
    return 0;
  else if (int_a < int_b)
    return -1;
  else
    return 1;
}

int main() {
  init();
  FILE *fp = fopen("./2022/7/input.txt", "r");
  if (fp == NULL) {
    perror("Unable to open file!");
    exit(1);
  }

  char chunk[LINE_MAX];

  struct FS *cwd = &filesystem;

  while (fgets(chunk, sizeof(chunk), fp) != NULL) {
    fputs(chunk, stdout);

    if (strcmp(chunk, "$ cd /\n") == 0) {
      strcpy(filesystem.name, "/");
      filesystem.size = 0;
      filesystem.type = DIRECTORY_T;
      filesystem.filesystemIndex = 0;
      filesystem.parent = NULL;
      continue;
    }

    if (chunk[0] == '$') {
      // a command
      if (strcmp(chunk, "$ ls\n") == 0) {
        // we can basically ignore this, and assume that every other line
        // that does not begin with $ is the output of `ls`
      } else if (strcmp(chunk, "$ cd ..\n") == 0) {
        cwd = cwd->parent;
      } else if (strncmp(chunk, "$ cd ", 5) == 0) {
        char *token;
        token = strtok(chunk, " "); // $
        token = strtok(NULL, " ");  // cd
        token = strtok(NULL, " ");  // <dirname>\n
        // trim newline off
        token[strlen(token) - 1] = '\0';
        // find the directory in the hierarchy and point cwd to it
        for (u_int64_t i = 0; i <= cwd->filesystemIndex - 1; i++) {
          if (strcmp(cwd->filesystem[i]->name, token) == 0) {
            cwd = cwd->filesystem[i];
            break;
          }
        }
      }
    } else {
      // is files/directories
      if (strncmp(chunk, "dir", 3) == 0) {
        struct FS *directory;
        directory = malloc(sizeof(FS));
        char *token;
        token = strtok(chunk, " "); // dir
        token = strtok(NULL, " ");  // <dirname>\n
        strncpy(directory->name, token, strlen(token) - 1);
        directory->size = 0;
        directory->type = DIRECTORY_T;
        directory->filesystemIndex = 0;
        directory->parent = cwd;
        cwd->filesystem[cwd->filesystemIndex] = directory;
        cwd->filesystemIndex++;
        all[allIndex++] = directory;
      } else {
        fputs("file\n", stdout);
        struct FS *file;
        file = malloc(sizeof(FS));
        char *token;
        token = strtok(chunk, " ");
        file->size = atoi(token);
        token = strtok(NULL, " ");
        strncpy(file->name, token, strlen(token) - 1);
        file->type = FILE_T;
        file->parent = cwd;
        struct FS *parentPtr = file->parent;
        do {
          parentPtr->size += file->size;
          parentPtr = parentPtr->parent;
        } while (parentPtr != NULL);
        cwd->filesystem[cwd->filesystemIndex] = file;
        cwd->filesystemIndex++;
        all[allIndex++] = file;
      }
    }

    fputs("\n", stdout);
  }

  // Print a tree representation of the filesystem
  printDir(&filesystem, 0);
  u_int64_t dirSum = 0;
  u_int64_t SIZE_LIMIT = 100000;

  for (u_int64_t i = 0; i < allIndex; i++) {
    struct FS *fs = all[i];

    if (fs->type == DIRECTORY_T && fs->size < SIZE_LIMIT) {
      dirSum += fs->size;
    }
  }

  fprintf(stdout, "Part one: %lu\n", dirSum);

  // Get all directory sizes in an array
  u_int64_t allSizes[allIndex];

  for (u_int64_t i = 0; i < allIndex; i++) {
    struct FS *fs = all[i];

    if (fs->type == DIRECTORY_T) {
      allSizes[i] = fs->size;
    } else {
      // ignoring files
      allSizes[i] = 0;
    }
  }
  // sort them by size
  qsort(allSizes, allIndex, sizeof(u_int64_t), compare);
  u_int64_t REQUIRED_SIZE = 30000000;
  u_int64_t FS_SIZE = 70000000;
  u_int64_t totalFreeSpace = FS_SIZE - filesystem.size;
  u_int64_t requiredFreeSpace = REQUIRED_SIZE - totalFreeSpace;

  // Loop through and find the first directory size that passes the limit
  u_int64_t partTwoIndex = allIndex - 1;
  for (u_int64_t i = 0; i < allIndex; i++) {
    /* fprintf(stdout, "%lu: %lu\n", i, allSizes[i]); */
    u_int64_t size = allSizes[i];

    if (size > requiredFreeSpace) {
      // limit passed
      partTwoIndex = i;
      break;
    }
  }
  fprintf(stdout, "Part two: %lu\n", allSizes[partTwoIndex]);
  fclose(fp);
  exit(EXIT_SUCCESS);
}
