#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
// #define TEST_MODE
#include "../../utils.h"

#define DECRYPTION_KEY 811589153

typedef struct list_t {
  int64_t value;
  struct list_t *next;
  struct list_t *prev;
  // to preserve iteration order
  struct list_t *originalNext;
} list_t;

// part one
list_t *firstItem;
list_t *originalList;

// part two
list_t *decryptedList;
list_t *firstDecryptedItem;

int itemCount = 0;

void lineHandler(char *line) {
  fprintf(stdout, "line: %s\n", line);

  list_t *item = malloc(sizeof(list_t));
  item->value = atoi(line);
  list_t *decryptedItem = malloc(sizeof(list_t));
  decryptedItem->value = item->value * DECRYPTION_KEY;
  itemCount++;

  if (originalList == NULL) {
    originalList = item;
    firstItem = item;

    decryptedList = decryptedItem;
    firstDecryptedItem = decryptedItem;
  } else {
    originalList->next = item;
    originalList->originalNext = item;
    item->prev = originalList;
    originalList = item;

    decryptedList->next = decryptedItem;
    decryptedList->originalNext = decryptedItem;
    decryptedItem->prev = decryptedList;
    decryptedList = decryptedItem;
  }
}

void printList(list_t *list, int dir) {
  list_t *item = list;
  for (int i = 0; i < itemCount; i++) {
    fprintf(stdout, "%ld, ", item->value);
    if (dir == 1) {
      item = item->next;
    } else {
      item = item->prev;
    }
  }
  fputs("\n", stdout);
}

void moveItem(list_t *item) {
  int64_t moves = item->value;
  if (abs(moves) > itemCount) {
    // we could definitely optimise this by pre-calculating
    moves = moves % (itemCount - 1);
  }
  // zero moves does not affect the list
  if (moves == 0) {
    return;
  }
  // remove from the list
  list_t *next = item->next;
  list_t *prev = item->prev;

  // leave item->next and ->prev pointing to original items
  // but re-attach next/prev together
  next->prev = prev;
  prev->next = next;

  // move <moves> direction using item's existing pointers
  list_t *newNext;
  list_t *newPrev;

  if (moves > 0) {
    // forwards
    newNext = item->next;
    do {
      newNext = newNext->next;
    } while (--moves);
    newPrev = newNext->prev;
  } else {
    // backwards
    newPrev = item->prev;
    do {
      newPrev = newPrev->prev;
    } while (++moves != 0);
    newNext = newPrev->next;
  }

  // insert into list at that point
  newPrev->next = item;
  item->prev = newPrev;
  newNext->prev = item;
  item->next = newNext;
}

int64_t getGroveCoords(list_t *list) {
  int64_t groveCoords = 0;

  list_t *zeroItem = list;
  do {
    zeroItem = zeroItem->next;
  } while (zeroItem->value != 0);

  list_t *item = zeroItem;
  for (int i = 1; i <= 3000; i++) {
    // advance
    item = item->next;

    // then check what index we are at
    if (i % 1000 == 0) {
      groveCoords += item->value;
    }
  }
  return groveCoords;
}

void mix(list_t *list) {
  list_t *item = list;
  for (int i = 0; i < itemCount; i++) {
    moveItem(item);
    item = item->originalNext;
  }
}

int main() {
  readInput(__FILE__, lineHandler);

  // circulate them listy bois
  originalList->next = firstItem;
  originalList->originalNext = firstItem;
  firstItem->prev = originalList;
  originalList = firstItem;

  decryptedList->next = firstDecryptedItem;
  decryptedList->originalNext = firstDecryptedItem;
  firstDecryptedItem->prev = decryptedList;
  decryptedList = firstDecryptedItem;

  // printList(originalList, 1);
  mix(originalList);
  // printList(originalList, 1);

  int64_t partOne = getGroveCoords(originalList);

  fprintf(stdout, "Part one: %ld\n", partOne);
#ifdef TEST_MODE
  assert(partOne == 3);
#else
  assert(partOne == 14888);
#endif

  // printList(decryptedList, 1);
  for (int i = 0; i < 10; i++) {
    fprintf(stdout, "mix %d\n", i);
    mix(decryptedList);
  }

  int64_t partTwo = getGroveCoords(decryptedList);
  fprintf(stdout, "Part two: %ld\n", partTwo);
#ifdef TEST_MODE
  assert(partTwo == 1623178306l);
#else
  assert(partTwo == 3760092545849l);
#endif
}
