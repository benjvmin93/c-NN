#define _DEFAULT_SOURCE

#include <string.h>

#include "neuralNet.h"
#include "../utils/matrix.h"
#include "../utils/xmalloc.h"
#include "../utils/files.h"
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
#define SIZE_OUTPUTS 4

#define MAX(X,Y) (X < Y) ? Y : X
#define MIN(X, Y) (X < Y) ? X : Y

const char *LABELS[SIZE_OUTPUTS] = { "Cat", "Dog", "Car", "Human" };

/*
* Initialize pixels matrices from SDL_Surface
* and set it inside a list of matrices.
* Currently only initialize a matrix containing the raw pixel values without differencing rgb pixels.
*/
struct Matrix **init_matrices_and_set_pixels(SDL_Surface *copy)
{
    Uint8 r, g, b;
    struct Matrix* pixels = init_matrix(copy->w, copy->h);
    
    for (int i = 0; i < copy->h; ++i)
    {
        for (int j = 0; j < copy->w; ++j)
        {
            Uint32 pixel = getpixel(copy, j, i);
            SDL_GetRGB(pixel, copy->format, &r, &g, &b);
            setElement(pixels, (float) pixel, i, j);
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

struct HiddenLayer *init_hiddenLayer(struct Matrix **layers, size_t nbLayers, struct Weights *biases)
{
    struct HiddenLayer *HiddenLayer = xmalloc(1, sizeof(struct HiddenLayer));
    HiddenLayer->nbLayers = nbLayers;
    HiddenLayer->layers = layers;

    HiddenLayer->biases = biases;

    return HiddenLayer;
}

float *normalize_flat(float *out, size_t length)
{
    float max = -1;
    float min = (float) INT64_MAX;

    for (size_t i = 0; i < length; ++i)
    {
        max = MAX(max, out[i]);
        min = MIN(min, out[i]);
    }

    float den = max-min;
    for (size_t i = 0; i < length; ++i)
    {
        out[i] = (out[i] - min) / den;
    }

    return out;
}

float *apply_weights(struct FullyConnected *fullyConnected)
{
    float **out = xmalloc(SIZE_OUTPUTS, sizeof(float *));

    for (size_t i = 0; i < SIZE_OUTPUTS; ++i)
        out[i] = xmalloc(fullyConnected->flatSize, sizeof(float));
    
    float *flat = fullyConnected->flatLayer;
    struct Weights *weights = fullyConnected->weights;
    struct Weights *bias = fullyConnected->biases;

    for (size_t o = 0; o < SIZE_OUTPUTS; ++o)
    {
        for (size_t l = 0; l < fullyConnected->flatSize; ++l)
        {
            out[o][l] = (flat[l] * weights->matrix[l][o]) + bias->matrix[l][0];
        }
    }

    float *results = xmalloc(SIZE_OUTPUTS, sizeof(float));

    for (size_t i = 0; i < SIZE_OUTPUTS; ++i)
    {
        results[i] = 0;
        out[i] = normalize_flat(out[i], fullyConnected->flatSize);
        for (size_t j = 0; j < fullyConnected->flatSize; ++j)
            results[i] += out[i][j];
    }

    for (size_t i = 0; i < SIZE_OUTPUTS; ++i)
        free(out[i]);
    free(out);

    return results;
}

struct NeuralNet* init_cnn(const char* file, bool verbose)
{
    // Load image and get the pixel input matrices.
    init_sdl();
    SDL_Surface *img = load_image(file);
    grayscale(img);
    if (verbose)
    {
        printf("Loaded image of size %dx%d (%d pixels).\n\n", img->w, img->h, img->w * img->h);
        display_image(img);
    }
    struct Matrix **input = init_matrices_and_set_pixels(img);
    SDL_FreeSurface(img);

    struct NeuralNet *neuralnet = xmalloc(1, sizeof(struct NeuralNet));
    neuralnet->verbose = verbose;
    neuralnet->input = input;

    neuralnet->filters = NULL;
    neuralnet->convolutionLayer = NULL;
    neuralnet->pooled_feature = NULL;
    neuralnet->fullyConnected = NULL;
    neuralnet->predictions = NULL;

    // If there already are filters => TRAINING MODE => we keep the same filters.
    // Otherwise initialize them.
    if (!neuralnet->filters)
        neuralnet->filters = init_filter();

    if (verbose)
    {
        printf("Filters:\n");
        for (size_t i = 0; i < neuralnet->filters->nbFilters; ++i)
        {
            print_matrix(neuralnet->filters->filters[i]);
            printf("\n");
        }
    }
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
            max = MAX(max, mat[k][l]);
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

struct Matrix *convolution(struct Matrix *input, struct Matrix *filter, struct Weights *biases)
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
                    float outputElt = (inputElt * filter->matrix[l][k]) + biases->matrix[lines][0];
                    // if (i < 1 && j < 10)
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
    free_weight(l->biases);
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
    if (neuralNet->predictions)
        free(neuralNet->predictions);

    free(neuralNet);
}

struct NeuralNet *run_convolution(struct NeuralNet *neuralnet, bool training)
{
    struct Filter *filters = neuralnet->filters;

    // Allocate enough space for our convolved features (4 input images * nbFilters)
    size_t nb_convolved_features = (SIZE_INPUTS * filters->nbFilters);
    struct Matrix **convolved_features = xmalloc(nb_convolved_features, sizeof(struct Matrix*));
    struct Weights *biases = NULL;
    
    // If the layer already exists => TRAINING MODE => we keep the same biases as before
    // Otherwise we initialize new ones.
    if (neuralnet->convolutionLayer)
        biases = neuralnet->convolutionLayer->biases;
    else 
        biases = init_weights(neuralnet->input[0]->lines, 1);

    size_t i = 0;
    size_t c = 0;
    
    // Convolution process
    while (i < SIZE_INPUTS)
    {
        convolved_features[c] = NULL;
        size_t f = 0;
        while (f < filters->nbFilters)
        {
            // Pad the input image before convolution
            struct Matrix *padded = pad_input(neuralnet->input[i], 1);
            convolved_features[c++] = convolution(padded, filters->filters[f], biases);
            if (neuralnet->verbose)
            {
                SDL_Surface *surf = pixels_to_surface(convolved_features[c - 1]);
                display_image(surf);
                SDL_FreeSurface(surf);
            }
            
            free_matrix(padded);

            f++;
        }
        i++;
    }

    if (!training)
        neuralnet->convolutionLayer = init_hiddenLayer(convolved_features, nb_convolved_features, biases);
    else
    {
        neuralnet->convolutionLayer->layers = convolved_features;
        neuralnet->convolutionLayer->nbLayers = nb_convolved_features;
    }

    return neuralnet;
}

struct NeuralNet *run_pooling(struct NeuralNet *neuralnet, bool training)
{
    size_t nb_pooled_features = neuralnet->convolutionLayer->nbLayers;
    struct Matrix **pooled_features = xmalloc(nb_pooled_features, sizeof(struct Matrix));

    size_t i = 0;
    while (i < nb_pooled_features)
    {
        struct Matrix *filter = init_matrix(2, 2);
        fill_matrix(filter, -1);
        pooled_features[i] = NULL;
        pooled_features[i] = pooling(neuralnet->convolutionLayer->layers[i], filter, 3);
        if (neuralnet->verbose)
        {
            SDL_Surface *surf = pixels_to_surface(pooled_features[i]);
            display_image(surf);
            SDL_FreeSurface(surf);
        }
        i++;
    }

    if (!training)
        neuralnet->pooled_feature = init_hiddenLayer(pooled_features, nb_pooled_features, NULL);
    else
    {
        neuralnet->pooled_feature->layers = pooled_features;
        neuralnet->pooled_feature->nbLayers = nb_pooled_features;
    }
    
    return neuralnet;
}

struct NeuralNet *run_flatting(struct NeuralNet *neuralnet)
{
    // Flat matrices process.
    if (!neuralnet->fullyConnected)
    {
        neuralnet->fullyConnected =  xmalloc(1, sizeof(struct FullyConnected));
        neuralnet->fullyConnected->biases = NULL;
        neuralnet->fullyConnected->weights = NULL;
        neuralnet->fullyConnected->flatLayer = NULL;
        neuralnet->fullyConnected->flatSize = 0;
    }

    struct FullyConnected *fullyConnected = neuralnet->fullyConnected;

    fullyConnected->flatLayer = flatMatrices(neuralnet->pooled_feature->layers, neuralnet->pooled_feature->nbLayers);
    for (size_t i = 0; i < neuralnet->pooled_feature->nbLayers; ++i)
        fullyConnected->flatSize += (neuralnet->pooled_feature->layers[i]->cols * neuralnet->pooled_feature->layers[i]->lines);

    if (!fullyConnected->weights && !fullyConnected->biases)
    {
        fullyConnected->weights = init_weights(fullyConnected->flatSize, SIZE_OUTPUTS);
        fullyConnected->biases = init_weights(fullyConnected->flatSize, 1);
    }

    return neuralnet;
}

/*
* Apply softmax activation function over a vector.
*/
float* activation(float *vector, size_t length)
{
    float num = 0;
    float den = 0;
    for (size_t i = 0; i < length; ++i)
    {
        den += vector[i];
    }

    for (size_t i = 0; i < length; ++i)
    {
        num = vector[i];
        vector[i] = num / den;
    }
    return vector;
}

struct NeuralNet *reset_and_load_input(const char *path, struct NeuralNet *nn)
{
    // Load image and get the pixel input matrices.
    SDL_Surface *img = load_image(path);
    grayscale(img);
    if (nn->verbose)
    {
        printf("Loaded image of size %dx%d (%d pixels).\n\n", img->w, img->h, img->w * img->h);
        display_image(img);
    }
    free_pixels_matrices(nn->input, SIZE_INPUTS);
    nn->input = init_matrices_and_set_pixels(img);
    SDL_FreeSurface(img);

    free_pixels_matrices(nn->convolutionLayer->layers, nn->convolutionLayer->nbLayers);
    nn->convolutionLayer->nbLayers = 0;
    free_pixels_matrices(nn->pooled_feature->layers, nn->pooled_feature->nbLayers);
    nn->convolutionLayer->nbLayers = 0;
    free(nn->predictions);

    free(nn->fullyConnected->flatLayer);
    nn->fullyConnected->flatSize = 0;
    return nn;
}

struct NeuralNet *run(const char *path, bool verbose, struct NeuralNet *neuralnet)
{
    bool training = false;
    if (!neuralnet)
    {
        neuralnet = init_cnn(path, verbose);
    }
    else
    {
        training = true;
        neuralnet = reset_and_load_input(path, neuralnet);
    }

    neuralnet = run_convolution(neuralnet, training);
    neuralnet = run_pooling(neuralnet, training);
    neuralnet = run_flatting(neuralnet);

    float *probabilities = apply_weights(neuralnet->fullyConnected);
    neuralnet->predictions = activation(probabilities, SIZE_OUTPUTS);
        
    return neuralnet;
}

struct NeuralNet *predict(const char *path, bool verbose)
{
    struct NeuralNet *nn = run(path, verbose, NULL);

    size_t posMax = 0;
    for (size_t i = 0; i < SIZE_OUTPUTS; ++i)
    {
        posMax = (nn->predictions[i] > nn->predictions[posMax]) ? i : posMax;
        if (verbose)
        {
            printf("%s: ", LABELS[i]);
            printf("%f\n", nn->predictions[i]);
        }
    }
    
    printf("\nPredicted %s: %f\n", LABELS[posMax], nn->predictions[posMax]);

    free_cnn(nn);
    return NULL;
}

/*
* Compute loss after forward propagation according to cross entropy formula.
*/
float compute_loss(float *predictions, float *expected)
{
    float loss = 0;

    for (size_t i = 0; i < SIZE_OUTPUTS; ++i)
        loss += expected[i] * log(predictions[i]);
    
    loss = -loss;

    return loss;
}

float *getExpectedPredictions(const char *dirName)
{
    float *expected = xcalloc(SIZE_OUTPUTS, sizeof(float));
    
    if (!strcmp(dirName, "cat"))
        expected[CAT] = 1;
    else if (!strcmp(dirName, "dog"))
       expected[DOG] = 1;
    else if (!strcmp(dirName, "human"))
       expected[HUMAN] = 1;
    else if (!strcmp(dirName, "car"))
       expected[CAR] = 1;
    else
        err(0, "neuralNet.getExpectedPredictions(): label %s is not recognized", dirName);

    return expected;
}

void back_propagation(struct NeuralNet *nn, float *expected)
{
    float loss = compute_loss(nn->predictions, expected);
    printf("Loss: %f\n", loss);
}

/*
* Training function.
* ${dataPath} argument is the directory name containing all the images the network will be training on.
*/
void train(struct NeuralNet *nn, const char *dataPath, bool verbose, int epoch)
{    
    char **imageDir = getDirNamesFromDir(dataPath);
    if (!*imageDir)
        err(1, "neuralNet.train(): No directory found inside %s directory.", dataPath);

    size_t i = 0;
    while (imageDir[i])
    {
        char *folderPath = xmalloc(strlen(dataPath) + strlen(imageDir[i]) + 1, sizeof(char));
        folderPath = strcpy(folderPath, dataPath);
        folderPath = strcat(folderPath, imageDir[i]);

        printf("folder path: %s\n", folderPath);
        char **images = getFileNamesFromDir(folderPath);
        float *expected = getExpectedPredictions(imageDir[i]);
        for (int i = 0; i < epoch; ++i)
        {
            size_t j = 0;
            while (images[j])
            {
                char *imagePath = strdup(folderPath);
                imagePath = realloc(imagePath, strlen(imagePath) + strlen(images[j]) + strlen("/") + 1);
                imagePath = strcat(imagePath, "/");
                imagePath = strcat(imagePath, images[j]);
                printf("image path: %s\n", imagePath);
                nn = run(imagePath, verbose, nn);
                if (verbose)
                {
                    printf("Filters:\n");
                    for (size_t i = 0; i < nn->filters->nbFilters; ++i)
                    {
                        print_matrix(nn->filters->filters[i]);
                        printf("\n");
                    }
                }
                back_propagation(nn, expected);
                free(imagePath);
                j++;
            }
        }
        printf("\n");
        for (size_t j = 0; images[j]; ++j)
            free(images[j]);
        free(images);
        free(expected);
        free(folderPath);
        i++;
    }
    for (size_t j = 0; imageDir[j]; ++j)
        free(imageDir[j]);
    free(imageDir);
    free_cnn(nn);
}