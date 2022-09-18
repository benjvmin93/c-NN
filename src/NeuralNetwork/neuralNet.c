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
    

    //neuralNet->firstLayer = malloc(sizeof(float) * size);

    fclose(f);

    return neuralNet;
}

struct Matrix *pad_input(struct Matrix *m, size_t padSize)
{
    size_t new_cols = m->cols + (2 * padSize);
    size_t new_lines = m->lines + (2 * padSize);

    int newSize = new_cols * new_lines;
    int *new = malloc(newSize * sizeof(int));
    if (!new)
        err(1, "neuralNet.pad_input: Couldn't allocate `new` matrix.");

    for (size_t i = 0; i < new_cols; ++i)
    {
        for (size_t j = 0; j < new_lines; ++j)
        {
            if (i < padSize || i > padSize + m->cols - 1 || j < padSize || j > padSize + m->lines - 1)
            {
                new[i * new_cols + j] = 0;
            }
            else
            {
                new[i * new_cols + j] = getElement(m, i - padSize, j - padSize);
            }
        }
    }

    free(m->matrix);

    m->matrix = new;
    m->cols += (2 * padSize);
    m->lines += (2 *padSize);

    printf("Matrix has been padded with a padSize of %ld.\n", padSize);

    return m;
}

int Max(struct Matrix *m)
{
    int max = INT32_MIN;
    for (size_t i = 0; i < m->cols; ++i)
    {
        for (size_t j = 0; j < m->lines; ++j)
        {
            int elt = getElement(m, i, j);
            if (elt > max)
                max = elt;
        }
    }

    if (max == INT32_MIN)
        err(1, "neuralNet.Max(): Failed to find maximum value.");
    
    return max;
}

int dotProduct(struct Matrix *input, size_t pos, struct Matrix *filter)
{
    int sum = 0;
    
    for (size_t i = 0; i < filter->cols /*&& i < input->cols - filter->cols*/; ++i)
    {
        for (size_t j = 0; j < filter->lines /*&& j < input->lines - filter->lines*/; ++j)
        {
            sum += input->matrix[pos] * filter->matrix[i * filter->cols + j];
        }
    }

    if (sum < 0)
        sum = 0;
    return sum;
}

struct Matrix **convolution(struct Matrix **input, struct Matrix *filter)
{
    printf("Starting convolution process.\n");

    int padSize = 1;

    struct Matrix **convolved_features = malloc(5 * sizeof(struct Matrix *));
    if (!convolved_features)
    {
        err(1, "neuralNet.convolution: Failed to allocate convolved_features.");
    }

    convolved_features[4] = NULL;

    for (int a = 0; convolved_features[a]; ++a)
    {
        printf("Convolved feature %d: ", a + 1);
        convolved_features[a] = init_matrix(0, 0);

        struct Matrix *convolved_feature = convolved_features[a];
        struct Matrix *padded = pad_input(input[a], padSize);

        convolved_feature->lines = padded->lines - filter->lines;


        int *convolved = convolved_feature->matrix;

        int c = 0;
        for (size_t i = 0; i < padded->cols && i < padded->cols - filter->cols; ++i)
        {
            for (size_t j = 0; j < padded->lines && j < padded->lines - filter->lines; ++j)
            {
                int result = dotProduct(padded, i * padded->cols + j, filter);

                convolved = realloc(convolved, (c + 1) * sizeof(int));
                if (!convolved)
                    err(1, "neuralNet.convolution: Failed to allocate convolved.");

                convolved[c] = result;
                c++;
            }
            if (convolved_feature->cols == 0)
                convolved_feature->cols = c;
        }

        convolved_feature->matrix = convolved;
        printf("Input convolved.\n\n");
    }

    return convolved_features;

}

void freeNeuralNet(struct NeuralNet* neuralNet)
{
    if (!neuralNet)
        return;

    free(neuralNet->firstLayer);
    free(neuralNet);
}