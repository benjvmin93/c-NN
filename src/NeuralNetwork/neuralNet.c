#include "neuralNet.h"
#include "../utils/matrix.h"
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

#define SIZE_INPUTS 1
#define SIZE_OUTPUTS 5

/*
* Initialize pixels matrices from SDL_Surface
* and set it inside a list of matrices.
*/
struct Matrix **init_matrices_and_set_pixels(SDL_Surface *copy)
{
    Uint8 r, g, b;
    struct Matrix* pixels = init_matrix(copy->w, copy->h);
    /* struct Matrix *redPixels = init_matrix(copy->w, copy->h);
    struct Matrix *greenPixels = init_matrix(copy->w, copy->h);
    struct Matrix *bluePixels = init_matrix(copy->w, copy->h); */
    
    for (int i = 0; i < copy->w; ++i)
    {
        for (int j = 0; j < copy->h; ++j)
        {
            Uint32 pixel = getpixel(copy, i, j);
            SDL_GetRGB(pixel, copy->format, &r, &g, &b);
            
            setElement(pixels, pixel, i, j);
            // setElement(redPixels, r, i, j);
            // setElement(greenPixels, g, i, j);
            // setElement(bluePixels, b, i, j);
        }
    } 

    struct Matrix **matrices = malloc(SIZE_INPUTS * sizeof(struct Matrix *));
    if (!matrices)
        err(1, "neuralNet.init_matrices_and_set_pixels(): Couldn't allocate matrices.");

    matrices[0] = pixels;
    // matrices[1] = redPixels;
    // matrices[2] = greenPixels;
    // matrices[3] = bluePixels;

    return matrices;
}

/*
* Initialize an array of doubles according to He initialization formula.
*/
struct Weights *init_weights(size_t lines, size_t cols)
{
    struct Weights *weights = malloc(sizeof(struct Weights));
    if (!weights)
        err(1, "neuralNet.init_weights: couldn't allocate weights.");
    weights->cols = cols;
    weights->lines = lines;
    weights->matrix = malloc(lines * sizeof(float *));
    if (!weights->matrix)
        err(1, "Couldn't allocate weights->matrix.");

    time_t t = time(NULL);
    srand(t);

    const double he = sqrt((double) 2 / cols * lines);
    for (size_t i = 0; i < lines; ++i)
    {
        weights->matrix[i] = malloc(cols * sizeof(double));
        if (!weights->matrix[i])
            err(1, "neuralNet.init_weights: couldn't allocate weights[i].");
        for (size_t j = 0; j < cols; ++j)
        {
            weights->matrix[i][j] = (double) rand() / RAND_MAX * he;
        }
    }

    return weights;
}

struct HiddenLayer *init_hiddenLayer(struct Matrix **layers, size_t nbLayers, int initWeights)
{
    struct HiddenLayer *HiddenLayer = malloc(sizeof(struct HiddenLayer));
    if (!HiddenLayer)
        err(1, "neuralNet.init_hiddenLayer(): Coudln't allocate HiddenLayer.");
    
    HiddenLayer->bias = NULL;
    HiddenLayer->weights = NULL;
    HiddenLayer->nbLayers = nbLayers;
    HiddenLayer->layers = layers;
    if (initWeights == 1)
    {
        HiddenLayer->weights = malloc(HiddenLayer->nbLayers * sizeof(struct Weights*));
        if (!HiddenLayer->weights)
            err(1, "neuralNet.init_hiddenLayer(): Coudln't allocate HiddenLayer->weights.");
        HiddenLayer->bias = malloc(HiddenLayer->nbLayers * sizeof(struct Weights*));
        if (!HiddenLayer->bias)
            err(1, "neuralNet.init_hiddenLayer(): Coudln't allocate HiddenLayer->bias.");

        for (size_t i = 0; i < nbLayers; ++i)
        {
            HiddenLayer->weights[i] = init_weights(layers[i]->lines, layers[i]->cols);
            HiddenLayer->bias[i] = init_weights(layers[i]->lines, 1);
        }
    }

    return HiddenLayer;
}

struct HiddenLayer *apply_weights(struct HiddenLayer *pooled_feature)
{
    for (size_t l = 0; l < pooled_feature->nbLayers; ++l)
    {
        struct Matrix *layer = pooled_feature->layers[l];
        struct Weights *weights = pooled_feature->weights[l];
        struct Weights *bias = pooled_feature->bias[l];

        for (size_t i = 0; i < layer->lines; ++i)
        {
            for (size_t j = 0; j < layer->cols; ++j)
            {
                double value = layer->matrix[i][j] * weights->matrix[i][j] + bias->matrix[i][0];
                setElement(layer, value, i, j);
            }
        }
    }

    return pooled_feature;
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
    struct NeuralNet *neuralnet = malloc(sizeof(struct NeuralNet));
    if (!neuralnet)
        err(1, "neuralNet.init_cnn(): Couldn't allocate neuralnet.");

