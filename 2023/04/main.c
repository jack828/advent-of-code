#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
// #define TEST_MODE
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
  int matches;
  int index;
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

  // printf("           Card  : %d %d %d %d %d | %d %d %d %d %d %d %d %d\n",
  //        w_num1, w_num2, w_num3, w_num4, w_num5, num1, num2, num3, num4, num5,
  //        num6, num7, num8);
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

  // printf("           Card  : %d %d %d %d %d %d %d %d %d %d | %d %d %d %d %d %d "
  //        "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
  //        w_num1, w_num2, w_num3, w_num4, w_num5, w_num6, w_num7, w_num8, w_num9,
  //        w_num10, num1, num2, num3, num4, num5, num6, num7, num8, num9, num10,
  //        num11, num12, num13, num14, num15, num16, num17, num18, num19, num20,
  //        num21, num22, num23, num24, num25);
  int nums[NUMBERS_COUNT] = {num1,  num2,  num3,  num4,  num5,  num6,  num7,
                             num8,  num9,  num10, num11, num12, num13, num14,
                             num15, num16, num17, num18, num19, num20, num21,
                             num22, num23, num24, num25};
  int w_nums[WINNING_NUMBERS_COUNT] = {w_num1, w_num2, w_num3, w_num4, w_num5,
                                       w_num6, w_num7, w_num8, w_num9, w_num10};
#endif

  struct scratchcard_t *card = malloc(sizeof(struct scratchcard_t));
  card->numbers = calloc(NUMBERS_COUNT, sizeof(int));
  memcpy(card->numbers, nums, NUMBERS_COUNT * sizeof(int));
  card->winning_numbers = calloc(WINNING_NUMBERS_COUNT, sizeof(int));
  memcpy(card->winning_numbers, w_nums, WINNING_NUMBERS_COUNT * sizeof(int));
  card->matches = 0;
  card->index = card_count;
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
          card->matches++;
        }
      }
    }
    part_one += points;
  }

  printf("Part one: %d\n", part_one);
#ifdef TEST_MODE
  assert(part_one == 13);
#else
  assert(part_one == 23673);
#endif

  int num_cards_checked = 0;
  for (int i = 0; i < card_count; i++) {
    num_cards_checked++;
    struct scratchcard_t *card = scratchcards[i];
    if (card->matches == 0) {
      continue;
    }
    scratchcards = realloc(scratchcards, (card_count + card->matches  ) *
                                             sizeof(struct scratchcard_t *));

    // printf("scrarchcards length %d\n", (card_count + card->matches));
    // printf("Check card %d - original %d has %d matches\n", i, card->index,
           // card->matches);
    for (int j = 1; j <= card->matches; j++) {
      struct scratchcard_t *new_card = malloc(sizeof(struct scratchcard_t));
      memcpy(new_card, scratchcards[card->index + j],
             sizeof(struct scratchcard_t));
      // printf("add card %d to position %d\n", card->index + j, card_count + j-1);
      scratchcards[card_count + j -1] = new_card;
    }
    card_count += card->matches;
  }

  printf("Part two: %d\n", card_count);
#ifdef TEST_MODE
  assert(card_count == 30);
#else
  assert(card_count == 12263631);
#endif
  exit(EXIT_SUCCESS);
}
