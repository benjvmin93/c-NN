#include "NeuralNetwork/neuralNet.h"
#include "image-process/SDL.h"
#include "utils/matrix.h"

#include <bits/getopt_core.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>

void usage()
{
    printf("Usage:\n./CNN [-v] [-t {TRAINING-FOLDER-PATH}] [-i {DEFAULT-PATH}]\n");
    exit(1);
}

int main(int argc, char **argv)
{
    bool VERBOSE = false;
    const char *path = NULL;
    int opt;

    while ((opt = getopt(argc, argv, "vt:")) != -1)
    {
        switch (opt)
        {
        case 'v':
            VERBOSE = true;
            break;
        case 't':
            if (path)
                usage();
            path = optarg;
            train(path, VERBOSE, 50);
            return 0;
        case 'i':
            if (path)
                usage();
            path = optarg;
            float *predictions = predict(path, VERBOSE);
            free(predictions);
            break;
        default:
            usage();
        }
    }

    return 0;
}