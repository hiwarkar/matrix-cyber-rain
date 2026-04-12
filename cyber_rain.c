#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>

#define MAX_TRAIL 40
#define MIN_DELAY 10000
#define MAX_DELAY 80000
#define FOG_PARTICLES 100

/* Added max and min speeds for the accumulator */
#define MAX_RAIN_SPEED 40
#define MIN_RAIN_SPEED -40

typedef struct {
    int x, y;
} Fog;

typedef struct {
    int x, y, target_y;
    int is_x;
} Particle;

/* 🔴 GLITCH STATE TRACKING */
typedef struct {
    int active;              /* Is this column glitched? */
    long start_time;         /* When glitch started */
    int glitch_speed;        /* Speed modifier during glitch */
    int glitch_color_idx;    /* Color override index */
    int glitch_brightness;   /* -1 = dim, 0 = normal, 1 = bright */
    long stop_until;         /* Time when column stops moving until */
    float intensity;         /* Smooth transition intensity 0.0-1.0 */
    /* 💥 NEW EFFECTS */
    int effect_type;         /* 0=garbage, 1=flash, 2=scramble */
    long flash_time;         /* When the flash occurs */
    int is_flashing;         /* Currently in flash state? */
} GlitchState;

long now_us() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000 + tv.tv_usec;
}

/* 🔴 Update glitch states with smooth transitions */
void update_glitches(GlitchState *glitch_state, int cols, int glitch_enabled, long current_time, int glitch_fade_duration) {
    for (int i = 0; i < cols; i++) {
        if (glitch_state[i].active) {
            long elapsed = current_time - glitch_state[i].start_time;
            float duration_s = (float)glitch_fade_duration / 1000000.0;

            if (glitch_enabled) {
                /* Fade in to full intensity */
                if (glitch_state[i].intensity < 1.0) {
                    glitch_state[i].intensity += 1.0 / (duration_s * 60.0);  /* Smooth fade-in over duration */
                    if (glitch_state[i].intensity > 1.0) glitch_state[i].intensity = 1.0;
                }
                /* 💥 UPDATE FLASH STATE */
                if (current_time >= glitch_state[i].flash_time) {
                    glitch_state[i].is_flashing = 1;
                    /* Schedule next flash */
                    glitch_state[i].flash_time = current_time + (150000 + rand() % 400000);  /* 0.15-0.55s between flashes */
                }
            } else {
                /* Fade out smoothly */
                if (glitch_state[i].intensity > 0.0) {
                    glitch_state[i].intensity -= 1.0 / (duration_s * 60.0);
                    if (glitch_state[i].intensity < 0.0) {
                        glitch_state[i].intensity = 0.0;
                        glitch_state[i].active = 0;  /* Fully deactivate */
                    }
                }
                glitch_state[i].is_flashing = 0;
            }
        }
    }
}

/* 💚 HMATRIX */
const char *hmatrix[] = {
    "  CCCCC   Y     Y   BBBBBB   EEEEEE  RRRRRRR  RRRRRRR  AAAAAA  IIIIII  NN   NN",
    "CC         Y   Y    BB   BB  EE      RR    RR RR    RR AA   AA   II    NNN  NN",
    "CC          Y Y     BBBBBB   EEEEE   RRRRRRR  RRRRRRR  AAAAAA    II    NN NN N",
    "CC           Y      BB   BB  EE      RR  RR   RR  RR   AA   AA   II    NN  NNN",
    " CCCCCC     Y       BBBBBB   EEEEEE  RR   RR  RR   RR  AA   AA IIIIII  NN   NN"
};

#define LOGO_H 5

