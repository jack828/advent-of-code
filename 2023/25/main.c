#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#define TEST_MODE
#include "../utils.h"

typedef struct node_t node_t;

struct node_t {
  int index;
  char *name;
  node_t **nodes;
  int node_count;

  // only used to construct graph
  char **edge_names;
  int edge_names_count;
};

typedef struct vertex_t vertex_t;
typedef struct edge_list_t edge_list_t;
typedef struct edge_t edge_t;

struct vertex_t {
  int index;
  edge_list_t *edges;
};

struct edge_t {
  vertex_t *v1;
  vertex_t *v2;
  int og_v1;
  int og_v2;
};

struct edge_list_t {
  edge_t *edge;
  edge_list_t *next;
};

typedef struct graph_t {
  vertex_t **verticies;
  int vertex_count;
  // Unique edges
  edge_list_t *edges;
  // Only used for random selection
  int edge_count;
} graph_t;

node_t **nodes;
int nodes_count = 0;

void fileHandler(int lines) {
  printf("lines: %d\n", lines);
  nodes = calloc(lines * 2, sizeof(node_t *));
}

void lineHandler(char *line, int length) {
  printf("line (%d): %s\n", length, line);
  node_t *node = malloc(sizeof(node_t));

  char *name = strtok(line, ":");
  char *edge_names = strtok(NULL, ":");
  // printf("name %s edge %s\n", name, edge_names);
  node->index = nodes_count;
  node->name = calloc(4, sizeof(char));
  strcpy(node->name, name);

  node->edge_names = calloc(8, sizeof(char *));
  node->edge_names_count = 0;
  char *edge = strtok(edge_names, " ");
  do {
    // printf("edge %s \n", edge);
    node->edge_names[node->edge_names_count] = calloc(4, sizeof(char));
    strcpy(node->edge_names[node->edge_names_count++], edge);

  } while ((edge = strtok(NULL, " ")) != NULL);

  node->nodes = calloc(64, sizeof(node_t *));
  node->node_count = 0;
  nodes[nodes_count++] = node;
}

// also creates nodes if needed
node_t *findNodeByName(char *name) {
  for (int i = 0; i < nodes_count; i++) {
    node_t *node = nodes[i];
    if (strcmp(name, node->name) == 0) {
      return node;
    }
  }

  node_t *node = malloc(sizeof(node_t));
  node->index = nodes_count;
  node->name = calloc(4, sizeof(char));
  strcpy(node->name, name);
  node->nodes = calloc(64, sizeof(node_t *));
  node->node_count = 0;
  node->edge_names = NULL;
  node->edge_names_count = 0;
  nodes[nodes_count++] = node;
  printf("%s , new\n", name);
  return node;
}

void linkNodes() {
  for (int i = 0; i < nodes_count; i++) {
    node_t *node = nodes[i];
    printf("node: %d \n", node->index);

    for (int j = 0; j < node->edge_names_count; j++) {
      char *edge = node->edge_names[j];

      node_t *found_node = findNodeByName(edge);
      node->nodes[node->node_count++] = found_node;
    }
  }
}

void printNodes(node_t **nodes_to_print, int node_qty) {
  for (int i = 0; i < node_qty; i++) {
    node_t *node = nodes_to_print[i];
    if (node == NULL) {
      printf("deleted node : %d\n", i);
      continue;
    }
    printf("node: %d ; %s ; %d edges\n", node->index, node->name,
           node->node_count);
    printf("edges: ");
    for (int j = 0; j < node->node_count; j++) {
      node_t *edge = node->nodes[j];
      if (edge->name == NULL) {
        printf("%d, ", edge->index);
      } else {
        printf("%s, ", edge->name);
      }
    }
    printf("\n");
  }
}

