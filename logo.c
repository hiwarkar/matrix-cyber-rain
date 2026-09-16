#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include "logo.h"
#include "utils.h"

/* 💚 HMATRIX */
const char *hmatrix[] = {
    "  CCCCC   Y     Y   BBBBBB   EEEEEE  RRRRRRR  RRRRRRR  AAAAAA  IIIIII  NN   NN",
    "CC         Y   Y    BB   BB  EE      RR    RR RR    RR AA   AA   II    NNN  NN",
    "CC          Y Y     BBBBBB   EEEEE   RRRRRRR  RRRRRRR  AAAAAA    II    NN NN N",
    "CC           Y      BB   BB  EE      RR  RR   RR  RR   AA   AA   II    NN  NNN",
    " CCCCCC     Y       BBBBBB   EEEEEE  RR   RR  RR   RR  AA   AA IIIIII  NN   NN"
};

Particle *logo_init(int cols, int rows, int *particle_count) {
    int logo_width = strlen(hmatrix[0]);
    int logo_x = cols / 2 - logo_width / 2;
    int logo_y = rows / 2 - LOGO_H_HEIGHT / 2;

    int max_particles = logo_width * LOGO_H_HEIGHT;
    Particle *particles = malloc(sizeof(Particle) * max_particles);
    *particle_count = 0;

    int global_x_offset = rand() % 5;

    for (int i = 0; i < LOGO_H_HEIGHT; i++) {
        for (int j = 0; j < logo_width; j++) {
            if (hmatrix[i][j] == ' ') continue;

            particles[*particle_count].x = logo_x + j;
            particles[*particle_count].target_y = logo_y + i;

            if (hmatrix[i][j] == 'X') {
                particles[*particle_count].y = -(rows + global_x_offset);
                particles[*particle_count].is_x = 1;
            } else {
                particles[*particle_count].y = -(rand() % rows);
                particles[*particle_count].is_x = 0;
            }

            (*particle_count)++;
        }
    }

    return particles;
}

int logo_render_intro(Particle *particles, int particle_count, int *logo_phase,
                      long *logo_time, int rows, int cols) {
    if (*logo_phase >= 3) return 0;

    clear();

    int done = 1;

    for (int i = 0; i < particle_count; i++) {
        Particle *p = &particles[i];

        if (*logo_phase == 0) {
            if (p->y < p->target_y) {
                p->y++;
                done = 0;
            }
        }

        if (*logo_phase == 2) {
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

    if (*logo_phase == 0 && done) {
        *logo_phase = 1;
        *logo_time = now_us();
    }

    if (*logo_phase == 1 && now_us() - *logo_time > 2000000)
        *logo_phase = 2;

    if (*logo_phase == 2 && particles[0].y > rows) {
        *logo_phase = 3;
        clear();
    }

    return 1;
}
