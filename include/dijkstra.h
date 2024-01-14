#pragma once
#include <stddef.h>
#include <stdint.h>

typedef struct successor_t {
    size_t idx;
    float cost;
} successor_t;

typedef void (*successor_fn)(size_t, const void *, successor_t **, uint8_t *);

typedef struct DijkstraMap {
    float *map;
    size_t w;
    size_t h;

    const void *successor_state;
    successor_fn successors;
} DijkstraMap;

typedef enum { DM_NO_ERR, DM_INAVLID_PTR, DM_NO_MEM } DMError;

DMError build_dijkstra_map(DijkstraMap *dm, size_t w, size_t h, size_t *sources, uint32_t n_sources);
DMError destroy_dijkstra_map(DijkstraMap *dm);
void set_successor_fn(DijkstraMap *dm, successor_fn s, const void *state);
