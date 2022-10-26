#ifndef NEURALNET_H
#define NEURALNET_H

#include "../utils/matrix.h"
#include "filter.h"

#include <stdlib.h>
#include <stdbool.h>

struct Weights
{
    size_t cols;
    size_t lines;
    float **matrix;
};

struct HiddenLayer
{
    struct Matrix **layers;
    struct Weights *biases;
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
    float *predictions;
    bool verbose;
};

enum Labels
{
    CAT = 0,
    DOG,
    CAR,
    HUMAN
};

struct NeuralNet* init_cnn(const char* file, bool verbose);
struct HiddenLayer *init_hiddenLayer(struct Matrix **layer, size_t nbLayers, struct Weights *biases);
struct Weights *init_weights(size_t lines, size_t cols);

void free_cnn(struct NeuralNet* neuralNet);
void free_hidden_layer(struct HiddenLayer *HiddenLayer);
void free_weight(struct Weights *weights);

struct Matrix *pad_input(struct Matrix *m, size_t padSize);
struct Matrix *convolution(struct Matrix *in, struct Matrix *m, struct Weights *biases);
struct Matrix *pooling(struct Matrix *convolved_feature, struct Matrix *filter, int stride);
struct Matrix *generate_filter(size_t cols, size_t lines);

void predict(struct NeuralNet *nn);
struct NeuralNet *run(const char *path, bool verbose, struct NeuralNet *neuralnet);
void train(struct NeuralNet *nn, const char *dataPath, bool verbose, int epoch);

#endif