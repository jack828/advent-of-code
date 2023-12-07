#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
// #define TEST_MODE
#include "../utils.h"

typedef enum {
  HIGH_CARD,
  ONE_PAIR,
  TWO_PAIR,
  THREE_OF_A_KIND,
  FULL_HOUSE,
  FOUR_OF_A_KIND,
  FIVE_OF_A_KIND,
} hand_type_t;

const char *const hand_type_str[] = {[FIVE_OF_A_KIND] = "FIVE_OF_A_KIND",
                                     [FOUR_OF_A_KIND] = "FOUR_OF_A_KIND",
                                     [FULL_HOUSE] = "FULL_HOUSE",
                                     [THREE_OF_A_KIND] = "THREE_OF_A_KIND",
                                     [TWO_PAIR] = "TWO_PAIR",
                                     [ONE_PAIR] = "ONE_PAIR",
                                     [HIGH_CARD] = "HIGH_CARD"};

#define HAND_SIZE 5
typedef struct {
  char cards[HAND_SIZE + 1];
  int bid;
  hand_type_t type;
} hand_t;

hand_t **hands;
int hand_count = 0;

void fileHandler(int lines) {
  printf("lines: %d\n", lines);
  hands = calloc(lines, sizeof(hand_t *));
}

int seen_sort_compare(const void *left, const void *right) {
  int left_int = *(int *)left;
  int right_int = *(int *)right;

  return left_int > right_int ? -1 : 1;
}

char *possible_cards = "AKQJT98765432";
int possible_cards_count = 13;

hand_type_t rank_hand_p1(int *cards_seen, int *times_seen) {
  if (cards_seen[0] == 5) {
    return FIVE_OF_A_KIND;
  }
  if (cards_seen[0] == 4) {
    return FOUR_OF_A_KIND;
  }
  if (cards_seen[0] == 3 && cards_seen[1] == 2) {
    return FULL_HOUSE;
  }
  if (cards_seen[0] == 3) {
    return THREE_OF_A_KIND;
  }
  if (cards_seen[0] == 2 && cards_seen[1] == 2) {
    return TWO_PAIR;
  }
  if (cards_seen[0] == 2) {
    return ONE_PAIR;
  }
  return HIGH_CARD;
}

hand_type_t rank_hand_p2(int cards_seen[], int times_seen[]) {
  int num_jokers = times_seen['J'];

  if (num_jokers == 5) {
    return FIVE_OF_A_KIND;
  } else if (num_jokers == 4) {
    // Jokers will just be whatever the leftover card would be
    return FIVE_OF_A_KIND;
  } else if (num_jokers == 3) {
    // are the last two cards the same?
    if (cards_seen[1] == 2) {
      // they are -> FIVE_OF_A_KIND
      return FIVE_OF_A_KIND;
    } else {
      // they are different -> FOUR_OF_A_KIND
      return FOUR_OF_A_KIND;
    }
  } else if (num_jokers == 2) {
    if (cards_seen[0] == 3) {
      // 3 cards same  -> FIVE_OF_A_KIND
      return FIVE_OF_A_KIND;
    } else if (cards_seen[0] == 2 && cards_seen[1] == 2 && cards_seen[2] == 1) {
      // 2 same 1 diff -> FOUR_OF_A_KIND
      return FOUR_OF_A_KIND;
    } else {
      // all diff      -> THREE OF A KIND
      return THREE_OF_A_KIND;
    }
  } else if (num_jokers == 1) {
    if (cards_seen[0] == 4) {
      // 4 cards same  -> FIVE_OF_A_KIND
      return FIVE_OF_A_KIND;
    }else if (cards_seen[0] == 3) {
      // 3 same 1 diff -> FOUR_OF_A_KIND
      return FOUR_OF_A_KIND;
    } else if (cards_seen[0] == 2 && cards_seen[1] == 2) {
      // 2 same 2 same -> FULL_HOUSE
      return FULL_HOUSE;
    } else if (cards_seen[0] == 2 && cards_seen[1] == 1 && cards_seen[2] == 1) {
      // 2 same 2 diff -> THREE_OF_A_KIND
      return THREE_OF_A_KIND;
    } else {
      // all diff      -> TWO_PAIR
      return ONE_PAIR;
    }
  }
  // num_jokers == 0 -> normal calculations

  if (cards_seen[0] == 5) {
    return FIVE_OF_A_KIND;
  }
  if (cards_seen[0] == 4) {
    return FOUR_OF_A_KIND;
  }
  if (cards_seen[0] == 3 && cards_seen[1] == 2) {
    return FULL_HOUSE;
  }
  if (cards_seen[0] == 3) {
    return THREE_OF_A_KIND;
  }
  if (cards_seen[0] == 2 && cards_seen[1] == 2) {
    return TWO_PAIR;
  }
  if (cards_seen[0] == 2) {
    return ONE_PAIR;
  }
  return HIGH_CARD;
}

