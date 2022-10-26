#include "NeuralNetwork/neuralNet.h"
#include "image-process/SDL.h"
#include "utils/matrix.h"

#include <bits/getopt_core.h>
#include <getopt.h>
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
    if (argc == 1)
        usage();
    bool VERBOSE = false;
    const char *path = NULL;
    int opt;

    while ((opt = getopt(argc, argv, "vt:i:")) != -1)
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
            train(NULL, path, VERBOSE, 1);
            return 0;
        case 'i':
            if (path)
                usage();
            path = optarg;
            free_cnn(run(path, VERBOSE, NULL));
            break;
        default:
            usage();
        }
    }

    return 0;
}