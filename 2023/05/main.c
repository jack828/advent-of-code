#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
// #define TEST_MODE
#include "../utils.h"

typedef enum parse_mode_t {
  SEEDS,
  SEED_TO_SOIL,
  SOIL_TO_FERT,
  FERT_TO_WATER,
  WATER_TO_LIGHT,
  LIGHT_TO_TEMP,
  TEMP_TO_HUMIDITY,
  HUMIDITY_TO_LOC
} parse_mode_t;

parse_mode_t parse_mode = SEEDS;

u_int64_t *seeds;
int seed_count = 0;

typedef struct map_t {
  u_int64_t dst_start;
  u_int64_t src_start;
  u_int64_t range;
} map_t;

map_t **seed_to_soil_maps;
int s_s_map_count = 0;
map_t **soil_to_fert_maps;
int s_f_map_count = 0;
map_t **fert_to_water_maps;
int f_w_map_count = 0;
map_t **water_to_light_maps;
int w_l_map_count = 0;
map_t **light_to_temp_maps;
int l_t_map_count = 0;
map_t **temp_to_humidity_maps;
int t_h_map_count = 0;
map_t **humidity_to_loc_maps;
int h_l_map_count = 0;

void fileHandler(int lines) { printf("lines: %d\n", lines); }

void lineHandler(char *line, int length) {
  printf("line (%d): %s\n", length, line);
  if (length == 1) {
    return;
  }
  if (line[length - 2] == ':') {
    // parse mode change!
    // printf("parse change\n");
    switch (line[0]) {
    case 's':
      parse_mode = parse_mode == SEED_TO_SOIL ? SOIL_TO_FERT : SEED_TO_SOIL;
      break;
    case 'f':
      parse_mode = FERT_TO_WATER;
      break;
    case 'w':
      parse_mode = WATER_TO_LIGHT;
      break;
    case 'l':
      parse_mode = LIGHT_TO_TEMP;
      break;
    case 't':
      parse_mode = TEMP_TO_HUMIDITY;
      break;
    case 'h':
      parse_mode = HUMIDITY_TO_LOC;
      break;
    }
    return;
  }

  map_t ***map_ptr;
  int *map_counter;
  switch (parse_mode) {
  case SEEDS: {
    char *ptr = strchr(line, ' ');
    while (ptr != NULL) {
      seed_count++;
      ptr = strchr(ptr + 1, ' ');
    }
    // printf("%d seeds\n", seed_count);
    seeds = calloc(seed_count, sizeof(u_int64_t));

    char *ptr2;
    char *seed_token = strtok_r(line, ":", &ptr2);
    int seed_index = 0;
    while ((seed_token = strtok_r(NULL, " ", &ptr2)) != NULL) {
      seeds[seed_index++] = strtoul(seed_token, NULL, 10);
      // printf("seed: %lu\n", strtoul(seed_token, NULL, 10));
    }
    return;
  }
  case SEED_TO_SOIL: {
    // printf("SEED_TO_SOIL\n");
    map_ptr = &seed_to_soil_maps;
    map_counter = &s_s_map_count;
    break;
  }
  case SOIL_TO_FERT: {
    // printf("SOIL_TO_FERT\n");
    map_ptr = &soil_to_fert_maps;
    map_counter = &s_f_map_count;
    break;
  }
  case FERT_TO_WATER: {
    // printf("FERT_TO_WATER\n");
    map_ptr = &fert_to_water_maps;
    map_counter = &f_w_map_count;
    break;
  }
  case WATER_TO_LIGHT: {
    // printf("WATER_TO_LIGHT\n");
    map_ptr = &water_to_light_maps;
    map_counter = &w_l_map_count;
    break;
  }
  case LIGHT_TO_TEMP: {
    // printf("LIGHT_TO_TEMP\n");
    map_ptr = &light_to_temp_maps;
    map_counter = &l_t_map_count;
    break;
  }
  case TEMP_TO_HUMIDITY: {
    // printf("TEMP_TO_HUMIDITY\n");
    map_ptr = &temp_to_humidity_maps;
    map_counter = &t_h_map_count;
    break;
  }
  case HUMIDITY_TO_LOC: {
    // printf("HUMIDITY_TO_LOC\n");
    map_ptr = &humidity_to_loc_maps;
    map_counter = &h_l_map_count;
    break;
  }
  }

  // I'm not quite sure this is the correct usage of pointers!
  if ((*map_ptr) == NULL) {
    (*map_ptr) = calloc(0, sizeof(map_t *));
  }
  (*map_counter)++;

  (*map_ptr) = realloc((*map_ptr), (*map_counter) * (sizeof(map_t *)));

  map_t *map = calloc(1, sizeof(map_t));
  sscanf(line, "%lu %lu %lu", &map->dst_start, &map->src_start, &map->range);
  // printf("sscanf %lu %lu %lu\n", map->dst_start, map->src_start, map->range);
  (*map_ptr)[(*map_counter) - 1] = map;
}