hand_type_t get_hand_type(hand_t *hand,
                          hand_type_t (*rank_hand)(int[], int[])) {
  char *cards = strdup(hand->cards);
  // printf("\nhand type: %s\n", hand->cards);
  int times_seen[256] = {0};

  // I wish C had JS objects
  for (int i = 0; i < HAND_SIZE; i++) {
    char card = cards[i];
    times_seen[card]++;
  }
  // basically get the values from the array
  // which is just a list of "how many times a card was seen, but without caring
  // what the card was"
  int cards_seen[13] = {0};
  for (int i = 0; i < possible_cards_count; i++) {
    char card = possible_cards[i];
    // printf("occ: '%c' = %d\n", card, times_seen[card]);
    cards_seen[i] = times_seen[card];
  }

  qsort(cards_seen, possible_cards_count, sizeof(int), seen_sort_compare);

  // for (int i = 0; i < possible_cards_count; i++) {
  //   int times = cards_seen[i];
  //   printf("seen:  %d\n", times);
  // }
  // Now we can analyse `cards_seen` and determine the rank
  return rank_hand(cards_seen, times_seen);
}

void lineHandler(char *line, int length) {
  printf("line (%d): %s\n", length, line);
  hand_t *hand = malloc(sizeof(hand_t));

  char *cards = strtok(line, " ");
  char *bid = strtok(NULL, " ");
  strcpy(hand->cards, cards);
  hand->bid = strtol(bid, NULL, 10);
  hand->type = get_hand_type(hand, rank_hand_p1);
  hands[hand_count++] = hand;
}

void print_hands() {
  for (int i = 0; i < hand_count; i++) {
    hand_t *hand = hands[i];
    printf("cards %s, bid %d, type %s\n", hand->cards, hand->bid,
           hand_type_str[hand->type]);
  }
}

// easiest way i could think of!
int card_values[] = {['A'] = 14, ['K'] = 13, ['Q'] = 12, ['J'] = 11, ['T'] = 10,
                     ['9'] = 9,  ['8'] = 8,  ['7'] = 7,  ['6'] = 6,  ['5'] = 5,
                     ['4'] = 4,  ['3'] = 3,  ['2'] = 2};

int card_compare(char a, char b) {
  int a_value = card_values[a];
  int b_value = card_values[b];
  return a_value > b_value ? 1 : -1;
}

int hand_order_compare(hand_t *left_hand, hand_t *right_hand) {
  for (int i = 0; i < HAND_SIZE; i++) {
    char left_card = left_hand->cards[i];
    char right_card = right_hand->cards[i];
    if (left_card != right_card) {
      return card_compare(left_card, right_card);
    }
  }
  return 0;
}

int hand_sort_compare(const void *left, const void *right) {
  hand_t *left_hand = *(hand_t **)left;
  hand_t *right_hand = *(hand_t **)right;

  if (left_hand->type == right_hand->type) {
    // second ordering rules apply
    return hand_order_compare(left_hand, right_hand);
  }
  return left_hand->type > right_hand->type;
}

// annoying amount of duplication
int card_values_p2[] = {['A'] = 14, ['K'] = 13, ['Q'] = 12, ['T'] = 10,
                     ['9'] = 9,  ['8'] = 8,  ['7'] = 7,  ['6'] = 6,  ['5'] = 5,
                     ['4'] = 4,  ['3'] = 3,  ['2'] = 2, ['J'] = 1};

int card_compare_p2(char a, char b) {
  int a_value = card_values_p2[a];
  int b_value = card_values_p2[b];
  return a_value > b_value ? 1 : -1;
}

int hand_order_compare_p2(hand_t *left_hand, hand_t *right_hand) {
  for (int i = 0; i < HAND_SIZE; i++) {
    char left_card = left_hand->cards[i];
    char right_card = right_hand->cards[i];
    if (left_card != right_card) {
      return card_compare_p2(left_card, right_card);
    }
  }
  return 0;
}

int hand_sort_compare_p2(const void *left, const void *right) {
  hand_t *left_hand = *(hand_t **)left;
  hand_t *right_hand = *(hand_t **)right;

  if (left_hand->type == right_hand->type) {
    // second ordering rules apply
    return hand_order_compare_p2(left_hand, right_hand);
  }
  return left_hand->type > right_hand->type;
}

int main() {
  init();
  readInputFile(__FILE__, lineHandler, fileHandler);

  // print_hands();
  qsort(hands, hand_count, sizeof(hand_t *), hand_sort_compare);
  // print_hands();

  int winnings = 0;

  for (int i = 0; i < hand_count; i++) {
    hand_t *hand = hands[i];
    winnings += (i + 1) * hand->bid;
  }
  printf("Part one: %d\n", winnings);
#ifdef TEST_MODE
  assert(winnings == 6592);
#else
  assert(winnings == 248569531);
#endif

  for (int i = 0; i < hand_count; i++) {
    hand_t *hand = hands[i];
    hand->type = get_hand_type(hand, rank_hand_p2);
  }
  qsort(hands, hand_count, sizeof(hand_t *), hand_sort_compare_p2);

  // print_hands();
  int winnings_p2 = 0;
  for (int i = 0; i < hand_count; i++) {
    hand_t *hand = hands[i];
    winnings_p2 += (i + 1) * hand->bid;
  }
  printf("Part two: %d\n", winnings_p2);
#ifdef TEST_MODE
  assert(winnings_p2 == 6839);
#else
  assert(winnings_p2 == 250382098);
#endif
  exit(EXIT_SUCCESS);
}
