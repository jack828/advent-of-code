#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#define TEST_MODE
#include "../utils.h"

#ifdef TEST_MODE
#define WINNING_NUMBERS_COUNT 5
#define NUMBERS_COUNT 8
#else
#define WINNING_NUMBERS_COUNT 10
#define NUMBERS_COUNT 25
#endif

struct scratchcard_t {
  int *winning_numbers;
  int *numbers;
  int points;
} scratchcard_t;

struct scratchcard_t **scratchcards;
int card_count = 0;

void fileHandler(int lines) {
  printf("lines: %d\n", lines);
  scratchcards = calloc(lines, sizeof(struct scratchcard_t *));
}

void lineHandler(char *line, int length) {
  printf("line (%d): %s\n", length, line);

  // YES I AM AWARE
#ifdef TEST_MODE
  int w_num1, w_num2, w_num3, w_num4, w_num5;
  int num1, num2, num3, num4, num5, num6, num7, num8;

  sscanf(line, "Card %*d: %d %d %d %d %d | %d %d %d %d %d %d %d %d", &w_num1,
         &w_num2, &w_num3, &w_num4, &w_num5, &num1, &num2, &num3, &num4, &num5,
         &num6, &num7, &num8);

  printf("           Card  : %d %d %d %d %d | %d %d %d %d %d %d %d %d\n",
         w_num1, w_num2, w_num3, w_num4, w_num5, num1, num2, num3, num4, num5,
         num6, num7, num8);
  int nums[NUMBERS_COUNT] = {num1, num2, num3, num4, num5, num6, num7, num8};
  int w_nums[WINNING_NUMBERS_COUNT] = {w_num1, w_num2, w_num3, w_num4, w_num5};
#else
  int w_num1, w_num2, w_num3, w_num4, w_num5, w_num6, w_num7, w_num8, w_num9,
      w_num10;
  int num1, num2, num3, num4, num5, num6, num7, num8, num9, num10, num11, num12,
      num13, num14, num15, num16, num17, num18, num19, num20, num21, num22,
      num23, num24, num25;

  sscanf(line,
         "Card %*d: %d %d %d %d %d %d %d %d %d %d | %d %d %d %d %d %d %d %d %d "
         "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
         &w_num1, &w_num2, &w_num3, &w_num4, &w_num5, &w_num6, &w_num7, &w_num8,
         &w_num9, &w_num10, &num1, &num2, &num3, &num4, &num5, &num6, &num7,
         &num8, &num9, &num10, &num11, &num12, &num13, &num14, &num15, &num16,
         &num17, &num18, &num19, &num20, &num21, &num22, &num23, &num24,
         &num25);

  printf("           Card  : %d %d %d %d %d %d %d %d %d %d | %d %d %d %d %d %d "
         "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
         w_num1, w_num2, w_num3, w_num4, w_num5, w_num6, w_num7, w_num8, w_num9,
         w_num10, num1, num2, num3, num4, num5, num6, num7, num8, num9, num10,
         num11, num12, num13, num14, num15, num16, num17, num18, num19, num20,
         num21, num22, num23, num24, num25);
  int nums[NUMBERS_COUNT] = {num1,  num2,  num3,  num4,  num5,  num6,  num7,
                             num8,  num9,  num10, num11, num12, num13, num14,
                             num15, num16, num17, num18, num19, num20, num21,
                             num22, num23, num24, num25};
  int w_nums[WINNING_NUMBERS_COUNT] = {w_num1, w_num2, w_num3, w_num4, w_num5,
                                       w_num6, w_num7, w_num8, w_num9, w_num10};
#endif

  struct scratchcard_t *card = malloc(sizeof(struct scratchcard_t *));
  card->numbers = calloc(NUMBERS_COUNT, sizeof(int));
  memcpy(card->numbers, nums, NUMBERS_COUNT * sizeof(int));
  card->winning_numbers = calloc(WINNING_NUMBERS_COUNT, sizeof(int));
  memcpy(card->winning_numbers, w_nums, WINNING_NUMBERS_COUNT * sizeof(int));
  card->points = 0;
  scratchcards[card_count++] = card;
}

int main() {
  init();
  readInputFile(__FILE__, lineHandler, fileHandler);

  int part_one = 0;
  for (int i = 0; i < card_count; i++) {
    struct scratchcard_t *card = scratchcards[i];
    int points = 0;

    for (int j = 0; j < WINNING_NUMBERS_COUNT; j++) {
      int winning_number = card->winning_numbers[j];
      for (int k = 0; k < NUMBERS_COUNT; k++) {
        int number = card->numbers[k];
        if (winning_number == number) {
          points = points == 0 ? 1 : points << 1;
        }
      }
    }
    card->points = points;
    part_one += points;
  }

  printf("Part one: %d\n", part_one);
#ifdef TEST_MODE
  assert(part_one == 13);
#else
  assert(part_one == 23673);
#endif

  printf("Part two: %d\n", 420);
#ifdef TEST_MODE
  assert(420 == 69);
#else
  assert(420 == 69);
#endif
  exit(EXIT_SUCCESS);
}
