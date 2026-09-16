#include <stdlib.h>
#include "glitch.h"
#include "utils.h"

GlitchState *glitch_init(int cols) {
    GlitchState *glitch_state = malloc(sizeof(GlitchState) * cols);
    for (int i = 0; i < cols; i++) {
        glitch_state[i].active = 0;
        glitch_state[i].intensity = 0.0;
    }
    return glitch_state;
}

void glitch_update(GlitchState *glitch_state, int cols, int glitch_enabled, 
                   long current_time, int glitch_fade_duration) {
    for (int i = 0; i < cols; i++) {
        if (glitch_state[i].active) {
            long elapsed = current_time - glitch_state[i].start_time;
            float duration_s = (float)glitch_fade_duration / 1000000.0;

            if (glitch_enabled) {
                /* Fade in to full intensity */
                if (glitch_state[i].intensity < 1.0) {
                    glitch_state[i].intensity += 1.0 / (duration_s * 60.0);
                    if (glitch_state[i].intensity > 1.0) glitch_state[i].intensity = 1.0;
                }
                /* Update flash state */
                if (current_time >= glitch_state[i].flash_time) {
                    glitch_state[i].is_flashing = 1;
                    glitch_state[i].flash_time = current_time + (150000 + rand() % 400000);
                }
            } else {
                /* Fade out smoothly */
                if (glitch_state[i].intensity > 0.0) {
                    glitch_state[i].intensity -= 1.0 / (duration_s * 60.0);
                    if (glitch_state[i].intensity < 0.0) {
                        glitch_state[i].intensity = 0.0;
                        glitch_state[i].active = 0;
                    }
                }
                glitch_state[i].is_flashing = 0;
            }
        }
    }
}

void glitch_activate(GlitchState *glitch_state, int cols, int num_colors) {
    int num_glitches = (cols / 2) + (rand() % (cols / 3));
    int glitched_count = 0;
    int red_lines_created = 0;
    int golden_lines_created = 0;
    int max_red_lines = 8 + rand() % 5;
    int max_golden_lines = 5 + rand() % 4;

    for (int g = 0; g < num_glitches && glitched_count < cols; g++) {
        int col = rand() % cols;
        if (!glitch_state[col].active) {
            glitch_state[col].active = 1;
            glitch_state[col].start_time = now_us();
            glitch_state[col].intensity = 0.0;
            glitch_state[col].glitch_speed = (rand() % 5 - 2) * 20;

            if (red_lines_created < max_red_lines) {
                glitch_state[col].glitch_color_idx = 2;  /* red */
                red_lines_created++;
            } else if (golden_lines_created < max_golden_lines) {
                glitch_state[col].glitch_color_idx = 4;  /* yellow/gold */
                golden_lines_created++;
            } else {
                glitch_state[col].glitch_color_idx = rand() % num_colors;
            }

            glitch_state[col].glitch_brightness = rand() % 3 - 1;
            glitch_state[col].stop_until = now_us() + (300000 + rand() % 500000);
            glitch_state[col].effect_type = rand() % 3;
            glitch_state[col].flash_time = now_us() + (100000 + rand() % 400000);
            glitch_state[col].is_flashing = 0;
            glitched_count++;
        }
    }
}

void glitch_deactivate(GlitchState *glitch_state, int cols) {
    for (int i = 0; i < cols; i++) {
        if (glitch_state[i].active) {
            glitch_state[i].intensity = 1.0;
        }
    }
}

void glitch_free(GlitchState *glitch_state) {
    free(glitch_state);
}
