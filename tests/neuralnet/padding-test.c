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
        for (size_t i = 1; i < 10; ++i)
        {
            for (size_t j = 1; j < 10; ++j)
            {
                cols = i;
                lines = j;
                
                struct Matrix *matrix = init_matrix(cols, lines); 
                fill_matrix(matrix, 0);
                struct Matrix *padded_input = pad_input(matrix, padSize);

                if (padded_input->cols != (cols + (2 * padSize)) || padded_input->lines != (lines + (2 * padSize)))
                {
                    printf("Error padSize %ld.\n", padSize);
                    printf("Initial matrix:\n");
                    print_matrix(matrix);

                    printf("\nPadded matrix:\n");
                    print_matrix(padded_input);
                    printf("\n\n");

                    exit(1);
                }

                // free_ptr(matrix->matrix, lines);

                free_matrix(padded_input);
                free_matrix(matrix);
            }
        }
    }

    return 0;
}