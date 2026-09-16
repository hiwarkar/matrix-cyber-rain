#include <stdlib.h>
#include <ncurses.h>
#include "rain.h"
#include "colors.h"

RainSystem *rain_init(int cols, int rows) {
    RainSystem *rain = malloc(sizeof(RainSystem));
    
    rain->head = malloc(sizeof(int) * cols);
    rain->len = malloc(sizeof(int) * cols);
    rain->stream = malloc(sizeof(char*) * cols);
    rain->col_reset_time = malloc(sizeof(long) * cols);
    rain->col_generation = malloc(sizeof(int) * cols);

    for (int i = 0; i < cols; i++) {
        rain->head[i] = -(rand() % rows);
        rain->len[i] = 20 + rand() % MAX_TRAIL;
        rain->col_reset_time[i] = 0;
        rain->col_generation[i] = 0;

        rain->stream[i] = malloc(rows);
        for (int j = 0; j < rows; j++)
            rain->stream[i][j] = 33 + rand() % 94;
    }

    return rain;
}

void rain_handle_resize(RainSystem *rain, int old_cols, int old_rows,
                        int new_cols, int new_rows) {
    if (new_cols != old_cols) {
        rain->head = realloc(rain->head, sizeof(int) * new_cols);
        rain->len = realloc(rain->len, sizeof(int) * new_cols);
        rain->stream = realloc(rain->stream, sizeof(char*) * new_cols);
        rain->col_generation = realloc(rain->col_generation, sizeof(int) * new_cols);

        for (int i = old_cols; i < new_cols; i++) {
            rain->stream[i] = malloc(new_rows);
            rain->head[i] = -(rand() % new_rows);
            rain->len[i] = 20 + rand() % MAX_TRAIL;
            rain->col_generation[i] = 0;

            for (int j = 0; j < new_rows; j++)
                rain->stream[i][j] = 33 + rand() % 94;
        }
    }

    if (new_rows != old_rows) {
        for (int i = 0; i < new_cols; i++) {
            rain->stream[i] = realloc(rain->stream[i], new_rows);

            for (int j = old_rows; j < new_rows; j++)
                rain->stream[i][j] = 33 + rand() % 94;
        }
    }
}

