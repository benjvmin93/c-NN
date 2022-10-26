#include "../../src/NeuralNetwork/neuralNet.h"
#include "../../src/utils/matrix.h"

#include <stdio.h>
#include <assert.h>

int main(void)
{
    
    // filter_null();
    // filter_one();

    for (size_t i = 3; i < 6; ++i)
    {
        for (size_t j = 3; j < 6; ++j)
        {
            struct Matrix *input = init_matrix(i, j);
            struct Matrix *filter = init_matrix(3, 3);
            fill_matrix(input, 2);
            fill_matrix(filter, 5);
            struct Matrix *output = NULL;
            struct Weights *biases = init_weights(input->lines, 1);

            if (i < 10)
            {
                // printf("\n========== NEW LOOP ===========\n\n");
                // printf("\ninput:\n");
                // print_matrix(input);
                // printf("\nfilter:\n");
                // print_matrix(filter);
                output = convolution(input, filter, biases);
                // printf("\noutput:\n");
                // print_matrix(output);
            }
            free_matrix(filter);
            free_matrix(input);
            free_matrix(output);
            free_weight(biases);
        }
        }


    return 0;
}