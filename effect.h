#ifndef __EFFECT_H__
#define __EFFECT_H__

#include "common.h"
#include <SDL.h>

#define FRAMERATE 50

extern const int width;
extern const int height;

extern void do_event(SDL_Event *ev);
extern void do_render(RGBA *pixels);
extern bool do_load();
extern void do_unload();

#endif
