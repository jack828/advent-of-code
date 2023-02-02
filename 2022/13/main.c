#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
// #define TEST_MODE
#include "../utils.h"

typedef enum type_t { TYPE_LIST, TYPE_NUMBER } type_t;

// basically a trinary lmao
typedef enum compare_result_t {
  RESULT_NULL,
  RESULT_TRUE,
  RESULT_FALSE
} compare_result_t;

typedef struct list_t {
  struct item_t **items;
  int length;
  bool isDivider;
  // printing only
  struct list_t *parent;
} list_t;

typedef struct item_t {
  type_t type;
  list_t *list;
  int value;
} item_t;

void list_add(list_t *list, item_t *item) {
  list->items[list->length++] = item;
  return;
  list->length++;
  if (list->items == NULL) {
    list->items = malloc(list->length * sizeof(item_t *));
  } else {
    list->items = realloc(list->items, list->length * sizeof(item_t *));
  }
}

list_t **lists;
int listCount = 0;

void fileHandler(int lines) {
  fprintf(stdout, "lines: %d\n", lines);
  lists = malloc(lines * sizeof(list_t *));
}

void lineHandler(char *line) {
  fprintf(stdout, "line: %s\n", line);

  // ignore empty lines
  if (line[0] == '\n') {
    return;
  }

  list_t *list = malloc(sizeof(list_t));
  list->isDivider = false;
  // TODO see how it compares with realloc
  list->items = malloc(200 * sizeof(item_t *));
  list->length = 0;

  list_t *topLevelList = list;

  int length = strlen(line);
  // first char is always beginning the list
  // last char is always end of the list
  char member[5] = {0};
  int memberLength = 0;
  for (int i = 1; i < length; i++) {
    char c = line[i];

    if (c == '[') {
      // go down level
      item_t *item = malloc(sizeof(item_t));
      item->type = TYPE_LIST;
      item->list = malloc(sizeof(list_t));
      item->list->isDivider = false;
      item->list->items = malloc(200 * sizeof(item_t *));
      item->list->length = 0;
      list_add(list, item);
      item->list->parent = list;
      list = item->list;
    } else if (c == ']') {
      if (memberLength > 0) {
        item_t *item = malloc(sizeof(item_t));
        item->type = TYPE_NUMBER;
        item->value = atoi(member);
        list_add(list, item);
        memset(member, 0, 5);
        memberLength = 0;
      }
      // go up level
      list = list->parent;
    } else if (c == ',') {
      // list member
      // convert `member`, add to list, and reset
      if (memberLength > 0) {
        item_t *item = malloc(sizeof(item_t));
        item->type = TYPE_NUMBER;
        item->value = atoi(member);
        list_add(list, item);
        memset(member, 0, 5);
        memberLength = 0;
      }
    } else {
      // is digit, add to `member`
      member[memberLength++] = c;
    }
  }

  lists[listCount++] = topLevelList;
}

void printItem(item_t *item);
void printList(list_t *list);

void printList(list_t *list) {
  // fprintf(stdout, "list: %d,\n", list->length);
  fprintf(stdout, "[");
  for (int j = 0; j < list->length; j++) {
    item_t *item = list->items[j];
    printItem(item);
    if (j != list->length - 1) {
      fprintf(stdout, ",");
    }
  }
  fprintf(stdout, "]");
}

void printItem(item_t *item) {
  // fprintf(stdout, "printitem");
  if (item == NULL) {
    fprintf(stdout, "NULL\n");
    return;
  }
  if (item->type == TYPE_LIST) {
    printList(item->list);
  } else if (item->type == TYPE_NUMBER) {
    fprintf(stdout, "%d", item->value);
  }
}

void printLists() {
  for (int i = 0; i < listCount; i++) {
    list_t *list = lists[i];
    fprintf(stdout, "%d:\n", i);
    printList(list);
    fprintf(stdout, "\n");
  }
}

