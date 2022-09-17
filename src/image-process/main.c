#include "../utils/matrix.h"
#include "SDL.h"

#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_surface.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

SDL_Surface *init_and_copy_img(const char *path)
{
    init_sdl();
    SDL_Surface *img = load_image(path);
    SDL_Surface *copy = SPG_CopySurface(img);
    SDL_FreeSurface(img);
    return copy;
}

void **init_matrices_and_set_pixels(SDL_Surface *copy)
{
    Uint8 r, g, b;
    struct Matrix* pixels = init_matrix(copy->w, copy->h);

    struct Matrix *redPixels = init_matrix(copy->w, copy->h);
    struct Matrix *greenPixels = init_matrix(copy->w, copy->h);
    struct Matrix *bluePixels = init_matrix(copy->w, copy->h);
    
    for (int i = 0; i < copy->w; ++i)
    {
        if (i == copy->w / 2)
            display_image(copy);
        for (int j = 0; j < copy->h; ++j)
        {
            Uint32 pixel = getpixel(copy, i, j);
            SDL_GetRGB(pixel, copy->format, &r, &g, &b);
            
            setElement(pixels, pixel, i, j);
            setElement(redPixels, r, i, j);
            setElement(greenPixels, g, i, j);
            setElement(bluePixels, b, i, j);
        }
    }

    void **matrices = malloc(5 * sizeof(struct Matrix *));

    matrices[4] = NULL;
    matrices[0] = pixels;
    matrices[1] = redPixels;
    matrices[2] = greenPixels;
    matrices[3] = bluePixels;

    return matrices;
}

void free_pixels_matrices(void **matrices)
{
    for(int i = 0; matrices[i]; ++i)
    {
        free_matrix(matrices[i]);
    }

    free(matrices);

}

int main(void)
{   
    SDL_Surface *copy = init_and_copy_img("img/tigre.bmp");

    display_image(copy);

    // grayscale(copy);

    void **matrices = init_matrices_and_set_pixels(copy);

    


    free_pixels_matrices(matrices);
 
    SDL_FreeSurface(copy);

    return 0;
}