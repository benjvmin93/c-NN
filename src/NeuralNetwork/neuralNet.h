#ifndef NEURALNET_H
#define NEURALNET_H

#include <stdlib.h>
#include "../utils/matrix.h"

/*
const char* labelsReference[] = { "Car",
                                 "Cat",
                                 "Dog" };
*/


struct NeuralNet
{
    size_t pixels;
    float* firstLayer;
    float** weights;
};

struct NeuralNet* init(const char* file);

void freeNeuralNet(struct NeuralNet* neuralNet);

struct Matrix **convolution(struct Matrix **input, struct Matrix *m);

#endif