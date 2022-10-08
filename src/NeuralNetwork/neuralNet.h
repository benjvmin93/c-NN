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

int **pad_input(struct Matrix *m, size_t padSize);
struct Matrix *convolution(struct Matrix *input, struct Matrix *m);
struct Matrix *pooling(struct Matrix *convolved_feature, struct Matrix *filter, int stride);
struct Matrix *generate_filter(size_t cols, size_t lines);

#endif