#ifndef __MATRIX_H__
#define __MATRIX_H__

#include "vector.h"

typedef float mx4f[4][4];

void mx4f_mult(mx4f *d, const mx4f *a, const mx4f *b);
void mx4f_transpose(mx4f *d, const mx4f *a);
void mx4f_identity(mx4f *d);
void mx4f_rotation(mx4f *d, float angleX, float angleY, float angleZ);
void mx4f_scale(mx4f *d, float scaleX, float scaleY, float scaleZ);
void mx4f_translate(mx4f *d, float moveX, float moveY, float moveZ);

void mx4f_frustum(mx4f *d, float left, float right, float top, float bottom,
                  float near, float far);
void mx4f_perspective(mx4f *d, float fovy, float aspect, float near, float far);

void mx4f_transform(const mx4f *m, const v3f *src, v3f *dst);

void mx4f_camera(mx4f *camera, const v3f *direction, const v3f *upwards,
                 const v3f *position);
void mx4f_camera_view(mx4f *camera, float azimuth, float elevation,
                      const v3f *position);
void mx4f_camera_look_at(mx4f *camera, const v3f *eye, const v3f *target,
                         const v3f *up);

#endif
