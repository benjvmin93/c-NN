#include "NeuralNetwork/neuralNet.h"
#include "image-process/SDL.h"
#include "utils/matrix.h"

#include <stdio.h>
#include <time.h>
#include <stdbool.h>

void usage()
{
    printf("Usage:\n./CNN [--train] [--verbose] {PATH}\n");
}

int main(int argc, char **argv)
{
    bool VERBOSE = false;
    const char *path = NULL;
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
    else if (!strcmp(argv[1], "--verbose") && argv[2])
    {
        path = argv[2];
        VERBOSE = true;
    }
    else 
    {
        path = argv[1];
    }

    float *predictions = predict(path, VERBOSE);
    free(predictions);

    return 0;
}