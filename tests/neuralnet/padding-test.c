#include "../../src/NeuralNetwork/neuralNet.h"
#include "../../src/utils/matrix.h"

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

void free_ptr(int **ptr, size_t lines)
{
    for (size_t i = 0; i < lines; ++i)
    {
        free(ptr[i]);
    }
    free(ptr);
}

int main(void)
{
    size_t cols = 0;
    size_t lines = 0;
    size_t padSize = 1;

    for (padSize = 1; padSize < 5; ++padSize) 
    {
        for (size_t i = 1; i < 15; ++i)
        {
            for (size_t j = 1; j < 15; ++j)
            {
                cols = i;
                lines = j;
                
                struct Matrix *matrix = init_matrix(cols, lines); 
                fill_matrix(matrix, 0);
                int **padded_input = pad_input(matrix, padSize);

                if (matrix->cols != (cols + (2 * padSize)) || matrix->lines != (lines + (2 * padSize)))
                {
                    printf("Error padSize %ld.\n", padSize);
                    printf("Initial matrix:\n");
                    print_matrix(matrix);

                    matrix->matrix = padded_input;
                    printf("\nPadded matrix:\n");
                    print_matrix(matrix);
                    printf("\n\n");

                    exit(1);
                }

                free_ptr(matrix->matrix, lines);
                matrix->matrix = padded_input;
                free_matrix(matrix);
            }
        }
    }

    return 0;
}