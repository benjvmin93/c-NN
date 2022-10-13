#include "neuralNet.h"
#include "../utils/matrix.h"
#include "../image-process/SDL.h"
#include "filter.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <err.h>
#include <stdlib.h>

static const enum ImageType output[5] = { ANIMAL, VEHICULE, WOMAN, MAN, CHILD };

/*
* Initialize pixels matrices from SDL_Surface
* and set it inside a list of matrices.
*/
struct Matrix **init_matrices_and_set_pixels(SDL_Surface *copy)
{
    Uint8 r, g, b;
    struct Matrix* pixels = init_matrix(copy->w, copy->h);
    struct Matrix *redPixels = init_matrix(copy->w, copy->h);
    struct Matrix *greenPixels = init_matrix(copy->w, copy->h);
    struct Matrix *bluePixels = init_matrix(copy->w, copy->h);
    
    for (int i = 0; i < copy->w; ++i)
    {
        for (int j = 0; j < copy->h; ++j)
        {
            Uint32 pixel = getpixel(copy, i, j);
            SDL_GetRGB(pixel, copy->format, &r, &g, &b);
            
            setElement(pixels, pixel, i, j);
            setElement(redPixels, r, i, j);
            setElement(greenPixels, g, i, j);
            setElement(bluePixels, b, i, j);
        }
    }

    struct Matrix **matrices = malloc(5 * sizeof(struct Matrix *));
    if (!matrices)
        err(1, "neuralNet.init_matrices_and_set_pixels(): Couldn't allocate matrices.");

    matrices[4] = NULL;

    matrices[0] = pixels;
    matrices[1] = redPixels;
    matrices[2] = greenPixels;
    matrices[3] = bluePixels;

    return matrices;
}

/*
* Initialize an array of doubles according to He initialization formula.
*/
double **init_weights(size_t lines, size_t cols)
{
    double **weights = malloc(lines * sizeof(double *));
    if (!weights)
        err(1, "neuralNet.init_weights: couldn't allocate weights.");
    
    time_t t = time(NULL);
    srand(t);

    const double he = sqrt((double) 2 / cols);
    for (size_t i = 0; i < lines; ++i)
    {
        weights[i] = malloc(cols * sizeof(double));
        if (!weights[i])
            err(1, "neuralNet.init_weights: couldn't allocate weights[i].");
        for (size_t j = 0; j < cols; ++j)
        {
            weights[i][j] = (double) rand() / RAND_MAX * he;
            printf("%f, ", weights[i][j]);
        }
    }

    return weights;
}

struct HiddenLayer *init_hiddenLayer(struct Matrix **layer)
{
    struct HiddenLayer *HiddenLayer = malloc(sizeof(struct HiddenLayer));
    if (!HiddenLayer)
        err(1, "neuralNet.init_hiddenLayer(): Coudln't allocate HiddenLayer.");
    
    HiddenLayer->layer = layer;

    for (size_t i = 0; layer[i]; ++i)
    {
        struct Matrix *m = layer[i];
        HiddenLayer->weights = init_weights(m->lines, m->cols);
    }

    return HiddenLayer;
}

enum ImageType run(const char *path)
{
    struct NeuralNet *neuralnet = init_cnn(path);
    struct Filter *filters = neuralnet->filters;

    printf("Filters generated:\n");
    for (size_t i = 0; i < filters->nbFilters; ++i)
        print_matrix(filters->filters[i]);

    // Allocate enough space for our convolved features (4 input images * nbFilters)
    size_t nb_convolved_features = (4 * filters->nbFilters) + 1;
    struct Matrix **convolved_features = malloc(nb_convolved_features * sizeof(struct Matrix));
    if (!convolved_features)
        err(1, "neuralNet.run(): Coudln't allocate convolved_features.");

    convolved_features[nb_convolved_features - 1] = NULL;

    size_t i = 0;
    // Convolution process
    printf("Starting convolution process");
    while (i < nb_convolved_features)
    {
        convolved_features[i] = NULL;
        size_t f = 0;
        while (f < filters->nbFilters)
        {
            printf(".");
            struct Matrix *padded = pad_input(neuralnet->input[i], 2);
            convolved_features[i] = convolution(padded, filters->filters[f]);
            free_matrix(padded);
            f++;
            i++;
        }
    }

    return output[0];
}

struct NeuralNet* init_cnn(const char* file)
{
    // Load image and get the pixel input matrices.
    init_sdl();
    SDL_Surface *img = load_image(file);
    struct Matrix **input = init_matrices_and_set_pixels(img);
    struct NeuralNet *neuralnet = malloc(sizeof(struct NeuralNet));
    if (!neuralnet)
        err(1, "neuralNet.init_cnn(): Couldn't allocate neuralnet.");

    neuralnet->input = input;
    neuralnet->filters = init_filter(NONE);

    neuralnet->convolutionLayer = NULL;
    neuralnet->pooled_feature = NULL;
    neuralnet->flatLayer = NULL;

    return neuralnet;
}

/*
* Apply softmax function to a Vector.
*/
double *softmax_function(struct Matrix *pooled_feature)
{
    size_t size = pooled_feature->cols * pooled_feature->lines;

    // Change the 2D matrix to a 1D matrix.
    int *flatLayer = flatMatrix(pooled_feature);

    double sum = 0;
    for (size_t i = 0; i < size; ++i)
    {
        sum += exp(flatLayer[i]);
    }


    double *out = malloc(size * sizeof(double));
    for (size_t i = 0; i < size; ++i)
    {
        out[i] = exp(flatLayer[i]) / sum;
    }

    return out;
}

