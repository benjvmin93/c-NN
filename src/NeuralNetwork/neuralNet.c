#define _DEFAULT_SOURCE

#include "neuralNet.h"
#include "../utils/matrix.h"
#include "../utils/xmalloc.h"
#include "../image-process/SDL.h"
#include "filter.h"

#include <SDL2/SDL_config.h>
#include <SDL2/SDL_hidapi.h>
#include <SDL2/SDL_surface.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <err.h>
#include <stdlib.h>
#include <sys/time.h>

#define SIZE_INPUTS 1
#define SIZE_OUTPUTS 5

#define MAX(X,Y) (X < Y) ? Y : X
#define MIN(X, Y) (X < Y) ? X : Y

/*
* Initialize pixels matrices from SDL_Surface
* and set it inside a list of matrices.
* Currently only initialize a matrix containing the raw pixel values without differencing rgb pixels.
*/
struct Matrix **init_matrices_and_set_pixels(SDL_Surface *copy)
{
    Uint8 r, g, b;
    struct Matrix* pixels = init_matrix(copy->w, copy->h);
    /* struct Matrix *redPixels = init_matrix(copy->w, copy->h);
    struct Matrix *greenPixels = init_matrix(copy->w, copy->h);
    struct Matrix *bluePixels = init_matrix(copy->w, copy->h); */
    
    for (int i = 0; i < copy->h; ++i)
    {
        for (int j = 0; j < copy->w; ++j)
        {
            Uint32 pixel = getpixel(copy, j, i);
            SDL_GetRGB(pixel, copy->format, &r, &g, &b);
            
            // printf("%d\n", pixel);
            setElement(pixels, (float) pixel, i, j);
            // setElement(redPixels, r, i, j);
            // setElement(greenPixels, g, i, j);
            // setElement(bluePixels, b, i, j);
        }
    } 

    struct Matrix **matrices = xmalloc(SIZE_INPUTS, sizeof(struct Matrix *));

    matrices[0] = pixels;

    return matrices;
}

/*
* Initialize an array of doubles according to He initialization formula.
*/
struct Weights *init_weights(size_t lines, size_t cols)
{
    struct Weights *weights = xmalloc(1, sizeof(struct Weights));
    weights->cols = cols;
    weights->lines = lines;
    weights->matrix = xmalloc(lines, sizeof(float *));
    
    struct timeval tm;
    gettimeofday(&tm, NULL);
    srandom(tm.tv_sec + tm.tv_usec * 1000000ul);

    // const double he = sqrt((double) 2 / cols * lines);
    for (size_t i = 0; i < lines; ++i)
    {
        weights->matrix[i] = xmalloc(cols, sizeof(float));
        for (size_t j = 0; j < cols; ++j)
        {
            float rand;
            while ((rand = (double) random() / RAND_MAX) == 0)
                continue;
            weights->matrix[i][j] = rand;
        }
    }

    return weights;
}

struct HiddenLayer *init_hiddenLayer(struct Matrix **layers, size_t nbLayers)
{
    struct HiddenLayer *HiddenLayer = xmalloc(1, sizeof(struct HiddenLayer));
    HiddenLayer->nbLayers = nbLayers;
    HiddenLayer->layers = layers;

    return HiddenLayer;
}

float *apply_weights(struct FullyConnected *fullyConnected)
{
    float *out = xmalloc(SIZE_OUTPUTS, sizeof(float));
    float *flat = fullyConnected->flatLayer;
    struct Weights *weights = fullyConnected->weights;
    struct Weights *bias = fullyConnected->biases;

    for (size_t o = 0; o < SIZE_OUTPUTS; ++o)
    {
        out[o] = 0;
        for (size_t l = 0; l < fullyConnected->flatSize; ++l)
        {
            out[o] += (flat[l] * weights->matrix[l][o]) + bias->matrix[l][0];
            // printf("apply weights: flat: %f, weights: %f, bias: %f\n", flat[l], weights->matrix[l][o], bias->matrix[l][0]);
        }
        printf("out: %f\n", out[o]);
    }

    return out;
}

void train(const char *dataPath)
{
    dataPath++;
    return;
}

struct NeuralNet* init_cnn(const char* file)
{
    // Load image and get the pixel input matrices.
    init_sdl();
    SDL_Surface *img = load_image(file);
    grayscale(img);
    display_image(img);
    struct Matrix **input = init_matrices_and_set_pixels(img);
    SDL_FreeSurface(img);
    struct NeuralNet *neuralnet = xmalloc(1, sizeof(struct NeuralNet));
    neuralnet->input = input;
    neuralnet->filters = init_filter(NONE);

    for (size_t i = 0; i < neuralnet->filters->nbFilters; ++i)
    {
        print_matrix(neuralnet->filters->filters[i]);
        printf("\n\n");
    }

