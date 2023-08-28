#pragma once

#include <stdlib.h>

typedef struct {
    int r: 8;
    int g: 8;
    int b: 8;
    int a: 8;
} Color;

#define COLOR(c) c.r, c.g, c.b, c.a
#define RANDCOLOR (Color){rand()%256, rand()%256, rand()%256, 255}


typedef struct {
    int window_width, window_height;
    int graphics_tps, physics_tps;

    float friction, gravity;

    Color bg_color;
} Settings;
