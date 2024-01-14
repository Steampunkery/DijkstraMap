#include "dijkstra.h"

#include <glib.h>
#include <stdlib.h>
#include <float.h>

typedef struct dm_node_t {
    size_t idx;
    float distance;
} dm_node_t;

static DMError do_dijkstra_map(DijkstraMap *dm, size_t *sources, uint32_t n_sources);

DMError build_dijkstra_map(DijkstraMap *dm, size_t w, size_t h, size_t *sources, uint32_t n_sources) {
    if (!dm || !sources) return DM_INAVLID_PTR;

    dm->map = malloc(w * h * sizeof(*dm->map));
    if (!dm->map) return DM_NO_MEM;

    for (size_t i = 0; i < w * h; i++)
        dm->map[i] = FLT_MAX;

    for (uint32_t i = 0; i < n_sources; i++)
        dm->map[sources[i]] = 0;

    DMError ret = do_dijkstra_map(dm, sources, n_sources);
    if (ret != DM_NO_ERR) return ret;

    return DM_NO_ERR;
}

void set_successor_fn(DijkstraMap *dm, successor_fn s, const void *state) {
    dm->successors = s;
    dm->successor_state = state;
}

DMError destroy_dijkstra_map(DijkstraMap *dm) {
    free(dm->map);
    return DM_NO_ERR;
}

static dm_node_t *make_node(size_t idx, float distance) {
    dm_node_t *node = malloc(sizeof(*node));
    *node = (dm_node_t) { idx, distance };
    return node;
}

static DMError do_dijkstra_map(DijkstraMap *dm, size_t *sources, uint32_t n_sources) {
    GQueue frontier = G_QUEUE_INIT;

    for (uint32_t i = 0; i < n_sources; i++)
        g_queue_push_tail(&frontier, make_node(sources[i], 0));

    dm_node_t *node;
    while ((node = g_queue_pop_head(&frontier))) {
        successor_t *exits;
        uint8_t n_exits;
        dm->successors(node->idx, dm->successor_state, &exits, &n_exits);

        for (uint8_t i = 0; i < n_exits; i++) {
            float new_dist = node->distance + exits[i].cost;
            float old_dist = dm->map[exits[i].idx];

            if (new_dist >= old_dist) continue;
            dm->map[exits[i].idx] = new_dist;
            g_queue_push_tail(&frontier, make_node(exits[i].idx, new_dist));
        }

        free(node);
    }

    g_queue_clear_full(&frontier, free);
    return DM_NO_ERR;
}