compare_result_t compare(item_t *left, item_t *right) {
  if (left == NULL) {
    fprintf(stdout, "l NULL!!\n");
    return RESULT_NULL;
  }
  if (right == NULL) {
    fprintf(stdout, "r NULL!!\n");
    return RESULT_NULL;
  }
  // If both values are integers,
  if (left->type == TYPE_NUMBER && right->type == TYPE_NUMBER) {
    // the lower integer should come first.
    // If the left integer is lower than the right integer,
    // the inputs are in the right order.
    if (left->value < right->value) {
      return RESULT_TRUE;
    }
    // If the left integer is higher than the right integer,
    // the inputs are not in the right order.
    if (left->value > right->value) {
      return RESULT_FALSE;
    }
    // Otherwise, the inputs are the same integer;
    // continue checking the next part of the input.
    return RESULT_NULL; // continue
  }

  // If both values are lists,
  if (left->type == TYPE_LIST && right->type == TYPE_LIST) {
    // compare the first value of each list, then the second value, and so on.
    // If the right list runs out of items first, the inputs are not in the
    // right order. If the lists are the same length and no comparison makes a
    // decision about the order, continue checking the next part of the input.

    list_t *leftList = left->list;
    list_t *rightList = right->list;
    int maxLength = max(leftList->length, rightList->length);
    for (int i = 0; i < maxLength; i++) {
      // If the left list runs out of items first, the inputs are in the right
      // order.
      if (i > leftList->length - 1) {
        return RESULT_TRUE;
      }
      // vice versa, out of order
      if (i > rightList->length - 1) {
        return RESULT_FALSE;
      }
      // if both have elements still, then compare each item

      item_t *leftItem = leftList->items[i];
      item_t *rightItem = rightList->items[i];
      compare_result_t result = compare(leftItem, rightItem);
      if (result != RESULT_NULL) {
        return result;
      }
    }
    return RESULT_NULL;
  }

  // If exactly one value is an integer,
  // convert the integer to a list which contains that integer as its only
  // value, then retry the comparison. For example, if comparing [0,0,0] and 2,
  // convert the right value to [2] (a list containing 2); the result is then
  // found by instead comparing [0,0,0] and [2].
  if (left->type == TYPE_LIST && right->type == TYPE_NUMBER) {
    item_t *rightItemList = malloc(sizeof(item_t));
    rightItemList->type = TYPE_LIST;
    rightItemList->list = malloc(sizeof(list_t));
    rightItemList->list->isDivider = false;
    rightItemList->list->items = malloc(200 * sizeof(item_t *));
    rightItemList->list->items[0] = right;
    rightItemList->list->length = 1;
    return compare(left, rightItemList);
  }
  if (left->type == TYPE_NUMBER && right->type == TYPE_LIST) {
    item_t *leftItemList = malloc(sizeof(item_t));
    leftItemList->type = TYPE_LIST;
    leftItemList->list = malloc(sizeof(list_t));
    leftItemList->list->isDivider = false;
    leftItemList->list->items = malloc(200 * sizeof(item_t *));
    leftItemList->list->items[0] = left;
    leftItemList->list->length = 1;
    return compare(leftItemList, right);
  }
  // should not get here
  return RESULT_NULL;
}

int qsortcompare(const void *left, const void *right) {
  list_t *leftList = *(list_t **)left;
  list_t *rightList = *(list_t **)right;

  item_t *leftItem = malloc(sizeof(item_t));
  leftItem->type = TYPE_LIST;
  leftItem->list = leftList;
  item_t *rightItem = malloc(sizeof(item_t));
  rightItem->type = TYPE_LIST;
  rightItem->list = rightList;

  if (compare(leftItem, rightItem) == RESULT_TRUE)
    return -1;
  else
    return 1;
}

int main() {
  init();
  readInputFile(__FILE__, lineHandler, fileHandler);

  // printLists();

  int rightOrderIndexesSum = 0;
  for (int i = 0; i < listCount; i += 2) {
    list_t *leftList = lists[i];
    list_t *rightList = lists[i + 1];
    // convert into items for simpler compare function
    item_t *leftItem = malloc(sizeof(item_t));
    leftItem->type = TYPE_LIST;
    leftItem->list = leftList;
    item_t *rightItem = malloc(sizeof(item_t));
    rightItem->type = TYPE_LIST;
    rightItem->list = rightList;

    if (compare(leftItem, rightItem) == RESULT_TRUE) {
      rightOrderIndexesSum += (i / 2) + 1;
    }
  }

  fprintf(stdout, "Part one: %d\n", rightOrderIndexesSum);
#ifdef TEST_MODE
  assert(rightOrderIndexesSum == 13);
#else
  assert(rightOrderIndexesSum == 5529);
#endif

  // parse and add the divider packets
  lineHandler("[[2]]");
  lineHandler("[[6]]");

  lists[listCount - 1]->isDivider = true;
  lists[listCount - 2]->isDivider = true;
  // sort the array using the comparison function
  qsort(lists, listCount, sizeof(list_t *), qsortcompare);
  int dividerIndexProduct = 1;
  for (int i = 0; i < listCount; i++) {
    list_t *list = lists[i];
    if (list->isDivider) {
      dividerIndexProduct *= i + 1;
    }
  }

  fprintf(stdout, "Part two: %d\n", dividerIndexProduct);
#ifdef TEST_MODE
  assert(dividerIndexProduct == 140);
#else
  assert(dividerIndexProduct == 27690);
#endif
  exit(EXIT_SUCCESS);
}
