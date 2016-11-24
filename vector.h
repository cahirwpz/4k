#ifndef __VECTOR_H__
#define __VECTOR_H__

typedef struct { float x, y, z; } v3f;

void v3f_add(v3f *d, const v3f *a, const v3f *b);
void v3f_sub(v3f *d, const v3f *a, const v3f *b);
void v3f_cross(v3f *d, const v3f *a, const v3f *b);
float v3f_dot(const v3f *a, const v3f *b);
float v3f_length(const v3f *a);
float v3f_dist(const v3f *a, const v3f *b);
void v3f_scale(v3f *d, const v3f *a, float s);
void v3f_norm(v3f *d, const v3f *a, float l);
void v3f_lerp(v3f *d, const v3f *a, const v3f *b, float s);

#endif
