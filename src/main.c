#include "NeuralNetwork/neuralNet.h"

#include <stdio.h>

int main(int argc, char **argv)
{
    if (argc != 2)
        return 1;
    else
    {
        const char* filename = argv[1];

        struct NeuralNet* neuralNet = init(filename);
        freeNeuralNet(neuralNet);    
        return 0;
    }
}