    neuralnet->convolutionLayer = NULL;
    neuralnet->pooled_feature = NULL;
    neuralnet->fullyConnected = NULL;

    return neuralnet;
}

struct Matrix *pad_input(struct Matrix *m, size_t padSize)
{
    float **input = m->matrix;

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

float Max(float a, float b)
{
    return a > b ? a : b;
}

int dotProduct(struct Matrix *input, size_t i, size_t j, struct Matrix *filter)
{
    int sum = 0;
    float **matrix = input->matrix;

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

float MaxPooling(struct Matrix *input, size_t i, size_t j, struct Matrix *filter)
{
    float max = 0;
    float **mat = input->matrix;
    
    for (size_t k = i; k < i + filter->lines; ++k)
    {
        for (size_t l = j; l < j + filter->cols; ++l)
        {
            max = Max(max, mat[k][l]);
        }
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

    float **m = NULL;
    for (size_t i = 0; i + filter->lines - 1 < convolved_feature->lines; ++i, ++lines)
    {
        m = realloc(m, lines * sizeof(float *));
        if (!m)
            err(1, "neuralNet.pooling: Couldn't allocate m");
        m[lines - 1] = NULL;
        for (size_t j = 0; j + filter->cols - 1 < convolved_feature->cols; j += stride, ++cols)
        {
            m[lines - 1] = realloc(m[lines - 1], cols * sizeof(float));
            if (!m[lines - 1])
                err(1, "neuralNet.pooling: Couldn't allocate m[i]");
            m[lines - 1][cols - 1] = MaxPooling(convolved_feature, i, j, filter);
            float output = m[lines - 1][cols - 1];
            if (i < 1 && j < 10)
                printf("\noutput pool: %f\n", output);                    
                    
        }
        if (tmp_cols == 0)
            tmp_cols = cols - 1;
        cols = 1;
    }

    struct Matrix *pooled_feature = init_matrix(tmp_cols, lines - 1);
    free_inside_matrix(pooled_feature);
    pooled_feature->matrix = m;

    free_matrix(filter);

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

    float sum = 0;
    

    for (size_t i = 0; i + filter->lines < input->lines; ++i)
    {
        for (size_t j = 0; j + filter->cols < input->cols; ++j)
        {
            for (size_t k = 0; k < filter->lines; ++k)
            {
                for (size_t l = 0; l < filter->cols; ++l)
                {
                    float inputElt = input->matrix[i + k][j + l];
                    float outputElt = inputElt * filter->matrix[l][k];
                    // if (i < 1 && j < 10)
                        // printf("\noutput conv: %f\n", outputElt);                    
                    sum += outputElt;
                }
            }
            // ReLu
            if (sum < 0)
                sum = 0;
            setElement(conv_matrix, sum, lines, cols);
            cols++;
            sum = 0;
        }
        cols = 0;
        lines++;
    }

   /* for (size_t i = 0; i < conv_matrix->lines; ++i)
    {
        for (size_t j = 0; j < conv_matrix->cols; ++j)
        {
            float elt = conv_matrix->matrix[i][j] / maxSum;
            setElement(conv_matrix, elt, i, j);
        }
    } */

    return conv_matrix;
}

void free_pixels_matrices(struct Matrix **matrices, size_t nbMatrices)
{
    for(size_t i = 0; i < nbMatrices; ++i)
    {
        free_matrix(matrices[i]);
    }

    free(matrices);
}

void free_weight(struct Weights *weights)
{
    if (!weights)
        return;
    for (size_t i = 0; i < weights->lines; ++i)
        free(weights->matrix[i]);
    
    free(weights->matrix);
    free(weights);
}

void free_hidden_layer(struct HiddenLayer *HiddenLayer)
{
    if (!HiddenLayer)
        return;

    struct HiddenLayer *l = HiddenLayer;
    
    if (!l->layers)
        return;
    free_pixels_matrices(l->layers, HiddenLayer->nbLayers);
    
    free(HiddenLayer);
}

//TODO: modify neuralNet attribute according to HiddenLayer structures contained inside.

void free_cnn(struct NeuralNet* neuralNet)
{
    if (!neuralNet)
        return;
    if (neuralNet->input)
        free_pixels_matrices(neuralNet->input, SIZE_INPUTS);
    if (neuralNet->convolutionLayer)
        free_hidden_layer(neuralNet->convolutionLayer);
    if (neuralNet->pooled_feature)
        free_hidden_layer(neuralNet->pooled_feature);
    if (neuralNet->fullyConnected)
    {
        free(neuralNet->fullyConnected->flatLayer);
        free_weight(neuralNet->fullyConnected->weights);
        free_weight(neuralNet->fullyConnected->biases);
        free(neuralNet->fullyConnected);
    }
    if (neuralNet->filters)
        free_filter(neuralNet->filters);
    free(neuralNet);
}

struct HiddenLayer *run_convolution(struct NeuralNet *neuralnet)
{
    struct Filter *filters = neuralnet->filters;

    // Allocate enough space for our convolved features (4 input images * nbFilters)
    size_t nb_convolved_features = (SIZE_INPUTS * filters->nbFilters);
    struct Matrix **convolved_features = xmalloc(nb_convolved_features, sizeof(struct Matrix*));
   
    size_t i = 0;
    size_t c = 0;
    
    // Convolution process
    while (i < SIZE_INPUTS)
    {
        convolved_features[c] = NULL;
        size_t f = 0;
        while (f < filters->nbFilters)
        {
            printf(".");

            // Pad the input image before convolution
            struct Matrix *padded = pad_input(neuralnet->input[i], 1);

            convolved_features[c++] = convolution(padded, filters->filters[f]);
            SDL_Surface *surf = pixels_to_surface(convolved_features[c - 1]);
            display_image(surf);
            SDL_FreeSurface(surf);
            free_matrix(padded);


            f++;
        }
        i++;
    }

    return init_hiddenLayer(convolved_features, nb_convolved_features);
}

struct HiddenLayer *run_pooling(struct NeuralNet *neuralnet)
{
    size_t nb_pooled_features = neuralnet->convolutionLayer->nbLayers;
    struct Matrix **pooled_features = xmalloc(nb_pooled_features, sizeof(struct Matrix));

    size_t i = 0;
    while (i < nb_pooled_features)
    {
        struct Matrix *filter = init_matrix(2, 2);
        fill_matrix(filter, -1);
        pooled_features[i] = NULL;
        printf(".");
        pooled_features[i] = pooling(neuralnet->convolutionLayer->layers[i], filter, 3);
        // SDL_Surface *surf = pixels_to_surface(pooled_features[i]);
        // display_image(surf);
        // SDL_FreeSurface(surf);
        i++;
    }


    return init_hiddenLayer(pooled_features, nb_pooled_features);
}

struct FullyConnected *run_flatting(struct NeuralNet *neuralnet)
{
    // Flat matrices process.
    struct FullyConnected *fullyConnected =  xmalloc(1, sizeof(struct FullyConnected));
     
    fullyConnected->flatSize = 0;
    fullyConnected->flatLayer = flatMatrices(neuralnet->pooled_feature->layers, neuralnet->pooled_feature->nbLayers);
    for (size_t i = 0; i < neuralnet->pooled_feature->nbLayers; ++i)
        fullyConnected->flatSize += (neuralnet->pooled_feature->layers[i]->cols * neuralnet->pooled_feature->layers[i]->lines);

    fullyConnected->weights = init_weights(fullyConnected->flatSize, SIZE_OUTPUTS);
    fullyConnected->biases = init_weights(fullyConnected->flatSize, 1);

    return fullyConnected;
}

float *normalize_flat(float *out)
{
    float max = -1;
    float min = (float) INT64_MAX;

    for (size_t i = 0; i < SIZE_OUTPUTS; ++i)
    {
        max = MAX(max, out[i]);
        min = MIN(min, out[i]);
    }

    float den = max-min;
    for (size_t i = 0; i < SIZE_OUTPUTS; ++i)
    {
        out[i] = (out[i] - min) / den;
    }

    return out;
}

enum ImageType run(const char *path)
{
    struct NeuralNet *neuralnet = init_cnn(path);

    printf("Starting convolution process ");
    neuralnet->convolutionLayer = run_convolution(neuralnet);
    printf("\nConvolution process ended\n\n");

    printf("Starting pooling process ");
    neuralnet->pooled_feature = run_pooling(neuralnet);
    printf("Pooling process ended.\n\n");

    neuralnet->fullyConnected = run_flatting(neuralnet);


    float *probabilities = apply_weights(neuralnet->fullyConnected);
    probabilities = normalize_flat(probabilities);

    float output[SIZE_OUTPUTS];
   
    for (size_t i = 0; i < SIZE_OUTPUTS; ++i)
    {
        float num = expf(probabilities[i]);
        printf("input = %f\n", num);

        float den = 0;
        for (size_t j = 0; j < SIZE_OUTPUTS; ++j)
        {
            float elt = probabilities[j];
            den += expf(elt);
            // printf("num: %f, den: %f\n", num, den);
        }
        output[i] = num / den;
        printf("apply weights: %f / %f = %f\n", num, den, output[i]);
    }

    free(probabilities);
    free_cnn(neuralnet);


    return 0;
}