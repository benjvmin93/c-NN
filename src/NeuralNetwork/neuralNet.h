#ifndef NEURALNET_H
#define NEURALNET_H

#include "../utils/matrix.h"
#include "filter.h"

#include <stdlib.h>

struct Weights
{
    size_t cols;
    size_t lines;
    float **matrix;
};

struct HiddenLayer
{
    struct Matrix **layers;
    size_t nbLayers;
};

struct FullyConnected
{
    float *flatLayer;
    size_t flatSize;
    struct Weights *weights;
    struct Weights *biases;
};

struct NeuralNet
{
    struct Filter *filters;
    struct Matrix **input;
    struct HiddenLayer *convolutionLayer;
    struct HiddenLayer *pooled_feature;
    struct FullyConnected *fullyConnected;
};

struct NeuralNet* init_cnn(const char* file);
struct HiddenLayer *init_hiddenLayer(struct Matrix **layer, size_t nbLayers);

void free_cnn(struct NeuralNet* neuralNet);
void free_hidden_layer(struct HiddenLayer *HiddenLayer);

struct Matrix *pad_input(struct Matrix *m, size_t padSize);
struct Matrix *convolution(struct Matrix *in, struct Matrix *m);
struct Matrix *pooling(struct Matrix *convolved_feature, struct Matrix *filter, int stride);
struct Matrix *generate_filter(size_t cols, size_t lines);

enum ImageType run(const char *path);
void train(const char *dataPath);

#endif