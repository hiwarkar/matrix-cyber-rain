#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <math.h>

#include "utils.h"
#include "colors.h"
#include "glitch.h"
#include "mix_mode.h"
#include "fog.h"
#include "logo.h"
#include "rain.h"

int main() {

    int rows, cols;

    initscr();
    noecho();
    curs_set(FALSE);
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);

    start_color();
    use_default_colors();

    getmaxyx(stdscr, rows, cols);

    srand(time(NULL));

    /* 🎨 Initialize color system */
    ColorSystem color_system;
    colors_init(&color_system);

    /* 🌧️ Initialize rain system */
    RainSystem *rain = rain_init(cols, rows);

    /* 🌫️ Initialize fog */
    Fog *fog = fog_init(cols, rows);
    int fog_enabled = 0;

    /* 💚 Initialize logo/intro */
    int particle_count = 0;
    Particle *particles = logo_init(cols, rows, &particle_count);
    int logo_phase = 0;
    long logo_time = now_us();

    /* 💡 Glitch system */
    int glitch_enabled = 0;
    GlitchState *glitch_state = glitch_init(cols);
    long glitch_activation_time = now_us();
    int glitch_fade_duration = 800000;

    /* 🎨 Mix mode system */
    MixMode *mix_mode = mix_mode_init();
    int *color_schemes = (int *)color_system.schemes;

    /* 🎬 Rain control variables */
    int rain_speed = 10;
    int rain_accumulator = 0;
    int current_generation = 0;
    int last_color_choice = -1;

    long delay = 30000;
    long last_time = now_us();

    clear();

    while (1) {

        int ch = getch();
        if (ch == 'q') break;

        /* 🎮 CONTROLS */
        if (ch == KEY_UP) {
            rain_speed--;
            if (rain_speed < MIN_RAIN_SPEED) rain_speed = MIN_RAIN_SPEED;
        }
        if (ch == KEY_DOWN) {
            rain_speed++;
            if (rain_speed > MAX_RAIN_SPEED) rain_speed = MAX_RAIN_SPEED;
        }

        /* Toggle fog with 'f' key */
        if (ch == 'f' && logo_phase >= 3) {
            fog_toggle(&fog_enabled);
        }

        /* Toggle glitch with 'g' key */
        if (ch == 'g' && logo_phase >= 3) {
            glitch_enabled = !glitch_enabled;
            glitch_activation_time = now_us();

            if (glitch_enabled) {
                glitch_activate(glitch_state, cols, NUM_COLOR_SCHEMES);
            } else {
                glitch_deactivate(glitch_state, cols);
            }
        }

        /* Change rain color with 'c' key */
        if (ch == 'c' && logo_phase >= 3) {
            current_generation++;

            int color_choice = rand() % NUM_COLOR_SCHEMES;
            while (color_choice == last_color_choice) {
                color_choice = rand() % NUM_COLOR_SCHEMES;
            }
            last_color_choice = color_choice;
            int *new_scheme = color_system.schemes[color_choice];

            colors_set_gen_scheme(&color_system, current_generation, new_scheme);
            mix_mode->enabled = 0;
        }

        /* 🎨 Mix mode toggle with 'm' key */
        if (ch == 'm' && logo_phase >= 3) {
            if (!mix_mode->enabled) {
                current_generation++;
                mix_mode_enable(mix_mode, (int **)color_system.schemes, NUM_COLOR_SCHEMES, current_generation);
            } else {
                mix_mode_disable(mix_mode, current_generation);
            }
        }

        /* FPS balance */
        long current = now_us();
        long frame_time = current - last_time;
        last_time = current;

        if (frame_time < 15000) delay += 1000;
        if (frame_time > 40000) delay -= 1000;

        if (delay < MIN_DELAY) delay = MIN_DELAY;
        if (delay > MAX_DELAY) delay = MAX_DELAY;

        /* Screen resize handling */
        int new_rows, new_cols;
        getmaxyx(stdscr, new_rows, new_cols);

        if (new_cols != cols || new_rows != rows) {
            rain_handle_resize(rain, cols, rows, new_cols, new_cols);
            cols = new_cols;
            rows = new_rows;
        }

        /* 🎬 INTRO PHASE */
        if (logo_render_intro(particles, particle_count, &logo_phase, &logo_time, rows, cols)) {
            wnoutrefresh(stdscr);
            doupdate();
            usleep(delay);
            continue;
        }

        /* 🔴 Update glitch states */
        long current_system_time = now_us();
        glitch_update(glitch_state, cols, glitch_enabled, current_system_time, glitch_fade_duration);

        /* 🎨 Update mix mode auto color changes */
        mix_mode_update(mix_mode, (int **)color_system.schemes, NUM_COLOR_SCHEMES, &current_generation);

        /* 🌧️ RAIN ANIMATION */
        rain_accumulator += rain_speed;

        while (abs(rain_accumulator) >= 10) {
            int dir = (rain_accumulator > 0) ? 1 : -1;
            rain_accumulator -= dir * 10;

            rain_update(rain, cols, rows, color_system.shades, 
                       colors_get_gen_scheme(&color_system, current_generation),
                       glitch_state, mix_mode, (int **)color_system.schemes,
                       current_generation, mix_mode->last_generation);

            /* 🌫️ fog */
            fog_update(fog, fog_enabled, cols, rows, dir);
        }

        wnoutrefresh(stdscr);
        doupdate();
        usleep(delay);
    }

    endwin();
    
    /* Cleanup */
    glitch_free(glitch_state);
    mix_mode_free(mix_mode);
    rain_free(rain);
    free(fog);
    free(particles);

    return 0;
}