void rain_update(RainSystem *rain, int cols, int rows, int shades, int *gen_scheme,
                 GlitchState *glitch_state, MixMode *mix_mode, int **color_schemes,
                 int current_generation, int last_mix_generation) {
    for (int i = 0; i < cols; i++) {
        /* Check if this column is frozen by glitch */
        int col_dir = 1;
        long current_time = 0;  /* Should be passed in if needed */

        if (glitch_state[i].intensity > 0.0) {
            if (current_time < glitch_state[i].stop_until) {
                col_dir = 0;
            } else {
                int speed_mod = (int)(glitch_state[i].glitch_speed * glitch_state[i].intensity);
                if (abs(speed_mod) > 0) {
                    if (rand() % 10 < abs(speed_mod) / 5) {
                        col_dir += (speed_mod > 0) ? 1 : -1;
                    }
                }
            }
        }

        /* Erase old character */
        int erase_y = (col_dir == 1) ? (rain->head[i] - rain->len[i]) : (rain->head[i] + 1);
        if (erase_y >= 0 && erase_y < rows)
            mvaddch(erase_y, i, ' ');

        rain->head[i] += col_dir;

        if (rain->head[i] >= 0 && rain->head[i] < rows) {
            attron(COLOR_PAIR(50) | A_BOLD);

            if (glitch_state[i].intensity > 0.0) {
                if (glitch_state[i].glitch_brightness > 0) {
                    attron(A_BOLD);
                } else if (glitch_state[i].glitch_brightness < 0) {
                    attron(A_DIM);
                }
                char garbage = (rand() % 2 == 0) ? (33 + rand() % 94) : rain->stream[i][rain->head[i] % rows];
                mvaddch(rain->head[i], i, garbage);
            } else {
                mvaddch(rain->head[i], i, rain->stream[i][rain->head[i] % rows]);
            }

            attroff(COLOR_PAIR(50) | A_BOLD);
            attroff(A_BOLD);
            attroff(A_DIM);

            if (rain->head[i] + 1 < rows)
                mvaddch(rain->head[i] + 1, i, rain->stream[i][rain->head[i] % rows]);
        }

        /* Draw rain trail */
        for (int j = 1; j < rain->len[i]; j++) {
            int y = rain->head[i] - j;

            if (y >= 0 && y < rows) {
                int shade = (j * shades) / rain->len[i];
                if (shade >= shades) shade = shades - 1;

                int gen_idx = rain->col_generation[i] % MAX_GENERATIONS;
                int pair_offset = gen_idx * shades + 1;

                int draw_y = y;
                int draw_x = i;
                char ch_to_draw = rain->stream[i][y % rows];

                /* Apply glitch effects */
                if (glitch_state[i].intensity > 0.0) {
                    if (glitch_state[i].glitch_color_idx == 2) {
                        int red[] = {52, 88, 124, 160, 161, 196, 197};
                        int red_color = red[shade];
                        init_pair(100 + shade, red_color, -1);
                        attron(COLOR_PAIR(100 + shade) | A_BOLD);
                    } else if (glitch_state[i].glitch_color_idx == 4) {
                        int yellow[] = {58, 100, 136, 142, 148, 184, 226};
                        int gold_color = yellow[shade];
                        init_pair(120 + shade, gold_color, -1);
                        attron(COLOR_PAIR(120 + shade) | A_BOLD);
                    }

                    if (glitch_state[i].glitch_brightness > 0) {
                        attron(A_BOLD);
                    } else if (glitch_state[i].glitch_brightness < 0) {
                        attron(A_DIM);
                    }

                    if (glitch_state[i].is_flashing && glitch_state[i].effect_type == 1) {
                        attron(A_BOLD | A_REVERSE);
                        ch_to_draw = (rand() % 2 == 0) ? '@' : '#';
                        glitch_state[i].is_flashing = 0;
                    } else if (glitch_state[i].effect_type == 2 && rand() % 100 < 35) {
                        int jump = (rand() % 5 - 2);
                        draw_y += jump;
                        if (draw_y < 0 || draw_y >= rows) draw_y = y;
                        ch_to_draw = (rand() % 2 == 0) ? (33 + rand() % 94) : rain->stream[i][draw_y % rows];
                    } else if (rand() % 100 < (glitch_state[i].intensity * 85)) {
                        ch_to_draw = 33 + rand() % 94;
                    }
                } else if ((rain->col_generation[i] == mix_mode->generation || 
                           rain->col_generation[i] == last_mix_generation) && 
                          (mix_mode->generation >= 0 || last_mix_generation >= 0)) {
                    int color_idx = mix_mode->colors[i % mix_mode->color_count];
                    int *selected_color = color_schemes[color_idx];
                    int mix_pair_base = 200 + (color_idx * shades) + shade;
                    if (mix_pair_base < 256) {
                        init_pair(mix_pair_base, selected_color[shade], -1);
                        attron(COLOR_PAIR(mix_pair_base));
                    } else {
                        attron(COLOR_PAIR(shade + pair_offset));
                    }
                } else {
                    attron(COLOR_PAIR(shade + pair_offset));
                }

                if (draw_y >= 0 && draw_y < rows) {
                    mvaddch(draw_y, draw_x, ch_to_draw);
                }

                if (glitch_state[i].intensity > 0.0) {
                    if (glitch_state[i].glitch_color_idx == 2) {
                        attroff(COLOR_PAIR(100 + shade));
                    } else if (glitch_state[i].glitch_color_idx == 4) {
                        attroff(COLOR_PAIR(120 + shade));
                    }
                    attroff(A_BOLD);
                    attroff(A_REVERSE);
                } else if ((rain->col_generation[i] == mix_mode->generation || 
                           rain->col_generation[i] == last_mix_generation) && 
                          (mix_mode->generation >= 0 || last_mix_generation >= 0)) {
                    int color_idx = mix_mode->colors[i % mix_mode->color_count];
                    int mix_pair_base = 200 + (color_idx * shades) + shade;
                    if (mix_pair_base < 256) {
                        attroff(COLOR_PAIR(mix_pair_base));
                    } else {
                        attroff(COLOR_PAIR(shade + pair_offset));
                    }
                } else {
                    attroff(COLOR_PAIR(shade + pair_offset));
                }
                attroff(A_BOLD);
                attroff(A_DIM);
            }
        }

        /* Wrapping behavior */
        if (col_dir == 1 && rain->head[i] - rain->len[i] > rows) {
            rain->head[i] = -(rand() % rows);
            rain->len[i] = 20 + rand() % MAX_TRAIL;
            rain->col_generation[i] = current_generation;
        } else if (col_dir == -1 && rain->head[i] + 1 < 0) {
            rain->head[i] = rows + rain->len[i] + (rand() % rows);
            rain->len[i] = 20 + rand() % MAX_TRAIL;
            rain->col_generation[i] = current_generation;
        }

        if (rand() % 1000 == 0) {
            int pos = rand() % rows;
            rain->stream[i][pos] = 33 + rand() % 94;
        }
    }
}

void rain_free(RainSystem *rain) {
    for (int i = 0; i < 80; i++) {  /* Assuming max 80 cols for cleanup */
        free(rain->stream[i]);
    }
    free(rain->head);
    free(rain->len);
    free(rain->stream);
    free(rain->col_generation);
    free(rain->col_reset_time);
    free(rain);
}
