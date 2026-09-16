#include <stdlib.h>
#include <ncurses.h>
#include <time.h>
#include <sys/time.h>
#include "train.h"

typedef struct {
    int active;
    int x, y;
    int len;
    int dir; /* 1 = right, -1 = left */
    int speed_ticks; /* frames between moves */
    int tick_acc;
    long stop_until;
    char ch;
    int color_idx;
} TrainBlock;

#define MAX_TRAINS 64
static TrainBlock trains[MAX_TRAINS];
static int g_cols = 0, g_rows = 0;
static int g_enabled = 0;

static long now_us() { struct timeval tv; gettimeofday(&tv, NULL); return tv.tv_sec*1000000 + tv.tv_usec; }

void train_init(int cols, int rows) {
    g_cols = cols; g_rows = rows;
    srand(time(NULL)+12345);
    for (int i = 0; i < MAX_TRAINS; i++) trains[i].active = 0;
}

void train_toggle() { g_enabled = !g_enabled; }
int train_enabled() { return g_enabled; }

static void spawn_train() {
    for (int i = 0; i < MAX_TRAINS; i++) {
        if (!trains[i].active) {
            trains[i].active = 1;
            trains[i].len = 3 + rand()%5; /* 3-7 */
            trains[i].dir = (rand()%2) ? 1 : -1;
            trains[i].y = rand() % (g_rows>1?g_rows:1);
            trains[i].x = (trains[i].dir==1) ? -trains[i].len : g_cols + trains[i].len;
            trains[i].speed_ticks = 3 + rand()%6; /* slower than rain */
            trains[i].tick_acc = 0;
            trains[i].stop_until = 0;
            trains[i].ch = 'A' + (rand()%26);
            trains[i].color_idx = rand()%6;
            break;
        }
    }
}

void train_update_and_draw(int cols, int rows, int shades, int *gen_color_scheme[], int num_colors, int mix_mode_enabled, int *mix_colors, int mix_color_count, int mix_mode_generation, int last_mix_generation) {
    if (!g_enabled) return;
    /* occasional spawn */
    if (rand()%100 < 8) spawn_train();

    for (int i = 0; i < MAX_TRAINS; i++) {
        TrainBlock *t = &trains[i];
        if (!t->active) continue;

        long now = now_us();
        if (t->stop_until > now) {
            /* draw stationary */
        } else {
            t->tick_acc++;
            if (t->tick_acc >= t->speed_ticks) {
                t->tick_acc = 0;
                t->x += t->dir;
                if (rand()%200 == 0) {
                    t->stop_until = now + (200000 + rand()%600000); /* stops */
                }
            }
        }

        /* draw block */
        for (int b = 0; b < t->len; b++) {
            int draw_x = t->x + (t->dir==1 ? b : -b);
            int draw_y = t->y;
            if (draw_x < 0 || draw_x >= cols || draw_y < 0 || draw_y >= rows) continue;

            /* choose color: prefer mix mode colors if enabled */
            if (mix_mode_enabled && mix_color_count > 0) {
                int color_idx = mix_colors[draw_x % mix_color_count];
                int pair = 200 + (color_idx * shades) + (b % shades);
                if (pair < 256) init_pair(pair, gen_color_scheme[color_idx][b % shades], -1);
                attron(COLOR_PAIR(pair) | A_BOLD);
            } else {
                int gen = 0; /* use generation 0 scheme */
                int pair = (gen * shades) + (b % shades) + 1;
                attron(COLOR_PAIR(pair) | A_BOLD);
            }

            mvaddch(draw_y, draw_x, t->ch);
            attroff(A_BOLD);
            if (mix_mode_enabled && mix_color_count > 0) {
                int color_idx = mix_colors[draw_x % mix_color_count];
                int pair = 200 + (color_idx * shades) + (b % shades);
                if (pair < 256) attroff(COLOR_PAIR(pair));
            } else {
                int gen = 0;
                int pair = (gen * shades) + (b % shades) + 1;
                attroff(COLOR_PAIR(pair));
            }
        }

        /* deactivate when fully out of bounds for a while */
        if ((t->dir==1 && t->x - t->len > cols) || (t->dir==-1 && t->x + t->len < 0)) {
            t->active = 0;
        }
    }
}

void train_free() {
    /* nothing to free */
}
