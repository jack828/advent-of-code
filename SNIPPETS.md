# Snippets

A collection of snippets that I find come in handy when doing the puzzles.

Also because I CBA to generalise and put in a lib folder.

Also may contain just links to a year/day where a particular algorithm is used.

## Lib Folder

I lied, there is a lib folder, for generic stuff.

It contains:

 - Priority Queue
 - Queue

# Common bits

## Dynamically allocated 2D array

```c
char **map;
int height = 0;
int width = 0;
int line_i = 0;

void fileHandler(int lines) {
  printf("lines: %d\n", lines);
  map = calloc(lines, sizeof(char *));
  height = lines;
}

void lineHandler(char *line, int length) {
  printf("line (%d): %s\n", length, line);

  if (width == 0) {
    width = length;
    for (int i = 0; i < height; i++) {
      map[i] = calloc(width, sizeof(char));
    }
  }

  strncpy(map[line_i++], line, length);
}
```

# Algorithms

## Flood Fill

 - 3D [2022 Day 18](./2022/18/main.c)
 - 2D [2023 Day 10](./2023/10/main.c)

## A*

 - [2022 Day 24](2022/24/main.c)

## DFS (Depth First Search)

 - [2022 Day 24](2022/24/main.c)
 - [2023 Day 10](2023/10/main.c)