graph_t *cloneGraph() {
  graph_t *graph = malloc(sizeof(graph_t));

  graph->verticies = calloc(nodes_count, sizeof(node_t *));
  graph->vertex_count = nodes_count;
  graph->edges = NULL;
  graph->edge_count = 0;

  // clone nodes list initially
  for (int i = 0; i < nodes_count; i++) {
    node_t *node = nodes[i];

    vertex_t *vertex = malloc(sizeof(vertex_t));
    vertex->index = node->index;
    vertex->edges = NULL;
    graph->verticies[i] = vertex;
  }

  // then clone edge references
  for (int i = 0; i < nodes_count; i++) {
    node_t *node = nodes[i];
    vertex_t *v1 = graph->verticies[i];

    for (int j = 0; j < node->node_count; j++) {
      node_t *found_node = node->nodes[j];
      vertex_t *v2 = graph->verticies[found_node->index];

      edge_t *edge = malloc(sizeof(edge_t));
      edge->v1 = v1;
      edge->v2 = v2;
      edge->og_v1 = v1->index;
      edge->og_v2 = v2->index;

      // add to v1
      edge_list_t *edge_node_v1 = malloc(sizeof(edge_list_t));
      edge_node_v1->edge = edge;
      edge_node_v1->next = v1->edges;
      v1->edges = edge_node_v1;

      // add to v2 (bidirectional, single edge ref)
      edge_list_t *edge_node_v2 = malloc(sizeof(edge_list_t));
      edge_node_v2->edge = edge;
      edge_node_v2->next = v2->edges;
      v2->edges = edge_node_v2;

      // unique reference in graph
      edge_list_t *edge_node_g = malloc(sizeof(edge_list_t));
      edge_node_g->edge = edge;
      edge_node_g->next = graph->edges;
      graph->edges = edge_node_g;
      graph->edge_count++;
    }
  }
  return graph;
}

void printGraph(graph_t *graph) {
  printf("printing graph: %d verticies\n", graph->vertex_count);
  for (int i = 0; i < graph->vertex_count; i++) {
    vertex_t *vertex = graph->verticies[i];
    printf("vertex %d:\n", vertex->index);
    printf("edges: ");
    edge_list_t *edge_node = vertex->edges;
    while (edge_node) {
      vertex_t *neighbour = (edge_node->edge->v1 == vertex)
                                ? edge_node->edge->v2
                                : edge_node->edge->v1;
      printf(" %d", neighbour->index);
      edge_node = edge_node->next;
      if (edge_node)
        printf(",");
    }
    printf("\n");
  }

  printf("printing graph: %d edges\n", graph->edge_count);

  edge_list_t *edge_node = graph->edges;
  while (edge_node) {
    printf(" %d <-> %d ", edge_node->edge->v1->index,
           edge_node->edge->v2->index);
    edge_node = edge_node->next;
    if (edge_node)
      printf(",");
  }
  printf("\noriginally:\n");

  edge_node = graph->edges;
  while (edge_node) {
    printf(" %d <-> %d ", edge_node->edge->og_v1, edge_node->edge->og_v2);
    edge_node = edge_node->next;
    if (edge_node)
      printf(",");
  }
  printf("\n");
}

void freeNode(node_t *node) {
  if (node->name != NULL) {
    free(node->name);
  }
  if (node->nodes != NULL) {
    free(node->nodes);
  }

  if (node->edge_names != NULL) {
    for (int j = 0; j < node->edge_names_count; j++) {
      free(node->edge_names[j]);
    }
    free(node->edge_names);
  }
  free(node);
}

void freeEdges(edge_list_t *edge_list) {
  edge_list_t *current = edge_list;
  while (current) {
    edge_list_t *next = current->next;
    free(current->edge);
    free(current);
    current = next;
  }
}

void freeEdgeList(edge_list_t *edge_list) {
  edge_list_t *current = edge_list;
  while (current) {
    edge_list_t *next = current->next;
    free(current);
    current = next;
  }
}

