#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
// #define TEST_MODE
#include "../utils.h"

char **games;
int gameIndex = 0;

enum selfMoves { SELF_ROCK = 'X', SELF_PAPER = 'Y', SELF_SCISSORS = 'Z' };
enum opponentMoves { OPP_ROCK = 'A', OPP_PAPER = 'B', OPP_SCISSORS = 'C' };
enum results { RESULT_LOSE = 'X', RESULT_DRAW = 'Y', RESULT_WIN = 'Z' };

void fileHandler(int lines) {
  fprintf(stdout, "lines: %d\n", lines);
  games = malloc(lines * sizeof(char *));
}

void lineHandler(char *line) {
  fprintf(stdout, "line: %s\n", line);
  games[gameIndex++] = strdup(line);
}

int calcResult(int opponent, int me) {
  switch (opponent) {
  case OPP_ROCK:
    opponent = SELF_ROCK;
    break;
  case OPP_PAPER:
    opponent = SELF_PAPER;
    break;
  case OPP_SCISSORS:
    opponent = SELF_SCISSORS;
    break;
  }
  if (opponent == me) {
    return 3;
  }
  if (opponent == 'X' && me == 'Y') {
    return 6;
  }
  if (opponent == 'X' && me == 'Z') {
    return 0;
  }
  if (opponent == 'Y' && me == 'X') {
    return 0;
  }
  if (opponent == 'Y' && me == 'Z') {
    return 6;
  }
  if (opponent == 'Z' && me == 'X') {
    return 6;
  }
  if (opponent == 'Z' && me == 'Y') {
    return 0;
  }
  return 9001;
}

int moveScore(int move) {
  switch (move) {
  case SELF_ROCK:
    return 1;
  case SELF_PAPER:
    return 2;
  case SELF_SCISSORS:
    return 3;
  }
  return 9001;
}

int main() {
  init();
  readInputFile(__FILE__, lineHandler, fileHandler);

  int partOneScore = 0;
  int partTwoScore = 0;
  for (int i = 0; i < gameIndex; i++) {
    char *game = games[i];
    // fprintf(stdout, "Game %d: %s\n", i, game);

    // Part One
    char opponentMove = game[0];
    char selfMove = game[2];

    partOneScore += moveScore(selfMove) + calcResult(opponentMove, selfMove);

    // Part Two
    char result = game[2];
    char resultMove = 0;
    char resultScore = 0;
    switch (opponentMove) {
    case OPP_ROCK:
      switch (result) {
      case RESULT_WIN:
        resultMove = SELF_PAPER;
        resultScore = 6;
        break;
      case RESULT_DRAW:
        resultMove = SELF_ROCK;
        resultScore = 3;
        break;
      case RESULT_LOSE:
        resultMove = SELF_SCISSORS;
        resultScore = 0;
        break;
      }
      break;
    case OPP_PAPER:
      switch (result) {
      case RESULT_WIN:
        resultMove = SELF_SCISSORS;
        resultScore = 6;
        break;
      case RESULT_DRAW:
        resultMove = SELF_PAPER;
        resultScore = 3;
        break;
      case RESULT_LOSE:
        resultMove = SELF_ROCK;
        resultScore = 0;
        break;
      }
      break;
    case OPP_SCISSORS:
      switch (result) {
      case RESULT_WIN:
        resultMove = SELF_ROCK;
        resultScore = 6;
        break;
      case RESULT_DRAW:
        resultMove = SELF_SCISSORS;
        resultScore = 3;
        break;
      case RESULT_LOSE:
        resultMove = SELF_PAPER;
        resultScore = 0;
        break;
      }
      break;
    }

    partTwoScore += moveScore(resultMove) + resultScore;
  }

  fprintf(stdout, "Part one: %d\n", partOneScore);
#ifdef TEST_MODE
  assert(partOneScore == 15);
#else
  assert(partOneScore == 14264);
#endif

  fprintf(stdout, "Part two: %d\n", partTwoScore);
#ifdef TEST_MODE
  assert(partTwoScore == 12);
#else
  assert(partTwoScore == 12382);
#endif
  exit(EXIT_SUCCESS);
}
