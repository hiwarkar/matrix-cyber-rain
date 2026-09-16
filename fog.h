#ifndef FOG_H
#define FOG_H

#define FOG_PARTICLES 100

typedef struct {
    int x, y;
} Fog;

/* Initialize fog */
Fog *fog_init(int cols, int rows);

/* Toggle fog on/off */
void fog_toggle(int *enabled);

/* Update and render fog */
void fog_update(Fog *fog, int fog_enabled, int cols, int rows, int dir);

#endif /* FOG_H */
