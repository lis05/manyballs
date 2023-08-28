#include "timer.h"

#include <SDL2/SDL.h>
#include <sys/time.h>

static long double get_SDL_ticks() {
    return SDL_GetTicks64() * 1e-3l;
}

static long double get_SYS_ticks() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec * 1e-6l;
}


void tm_init_SDL(Timer *t, int tps) {
    t->first_tick = t->last_tick = get_SDL_ticks();
    t->tick_time = 1.0l / tps;
    t->ticks = 0;
}
void tm_init_SYS(Timer *t, int tps) {
    t->first_tick = t->last_tick = get_SYS_ticks();
    t->tick_time = 1.0l / tps;
    t->ticks = 0;
}
int tm_tick_SDL(Timer *t) {
    long double cur = get_SDL_ticks();
    int n = (cur - t->last_tick) / t->tick_time;
    t->last_tick += n * t->tick_time;

    if (cur - t->first_tick > 1) {
        t->first_tick = t->last_tick;
        t->ticks = 0;
    }

    return n;
}
int tm_tick_SYS(Timer *t) {
    long double cur = get_SYS_ticks();
    int n = (cur - t->last_tick) / t->tick_time;
    t->last_tick += n * t->tick_time;

    if (cur - t->first_tick > 1) {
        t->first_tick = t->last_tick;
        t->ticks = 0;
    }

    return n;
}
void tm_confirm(Timer *t, int n) {
    t->ticks += n;
}
int tm_tps_SDL(Timer *t) {
    return t->ticks / (get_SDL_ticks() - t->first_tick);
}
int tm_tps_SYS(Timer *t) {
    return t->ticks / (get_SYS_ticks() - t->first_tick);
}