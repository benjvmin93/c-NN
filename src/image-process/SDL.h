#ifndef SDL_H
#define SDL_H

#include "SDL2/SDL.h"
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>

void init_sdl();
SDL_Surface *load_image(const char *path);
void display_image(SDL_Surface *img);


#endif