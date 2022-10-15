#include "NeuralNetwork/neuralNet.h"
#include "image-process/SDL.h"
#include "utils/matrix.h"

#include <stdio.h>
#include <time.h>

void usage()
{
    printf("Usage:\n./CNN {PATH} [--train]\n");
}

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        usage();
        return 1;
    }
    if (!strcmp(argv[1], "--train") && argv[2])
    {
        train(argv[2]);
        return 0;
    }

    return run(argv[1]);
}