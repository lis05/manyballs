#include <stdio.h>
#include <pthread.h>

#include <SDL2/SDL.h>

#include "vec.h"
#include "timer.h"
#include "util.h"
#include "ball.h"
#include "laws.h"

Settings s;

SDL_Window *window;
SDL_Renderer *renderer;

pthread_mutex_t phs_lock;
Timer phs_t;

void gph_init() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        exit(1);
    }

    window = SDL_CreateWindow("Many balls", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                                s.window_width, s.window_height, 0);
    if (window == NULL) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        exit(1);
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    if (renderer == NULL) {
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        exit(1);
    }
}

void *gph_mainloop(void *arg) {
    Settings *s = (Settings*)arg;

    Timer t;

    tm_init_SDL(&t, s->graphics_tps);

    SDL_Event event;
    while (1) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    exit(1);
                default:
                    break;
            }
        }

        int n = tm_tick_SDL(&t);
        while (n--) {
            // processing a frame

            SDL_SetRenderDrawColor(renderer, COLOR(s->bg_color));
            SDL_RenderClear(renderer);

            {
                for (int i = 0; i < bl_count(); i++) {
                    bl_draw(bl(i), renderer);
                }
            }

            SDL_RenderPresent(renderer);


            static char title[128];
            sprintf(title, "GPH: %d/%d, PHS: %d/%d", tm_tps_SDL(&t), s->graphics_tps, tm_tps_SYS(&phs_t), s->physics_tps); // last two TBD
            SDL_SetWindowTitle(window, title);

            tm_confirm(&t, 1);
        }
    }
}

void phs_init() {
    if (pthread_mutex_init(&phs_lock, NULL) != 0) {
        printf("Error creating phs_lock\n");
        exit(1);
    }
}

void *phs_mainloop(void *arg) {
    tm_init_SYS(&phs_t, s.physics_tps);

    while (1) {
        int n = tm_tick_SYS(&phs_t);
        while (n--) {
            // processing a frame

            pthread_mutex_lock(&phs_lock);
            {
                for (int i = 0; i < bl_count(); i++) apply_laws(BEFORE_TICK, bl(i));

                for (int i = 0; i < bl_count(); i++) apply_laws(BETWEEN_COLLISIONS_AND_UPDATES, bl(i));

                for (int i = 0; i < bl_count(); i++) {
                    bl_update(bl(i), phs_t.tick_time, s.friction);
                }

                bl_collide_all(phs_t.tick_time, &s);

                for (int i = 0; i < bl_count(); i++) apply_laws(AFTER_TICK, bl(i));
            }
            pthread_mutex_unlock(&phs_lock);



            tm_confirm(&phs_t, 1);
        }
    }
}

// laws
void bounds_law(Ball *b) {
    const float f = 0.7;

    if (b->pos.x - b->radius < 0) 
        b->speed.x = -b->speed.x, b->pos.x = b->radius;
    if (b->pos.x + b->radius > s.window_width) 
        b->speed.x = -b->speed.x, b->pos.x = s.window_width - b->radius;
    if (b->pos.y - b->radius < 0) 
        b->speed.y = -b->speed.y, b->pos.y = b->radius;
    if (b->pos.y + b->radius > s.window_height) {
        b->speed.y = -b->speed.y, b->pos.y = s.window_height - b->radius;
        b->speed.y *= f;
    }
}

void gravity_law(Ball *b) {
    bl_apply_force(b, vec_mul(vec_new(0, 1), s.gravity * b->mass), phs_t.tick_time);
}

int main() {
    s.window_width = 800;
    s.window_height = 800;

    s.graphics_tps = 60;
    s.physics_tps = 1200;

    s.friction = 0.00001;
    s.gravity = 1000;

    s.bg_color = (Color){30, 30, 30, 255};

    // laws
    register_law((Law){BEFORE_TICK, bounds_law});
    register_law((Law){BEFORE_TICK, gravity_law});

    // ==================
    gph_init(&s);
    phs_init();

    pthread_t gph_thread;
    pthread_t phs_thread;

    if (pthread_create(&gph_thread, NULL, gph_mainloop, (void*)&s) != 0) {
        printf("Error creating gph_thread\n");
        return 1;
    }

    if (pthread_create(&phs_thread, NULL, phs_mainloop, (void*)&s) != 0) {
        printf("Error creating phs_thread\n");
        return 1;
    }
    // ==================

    Timer t;
    tm_init_SYS(&t, 60);

    while (1) {
        int n = tm_tick_SYS(&t);
        while (n--) {
            int x, y;
            int mask = SDL_GetMouseState(&x, &y);
            if (mask & SDL_BUTTON(1)) {
                printf("%d\n", bl_init(vec_new(x, y), vec_new(-100 + rand()%200, 0), 1, 15, RANDCOLOR)->id);
            }
            tm_confirm(&t, 1);
        }
    }

    // ==================
    pthread_join(gph_thread, NULL);
    pthread_join(phs_thread, NULL);
}