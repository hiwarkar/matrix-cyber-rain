#include <stdlib.h>
#include "mix_mode.h"
#include "utils.h"

MixMode *mix_mode_init(void) {
    MixMode *mm = malloc(sizeof(MixMode));
    mm->enabled = 0;
    mm->color_count = 0;
    mm->generation = -1;
    mm->last_generation = -1;
    mm->pre_generation = 0;
    mm->color_change_time = now_us();
    mm->change_interval = 210000000;  /* 3.5 minutes */
    mm->color_schemes[0] = NULL;
    mm->color_schemes[1] = NULL;
    mm->color_schemes[2] = NULL;
    return mm;
}

static void pick_random_colors(MixMode *mm, int **color_schemes, int num_colors) {
    mm->color_count = 2 + rand() % 2;  /* 2 or 3 colors */

    /* Pick first color */
    mm->colors[0] = rand() % num_colors;

    /* Pick second (must be different) */
    mm->colors[1] = rand() % num_colors;
    while (mm->colors[1] == mm->colors[0]) {
        mm->colors[1] = rand() % num_colors;
    }

    /* Pick third if needed (must be different from both) */
    if (mm->color_count == 3) {
        mm->colors[2] = rand() % num_colors;
        while (mm->colors[2] == mm->colors[0] || mm->colors[2] == mm->colors[1]) {
            mm->colors[2] = rand() % num_colors;
        }
    }

    mm->color_schemes[0] = color_schemes[mm->colors[0]];
    mm->color_schemes[1] = color_schemes[mm->colors[1]];
    if (mm->color_count == 3) {
        mm->color_schemes[2] = color_schemes[mm->colors[2]];
    }
}

void mix_mode_enable(MixMode *mm, int **color_schemes, int num_colors, int current_gen) {
    mm->enabled = 1;
    mm->pre_generation = current_gen;
    mm->generation = current_gen + 1;
    pick_random_colors(mm, color_schemes, num_colors);
}

void mix_mode_disable(MixMode *mm, int pre_gen) {
    mm->enabled = 0;
    mm->last_generation = mm->generation;
    mm->generation = -1;
}

void mix_mode_update(MixMode *mm, int **color_schemes, int num_colors, int *current_gen) {
    if (!mm->enabled) return;

    long current_time = now_us();
    if (current_time - mm->color_change_time > mm->change_interval) {
        mm->last_generation = mm->generation;
        (*current_gen)++;
        mm->generation = *current_gen;
        pick_random_colors(mm, color_schemes, num_colors);
        mm->color_change_time = current_time;
    }
}

void mix_mode_free(MixMode *mm) {
    free(mm);
}
