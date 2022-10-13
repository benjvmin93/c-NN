#ifndef NEURALNET_H
#define NEURALNET_H

#include "../utils/matrix.h"
#include "filter.h"

#include <stdlib.h>


struct HiddenLayer
{
    struct Matrix **layer;
    double **weights;
    double **bias;
};

struct NeuralNet
{
    struct Matrix **input;
    struct HiddenLayer *convolutionLayer;
    struct HiddenLayer *pooled_feature;
    int *flatLayer;
    struct Filter *filters;
};

struct NeuralNet* init_cnn(const char* file);
struct HiddenLayer *init_hiddenLayer(struct Matrix **layer);

void free_cnn(struct NeuralNet* neuralNet);
void free_hidden_layer(struct HiddenLayer *HiddenLayer);

struct Matrix *pad_input(struct Matrix *m, size_t padSize);
struct Matrix *convolution(struct Matrix *in, struct Matrix *m);
struct Matrix *pooling(struct Matrix *convolved_feature, struct Matrix *filter, int stride);
struct Matrix *generate_filter(size_t cols, size_t lines);

#endif