#ifndef COLORS_H
#define COLORS_H

#define MAX_GENERATIONS 12
#define NUM_COLOR_SCHEMES 9

typedef struct {
    int *schemes[NUM_COLOR_SCHEMES];
    int *gen_color_scheme[MAX_GENERATIONS];
    int shades;
} ColorSystem;

/* Initialize color system */
void colors_init(ColorSystem *cs);

/* Get color schemes */
int **colors_get_schemes(ColorSystem *cs);

/* Get generation color scheme */
int *colors_get_gen_scheme(ColorSystem *cs, int gen);

/* Set generation scheme */
void colors_set_gen_scheme(ColorSystem *cs, int gen, int *scheme);

#endif /* COLORS_H */
