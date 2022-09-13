#include "SDL.h"

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_surface.h>
#include <err.h>

void init_sdl()
{

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) == -1)
        errx(1, "Could not initialize SDL: %s.\n", SDL_GetError());
}

SDL_Surface *load_image(const char *path)
{
    if (IMG_Init(IMG_INIT_PNG == 0))
        errx(1, "Error SDL2_image Initialization: %s.\n", IMG_GetError());

    SDL_Surface *img = IMG_Load(path);
    if (!img)
        errx(3, "Can't load %s: %s", path, IMG_GetError());
    
    return img;
}

void display_image(SDL_Surface *img)
{
    SDL_Window* window = SDL_CreateWindow("First program", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL);
    if (window == NULL) 
	    errx(3, "Error window creation");
	
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL)
        errx(3, "Error renderer creation");
    

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, img);
    if (texture == NULL)
        errx(6, "Error creating texture");

    while (1) {
        SDL_Event e;
        if (SDL_WaitEvent(&e)) 
        {
            if (e.type == SDL_QUIT)
                break;
        }
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_FreeSurface(img);
    IMG_Quit();
    SDL_Quit();
}

Uint32 getpixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

switch (bpp)
{
    case 1:
        return *p;
        break;

    case 2:
        return *(Uint16 *)p;
        break;

    case 3:
        if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
        {
            return p[0] | p[1] << 8 | p[2] << 16;
            break;
        }
        case 4:
            return *(Uint32 *)p;
            break;

        default:
            return 0;       /* shouldn't happen, but avoids warnings */
      }
}



/*
int main(void) {
    init_sdl();
    SDL_Surface *img = load_image("img/tigre.bmp");
    display_image(img);

    SDL_Color rgb;
    Uint32 data = getpixel(img, 200, 200);
    SDL_GetRGB(data, img->format, &rgb.r, &rgb.g, &rgb.b);

    printf("r: %d, g: %d, b: %d", rgb.r, rgb.g, rgb.b);
    return 0;
}
*/


