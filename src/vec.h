#pragma once

typedef struct {
    float x, y;
} Vec;


#define vec_new(x, y) ((Vec){x, y})
#define vec_neg(a) ((Vec){-(a).x, -(a).y})
#define vec_add(a, b) ((Vec){(a).x+(b).x, (a).y+(b).y})
#define vec_sub(a, b) ((Vec){(a).x-(b).x, (a).y-(b).y})
#define vec_mul(a, val) ((Vec){(a).x*val, (a).y*val})
#define vec_mag(a) sqrt((a).x*(a).x + (a).y*(a).y)
#define vec_mag2(a) ((a).x*(a).x + (a).y*(a).y)
#define vec_dot(a, b) ((a).x*(b).x + (a).y*(b).y)

/*
Vec vec_new(float x, float y);
Vec vec_neg(Vec a);
Vec vec_add(Vec a, Vec b);
Vec vec_sub(Vec a, Vec b);
Vec vec_mul(Vec a, float val);
float vec_mag(Vec a);
float vec_mag2(Vec a);
float vec_dot(Vec a, Vec b);
*/
