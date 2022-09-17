#ifndef SDL_H
#define SDL_H

#include "SDL2/SDL.h"
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>

void init_sdl();
SDL_Surface *load_image(const char *path);
void display_image(SDL_Surface *img);
void grayscale(SDL_Surface *img);
SDL_Surface *SPG_CopySurface(SDL_Surface *img);
void filterImage(SDL_Surface *surface);
Uint32 getpixel(SDL_Surface *surface, int x, int y);
void set_pixel(SDL_Surface *surface, int x, int y, Uint32 pixel);

#endif