#include "../../src/NeuralNetwork/neuralNet.h"
#include "../../src/utils/matrix.h"

#include <stdio.h>
#include <assert.h>

void filter_null()
{
    for (size_t k = 3; k < 5; ++k)
    {
        for (size_t l = 3; l < 5; ++l)
        {
            struct Matrix *input = init_matrix(k, l);
            fill_matrix(input, 1);

            struct Matrix *filter = init_matrix(3, 3);
            fill_matrix(filter, 0);

            input = convolution(input, filter);

            for (size_t i = 0; i < input->lines; ++i)
            {
                for (size_t j = 0; j < input->cols; ++j)
                {
                    if (input->matrix[j][i] != 0)
                    {
                        printf("Error: elt at pos %ld, %ld != 0.\n", j, i);
                        printf("Input:\n");
                        print_matrix(input);
                        printf("\nFilter:\n");
                        print_matrix(filter);
                        exit(1);
                    }
                }
            }
            free_matrix(input);
            free_matrix(filter);
        }
    }
}

void filter_one()
{
    for (size_t k = 3; k < 5; ++k)
    {
        for (size_t l = 3; l < 5; ++l)
        {
            printf("NEW LOOP:\n");
            struct Matrix *input = init_matrix(k, l);
            fill_matrix(input, 2);
            struct Matrix *copy = init_matrix(k, l);
            fill_matrix(copy, 2);

            struct Matrix *filter = init_matrix(3, 3);
            fill_matrix(filter, 1);

            // printf("input:\n");
            // print_matrix(input);
            // printf("\nfilter:\n");
            // print_matrix(filter);
            input = convolution(input, filter);
            // printf("\noutput:\n");
            // print_matrix(input);

            for (size_t i = 0; i < input->lines; ++i)
            {
                for (size_t j = 0; j < input->cols; ++j)
                {
                    if (input->matrix[j][i] != copy->matrix[j][i])
                    {
                        printf("Error: elt at pos %ld, %ld != 0.\n", j, i);
                        printf("Input:\n");
                        print_matrix(input);
                        printf("\nCopy:\n");
                        print_matrix(copy);
                        exit(1);
                    }
                }
            }
            
            free_matrix(filter);
            free_matrix(input);
            free_matrix(copy);
        }
    }
}

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

            if (i < 10)
            {
                // printf("\n========== NEW LOOP ===========\n\n");
                // printf("\ninput:\n");
                // print_matrix(input);
                // printf("\nfilter:\n");
                // print_matrix(filter);
                output = convolution(input, filter);
                // printf("\noutput:\n");
                // print_matrix(output);
            }
            free_matrix(filter);
            free_matrix(input);
            free_matrix(output);
        }
        }


    return 0;
}