    neuralnet->input = input;
    neuralnet->filters = init_filter(NONE);

    neuralnet->convolutionLayer = NULL;
    neuralnet->pooled_feature = NULL;
    neuralnet->fullyConnected = NULL;

    return neuralnet;
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
    return a > b ? a : b;
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

int MaxPooling(struct Matrix *input, size_t i, size_t j, struct Matrix *filter)
{
    int max = 0;
    int **mat = input->matrix;
    
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
            m[lines - 1][cols - 1] = MaxPooling(convolved_feature, i, j, filter);
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

    if (l->weights && l->bias)
    {
        struct Matrix *m = l->layers[0];
        for (size_t i = 0; i < m->lines; ++i)
        {
            free_weight(l->weights[i]);
            free_weight(l->bias[i]);
        }
    }
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
    struct Matrix **convolved_features = malloc(nb_convolved_features * sizeof(struct Matrix*));
    if (!convolved_features)
        err(1, "neuralNet.run(): Coudln't allocate convolved_features.");

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
            struct Matrix *padded = pad_input(neuralnet->input[i], 2);

            convolved_features[c++] = convolution(padded, filters->filters[f]);
            SDL_Surface *surf = pixels_to_surface(convolved_features[c - 1]);
            display_image(surf);
            SDL_FreeSurface(surf);
            free_matrix(padded);
            f++;
        }
        i++;
    }

    return init_hiddenLayer(convolved_features, nb_convolved_features, 0);
}

struct HiddenLayer *run_pooling(struct NeuralNet *neuralnet)
{
    size_t nb_pooled_features = neuralnet->convolutionLayer->nbLayers;
    struct Matrix **pooled_features = malloc(nb_pooled_features * sizeof(struct Matrix));
    if (!pooled_features)
        err(1, "neuralNet.run(): Couldn't allocate pooled_features");
    size_t i = 0;
    while (i < nb_pooled_features)
    {
        pooled_features[i] = NULL;
        printf(".");
        pooled_features[i] = pooling(neuralnet->convolutionLayer->layers[i], neuralnet->filters->filters[0], 2);
        SDL_Surface *surf = pixels_to_surface(pooled_features[i]);
        display_image(surf);
        SDL_FreeSurface(surf);
        i++;
    }


    return init_hiddenLayer(pooled_features, nb_pooled_features, 0);
}

struct FullyConnected *run_flatting(struct NeuralNet *neuralnet)
{
    // Flat matrices process.
    struct FullyConnected *fullyConnected =  malloc(sizeof(struct FullyConnected));
    if (!fullyConnected)
        err(1, "neuralNet.run_flatting(): Couldn't allocate fullyConnected.");
    
    fullyConnected->flatSize = 0;
    fullyConnected->flatLayer = flatMatrices(neuralnet->pooled_feature->layers, neuralnet->pooled_feature->nbLayers);
    for (size_t i = 0; i < neuralnet->pooled_feature->nbLayers; ++i)
        fullyConnected->flatSize += (neuralnet->pooled_feature->layers[i]->cols * neuralnet->pooled_feature->layers[i]->lines);

    fullyConnected->weights = init_weights(fullyConnected->flatSize, SIZE_OUTPUTS);
    fullyConnected->biases = init_weights(fullyConnected->flatSize, 1);

    return fullyConnected;

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

    double probabilities[5];
    // double max = 0;
    // double min = (double) SIZE_MAX;
    for (size_t i = 0 ; i < SIZE_OUTPUTS; ++i)
    {
        double dotProduct = 0;
        for (size_t j = 0; j < neuralnet->fullyConnected->flatSize; ++j)
        {
            dotProduct = dotProduct + (neuralnet->fullyConnected->flatLayer[j] * neuralnet->fullyConnected->weights->matrix[j][i]) + neuralnet->fullyConnected->biases->matrix[j][0];
        }
        probabilities[i] = dotProduct;
        // max = (max > probabilities[i]) ? max : probabilities[i];
        // min = (min < probabilities[i]) ? min: probabilities[i];
        printf("\n%ld: %f\n", i, probabilities[i]);
    }


    for (size_t i = 0; i < SIZE_OUTPUTS; ++i)
    {
        // probabilities[i] = 1 / ((max - min) * (probabilities[i] - max));
        printf("Normalized probabilitiy[%ld] = %f\n", i, probabilities[i]);
        double num = (double) exp(probabilities[i]) ;
        double den = 0;
        for (size_t j = 0; j < SIZE_INPUTS; ++j)
        {
            den = den + exp(probabilities[j]);
            // printf("num: %f, den: %f\n", num, den);
        }
        probabilities[i] = num / den;
        printf("\nnum: %f, den: %f\n", num, den);
        printf("%ld: %f\n", i, probabilities[i]);
    }

    free_cnn(neuralnet);

    return 0;
}