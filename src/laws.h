#pragma once

#include "ball.h"

typedef struct {
    int place;
    void (*f)(Ball*);
} Law;

void register_law(Law l);
void apply_laws(int place, Ball *b);

#define BEFORE_TICK 0
#define BETWEEN_COLLISIONS_AND_UPDATES 1
#define AFTER_TICK 2