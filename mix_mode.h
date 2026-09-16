#ifndef MIX_MODE_H
#define MIX_MODE_H

typedef struct {
    int enabled;
    int colors[3];
    int color_count;
    int generation;
    int last_generation;
    int pre_generation;
    int *color_schemes[3];
    long color_change_time;
    int change_interval;
} MixMode;

/* Initialize mix mode */
MixMode *mix_mode_init(void);

/* Enable mix mode with random color combination */
void mix_mode_enable(MixMode *mm, int **color_schemes, int num_colors, int current_gen);

/* Disable mix mode smoothly */
void mix_mode_disable(MixMode *mm, int pre_gen);

/* Update mix mode auto color changes */
void mix_mode_update(MixMode *mm, int **color_schemes, int num_colors, int *current_gen);

/* Free mix mode memory */
void mix_mode_free(MixMode *mm);

#endif /* MIX_MODE_H */
