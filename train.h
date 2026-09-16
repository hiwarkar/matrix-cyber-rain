// Simple train effect API
#ifndef TRAIN_H
#define TRAIN_H

void train_init(int cols, int rows);
void train_toggle();
int train_enabled();
void train_update_and_draw(int cols, int rows, int shades, int *gen_color_scheme[], int num_colors, int mix_mode_enabled, int *mix_colors, int mix_color_count, int mix_mode_generation, int last_mix_generation);
void train_free();

#endif
