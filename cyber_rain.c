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

long now_us() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000 + tv.tv_usec;
}

typedef struct {
    int x, y;
} Fog;

typedef struct {
    int x, y, target_y;
    int is_x;
} Particle;

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

    int *color_schemes[] = {green_shades, purple_shades, red_shades, blue_shades};
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

        /* Change rain color with 'c' key - only if intro is done */
        if (ch == 'c' && logo_phase >= 3) {
            /* Increment generation */
            current_generation++;

            /* Generate new random color scheme (ensure it's different from last) */
            int color_choice = rand() % 4;
            while (color_choice == last_color_choice) {
                color_choice = rand() % 4;
            }
            last_color_choice = color_choice;
            int *new_scheme = color_schemes[color_choice];

            /* Update the color pair set for this generation (12 slots cycling for buffer) */
            int gen_idx = current_generation % MAX_GENERATIONS;
            gen_color_scheme[gen_idx] = new_scheme;

            for (int i = 0; i < shades; i++)
                init_pair(gen_idx * shades + i + 1, gen_color_scheme[gen_idx][i], -1);

            color_change_time = now_us();
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

            /* 🌧️ MATRIX RAIN & FOG (MODIFIED FOR ACCUMULATOR & REVERSE) */
            rain_accumulator += rain_speed;

            while (abs(rain_accumulator) >= 10) {
                int dir = (rain_accumulator > 0) ? 1 : -1;
                rain_accumulator -= dir * 10;

                for (int i = 0; i < cols; i++) {

                    /* Dynamic erase bounds depending on direction */
                    int erase_y = (dir == 1) ? (head[i] - len[i]) : (head[i] + 1);
                    if (erase_y >= 0 && erase_y < rows)
                        mvaddch(erase_y, i, ' ');

                    head[i] += dir;

                    if (head[i] >= 0 && head[i] < rows) {
                        attron(COLOR_PAIR(50) | A_BOLD);
                        mvaddch(head[i], i, stream[i][head[i] % rows]);
                        attroff(COLOR_PAIR(50) | A_BOLD);

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
                            attron(COLOR_PAIR(shade + pair_offset));
                            mvaddch(y, i, stream[i][y % rows]);
                            attroff(COLOR_PAIR(shade + pair_offset));
                        }
                    }

                    /* Wrapping behavior bounds adjusted for both directions */
                    if (dir == 1 && head[i] - len[i] > rows) {
                        head[i] = -(rand() % rows);
                        len[i]  = 20 + rand() % MAX_TRAIL;
                        col_generation[i] = current_generation;
                    } else if (dir == -1 && head[i] + 1 < 0) {
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
    return 0;
}
