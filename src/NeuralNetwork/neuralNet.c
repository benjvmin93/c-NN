#include "neuralNet.h"
#include "../utils/matrix.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <err.h>
#include <stdlib.h>


struct NeuralNet* init(const char* file)
{
    struct NeuralNet* neuralNet = malloc(sizeof(struct NeuralNet));
    if (!neuralNet)
        err(1, "Failed to allocate neuralNet structure");

    FILE* f = fopen(file, "r");
    if (!f)
        err(1, "Failed to open %s file", file);
    
    fclose(f);

    return neuralNet;
}

struct Matrix *generate_filter(size_t cols, size_t lines)
{
    struct Matrix *filter = init_matrix(cols, lines);
    fill_matrix(filter, -1);

    return filter;
}

int **pad_input(struct Matrix *m, size_t padSize)
{
    int **input = m->matrix;

    size_t cols_pad = m->cols + 2 * padSize;
    size_t lines_pad = m->lines + 2 * padSize;

    int **padded_input = malloc(lines_pad * sizeof(int *));
    if (!padded_input)
        err(1, "neuralNet.pad_input: Couldn't allocate `padded_input`.");

    for (size_t i = 0; i < lines_pad; ++i)
    {
        padded_input[i] = malloc(cols_pad * sizeof(int));
        if (!padded_input[i])
            err(1, "neuralNet.pad_input: Couldn't allocate `padded_input[i]`.");
        for (size_t j = 0; j < cols_pad; ++j)
        {
            padded_input[i][j] = 0;
        }
    }

    /* Fill center. */
    for (size_t i = padSize; i < lines_pad - padSize; ++i)
    {
        for (size_t j = padSize; j < cols_pad - padSize; ++j)
        { 
            padded_input[i][j] = input[i - padSize][j - padSize];
        }
    }

    m->cols = cols_pad;
    m->lines = lines_pad;
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

void freeNeuralNet(struct NeuralNet* neuralNet)
{
    if (!neuralNet)
        return;

    free(neuralNet->firstLayer);
    free(neuralNet);
}