u_int64_t map_num(u_int64_t x, u_int64_t in_min, u_int64_t in_max,
                  u_int64_t out_min, u_int64_t out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

u_int64_t findMap(u_int64_t num, map_t **maps, int map_count) {
  for (int i = 0; i < map_count; i++) {
    map_t *map = maps[i];
    // printf("ss map %d, dst %d src %d range %d\n", i, map->dst_start,
    // map->src_start, map->range);
    bool is_mapped =
        num >= map->src_start && num <= map->src_start + map->range;
    // printf("is mapped: %c\n", is_mapped ? 'Y' : 'N');
    if (is_mapped) {
      return map_num(num, map->src_start, map->src_start + map->range,
                     map->dst_start, map->dst_start + map->range);
    }
  }
  return num;
}

u_int64_t min_lu(u_int64_t a, u_int64_t b) { return a < b ? a : b; }

u_int64_t get_location(u_int64_t seed) {
  u_int64_t s_s_mapped = findMap(seed, seed_to_soil_maps, s_s_map_count);
  // printf("->     soil: %lu\n", s_s_mapped);
  u_int64_t s_f_mapped = findMap(s_s_mapped, soil_to_fert_maps, s_f_map_count);
  // printf("->     fert: %lu\n", s_f_mapped);
  u_int64_t f_w_mapped = findMap(s_f_mapped, fert_to_water_maps, f_w_map_count);
  // printf("->    water: %lu\n", f_w_mapped);
  u_int64_t w_l_mapped =
      findMap(f_w_mapped, water_to_light_maps, w_l_map_count);
  // printf("->    light: %lu\n", w_l_mapped);
  u_int64_t l_t_mapped = findMap(w_l_mapped, light_to_temp_maps, l_t_map_count);
  // printf("->     temp: %lu\n", l_t_mapped);
  u_int64_t t_h_mapped =
      findMap(l_t_mapped, temp_to_humidity_maps, t_h_map_count);
  // printf("-> humidity: %lu\n", t_h_mapped);
  u_int64_t h_l_mapped =
      findMap(t_h_mapped, humidity_to_loc_maps, h_l_map_count);
  // printf("->      loc: %lu\n", h_l_mapped);
  return h_l_mapped;
}

int main() {
  init();
  readInputFile(__FILE__, lineHandler, fileHandler);

  u_int64_t lowest_location_p1 = UINT64_MAX;
  for (int i = 0; i < seed_count; i++) {
    u_int64_t seed = seeds[i];
    // printf("Seed: %lu\n", seed);

    u_int64_t location = get_location(seed);
    lowest_location_p1 = min_lu(lowest_location_p1, location);
  }

  printf("Part one: %lu\n", lowest_location_p1);
#ifdef TEST_MODE
  assert(lowest_location_p1 == 35);
#else
  assert(lowest_location_p1 == 57075758);
#endif

  u_int64_t lowest_location_p2 = UINT64_MAX;

  // it's only ~2bn calculations how long can it take?
  for (int i = 0; i < seed_count; i += 2) {
    u_int64_t seed_start = seeds[i];
    u_int64_t seed_range = seeds[i + 1];
    u_int64_t seed_end = seed_start + seed_range;
    printf("Seed: %lu -> %lu\n", seed_start, seed_end);

    for (u_int64_t seed = seed_start; seed < seed_end; seed++) {
      u_int64_t location = get_location(seed);
      lowest_location_p2 = min_lu(lowest_location_p2, location);
    }
  }

  printf("Part two: %lu\n", lowest_location_p2);
#ifdef TEST_MODE
  assert(lowest_location_p2 == 46);
#else
  assert(lowest_location_p2 == 31161857);
#endif
  exit(EXIT_SUCCESS);
}
