#ifndef RAIN_H
#define RAIN_H

#include "glitch.h"
#include "mix_mode.h"

#define MAX_TRAIL 40
#define MIN_DELAY 10000
#define MAX_DELAY 80000
#define MAX_RAIN_SPEED 40
#define MIN_RAIN_SPEED -40

typedef struct {
    int *head;
    int *len;
    char **stream;
    long *col_reset_time;
    int *col_generation;
} RainSystem;

/* Initialize rain system */
RainSystem *rain_init(int cols, int rows);

/* Update and render rain */
void rain_update(RainSystem *rain, int cols, int rows, int shades, int *gen_scheme,
                 GlitchState *glitch_state, MixMode *mix_mode, int **color_schemes,
                 int rainbow_generation, int last_mix_generation);

/* Handle screen resize */
void rain_handle_resize(RainSystem *rain, int old_cols, int old_rows, 
                        int new_cols, int new_rows);

/* Free rain system */
void rain_free(RainSystem *rain);

#endif /* RAIN_H */
