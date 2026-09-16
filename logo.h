#ifndef LOGO_H
#define LOGO_H

#define LOGO_H_HEIGHT 5

typedef struct {
    int x, y;
    int target_y;
    int is_x;
} Particle;

/* Initialize logo particles */
Particle *logo_init(int cols, int rows, int *particle_count);

/* Render intro animation */
int logo_render_intro(Particle *particles, int particle_count, int *logo_phase, 
                      long *logo_time, int rows, int cols);

#endif /* LOGO_H */
