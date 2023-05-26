#include "grid.h"
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>

const int grid_width = 30;
const int grid_height = 30;
static pthread_t *threads = NULL;

void freeThreadArray() {
    if (threads != NULL) {
        free(threads);
        threads = NULL;
    }
}

char *create_grid() {
    return malloc(sizeof(char) * grid_width * grid_height);
}

void destroy_grid(char *grid) {
    free(grid);
    freeThreadArray();
}


void draw_grid(char *grid) {
    for (int i = 0; i < grid_height; ++i) {
        // Two characters for more uniform spaces (vertical vs horizontal)
        for (int j = 0; j < grid_width; ++j) {
            if (grid[i * grid_width + j]) {
                mvprintw(i, j * 2, "■");
                mvprintw(i, j * 2 + 1, " ");
            } else {
                mvprintw(i, j * 2, " ");
                mvprintw(i, j * 2 + 1, " ");
            }
        }
    }

    refresh();
}

void init_grid(char *grid) {
    for (int i = 0; i < grid_width * grid_height; ++i)
        grid[i] = rand() % 2 == 0;
}

bool is_alive(int row, int col, char *grid) {

    int count = 0;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if (i == 0 && j == 0) {
                continue;
            }
            int r = row + i;
            int c = col + j;
            if (r < 0 || r >= grid_height || c < 0 || c >= grid_width) {
                continue;
            }
            if (grid[grid_width * r + c]) {
                count++;
            }
        }
    }

    if (grid[row * grid_width + col]) {
        if (count == 2 || count == 3)
            return true;
        else
            return false;
    } else {
        if (count == 3)
            return true;
        else
            return false;
    }
}

void update_grid(char *src, char *dst) {
    for (int i = 0; i < grid_height; ++i) {
        for (int j = 0; j < grid_width; ++j) {
            dst[i * grid_width + j] = is_alive(i, j, src);
        }
    }
}

typedef struct {
    char *src;
    char *dst;
    int s;
    int e;
} CellThread;

void handler(int signo, siginfo_t *info, void *context) {}

void *updateCell(void *arg) {
    CellThread *now = (CellThread *) arg;

    while (1) {
        for (int i = now->s; i < now->e; i++) {
            now->dst[i] = is_alive(i / grid_width, i % grid_width, now->src);
        }
        pause();
        char *tmp = now->src;
        now->src = now->dst;
        now->dst = tmp;
    }

}

int min(int a, int b) {
    if (a < b) {
        return a;
    }
    return b;
}

int max(int a, int b) {
    if (a > b) {
        return a;
    }
    return b;
}

void createMultiThread(char *src, char *dst, int blocks) {

    int all = grid_width * grid_height;

    threads = malloc(sizeof(pthread_t) * blocks);

    int skip = all / blocks;
    int count = 0;

    for (int i = 0; i < all; i += skip) {
        CellThread *cell = malloc(sizeof(CellThread));
        cell->src = src;
        cell->dst = dst;
        cell->s = i;
        cell->e = min(i + skip, all);
        pthread_create(&threads[count], NULL, updateCell, (void *) cell);
        count += 1;
    }


}


void updateMultiThread(int blocks) {

    struct sigaction action;
    sigemptyset(&action.sa_mask);
    action.sa_sigaction = handler;
    sigaction(SIGUSR1, &action, NULL);


    for (int x = 0; x < blocks; ++x) {
        pthread_kill(threads[x], SIGUSR1);
    }

}
