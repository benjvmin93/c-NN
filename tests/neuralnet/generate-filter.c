#include "../../src/NeuralNetwork/neuralNet.h"
#include "../../src/utils/matrix.h"

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>

int isEqual(struct Matrix *m1, struct Matrix *m2)
{
    if (m1->cols != m2->cols || m1->lines != m2->lines)
        return 0;
    for (size_t i = 0; i < m1->cols; ++i)
    {
        for (size_t j = 0; j < m1->lines; ++j)
        {
            if (m1->matrix[i][j] != m2->matrix[i][j])
                return 0;
        }
    }
    return 1;
}

int main(void)
{
    for (size_t i = 0; i < 10; ++i)
    {
        // printf("Filter %ld:\n", i);
        struct Matrix *filter = generate_filter(3, 3);
        sleep(1);
        // print_matrix(filter);
        // printf("\n");
        free_matrix(filter);
    }

    return 0;
}