int main() {

    int rows, cols;

    initscr();
    noecho();
    curs_set(FALSE);
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);

    start_color();
    use_default_colors();

    /* 🎨 Color schemes */
    int green_shades[] = {22, 28, 34, 40, 46, 82, 118};
    int purple_shades[] = {55, 56, 57, 92, 93, 98, 99};
    int red_shades[] = {52, 88, 124, 160, 161, 196, 197};
    int blue_shades[] = {18, 19, 20, 21, 27, 33, 39};
    int yellow_shades[] = {58, 100, 136, 142, 148, 184, 226};  /* 🟡 GOLD/YELLOW */
    int orange_shades[] = {94, 130, 166, 172, 208, 214, 220};  /* 🟠 ORANGE */
    int cyan_shades[] = {23, 29, 35, 41, 50, 87, 123};         /* 🔵 CYAN */
    int magenta_shades[] = {53, 89, 125, 127, 163, 165, 201};  /* 🟣 MAGENTA */
    int pink_shades[] = {175, 176, 177, 210, 211, 212, 217};   /* 🌸 CHERRY BLOSSOM PINK */

    int *color_schemes[] = {green_shades, purple_shades, red_shades, blue_shades,
                            yellow_shades, orange_shades, cyan_shades, magenta_shades, pink_shades};
    int num_colors = 9;  /* Total number of color schemes */
    int shades = sizeof(green_shades)/sizeof(int);

    /* Pre-initialize color pairs for multiple generations (pairs 1-84 = 12 generations of 7 pairs each)
       Using 12 slots instead of 4 provides buffer so existing rain doesn't get color reused while falling */
    #define MAX_GENERATIONS 12
    int *gen_color_scheme[MAX_GENERATIONS];
    for (int g = 0; g < MAX_GENERATIONS; g++) {
        gen_color_scheme[g] = green_shades;  /* Default to green */
        for (int i = 0; i < shades; i++)
            init_pair(g * shades + i + 1, gen_color_scheme[g][i], -1);
    }

    init_pair(50, COLOR_WHITE, -1);

    getmaxyx(stdscr, rows, cols);

    srand(time(NULL));

    /* 🌧️ RAIN (UNCHANGED) */
    int *head = malloc(sizeof(int) * cols);
    int *len  = malloc(sizeof(int) * cols);
    char **stream = malloc(sizeof(char*) * cols);
    long *col_reset_time = malloc(sizeof(long) * cols);
    int *col_generation = malloc(sizeof(int) * cols);

    for (int i = 0; i < cols; i++) {
        head[i] = -(rand() % rows);
        len[i]  = 20 + rand() % MAX_TRAIL;
        col_reset_time[i] = now_us();
        col_generation[i] = 0;

        stream[i] = malloc(rows);
        for (int j = 0; j < rows; j++)
            stream[i][j] = 33 + rand() % 94;
    }

    /* 🌫️ fog */
    Fog fog[FOG_PARTICLES];
    for (int i = 0; i < FOG_PARTICLES; i++) {
        fog[i].x = rand() % cols;
        fog[i].y = rand() % rows;
    }
    int fog_enabled = 0;

    /* 🎬 particles */
    Particle particles[2000];
    int particle_count = 0;

    int logo_width = strlen(hmatrix[0]);
    int logo_x = cols/2 - logo_width/2;
    int logo_y = rows/2 - LOGO_H/2;

    /* 💡 IMPORTANT FIX: stable X formation */
    int global_x_offset = rand() % 5;  // shared timing for X

    for (int i = 0; i < LOGO_H; i++) {
        for (int j = 0; j < logo_width; j++) {

            if (hmatrix[i][j] == ' ') continue;

            particles[particle_count].x = logo_x + j;
            particles[particle_count].target_y = logo_y + i;

            /* 🔥 FIX: X gets synchronized movement */
            if (hmatrix[i][j] == 'X') {
                particles[particle_count].y = -(rows + global_x_offset);
                particles[particle_count].is_x = 1;
            } else {
                particles[particle_count].y = -(rand() % rows);
                particles[particle_count].is_x = 0;
            }

            particle_count++;
        }
    }

    int logo_phase = 0;
    long logo_time = now_us();

    long delay = 30000;
    long last_time = now_us();

    /* New variables for logic-based speed and reversing */
    int rain_speed = 10;
    int rain_accumulator = 0;
    long color_change_time = now_us();
    int current_generation = 0;
    int last_color_choice = -1;  /* Track last color to avoid repeats */

    /* 🎨 MIX MODE SYSTEM */
    int mix_mode_enabled = 0;
    int mix_colors[3] = {0, 0, 0};  /* Store 2-3 colors for mix */
    int mix_color_count = 0;  /* How many colors in mix */
    int mix_mode_generation = -1;  /* Track generation when mix mode activated */
    int last_mix_generation = -1;  /* Track last mix generation for smooth disable */
    int *mix_mode_schemes[3] = {NULL, NULL, NULL};  /* Color schemes for mix */

    /* 🔴 GLITCH SYSTEM */
    int glitch_enabled = 0;
    GlitchState *glitch_state = malloc(sizeof(GlitchState) * cols);
    for (int i = 0; i < cols; i++) {
        glitch_state[i].active = 0;
        glitch_state[i].intensity = 0.0;
    }
    long glitch_activation_time = now_us();
    int glitch_fade_duration = 800000;  /* 0.8 seconds smooth fade */

    clear();

    while (1) {

        int ch = getch();
        if (ch == 'q') break;

        /* Directional speed controls replaced with `rain_speed` modifiers */
        if (ch == KEY_UP) {
            rain_speed--;
            if (rain_speed < MIN_RAIN_SPEED) rain_speed = MIN_RAIN_SPEED;
        }
        if (ch == KEY_DOWN) {
            rain_speed++;
            if (rain_speed > MAX_RAIN_SPEED) rain_speed = MAX_RAIN_SPEED;
        }

        /* Toggle fog with 'f' key */
        if (ch == 'f') fog_enabled = !fog_enabled;

        /* Toggle glitch with 'g' key - only if intro is done */
        if (ch == 'g' && logo_phase >= 3) {
            glitch_enabled = !glitch_enabled;
            glitch_activation_time = now_us();

            if (glitch_enabled) {
                /* 🔴 MASSIVE VIRAL GLITCH ATTACK - 30-50% of screen corrupted */
                int num_glitches = (cols / 2) + (rand() % (cols / 3));  /* 50-83% of columns get glitched */
                int glitched_count = 0;
                int red_lines_created = 0;
                int golden_lines_created = 0;
                int max_red_lines = 8 + rand() % 5;  /* 8-12 red lines always visible */
                int max_golden_lines = 5 + rand() % 4;  /* 5-8 golden lines for contrast */

                for (int g = 0; g < num_glitches && glitched_count < cols; g++) {
                    int col = rand() % cols;
                    if (!glitch_state[col].active) {
                        glitch_state[col].active = 1;
                        glitch_state[col].start_time = now_us();
                        glitch_state[col].intensity = 0.0;
                        /* 🔴 MORE EXTREME SPEED VARIATIONS */
                        glitch_state[col].glitch_speed = (rand() % 5 - 2) * 20;  /* -40, -20, 0, 20, 40 */

                        /* 🔴 ENSURE SOME COLUMNS ARE ALWAYS RED OR 🟡 GOLDEN */
                        if (red_lines_created < max_red_lines) {
                            glitch_state[col].glitch_color_idx = 2;  /* 2 = red_shades */
                            red_lines_created++;
                        } else if (golden_lines_created < max_golden_lines) {
                            glitch_state[col].glitch_color_idx = 4;  /* 4 = yellow_shades (golden) */
                            golden_lines_created++;
                        } else {
                            glitch_state[col].glitch_color_idx = rand() % num_colors;
                        }

                        glitch_state[col].glitch_brightness = rand() % 3 - 1;  /* -1, 0, 1 */
                        /* 🔴 LONGER FREEZE TIMES FOR MORE CHAOS */
                        glitch_state[col].stop_until = now_us() + (300000 + rand() % 500000);  /* 0.3-0.8s stops */
                        /* 💥 RANDOM EFFECT ASSIGNMENT */
                        glitch_state[col].effect_type = rand() % 3;  /* 0=garbage, 1=flash, 2=scramble */
                        glitch_state[col].flash_time = now_us() + (100000 + rand() % 400000);  /* Random flash timing */
                        glitch_state[col].is_flashing = 0;
                        glitched_count++;
                    }
                }
            } else {
                /* Smoothly deactivate all glitches */
                for (int i = 0; i < cols; i++) {
                    if (glitch_state[i].active) {
                        glitch_state[i].intensity = 1.0;  /* Start fade from full intensity */
                    }
                }
            }
        }

        /* Change rain color with 'c' key - only if intro is done */
        if (ch == 'c' && logo_phase >= 3) {
            /* Increment generation */
            current_generation++;

            /* Generate new random color scheme (ensure it's different from last) */
            int color_choice = rand() % num_colors;
            while (color_choice == last_color_choice) {
                color_choice = rand() % num_colors;
            }
            last_color_choice = color_choice;
            int *new_scheme = color_schemes[color_choice];

            /* Update the color pair set for this generation (12 slots cycling for buffer) */
            int gen_idx = current_generation % MAX_GENERATIONS;
            gen_color_scheme[gen_idx] = new_scheme;

            for (int i = 0; i < shades; i++)
                init_pair(gen_idx * shades + i + 1, gen_color_scheme[gen_idx][i], -1);

            color_change_time = now_us();
            mix_mode_enabled = 0;  /* Disable mix mode when changing single color */
        }

        /* 🎨 MIX MODE: Press 'm' to enable/disable color mixing - SMOOTH TRANSITION */
        if (ch == 'm' && logo_phase >= 3) {
            mix_mode_enabled = !mix_mode_enabled;

            if (mix_mode_enabled) {
                /* 🎨 NEW GENERATION for smooth transition (like color change) */
                current_generation++;
                mix_mode_generation = current_generation;

                /* Generate 2-3 contrasting colors for mix */
                mix_color_count = 2 + rand() % 2;  /* 2 or 3 colors */

                /* Pick colors and store their schemes */
                /* Pick first color */
                mix_colors[0] = rand() % num_colors;

                /* Pick second color (must be different) */
                mix_colors[1] = rand() % num_colors;
                while (mix_colors[1] == mix_colors[0]) {
                    mix_colors[1] = rand() % num_colors;
                }

                /* Pick third color if mix_color_count == 3 (must be different from both) */
                if (mix_color_count == 3) {
                    mix_colors[2] = rand() % num_colors;
                    while (mix_colors[2] == mix_colors[0] || mix_colors[2] == mix_colors[1]) {
                        mix_colors[2] = rand() % num_colors;
                    }
                }

                /* Store the color schemes for this generation */
                mix_mode_schemes[0] = color_schemes[mix_colors[0]];
                mix_mode_schemes[1] = color_schemes[mix_colors[1]];
                if (mix_color_count == 3) {
                    mix_mode_schemes[2] = color_schemes[mix_colors[2]];
                }
            } else {
                /* Disable mix mode - next generation will use single color */
                mix_mode_generation = -1;
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

        /* ---------------------------------------------------- */
        /* 🛠️ SCREEN RESIZE LOGIC ADDED HERE                    */
        /* ---------------------------------------------------- */
        int new_rows, new_cols;
        getmaxyx(stdscr, new_rows, new_cols);

        if (new_cols != cols) {
            head = realloc(head, sizeof(int) * new_cols);
            len  = realloc(len,  sizeof(int) * new_cols);
            stream = realloc(stream, sizeof(char*) * new_cols);

            for (int i = cols; i < new_cols; i++) {
                stream[i] = malloc(new_rows);
                head[i] = -(rand() % new_rows);
                len[i]  = 20 + rand() % MAX_TRAIL;

                for (int j = 0; j < new_rows; j++)
                    stream[i][j] = 33 + rand() % 94;
            }
            cols = new_cols;
        }

        if (new_rows != rows) {
            for (int i = 0; i < cols; i++) {
                stream[i] = realloc(stream[i], new_rows);

                for (int j = rows; j < new_rows; j++)
                    stream[i][j] = 33 + rand() % 94;
            }
            rows = new_rows;
        }
        /* ---------------------------------------------------- */

        /* 🎬 INTRO */
        if (logo_phase < 3) {

            clear();

            int done = 1;

            for (int i = 0; i < particle_count; i++) {

                Particle *p = &particles[i];

                if (logo_phase == 0) {
                    if (p->y < p->target_y) {
                        p->y++;
                        done = 0;
                    }
                }

                if (logo_phase == 2) {
                    p->y++;
                }

                if (p->y >= 0 && p->y < rows) {

                    char glow_chars[] = "@%#*+=-:.";
                    char draw = glow_chars[rand() % 7];

                    attron(COLOR_PAIR(1) | A_BOLD);
                    mvaddch(p->y, p->x, draw);
                    attroff(COLOR_PAIR(1) | A_BOLD);
                }
            }

            if (logo_phase == 0 && done) {
                logo_phase = 1;
                logo_time = now_us();
            }

            if (logo_phase == 1 && now_us() - logo_time > 2000000)
                logo_phase = 2;

            if (logo_phase == 2 && particles[0].y > rows) {
                logo_phase = 3;
                clear();
            }

        } else {

            /* 🔴 Update glitch states */
            long current = now_us();
            update_glitches(glitch_state, cols, glitch_enabled, current, glitch_fade_duration);

            /* 🌧️ MATRIX RAIN & FOG (MODIFIED FOR ACCUMULATOR & REVERSE) */
            rain_accumulator += rain_speed;

            while (abs(rain_accumulator) >= 10) {
                int dir = (rain_accumulator > 0) ? 1 : -1;
                rain_accumulator -= dir * 10;

                for (int i = 0; i < cols; i++) {

                    /* 🔴 Apply glitch speed modifier */
                    int col_dir = dir;
                    long current_time = now_us();

                    if (glitch_state[i].intensity > 0.0) {
                        /* Check if this column is frozen */
                        if (current_time < glitch_state[i].stop_until) {
                            col_dir = 0;  /* Don't move */
                        } else {
                            /* Apply glitch speed variation */
                            int speed_mod = (int)(glitch_state[i].glitch_speed * glitch_state[i].intensity);
                            if (abs(speed_mod) > 0) {
                                /* Modulate movement by glitch speed */
                                if (rand() % 10 < abs(speed_mod) / 5) {
                                    col_dir += (speed_mod > 0) ? 1 : -1;
                                }
                            }
                        }
                    }

                    /* Dynamic erase bounds depending on direction */
                    int erase_y = (col_dir == 1) ? (head[i] - len[i]) : (head[i] + 1);
                    if (erase_y >= 0 && erase_y < rows)
                        mvaddch(erase_y, i, ' ');

                    head[i] += col_dir;

                    if (head[i] >= 0 && head[i] < rows) {
                        /* 🔴 Apply glitch effects to head */
                        attron(COLOR_PAIR(50) | A_BOLD);

                        if (glitch_state[i].intensity > 0.0) {
                            /* Glitch: add brightness variation */
                            if (glitch_state[i].glitch_brightness > 0) {
                                attron(A_BOLD);
                            } else if (glitch_state[i].glitch_brightness < 0) {
                                attron(A_DIM);
                            }

                            /* Glitch: garbage characters */
                            char garbage = (rand() % 2 == 0) ? (33 + rand() % 94) : stream[i][head[i] % rows];
                            mvaddch(head[i], i, garbage);
                        } else {
                            mvaddch(head[i], i, stream[i][head[i] % rows]);
                        }

                        attroff(COLOR_PAIR(50) | A_BOLD);
                        attroff(A_BOLD);
                        attroff(A_DIM);

                        if (head[i] + 1 < rows)
                            mvaddch(head[i] + 1, i, stream[i][head[i] % rows]);
                    }

                    for (int j = 1; j < len[i]; j++) {
                        int y = head[i] - j;

                        if (y >= 0 && y < rows) {
                            int shade = (j * shades) / len[i];
                            if (shade >= shades) shade = shades - 1;

                            /* Map generation to color pair set (12 generations cycling) */
                            int gen_idx = col_generation[i] % MAX_GENERATIONS;
                            int pair_offset = gen_idx * shades + 1;

                            /* 💥 APPLY DIFFERENT EFFECTS BASED ON TYPE */
                            int draw_y = y;
                            int draw_x = i;
                            char ch_to_draw = stream[i][y % rows];

                            /* 🔴 Apply glitch color override */
                            if (glitch_state[i].intensity > 0.0) {
                                /* 🔴 RED ALERT: Use red_shades directly for red columns */
                                if (glitch_state[i].glitch_color_idx == 2) {
                                    int red_color = red_shades[shade];
                                    init_pair(100 + shade, red_color, -1);
                                    attron(COLOR_PAIR(100 + shade) | A_BOLD);
                                }
                                /* 🟡 GOLDEN ALERT: Use yellow_shades directly for golden columns */
                                else if (glitch_state[i].glitch_color_idx == 4) {
                                    int gold_color = yellow_shades[shade];
                                    init_pair(120 + shade, gold_color, -1);
                                    attron(COLOR_PAIR(120 + shade) | A_BOLD);
                                }
                                else {
                                    int glitch_pair = (glitch_state[i].glitch_color_idx * shades + shade + 1);
                                    attron(COLOR_PAIR(glitch_pair));
                                }

                                if (glitch_state[i].glitch_brightness > 0) {
                                    attron(A_BOLD);
                                } else if (glitch_state[i].glitch_brightness < 0) {
                                    attron(A_DIM);
                                }

                                /* 💥 FLASH EFFECT: Bright white flash with intensity */
                                if (glitch_state[i].is_flashing && glitch_state[i].effect_type == 1) {
                                    attron(A_BOLD | A_REVERSE);
                                    ch_to_draw = (rand() % 2 == 0) ? '@' : '#';
                                    glitch_state[i].is_flashing = 0;
                                }
                                /* 💥 SCRAMBLE EFFECT: More aggressive character position jump */
                                else if (glitch_state[i].effect_type == 2 && rand() % 100 < 35) {
                                    int jump = (rand() % 5 - 2);  /* Jump -2, -1, 0, 1, 2 */
                                    draw_y += jump;
                                    if (draw_y < 0 || draw_y >= rows) draw_y = y;
                                    ch_to_draw = (rand() % 2 == 0) ? (33 + rand() % 94) : stream[i][draw_y % rows];
                                }
                                /* 💥 GARBAGE EFFECT: More intense random noise */
                                else if (rand() % 100 < (glitch_state[i].intensity * 85)) {
                                    ch_to_draw = 33 + rand() % 94;
                                }
                            } else if (mix_mode_enabled && col_generation[i] == mix_mode_generation) {
                                /* 🎨 MIX MODE: Only apply to current generation (smooth transition) */
                                /* Pre-initialize mix mode color pairs at startup to avoid rendering issues */
                                int color_idx = mix_colors[i % mix_color_count];
                                int *selected_color = color_schemes[color_idx];
                                /* Use pre-calculated pair index based on mix colors */
                                int mix_pair_base = 200 + (color_idx * shades) + shade;
                                if (mix_pair_base < 256) {
                                    /* Ensure color pair is initialized */
                                    init_pair(mix_pair_base, selected_color[shade], -1);
                                    attron(COLOR_PAIR(mix_pair_base));
                                } else {
                                    /* Fallback if pair index too high */
                                    attron(COLOR_PAIR(shade + pair_offset));
                                }
                            } else {
                                attron(COLOR_PAIR(shade + pair_offset));
                            }

                            if (draw_y >= 0 && draw_y < rows) {
                                 mvaddch(draw_y, draw_x, ch_to_draw);
                            }

                            if (glitch_state[i].intensity > 0.0) {
                                /* 🔴 RED ALERT: Force red color for red columns */
                                if (glitch_state[i].glitch_color_idx == 2) {
                                    int red_color = red_shades[shade];
                                    init_pair(100 + shade, red_color, -1);
                                    attroff(COLOR_PAIR(100 + shade));
                                }
                                /* 🟡 GOLDEN ALERT: Force golden color for golden columns */
                                else if (glitch_state[i].glitch_color_idx == 4) {
                                    int gold_color = yellow_shades[shade];
                                    init_pair(120 + shade, gold_color, -1);
                                    attroff(COLOR_PAIR(120 + shade));
                                }
                                else {
                                    int glitch_pair = (glitch_state[i].glitch_color_idx * shades + shade + 1);
                                    attroff(COLOR_PAIR(glitch_pair));
                                }
                                attroff(A_BOLD);
                                attroff(A_REVERSE);
                            } else if (mix_mode_enabled && col_generation[i] == mix_mode_generation) {
                                int color_idx = mix_colors[i % mix_color_count];
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

                    /* Wrapping behavior bounds adjusted for both directions */
                    if (col_dir == 1 && head[i] - len[i] > rows) {
                        head[i] = -(rand() % rows);
                        len[i]  = 20 + rand() % MAX_TRAIL;
                        col_generation[i] = current_generation;
                    } else if (col_dir == -1 && head[i] + 1 < 0) {
                        head[i] = rows + len[i] + (rand() % rows);
                        len[i]  = 20 + rand() % MAX_TRAIL;
                        col_generation[i] = current_generation;
                    }

                    if (rand() % 1000 == 0) {
                        int pos = rand() % rows;
                        stream[i][pos] = 33 + rand() % 94;
                    }
                }

                /* 🌫️ fog */
                if (fog_enabled) {
                    for (int i = 0; i < FOG_PARTICLES; i++) {
                        attron(A_DIM);
                        mvaddch(fog[i].y, fog[i].x, '.');
                        attroff(A_DIM);

                        fog[i].y += dir * (rand() % 2);

                        if (fog[i].y >= rows) {
                            fog[i].y = 0;
                            fog[i].x = rand() % cols;
                        } else if (fog[i].y < 0) {
                            fog[i].y = rows - 1;
                            fog[i].x = rand() % cols;
                        }
                    }
                }
            }
        }

        wnoutrefresh(stdscr);
        doupdate();
        usleep(delay);
    }

    endwin();
    free(glitch_state);
    return 0;
}
