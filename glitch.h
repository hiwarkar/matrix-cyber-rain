#ifndef GLITCH_H
#define GLITCH_H

typedef struct {
    int active;              /* Is this column glitched? */
    long start_time;         /* When glitch started */
    int glitch_speed;        /* Speed modifier during glitch */
    int glitch_color_idx;    /* Color override index */
    int glitch_brightness;   /* -1 = dim, 0 = normal, 1 = bright */
    long stop_until;         /* Time when column stops moving until */
    float intensity;         /* Smooth transition intensity 0.0-1.0 */
    int effect_type;         /* 0=garbage, 1=flash, 2=scramble */
    long flash_time;         /* When the flash occurs */
    int is_flashing;         /* Currently in flash state? */
} GlitchState;

/* Initialize glitch state for all columns */
GlitchState *glitch_init(int cols);

/* Update glitch states with smooth transitions */
void glitch_update(GlitchState *glitch_state, int cols, int glitch_enabled, 
                   long current_time, int glitch_fade_duration);

/* Activate glitch on random columns */
void glitch_activate(GlitchState *glitch_state, int cols, int num_colors);

/* Deactivate all glitches smoothly */
void glitch_deactivate(GlitchState *glitch_state, int cols);

/* Free glitch state memory */
void glitch_free(GlitchState *glitch_state);

#endif /* GLITCH_H */
