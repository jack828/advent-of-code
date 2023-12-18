#ifndef __HASHMAP_H_
#define __HASHMAP_H_

#include <stdlib.h>
#include <string.h>

#ifndef HASHMAP_SIZE
#define HASHMAP_SIZE (2 << 15)
#endif

// structure for the linked list in each bucket
typedef struct hashmap_node_t hashmap_node_t;

struct hashmap_node_t {
  unsigned long key; // a hash
  void *data;        // store whatever you like
  hashmap_node_t *next;
};

typedef struct {
  hashmap_node_t *head;
} bucket_t;

typedef struct {
  bucket_t buckets[HASHMAP_SIZE];
} hashmap_t;

// djb2 hash implementation for strings of n length
unsigned long djb2_hash_str(char *str, int length) {
  unsigned long hash = 5381;

  for (int i = 0; i <= length; i++) {
    char c = str[i];
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
  }

  return hash % HASHMAP_SIZE;
}

// Create a new hashmap with HASHMAP_SIZE buckets
hashmap_t *hm_create() {
  hashmap_t *hashmap = malloc(sizeof(hashmap_t));
  memset(hashmap->buckets, 0, sizeof(hashmap->buckets));
  return hashmap;
}

// Destroy a hashmap
void hm_destroy(hashmap_t *hashmap) {
  // Free memory for each linked list in the buckets
  for (int i = 0; i < HASHMAP_SIZE; i++) {
    hashmap_node_t *current = hashmap->buckets[i].head;
    while (current != NULL) {
      hashmap_node_t *next = current->next;
      free(current);
      current = next;
    }
  }

  // Free memory for the hashmap structure
  free(hashmap);
}

/* Set a bucket value to a data value
 * You need to hash the key yourself
 */
void hm_set(hashmap_t *hashmap, unsigned long hash, void *data) {
  hashmap_node_t *newNode = malloc(sizeof(hashmap_node_t));
  newNode->key = hash;
  newNode->data = data;
  newNode->next = NULL;

  // Insert at the beginning of the linked list (simplest collision resolution)
  newNode->next = hashmap->buckets[hash].head;
  hashmap->buckets[hash].head = newNode;
}

/* Get a bucket value from a hash
 * You need to hash the key yourself
 */
void *hm_get(hashmap_t *hashmap, unsigned long hash) {
  hashmap_node_t *current = hashmap->buckets[hash].head;
  while (current != NULL) {
    if (current->key == hash) {
      return current->data;
    }
    current = current->next;
  }

  // not found
  return NULL;
}

#endif
