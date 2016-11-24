#include <assert.h>
#include <math.h>

#include "common.h"
#include "matrix.h"

#define M_FOREACH(I, J)                                                        \
  for ((I) = 0; (I) < 4; (I)++)                                                \
    for ((J) = 0; (J) < 4; (J)++)

#define M(A, I, J) (*A)[I][J]

void mx4f_mult(mx4f *d, const mx4f *a, const mx4f *b) {
  int i, j, k;

  M_FOREACH(i, j) {
    M(d, i, j) = 0.0f;

    for (k = 0; k < 4; k++)
      M(d, i, j) += M(a, i, k) * M(b, k, j);
  }
}

void mx4f_transpose(mx4f *d, const mx4f *a) {
  int i, j;

  M_FOREACH(i, j)
  M(d, i, j) = M(a, j, i);
}

void mx4f_identity(mx4f *d) {
  int i, j;

  M_FOREACH(i, j)
  M(d, i, j) = (i == j) ? 1.0f : 0.0f;
}

void mx4f_rotation(mx4f *d, float angleX, float angleY, float angleZ) {
  mx4f_identity(d);

  angleX *= M_PI / 180.0f;
  angleY *= M_PI / 180.0f;
  angleZ *= M_PI / 180.0f;

  {
    float sinX = sinf(angleX);
    float cosX = cosf(angleX);
    float sinY = sinf(angleY);
    float cosY = cosf(angleY);
    float sinZ = sinf(angleZ);
    float cosZ = cosf(angleZ);

    M(d, 0, 0) = cosY * cosZ;
    M(d, 0, 1) = cosY * sinZ;
    M(d, 0, 2) = -sinY;
    M(d, 1, 0) = sinX * sinY * cosZ - cosX * sinZ;
    M(d, 1, 1) = sinX * sinY * sinZ + cosX * cosZ;
    M(d, 1, 2) = sinX * cosY;
    M(d, 2, 0) = cosX * sinY * cosZ + sinX * sinZ;
    M(d, 2, 1) = cosX * sinY * sinZ - sinX * cosZ;
    M(d, 2, 2) = cosX * cosY;
  }
}

void mx4f_scale(mx4f *d, float scaleX, float scaleY, float scaleZ) {
  mx4f_identity(d);

  M(d, 0, 0) = scaleX;
  M(d, 1, 1) = scaleY;
  M(d, 2, 2) = scaleZ;
}

void mx4f_translate(mx4f *d, float moveX, float moveY, float moveZ) {
  mx4f_identity(d);

  M(d, 3, 0) = moveX;
  M(d, 3, 1) = moveY;
  M(d, 3, 2) = moveZ;
}

void mx4f_frustum(mx4f *d, float left, float right, float top, float bottom,
                  float near, float far) {
  assert(near > 0 && far > 0);

  mx4f_identity(d);

  M(d, 0, 0) = 2 * near / (right - left);
  M(d, 0, 2) = (right + left) / (right - left);
  M(d, 1, 1) = 2 * near / (top - bottom);
  M(d, 1, 2) = (top + bottom) / (top - bottom);
  M(d, 2, 2) = -(far + near) / (far - near);
  M(d, 2, 3) = -2 * far * near / (far - near);
  M(d, 3, 2) = -1;
  M(d, 3, 3) = 0;
}

void mx4f_perspective(mx4f *d, float fovy, float aspect, float near,
                      float far) {
  float fH = (float)tanf(radians(fovy / 2)) * near;
  float fW = fH * aspect;

  return mx4f_frustum(d, -fW, fW, -fH, fH, near, far);
}

void mx4f_transform(const mx4f *m, const v3f *src, v3f *dst) {
  float x = src->x;
  float y = src->y;
  float z = src->z;

  dst->x = M(m, 0, 0) * x + M(m, 1, 0) * y + M(m, 2, 0) * z + M(m, 3, 0);
  dst->y = M(m, 0, 1) * x + M(m, 1, 1) * y + M(m, 2, 1) * z + M(m, 3, 1);
  dst->z = M(m, 0, 2) * x + M(m, 1, 2) * y + M(m, 2, 2) * z + M(m, 3, 2);
}

void mx4f_camera(mx4f *camera, const v3f *direction, const v3f *upwards,
                 const v3f *position) {
  v3f d, u, r;

  v3f_norm(&d, direction, 1.0f);
  v3f_norm(&u, upwards, 1.0f);
  v3f_cross(&r, &u, &d);
  v3f_norm(&r, &r, 1.0f);

  M(camera, 0, 0) = r.x;
  M(camera, 1, 0) = r.y;
  M(camera, 2, 0) = r.z;
  M(camera, 3, 0) = 0.0f;
  M(camera, 0, 1) = u.x;
  M(camera, 1, 1) = u.y;
  M(camera, 2, 1) = u.z;
  M(camera, 3, 1) = 0.0f;
  M(camera, 0, 2) = d.x;
  M(camera, 1, 2) = d.y;
  M(camera, 2, 2) = d.z;
  M(camera, 3, 2) = 0.0f;
  M(camera, 0, 3) = position->x;
  M(camera, 1, 3) = position->y;
  M(camera, 2, 3) = position->z;
  M(camera, 3, 3) = 1.0f;
}

void mx4f_camera_view(mx4f *camera, float azimuth, float elevation,
                      const v3f *position) {
  v3f direction = {.x = sin(elevation) * cos(azimuth),
                   .y = sin(elevation) * sin(azimuth),
                   .z = cos(elevation)};
  const v3f right = {1.0f, 0.0f, 0.0f};
  v3f upwards;

  v3f_cross(&upwards, &direction, &right);
  v3f_norm(&upwards, &upwards, 1.0f);

  mx4f_camera(camera, &direction, &upwards, position);
}

/* http://3dgep.com/understanding-the-view-matrix/#Look_At_Camera */
void mx4f_camera_look_at(mx4f *camera, const v3f *eye, const v3f *target,
                         const v3f *up) {
  v3f xaxis, yaxis, zaxis;

  v3f_sub(&zaxis, eye, target);
  v3f_norm(&zaxis, &zaxis, 1.0f);
  v3f_cross(&xaxis, up, &zaxis);
  v3f_norm(&xaxis, &xaxis, 1.0f);
  v3f_cross(&yaxis, &zaxis, &xaxis);

  mx4f_identity(camera);

  M(camera, 0, 0) = xaxis.x;
  M(camera, 0, 1) = yaxis.x;
  M(camera, 0, 2) = zaxis.x;
  M(camera, 1, 0) = xaxis.y;
  M(camera, 1, 1) = yaxis.y;
  M(camera, 1, 2) = zaxis.y;
  M(camera, 2, 0) = xaxis.z;
  M(camera, 2, 1) = yaxis.z;
  M(camera, 2, 2) = zaxis.z;
  M(camera, 3, 0) = -v3f_dot(&xaxis, eye);
  M(camera, 3, 1) = -v3f_dot(&yaxis, eye);
  M(camera, 3, 2) = -v3f_dot(&zaxis, eye);
}
