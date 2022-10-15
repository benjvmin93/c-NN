#include "NeuralNetwork/neuralNet.h"
#include "image-process/SDL.h"
#include "utils/matrix.h"

#include <stdio.h>
#include <time.h>

int main(int argc, char **argv)
{
    if (argc == 1)
        return 1;
    
    if (!strcmp(argv[1], "--train") && argv[2])
    {
        train(argv[2]);
        return 0;
    }

    return run(argv[1]);
    /*const char* filename = argv[1];
    SDL_Surface *copy = init_and_copy_img("tests/images/cats/images.jpg");
    grayscale(copy);

    display_image(copy);

    struct NeuralNet *neuralNet = init_cnn("image-process/img/tigre.bmp");

    return 0; */
}