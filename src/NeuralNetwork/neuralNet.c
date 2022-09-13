#include "neuralNet.h"

#include <stdio.h>
#include <err.h>

struct NeuralNet* init(const char* file)
{
    struct NeuralNet* neuralNet = malloc(sizeof(struct NeuralNet));
    if (!neuralNet)
        err(1, "Failed to allocate neuralNet structure");

    FILE* f = fopen(file, "r");
    if (!f)
        err(1, "Failed to open %s file", file);
    

    //neuralNet->firstLayer = malloc(sizeof(float) * size);

    fclose(f);

    return neuralNet;
}
void freeNeuralNet(struct NeuralNet* neuralNet)
{
    if (!neuralNet)
        return;

    free(neuralNet->firstLayer);
    free(neuralNet);
}