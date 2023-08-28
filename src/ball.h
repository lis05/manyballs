#pragma once

#include <SDL2/SDL.h>

#include "vec.h"
#include "util.h"


typedef struct {
    Vec pos, speed;
    float mass;

    float radius;
    Color color;

    int id;
} Ball;

Ball *bl_init(Vec pos, Vec speed, float mass, float radius, Color color);
void bl_apply_force(Ball *b, Vec force, float time);
void bl_update(Ball *b, float time, float friction);
void bl_collide_all(float time, Settings *s);
Ball *bl(int id);
int bl_count();

void bl_draw(Ball *b, SDL_Renderer *renderer);