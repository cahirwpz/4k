#ifndef __COMMON_H__
#define __COMMON_H__

#include <math.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct { uint8_t a, b, g, r; } RGBA;

#define SDL_Fail(...)                                                          \
  {                                                                            \
    SDL_Log(__VA_ARGS__);                                                      \
    exit(EXIT_FAILURE);                                                        \
  }

static inline float frpart(float x) {
  float i;
  return modff(x, &i);
}

static inline float radians(float a) { return a * 180.0f / M_PI; }

static inline float constrain(float v, float lo, float hi) {
  if (v < lo)
    return lo;
  if (v > hi)
    return hi;
  return v;
}

#endif
