#include <stdlib.h>
#include <ncurses.h>
#include "fog.h"

Fog *fog_init(int cols, int rows) {
    Fog *fog = malloc(sizeof(Fog) * FOG_PARTICLES);
    for (int i = 0; i < FOG_PARTICLES; i++) {
        fog[i].x = rand() % cols;
        fog[i].y = rand() % rows;
    }
    return fog;
}

void fog_toggle(int *enabled) {
    *enabled = !(*enabled);
}

void fog_update(Fog *fog, int fog_enabled, int cols, int rows, int dir) {
    if (!fog_enabled) return;

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
