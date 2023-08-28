#include <string.h>
#include <pthread.h>

#include "ball.h"
#include "sdl_circle.h"

typedef struct {
    Ball *arr[1000];
    int len;
} BallVec;


void bv_init(BallVec *bv) {
    bv->len = 0;
}
void bv_add(BallVec *bv, Ball*b) {
    bv->arr[bv->len++] = b;
}
 
#define NUM_BALLS 30000
static Ball balls[NUM_BALLS];
static int next_ball;


Ball *bl_init(Vec pos, Vec speed, float mass, float radius, Color color) {
    balls[next_ball].pos = pos;
    balls[next_ball].speed = speed;
    balls[next_ball].mass = mass;
    balls[next_ball].radius = radius;
    balls[next_ball].color = color;
    balls[next_ball].id = next_ball;

    return &balls[next_ball++];
}

void bl_apply_force(Ball *b, Vec force, float time) {
    // f = ma -> a = f/m; dv = at = f*t/m
    b->speed = vec_add(b->speed, vec_mul(force, time/b->mass));
}

void bl_update(Ball *b, float time, float friction) {
    b->speed = vec_mul(b->speed, 1-friction);
    b->pos = vec_add(b->pos, vec_mul(b->speed, time));
}

static int bl_check_collision(Ball *a, Ball *b) {
    return (1ll * (a->radius + b->radius) * (a->radius + b->radius))
            >
            vec_mag2(vec_sub(a->pos, b->pos));
}

static int bl_check_collision_passive(Ball *a, Ball *b) {
    return (1ll * (a->radius + b->radius) * (a->radius + b->radius)) -1
            >
            vec_mag2(vec_sub(a->pos, b->pos));
}

static int bl_check_collision_aggressive(Ball *a, Ball *b) {
    return (1ll * (a->radius + b->radius) * (a->radius + b->radius)) +1
            >
            vec_mag2(vec_sub(a->pos, b->pos));
}

static void bl_update_frictionless(Ball *b, float time) {
    b->pos = vec_add(b->pos, vec_mul(b->speed, time));
}

#define COLLISION_STEPS 1
float bl_determine_collision_time(Ball *a, Ball *b, float time) {
    float t = 0;
    float step = time/2;

    for (int i = 0; i < COLLISION_STEPS; i++, step *= 0.5) {

        Vec aspeed = a->speed;
        Vec bspeed = b->speed;
        Vec apos = a->pos;
        Vec bpos = b->pos;

        a->pos = vec_add(a->pos, vec_mul(a->speed, t+step));
        b->pos = vec_add(b->pos, vec_mul(b->speed, t+step));

        if (!bl_check_collision(a, b)) {
            t += step;
        }

        a->speed = aspeed;
        b->speed = bspeed;
        a->pos = apos;
        b->pos = bpos;
    }

    return t;
}
static void bl_collide(Ball *a, Ball *b, float time) {
    if (vec_mag2(vec_sub(a->pos, b->pos)) >= 1.1*(a->radius + b->radius)*(a->radius + b->radius))return;

    // 1 if they collide this frame
    int before = bl_check_collision_passive(a, b);

    Vec aspeed = a->speed;
    Vec bspeed = b->speed;
    Vec apos = a->pos;
    Vec bpos = b->pos;

    bl_update_frictionless(a, time);
    bl_update_frictionless(b, time);

    // 1 if they collide the next frame
    int after = bl_check_collision_aggressive(a, b);
    
    a->speed = aspeed;
    b->speed = bspeed;
    a->pos = apos;
    b->pos = bpos;

    if (before && after) {
        // they are stuck together
        Vec dist = vec_sub(a->pos, b->pos);
        Vec dist2 = vec_mul(dist, sqrt((a->radius + b->radius)*(a->radius + b->radius)/vec_mag2(dist)));

        dist = vec_mul(dist, 0.5);
        dist2 = vec_mul(dist2, 0.501);
        a->pos = vec_add(vec_sub(a->pos, dist), dist2);
        b->pos = vec_sub(vec_add(b->pos, dist), dist2);
        a->speed = vec_new(0, 0);
        b->speed = vec_new(0, 0);
    } else if (before < after) {
        // they are gonna collide

        /*float collision_time = bl_determine_collision_time(a, b, time);

        bl_update_frictionless(a, collision_time);
        bl_update_frictionless(b, collision_time);*/

        // https://en.wikipedia.org/wiki/Elastic_collision

        Vec v1 = a->speed;
        Vec v2 = b->speed;
        Vec x1 = a->pos;
        Vec x2 = b->pos;
        float m1 = a->mass;
        float m2 = b->mass;

        Vec x12 = vec_sub(x1, x2);
        Vec x21 = vec_neg(x12);
        Vec v12 = vec_sub(v1, v2);
        Vec v21 = vec_neg(v12);


        a->speed = vec_sub(v1, vec_mul(x12, 2*m2*vec_dot(v12, x12)/(m1+m2)/vec_mag2(x12)));
        b->speed = vec_sub(v2, vec_mul(x21, 2*m1*vec_dot(v21, x21)/(m1+m2)/vec_mag2(x21)));
    }
}


Ball buff[30000];
Ball buff2[30000];

#define WINDOW 60

int comp(int a, int b) { // 1 if a < b
    int window1 = ((int)buff[a].pos.x)/WINDOW;
    int window2 = ((int)buff[b].pos.x)/WINDOW;

    if (window1 != window2) return window1 < window2;
    return buff[a].pos.y < buff[b].pos.y;
}

void merge(int l1, int r1, int l2, int r2) {
    int pos = 0;
    while (l1 <= r1 && l2 <= r2) {
        if (comp(l1, l2)) {
            buff2[pos++] = buff[l1++];
        } else {
            buff2[pos++] = buff[l2++];
        }
    }
    
    while (l1 <= r1) buff2[pos++] = buff[l1++];
    while (l2 <= r2) buff2[pos++] = buff[l2++];
}

void sort_balls(int l, int r) {
    if (r <= l) return;
    int m = (l+r)/2;

    sort_balls(l, m);
    sort_balls(m+1, r);

    merge(l, m, m+1, r);
    memcpy(buff+l, buff2, (sizeof buff[0]) * (r-l+1));
}

void bl_collide_all(float time, Settings *s) {
    for (int i = 0; i < next_ball; i++) buff[i] = balls[i];
    sort_balls(0, next_ball - 1);

    for (int i = 0; i < next_ball; i++) {
        for (int ii = i+1; ii < next_ball; ii++) {
            if ((buff[i].pos.x - buff[ii].pos.x) * (buff[i].pos.x - buff[ii].pos.x) > 2 * WINDOW * WINDOW) break;
            bl_collide(&balls[buff[i].id], &balls[buff[ii].id], time);
        }
    }
}

Ball *bl(int id) {
    return &balls[id];
}
int bl_count() {
    return next_ball;
}

void bl_draw(Ball *b, SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, COLOR(b->color));
    SDL_RenderFillCircle(renderer, b->pos.x, b->pos.y, b->radius);
}