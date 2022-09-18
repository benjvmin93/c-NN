#include "NeuralNetwork/neuralNet.h"
#include "image-process/SDL.h"
#include "utils/matrix.h"

#include <stdio.h>


SDL_Surface *init_and_copy_img(const char *path)
{
    init_sdl();
    SDL_Surface *img = load_image(path);
    SDL_Surface *copy = SPG_CopySurface(img);
    SDL_FreeSurface(img);
    return copy;
}

struct Matrix **init_matrices_and_set_pixels(SDL_Surface *copy)
{
    Uint8 r, g, b;
    printf("Matrix of pixels: ");
    struct Matrix* pixels = init_matrix(copy->w, copy->h);

    printf("Matrix of red pixels: ");
    struct Matrix *redPixels = init_matrix(copy->w, copy->h);
    printf("Matrix of green pixels: ");
    struct Matrix *greenPixels = init_matrix(copy->w, copy->h);
    printf("Matrix of blue pixels: ");
    struct Matrix *bluePixels = init_matrix(copy->w, copy->h);
    
    for (int i = 0; i < copy->w; ++i)
    {
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

    struct Matrix **matrices = malloc(5 * sizeof(struct Matrix *));

    matrices[4] = NULL;
    matrices[0] = pixels;
    matrices[1] = redPixels;
    matrices[2] = greenPixels;
    matrices[3] = bluePixels;

    return matrices;
}

void free_pixels_matrices(struct Matrix **matrices)
{
    for(int i = 0; matrices[i]; ++i)
    {
        free_matrix(matrices[i]);
    }

    free(matrices);
}

int main(void)
{   
    SDL_Surface *copy = init_and_copy_img("tests/images/cats/images.jpg");

    // display_image(copy);

    grayscale(copy);

    struct Matrix **matrices = init_matrices_and_set_pixels(copy);
    SDL_FreeSurface(copy);


    struct Matrix *filter = init_matrix(3, 3);
    fill_matrix(filter, 2);

    struct Matrix **convolution_features = convolution(matrices, filter);

    free_pixels_matrices(matrices);
    free_pixels_matrices(convolution_features);
    free_matrix(filter);
 


    return 0;
}

/*
int main(int argc, char **argv)
{
    if (argc != 2)
        return 1;
    else
    {
        const char* filename = argv[1];

        filename++;
        
    }
}*/