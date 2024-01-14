#include "dijkstra.h"

#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

// Hand-picked constants to make to colors look nice
#define COLOR_DIV 12
#define COLOR_CAP 230

int MAP_WIDTH, MAP_HEIGHT;

int myround(float x) {
    if (x > MAP_WIDTH) x = MAP_WIDTH;
    return (int)(x < 0 ? (x - 0.49999997f) : (x + 0.49999997f));
}

void successors8(size_t idx, const void *, successor_t **exits, uint8_t *n_exits) {
    static successor_t all[8];
    int x_dir[] = { 1, 0, -1, 0, 1, -1, -1, 1 };
    int y_dir[] = { 0, 1, 0, -1, 1, 1, -1, -1 };

    *n_exits = 0;
    for (int i = 0; i < 8; i++) {
        int x = idx % MAP_WIDTH + x_dir[i];
        int y = idx / MAP_WIDTH + y_dir[i];
        if (x < 0 || x > MAP_WIDTH-1 || y < 0 || y > MAP_HEIGHT-1)
            continue;

        all[*n_exits].idx = (int)idx + x_dir[i] + y_dir[i] * MAP_WIDTH;
        all[*n_exits].cost = i > 3 ? 1.4f : 1.0f;
        (*n_exits)++;
    }

    *exits = all;
}

int get_green(int i) {
    if (!i) return 0;

    if (i > COLOR_DIV) {
        return COLOR_CAP;
    } else {
        int color = (COLOR_CAP/COLOR_DIV)*i;
        return color > COLOR_CAP ? COLOR_CAP : color;
    }
}

int get_red(int i) {
    if (!i) return 255;

    if (i > COLOR_DIV) {
        int color = COLOR_CAP-(COLOR_CAP/COLOR_DIV)*(i-COLOR_DIV);
        return color < 0 ? 0 : color;
    } else {
        return COLOR_CAP;
    }
}

int main(void) {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    int cap = 1<<28;
    arena scratch = {0};
    scratch.beg = malloc(cap);
    scratch.end = scratch.beg + cap;

    MAP_WIDTH = w.ws_col;
    MAP_HEIGHT = w.ws_row - 2;

    uint32_t n_sources = 10;
    size_t sources[10];

    srand(time(NULL));
    for (int i = 0; i < n_sources; i++)
        sources[i] = random() % (MAP_WIDTH * MAP_HEIGHT);

    DijkstraMap dm;
    set_successor_fn(&dm, successors8, NULL);

    struct timespec begin, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &begin);

    build_dijkstra_map(&dm, MAP_WIDTH, MAP_HEIGHT, sources, n_sources, &scratch);

    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    double time_spent = (end.tv_nsec - begin.tv_nsec) / 1000000000.0 +
        (end.tv_sec  - begin.tv_sec);

    printf("Time to compute map: %lf\n", time_spent);

    for (int i = 0; i < MAP_WIDTH * MAP_HEIGHT; i++) {
        int rounded = myround(dm.map[i]);

        printf("\033[48;2;%d;%d;0m \e[0m", get_red(rounded), get_green(rounded));
        if (i % MAP_WIDTH == MAP_WIDTH - 1) printf("\n");
    }

    return 0;
}