struct Matrix *pad_input(struct Matrix *m, size_t padSize)
{
    int **input = m->matrix;

    size_t cols_pad = m->cols + 2 * padSize;
    size_t lines_pad = m->lines + 2 * padSize;

    struct Matrix *padded_input = init_matrix(cols_pad, lines_pad);
    
    /* Fill center. */
    for (size_t i = padSize; i < lines_pad - padSize; ++i)
    {
        for (size_t j = padSize; j < cols_pad - padSize; ++j)
            setElement(padded_input,
                input[i - padSize][j - padSize], i, j);
    }

    return padded_input;
}

int Max(int a, int b)
{
    return a < b ? b : a;
}

int dotProduct(struct Matrix *input, size_t i, size_t j, struct Matrix *filter)
{
    int sum = 0;
    int **matrix = input->matrix;

    for (; i < filter->cols; ++i)
    {
        for (; j < filter->lines; ++j)
        {
            sum += matrix[i][j] * filter->matrix[i][j];
        }
    }
    if (sum < 0)
        sum = 0;

    return sum;
}

int activationFunction(struct Matrix *input, size_t i, size_t j, struct Matrix *filter)
{
    int max = 0;
    int **mat = input->matrix;
    
    for (size_t k = i; k < i + filter->lines; ++k)
    {
        for (size_t l = j; l < j + filter->cols; ++l)
        {
            // printf("i: %ld, j: %ld, elt: %d | ", i, j, mat[i][j]);
            max = Max(max, mat[k][l]);
        }
        // printf("\n");
    }

    return max;
}

/*
* Pooling process.
* Apply the max pooling method to the convolved feature.
* Returns the pooled feature.
*/
struct Matrix *pooling(struct Matrix *convolved_feature, struct Matrix *filter, int stride)
{
    size_t lines = 1; 
    size_t tmp_cols = 0;
    size_t cols = 1;

    int **m = NULL;
    for (size_t i = 0; i + filter->lines - 1 < convolved_feature->lines; ++i, ++lines)
    {
        m = realloc(m, lines * sizeof(int *));
        if (!m)
            err(1, "neuralNet.pooling: Couldn't allocate m");
        m[lines - 1] = NULL;
        for (size_t j = 0; j + filter->cols - 1 < convolved_feature->cols; j += stride, ++cols)
        {
            m[lines - 1] = realloc(m[lines - 1], cols * sizeof(int));
            if (!m[lines - 1])
                err(1, "neuralNet.pooling: Couldn't allocate m[i]");
            m[lines - 1][cols - 1] = activationFunction(convolved_feature, i, j, filter);
        }
        if (tmp_cols == 0)
            tmp_cols = cols - 1;
        cols = 1;
    }

    struct Matrix *pooled_feature = init_matrix(tmp_cols, lines - 1);
    free_inside_matrix(pooled_feature);
    pooled_feature->matrix = m;

    return pooled_feature;
}



/*
* Convolution process.
* Iterate through an input matrix and apply a convolution operation with the given filter.
* Finally apply ReLU function for each computed element of the output.
* Returns the convolved feature.
*/
struct Matrix *convolution(struct Matrix *input, struct Matrix *filter)
{
    size_t conv_cols = input->cols - 2;
    size_t conv_lines = input->lines - 2;
    struct Matrix *conv_matrix = init_matrix(conv_cols, conv_lines);
    size_t cols = 0;
    size_t lines = 0;

    for (size_t i = 0; i + filter->lines - 1 < input->lines; ++i)
    {
        for (size_t j = 0; j + filter->cols - 1 < input->cols; ++j)
        {
            int sum = 0;
            for (size_t k = 0; k < filter->lines; ++k)
            {
                for (size_t l = 0; l < filter->cols; ++l)
                {
                    int inputElt = input->matrix[i + k][j + l];
                    int outputElt = inputElt * filter->matrix[l][k];
                    sum += outputElt;
                }
            }
            if (sum < 0)
                sum = 0;
            setElement(conv_matrix, sum, lines, cols);
            cols++;
            sum = 0;
        }
        cols = 0;
        lines++;
    }

    return conv_matrix;
}

void free_pixels_matrices(struct Matrix **matrices)
{
    for(int i = 0; matrices[i]; ++i)
    {
        free_matrix(matrices[i]);
    }

    free(matrices);
}

void free_hidden_layer(struct HiddenLayer *HiddenLayer)
{
    if (!HiddenLayer)
        return;

    struct HiddenLayer *l = HiddenLayer;
    
    if (!l->layer || !l->weights || !l->bias)
        return;
    
    struct Matrix *m = l->layer[0];
    for (size_t i = 0; i < m->lines; ++i)
    {
        free(l->weights[i]);
        free(l->bias[i]);
    }

    free(l->weights);
    free(l->bias);
    free_pixels_matrices(HiddenLayer->layer);
}


//TODO: modify neuralNet attribute according to HiddenLayer structures contained inside.

void free_cnn(struct NeuralNet* neuralNet)
{
    if (!neuralNet)
        return;
    if (neuralNet->input)
        free_pixels_matrices(neuralNet->input);
    if (neuralNet->convolutionLayer)
        free_hidden_layer(neuralNet->convolutionLayer);
    if (neuralNet->pooled_feature)
        free_hidden_layer(neuralNet->convolutionLayer);

    free(neuralNet);
}