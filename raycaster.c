#include <SDL.h>
#include <SDL_image.h>

#include "common.h"
#include "effect.h"
#include "matrix.h"
#include "vector.h"

typedef struct {
  float azimuth, elevation;
  v3f eye;
  /* view plane */
  union {
    struct {
      v3f top_left, bot_left;
      v3f top_right, bot_right;
    };
    v3f view[4];
  };
} camera_t;

const int width = 640;
const int height = 480;

static SDL_Surface *texture;
static camera_t camera;

bool do_load() {
  SDL_Surface *image;
  const char *path = "data/rork-1.png";

  if (!(image = IMG_Load(path))) {
    SDL_Log("Failed to load '%s': %s\n", path, IMG_GetError());
    return false;
  }

  if (!(texture =
            SDL_ConvertSurfaceFormat(image, SDL_PIXELFORMAT_RGBA8888, 0))) {
    SDL_FreeSurface(image);
    SDL_Log("SDL_ConvertSurface: %s\n", SDL_GetError());
    return false;
  }

  SDL_FreeSurface(image);

  return true;
}

void do_unload() { SDL_FreeSurface(texture); }

void do_event(SDL_Event *ev) {
  if (ev->type == SDL_KEYDOWN) {
    SDL_Keycode key = ev->key.keysym.sym;
    if (key == SDLK_a)
      camera.eye.x -= 0.25 / FRAMERATE;
    if (key == SDLK_d)
      camera.eye.x += 0.25 / FRAMERATE;
    if (key == SDLK_w)
      camera.eye.y -= 0.25 / FRAMERATE;
    if (key == SDLK_s)
      camera.eye.y += 0.25 / FRAMERATE;
    if (key == SDLK_q)
      camera.eye.z -= 0.5 / FRAMERATE;
    if (key == SDLK_e)
      camera.eye.z += 0.5 / FRAMERATE;
  }

  if (ev->type == SDL_MOUSEMOTION) {
    Uint32 button = ev->motion.state;
    if (button & SDL_BUTTON_LMASK) {
      camera.azimuth += (float)ev->motion.xrel / width;
      camera.elevation += (float)ev->motion.yrel / height;
    }
  }
}

static void recalc_view(camera_t *camera) {
  float ratio = (float)height / (float)width;

  mx4f mx;
  mx4f_camera_view(&mx, camera->azimuth, camera->elevation, &camera->eye);

  camera->top_left = (v3f){-1.0, -ratio, 1.0};
  camera->bot_left = (v3f){-1.0, ratio, 1.0};
  camera->top_right = (v3f){1.0, -ratio, 1.0};
  camera->bot_right = (v3f){1.0, ratio, 1.0};

  for (int i = 0; i < 4; i++)
    mx4f_transform(&mx, &camera->view[i], &camera->view[i]);
}

static bool intersection(camera_t *camera, const v3f *ray, float *t) {
  float cx = camera->eye.x;
  float cy = camera->eye.y;
  float rx = ray->x;
  float ry = ray->y;

  bool inside = (cx * cx + cy * cy) < 1;

  float A = 2 * (rx * rx + ry * ry);
  float B = -2 * (rx * cx + ry * cy);
  float C = cx * cx + cy * cy - 1;
  float Z = B * B - 2 * A * C;

  if (A == 0.0f || Z < 0.0f)
    return false;

  float s = (inside ? (B + sqrtf(Z)) : (B - sqrtf(Z))) / A;

  if (s < 0.0f)
    return false;

  *t = s;
  return true;
}

static const float fallOff = 6.0;

static void calc_uv(v3f *eye, v3f *p, v3f *s) {
  float d = fabsf(p->z - eye->z) - fallOff;

  s->x = atan2(p->x, p->y) / (2 * M_PI);
  s->y = p->z / fallOff;
  s->z = (d > 0) ? constrain(d - 1.0, 0.0, 1.0) : 0.0;
}

static bool raycast(camera_t *camera, v3f *ray, v3f *p) {
  float t;

  if (intersection(camera, ray, &t)) {
    v3f tmp;
    v3f_scale(&tmp, ray, t);
    v3f_add(&tmp, &tmp, &camera->eye);
    calc_uv(&camera->eye, &tmp, p);
    return true;
  }

  return false;
}

static RGBA get_pixel(float u, float v) {
  RGBA *tex = (RGBA *)texture->pixels;

  u = frpart(u) * texture->w;
  v = frpart(v) * texture->h;

  int x0 = floorf(u);
  int y0 = floorf(v);
  int x1 = (x0 + 1) % texture->w;
  int y1 = (y0 + 1) % texture->h;
  float uf = u - x0;
  float vf = v - y0;

  RGBA p0 = tex[x0 + y0 * texture->w];
  RGBA p1 = tex[x1 + y0 * texture->w];
  RGBA p2 = tex[x0 + y1 * texture->w];
  RGBA p3 = tex[x1 + y1 * texture->w];

  float r = (p0.r * (1.0f - uf) + p1.r * uf) * (1.0f - vf) +
            (p2.r * (1.0f - uf) + p3.r * uf) * vf;
  float g = (p0.g * (1.0f - uf) + p1.g * uf) * (1.0f - vf) +
            (p2.g * (1.0f - uf) + p3.g * uf) * vf;
  float b = (p0.b * (1.0f - uf) + p1.b * uf) * (1.0f - vf) +
            (p2.b * (1.0f - uf) + p3.b * uf) * vf;

  return (RGBA){0, r, g, b};
}

void do_render(RGBA *pixels) {
  recalc_view(&camera);

  if (SDL_LockSurface(texture) < 0)
    SDL_Fail("SDL_LockSurface failed: %s", SDL_GetError());

  for (int y = 0; y < height; y++) {
    v3f left, right;

    v3f_lerp(&left, &camera.top_left, &camera.bot_left, (float)y / height);
    v3f_lerp(&right, &camera.top_right, &camera.bot_right, (float)y / height);

    for (int x = 0; x < width; x++) {
      v3f ray, p;
      v3f_lerp(&ray, &left, &right, (float)x / width);
      *pixels++ = raycast(&camera, &ray, &p) ? get_pixel(p.x, p.y) : (RGBA){0};
    }
  }

  SDL_UnlockSurface(texture);
}
