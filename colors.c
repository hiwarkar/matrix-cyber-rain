#include <ncurses.h>
#include "colors.h"

void colors_init(ColorSystem *cs) {
    /* 🎨 Color schemes */
    int green[] = {22, 28, 34, 40, 46, 82, 118};
    int purple[] = {55, 56, 57, 92, 93, 98, 99};
    int red[] = {52, 88, 124, 160, 161, 196, 197};
    int blue[] = {18, 19, 20, 21, 27, 33, 39};
    int yellow[] = {58, 100, 136, 142, 148, 184, 226};
    int orange[] = {94, 130, 166, 172, 208, 214, 220};
    int cyan[] = {23, 29, 35, 41, 50, 87, 123};
    int magenta[] = {53, 89, 125, 127, 163, 165, 201};
    int pink[] = {175, 176, 177, 210, 211, 212, 217};

    cs->schemes[0] = green;
    cs->schemes[1] = purple;
    cs->schemes[2] = red;
    cs->schemes[3] = blue;
    cs->schemes[4] = yellow;
    cs->schemes[5] = orange;
    cs->schemes[6] = cyan;
    cs->schemes[7] = magenta;
    cs->schemes[8] = pink;

    cs->shades = 7;

    /* Pre-initialize color pairs for multiple generations */
    for (int g = 0; g < MAX_GENERATIONS; g++) {
        cs->gen_color_scheme[g] = green;  /* Default to green */
        for (int i = 0; i < cs->shades; i++)
            init_pair(g * cs->shades + i + 1, cs->gen_color_scheme[g][i], -1);
    }

    init_pair(50, COLOR_WHITE, -1);
}

int **colors_get_schemes(ColorSystem *cs) {
    return cs->schemes;
}

int *colors_get_gen_scheme(ColorSystem *cs, int gen) {
    int idx = gen % MAX_GENERATIONS;
    return cs->gen_color_scheme[idx];
}

void colors_set_gen_scheme(ColorSystem *cs, int gen, int *scheme) {
    int idx = gen % MAX_GENERATIONS;
    cs->gen_color_scheme[idx] = scheme;
    
    for (int i = 0; i < cs->shades; i++)
        init_pair(idx * cs->shades + i + 1, scheme[i], -1);
}
