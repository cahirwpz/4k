#include <SDL.h>
#include <SDL_image.h>

#include "effect.h"

static SDL_Window *window;
static SDL_Renderer *renderer;
static SDL_Texture *texture;
static SDL_TimerID timer;

static unsigned frame = 0;

static Uint32 count_frames(Uint32 interval, void *unused) {
  frame++;
  return interval;
}

static void gfx_end() {
  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
}

void gfx_init() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0)
    SDL_Fail("Couldn't initialize SDL: %s", SDL_GetError());

  atexit(SDL_Quit);

  if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG)
    SDL_Fail("Couldn't initialize SDL_image: %s", IMG_GetError());

  atexit(IMG_Quit);

  SDL_DisplayMode dm;

  if (SDL_GetDesktopDisplayMode(0, &dm) < 0)
    SDL_Fail("SDL_GetDesktopDisplayMode failed: %s", SDL_GetError());

  int win_x = (dm.w - width) / 2;
  int win_y = (dm.h - height) / 2;

  window = SDL_CreateWindow("Raycaster", win_x, win_y, width, height, 0);
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                              SDL_TEXTUREACCESS_STREAMING, width, height);
  timer = SDL_AddTimer(20, count_frames, NULL);
  atexit(gfx_end);

  SDL_assert(texture != NULL);
}

void gfx_run() {
  while (true) {
    int pitch;
    RGBA *pixels;

    if (SDL_LockTexture(texture, NULL, (void **)&pixels, &pitch) < 0)
      SDL_Fail("SDL_LockTexture failed: %s", SDL_GetError());

    do_render(pixels);

    SDL_UnlockTexture(texture);

    if (SDL_RenderCopy(renderer, texture, NULL, NULL) < 0)
      SDL_Fail("SDL_RenderCopy failed: %s", SDL_GetError());

    SDL_RenderPresent(renderer);

    SDL_Event ev;

    while (SDL_PollEvent(&ev)) {
      if (ev.type == SDL_QUIT)
        return;
      do_event(&ev);
    }
  }
}

int main(int argc, char **argv) {
  gfx_init();
  if (do_load())
    gfx_run();
  do_unload();
  return 0;
}
