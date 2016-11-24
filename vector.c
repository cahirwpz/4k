#include <math.h>

#include "vector.h"

void v3f_add(v3f *d, const v3f *a, const v3f *b) {
  d->x = a->x + b->x;
  d->y = a->y + b->y;
  d->z = a->z + b->z;
}

void v3f_sub(v3f *d, const v3f *a, const v3f *b) {
  d->x = a->x - b->x;
  d->y = a->y - b->y;
  d->z = a->z - b->z;
}

void v3f_cross(v3f *d, const v3f *a, const v3f *b) {
  v3f w = {.x = a->y * b->z - b->y * a->z,
           .y = a->z * b->x - b->z * a->x,
           .z = a->x * b->y - b->x * a->y};

  d->x = w.x;
  d->y = w.y;
  d->z = w.z;
}

float v3f_dot(const v3f *a, const v3f *b) {
  return a->x * b->x + a->y * b->y + a->z * b->z;
}

float v3f_length(const v3f *a) {
  return sqrtf(a->x * a->x + a->y * a->y + a->z * a->z);
}

float v3f_dist(const v3f *a, const v3f *b) {
  v3f d;
  v3f_sub(&d, a, b);
  return v3f_length(&d);
}

void v3f_scale(v3f *d, const v3f *a, float s) {
  d->x = a->x * s;
  d->y = a->y * s;
  d->z = a->z * s;
}

void v3f_norm(v3f *d, const v3f *a, float l) {
  v3f_scale(d, a, l / v3f_length(a));
}

void v3f_lerp(v3f *d, const v3f *a, const v3f *b, float s) {
  v3f as, bs;

  v3f_scale(&as, a, 1.0f - s);
  v3f_scale(&bs, b, s);
  v3f_add(d, &as, &bs);
}
