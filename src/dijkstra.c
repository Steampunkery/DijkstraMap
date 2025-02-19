#include "dijkstra.h"

#include <assert.h>
#include <string.h>
#include <float.h>

#define new(a, t, n)  (t *)alloc(a, sizeof(t), n)

static void *alloc(DMArena *a, ptrdiff_t size, ptrdiff_t count)
{
    ptrdiff_t alignment = -(uintptr_t)a->beg & (sizeof(void *) - 1);
    ptrdiff_t available = a->end - a->beg - alignment;
    if (count > available/size) {
        assert(0);  // out of memory
    }
    char *r = a->beg + alignment;
    a->beg += alignment + size*count;
    return memset(r, 0, size*count);
}

typedef struct node node;
struct node {
    node  *next;
    size_t idx;
};

typedef struct {
    node  *free;
    node  *head;
    node **tail;
} queue;

static queue *newqueue(DMArena *perm)
{
    queue *q = new(perm, queue, 1);
    q->tail = &q->head;
    return q;
}

static void push(queue *q, size_t idx, DMArena *perm)
{
    node *n = q->free;
    if (n) {
        q->free = n->next;
    } else {
        n = new(perm, node, 1);
    }
    n->idx = idx;
    *q->tail = n;
    q->tail = &n->next;
}

static size_t pop(queue *q)
{
    assert(q->head);
    node *n = q->head;
    q->head = n->next;
    if (!n->next) {
        q->tail = &q->head;
    }
    n->next = q->free;
    q->free = n;
    return n->idx;
}

static DMError do_dijkstra_map(DijkstraMap *dm, size_t *sources, uint32_t n_sources, DMArena);

DMError init_dijkstra_map(DijkstraMap *dm, size_t w, size_t h, successor_fn s, const void *state, DMArena *perm) {
    if (!dm) return DM_INAVLID_PTR;

    assert(!w || h <= SIZE_MAX/w);
    dm->map = new(perm, float, (size_t)w*h);
    dm->w = w;
    dm->h = h;

    dm->successors = s;
    dm->successor_state = state;

    return DM_NO_ERR;
}

DMError build_dijkstra_map(DijkstraMap *dm, size_t *sources, uint32_t n_sources, DMArena *perm) {
    if (!sources) return DM_INAVLID_PTR;

    for (size_t i = 0; i < dm->w * dm->h; i++)
        dm->map[i] = FLT_MAX;

    for (uint32_t i = 0; i < n_sources; i++)
        dm->map[sources[i]] = 0;

    DMError ret = do_dijkstra_map(dm, sources, n_sources, *perm);

    return ret;
}

static DMError do_dijkstra_map(DijkstraMap *dm, size_t *sources, uint32_t n_sources, DMArena scratch) {
    queue *frontier = newqueue(&scratch);

    for (uint32_t i = 0; i < n_sources; i++)
        push(frontier, sources[i], &scratch);

    while (frontier->head) {
        size_t idx = pop(frontier);
        successor_t *exits;
        uint8_t n_exits;
        dm->successors(idx, dm->successor_state, &exits, &n_exits);

        for (uint8_t i = 0; i < n_exits; i++) {
            float new_dist = dm->map[idx] + exits[i].cost;
            float old_dist = dm->map[exits[i].idx];

            if (new_dist >= old_dist) continue;
            dm->map[exits[i].idx] = new_dist;
            push(frontier, exits[i].idx, &scratch);
        }
    }

    return DM_NO_ERR;
}