void freeGraph(graph_t *graph) {
  for (int i = 0; i < graph->vertex_count; i++) {
    vertex_t *vertex = graph->verticies[i];
    if (vertex == NULL) {
      continue;
    }
    freeEdgeList(vertex->edges);

    free(vertex);
  }
  free(graph->verticies);
  freeEdges(graph->edges);
  free(graph);
}

graph_t *kargersAlgorithm() {
  // obtain a clone of all nodes into a graph_t object
  // while node_count != 2
  //   select a node at random
  //    - select an edge (connected node) at random
  //    - contract it
  graph_t *graph = cloneGraph();

  // printGraph(graph);
  // printf("\n--- running kargers. node_count before %d\n",
  // graph->vertex_count);
  while (graph->vertex_count > 2) {
    // printf("kargers node_count = %d, edges = %d\n", graph->vertex_count,
    // graph->edge_count);

    // Select a random edge to contract
    int edge_num = rand_range(0, graph->edge_count);
    edge_list_t *target = graph->edges;
    edge_t *target_edge = target->edge;
    while (edge_num--) {
      target = target->next;
      target_edge = target->edge;
    }

    // Contract target_edge by merging `v1` into `v2`
    vertex_t *target_v1 = target_edge->v1;
    vertex_t *target_v2 = target_edge->v2;

    edge_list_t *prev = NULL;
    edge_list_t *next = graph->edges;
    while (next) {
      edge_t *edge = next->edge;

      // Redirect any reference to `target_v1` to `target_v2`
      if (edge->v1 == target_v1) {
        edge->v1 = target_v2;
      }
      if (edge->v2 == target_v1) {
        edge->v2 = target_v2;
      }

      // Remove self-loops
      if (edge->v1 == edge->v2) {
        if (prev == NULL) {
          graph->edges = next->next; // Move head pointer if at the start
        } else {
          prev->next = next->next;
        }
        edge_list_t *tmp = next;
        next = next->next;

        // Free the self-loop edge
        // free(tmp->edge);
        free(tmp);
        graph->edge_count--;
        continue; // Skip updating prev since the node is removed
      }

      // Move to next edge in the list
      prev = next;
      next = next->next;
    }

    // Decrement the vertex count to reflect the contraction
    graph->vertex_count--;
  }
  //
  printf("exit loop, edges: %d\n", graph->edge_count);
  // printGraph(graph);

  if (graph->edge_count == 3) {
    printGraph(graph);
    return graph;
  }

  freeGraph(graph);
  return NULL;
}

int main() {
  init();
  srand(time(NULL));
  readInputFile(__FILE__, lineHandler, fileHandler);

  // printNodes();
  printf("\n---\n");
  linkNodes();
  printf("\n---\n");
  printNodes(nodes, nodes_count);
  printf("\n---\n");

  // do {
  // } while (contracted_to != 3)

  graph_t *min_cut_graph = NULL;
  int runs = 1;

  while ((min_cut_graph = kargersAlgorithm()) == NULL) {
    runs++;
  }

  // now we can iterate graph->edges and find the original verticies for the
  // remaining edges
  // then, for each remaining edge
  //   remove it from graph->edges
  //   remove it from vertex edges
  //
  // then, explore all edges (DFS?) from vertex[0] and count the number of seen verticies
  // then, (node_count - seen_verticies) * node_count == part_one
  printf("runs: %d\n", runs);
  freeGraph(min_cut_graph);
  printf("Part one: %d\n", 69);
#ifdef TEST_MODE
  // assert(69 == 420);
#else
  assert(69 == 420);
#endif

  printf("Part two: %d\n", 420);
#ifdef TEST_MODE
  // assert(420 == 69);
#else
  assert(420 == 69);
#endif

  for (int i = 0; i < nodes_count; i++) {
    node_t *node = nodes[i];
    if (node == NULL) {
      continue;
    }
    freeNode(node);
  }
  free(nodes);
  exit(EXIT_SUCCESS);
}
