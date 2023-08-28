#pragma once

#include <stdint.h>

typedef struct {
    long double first_tick;
    long double last_tick;
    long double tick_time;
    int ticks;
} Timer;


void tm_init_SDL(Timer *t, int tps);
void tm_init_SYS(Timer *t, int tps);
int tm_tick_SDL(Timer *t);
int tm_tick_SYS(Timer *t);
void tm_confirm(Timer *t, int n);
int tm_tps_SDL(Timer *t);
int tm_tps_SYS(Timer *t);