#include "SDL.h"

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

void set_pixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{    
  Uint32 * const target_pixel = (Uint32 *) ((Uint8 *) surface->pixels
                                             + y * surface->pitch
                                             + x * surface->format->BytesPerPixel);
  *target_pixel = pixel;
}

SDL_Surface* SPG_CopySurface(SDL_Surface* src)
{
    return SDL_ConvertSurface(src, SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888), SDL_SWSURFACE);
}

void applyFilter(SDL_Surface *surface, int x, int y, int **filter)
{

    Uint32 *pixels = surface->pixels;
    Uint32 *p = pixels + (y * surface->w + x);

    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            if (!p + (i * 3 + j))
                break;
            
            printf("%d\n", filter[i][j]);
            set_pixel(surface, x, y, p[j * 3 + i] * filter[i][j]);
        }
    }

}

void filterImage(SDL_Surface *surface)
{
    int **filter = malloc(3 * sizeof(int*));
    for (int i = 0; i < 3; ++i)
    {
        filter[i] = malloc(3 * sizeof(int));
    }

    srand(time(NULL));
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
            filter[i][j] = rand() % 2;
    }

    for (int i = 0; i < surface->w; ++i) 
    {
        for (int j = 0; j < surface->h; ++j)
        {
            applyFilter(surface, i, j, filter);
        }
    }
    for (int i = 0; i < 3; ++i)
    {
        free(filter[i]);
    }
    free(filter);
}

int main(void) {
    init_sdl();
    SDL_Surface *img = load_image("img/tigre.bmp");
    display_image(img);

    // Uint32 *pixels = img->pixels;

    


   /* SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(0, img->w, img->h, img->, SDL_PIXELFORMAT_RGBA8888);

    for (int i = 0; i < img->w; ++i)
    {
        if (i % 100 == 0)
                display_image(surface);
        for (int j = 0; j < img->h; ++j)
        {
            printf("img height: %d\nimg_weight: %d\n", img->h, img->w);

            printf("i: %d, j: %d\n", i, j);
            set_pixel(surface, i, j, pixels[i * img->w + j]);
            
        }
    }*/


    SDL_Surface *copy = SPG_CopySurface(img);

    display_image(copy);
    int i = 5;
    while (i--)
    {
        filterImage(copy);
        display_image(copy);
    }


   /* SDL_Renderer *renderer = NULL;
    SDL_Texture *texture = NULL;
    SDL_PixelFormat *format = NULL;

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 3, 3);
    format = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888);
    SDL_LockTexture(texture, NULL, &tmp, &pitch);
    pixels = tmp;
    for (int i = 0; i < img->h; ++i)
    {
        for (int j = 0; j < img->w; ++j)
            pixels[i * img->w + j] = SDL_MapRGBA(format, (Uint8)i, 0, 0, 255);
    }

    SDL_UnlockTexture(texture);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

    SDL_FreeFormat(format);*/

   /* printf("height: %d, weight: %d\n", img->h, img->w);
    for (int i = 0; i < img->w; ++i) 
    {
        for (int j = 0; j < img->h; ++j)
        {
            Uint32 data = getpixel(img, i, j);
            SDL_GetRGB(data, img->format, &rgb.r, &rgb.g, &rgb.b);
            printf("%d, %d, r: %d, g: %d, b: %d\n", i, j, rgb.r, rgb.g, rgb.b);
        }
    }
*/
    SDL_FreeSurface(img);
    SDL_FreeSurface(copy);


    return 0;
}


