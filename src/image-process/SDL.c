#include "SDL.h"
#include "../utils/xmalloc.h"

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <err.h>
#include <time.h>

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
        errx(3, "%s", IMG_GetError());
    
    return img;
}

void display_image(SDL_Surface *img)
{
    SDL_Window* window = SDL_CreateWindow("First program", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, img->w + 500, img->h + 500, SDL_WINDOW_OPENGL);
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
            if (e.type == SDL_KEYDOWN)
                break;
        }
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
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

void set_pixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{    
    Uint32 * const target_pixel = (Uint32 *) ((Uint8 *) surface->pixels
                                             + y * surface->pitch
                                             + x * surface->format->BytesPerPixel);
    *target_pixel = pixel;
}

SDL_Surface *pixels_to_surface(struct Matrix *pixelMatrix)
{

    struct SDL_Surface *surface = SDL_CreateRGBSurface(0, pixelMatrix->cols, pixelMatrix->lines, 32, 0, 0, 0, 0);

    for (size_t i = 0; i < pixelMatrix->lines; ++i)
    {
        for (size_t j = 0; j < pixelMatrix->cols; ++j)
        {
            set_pixel(surface, j, i, (Uint32) pixelMatrix->matrix[i][j]);
        }
    }

    return surface;
}

SDL_Surface* SPG_CopySurface(SDL_Surface* src)
{
    return SDL_ConvertSurface(src, SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888), SDL_SWSURFACE);
}

float **image_to_grayscale_matrix(SDL_Surface *image_surface)
{
    int width = image_surface->w;
	int height = image_surface->h;
    Uint32 pixel;
	Uint8 r,g,b = 0;
	float avg = 0;

    float **grayscaled = xmalloc(height, sizeof(float*));

    for (int i = 0; i < height; ++i)
    {
        grayscaled[i] = xmalloc(width, sizeof(float));
        for (int j = 0; j < width; ++j)
        {
            pixel = getpixel(image_surface, j, i);
            SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);
            avg = 0.3 * r + 0.59 * g + 0.11 * b;
            grayscaled[i][j] = avg / 255;
        }
    }

    return grayscaled;
}

void grayscale(SDL_Surface* image_surface){
	int width = image_surface->w;
	int height = image_surface->h;
	Uint32 pixel;
	Uint8 r,g,b=0;
	Uint8 avg= 0;

	for(int i = 0; i<height; i++)
    {
		for(int j = 0; j<width; j++)
        {
			pixel = getpixel(image_surface, j, i);
			SDL_GetRGB(pixel, image_surface->format, &r, &g, &b);
			avg = 0.3 * r + 0.59 * g + 0.11 * b;
			r = avg;
			g = r;
			b = r;
			pixel = SDL_MapRGB(image_surface->format, r,g,b);
			set_pixel(image_surface,j, i, pixel);
		}
	}
}

SDL_Surface *init_and_copy_img(const char *path)
{
    init_sdl();
    SDL_Surface *img = load_image(path);
    SDL_Surface *copy = SPG_CopySurface(img);
    SDL_FreeSurface(img);
    return copy;
}