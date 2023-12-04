#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
// #define TEST_MODE
#include "../utils.h"

struct cubes_t {
  int blue;
  int red;
  int green;
} cubes_t;

struct gameline_t {
  int id;
  struct cubes_t **games;
  int gameCount;
} gameline_t;

struct gameline_t **allGames;
int allGamesCount = 0;

void fileHandler(int lines) {
  printf("lines: %d\n", lines);
  allGames = malloc(lines * sizeof(struct gameline_t *));
}

int strcount(char *string, char character) {
  int count = 0;
  int length = strlen(string);
  for (int i = 0; i < length; i++) {
    if (string[i] == character) {
      count++;
    }
  }
  return count;
}

// Game 1: 3 blue, 4 red; 1 red, 2 green, 6 blue; 2 green
void lineHandler(char *line, int length) {
  printf("line (%d): %s\n", length, line);
  struct gameline_t *game_line = malloc(sizeof(struct gameline_t));
  // first split on ':'
  //
  char *ptr1;
  // "Game 1"
  char *game_str = strtok_r(line, ":", &ptr1);
  char *ptr4;                     // yea i did this bit last
  strtok_r(game_str, " ", &ptr4); // discard "Game"
  char *game_id = strtok_r(NULL, " ", &ptr4);
  game_line->id = (int)strtol(game_id, NULL, 10);

  // "3 blue, 4 red; 1 red, 2 green, 6 blue; 2 green"
  char *cubeliststr = strtok_r(NULL, ":", &ptr1);
  // printf("%s ---- %s\n", game_id, cubeliststr);

  int numberOfGames = strcount(cubeliststr, ';') + 1;
  // printf("no of games %d\n", numberOfGames);
  game_line->games = calloc(numberOfGames, sizeof(struct cubes_t *));
  game_line->gameCount = numberOfGames;
  char *ptr2;
  char *gametoken = strtok_r(cubeliststr, ";", &ptr2);
  int gameindex = 0;
  // for each game
  while (gametoken != NULL) {
    // "3 blue, 4 red"
    // printf("gametoken: '%s'\n", gametoken);
    game_line->games[gameindex] = calloc(1, sizeof(struct cubes_t));
    // game_line->games[gameindex]->red = 0;
    char *ptr3;
    char *cubetoken = strtok_r(gametoken, ",", &ptr3);
    while (cubetoken != NULL) {
      // "3 blue"
      // printf("cubetoken: '%s'\n", cubetoken);
      bool isgreen = strchr(cubetoken, 'g') != NULL;
      // also matches green, so need to evaluate green first
      bool isred = strchr(cubetoken, 'r') != NULL;
      bool isblue = strchr(cubetoken, 'b') != NULL;
      int value = (int)strtol(cubetoken, NULL, 10);
      if (isgreen) {
        game_line->games[gameindex]->green = value;
      } else if (isred) {
        game_line->games[gameindex]->red = value;
      } else if (isblue) {
        game_line->games[gameindex]->blue = value;
      }
      // printf("game!: r '%d' g '%d' b '%d'\n",
      // game_line->games[gameindex]->red,
      //        game_line->games[gameindex]->green,
      //        game_line->games[gameindex]->blue);
      cubetoken = strtok_r(NULL, ",", &ptr3);
    }
    gametoken = strtok_r(NULL, ";", &ptr2);
    gameindex++;
  }

  allGames[allGamesCount++] = game_line;
}

bool isPossible(struct gameline_t *gameline, struct cubes_t limit) {
  // printf("limit: r '%d' g '%d' b '%d'\n", limit.red, limit.green,
  // limit.blue);
  for (int i = 0; i < gameline->gameCount; i++) {
    struct cubes_t *game = gameline->games[i];
    // printf("game: r '%d' g '%d' b '%d'\n", game->red, game->green,
    // game->blue);
    if (game->red > limit.red || game->green > limit.green ||
        game->blue > limit.blue) {
      // printf("BAD GAME %d\n", gameline->id);
      return false;
    }
  }
  // printf("GOOD GAME %d\n", gameline->id);
  return true;
}

bool getMinimumCubes(struct gameline_t *gameline, struct cubes_t* minimum) {
  for (int i = 0; i < gameline->gameCount; i++) {
    struct cubes_t *game = gameline->games[i];
    minimum->red = max(game->red, minimum->red);
    minimum->green = max(game->green, minimum->green);
    minimum->blue = max(game->blue, minimum->blue);
  }
  // printf("minimum: r '%d' g '%d' b '%d'\n", minimum.red, minimum.green,
         // minimum.blue);
  return true;
}

int getCubePower(struct cubes_t cubes) {
  return cubes.red * cubes.green * cubes.blue;
}

int main() {
  init();
  readInputFile(__FILE__, lineHandler, fileHandler);

  printf("total games = %d\n", allGamesCount);

  int partOne = 0;
  struct cubes_t limit = {.red = 12, .green = 13, .blue = 14};

  int partTwo = 0;

  for (int i = 0; i < allGamesCount; i++) {
    struct gameline_t *gameline = allGames[i];
    if (isPossible(gameline, limit)) {
      partOne += gameline->id;
    }
    struct cubes_t minimum = {.red = 0, .green = 0, .blue = 0};
    getMinimumCubes(gameline, &minimum);
    partTwo += getCubePower(minimum);
  }

  printf("Part one: %d\n", partOne);
#ifdef TEST_MODE
  assert(partOne == 8);
#else
  assert(partOne == 2169);
#endif

  printf("Part two: %d\n", partTwo);
#ifdef TEST_MODE
  assert(partTwo == 2286);
#else
  assert(partTwo == 60948);
#endif
  exit(EXIT_SUCCESS);
}
