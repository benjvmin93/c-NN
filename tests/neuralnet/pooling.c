#include "../../src/NeuralNetwork/neuralNet.h"
#include "../../src/NeuralNetwork/filter.h"
#include "../../src/utils/matrix.h"

#include <stdio.h>
#include <assert.h>

int main(void)
{

    for (size_t i = 3; i < 50; ++i)
    {
        for (size_t j = 3; j < 50; ++j)
        {
            struct Filter *filter = init_filter();
            struct Matrix *m = init_matrix(i, j);
            fill_matrix(m, 2);

            // struct Matrix *filter = generate_filter(3, 3);
            // fill_matrix(filter, -1);
            // printf("===========================\n\n");
            // print_matrix(m);
            // printf("\n");
            // print_matrix(filter);
            // printf("\n");

            struct Matrix *pooled_feature = pooling(m, filter->filters[0], 1);
            // print_matrix(pooled_feature);

            free_matrix(m);
            free_matrix(pooled_feature);
            free_filter(filter);

        }
    }


    return 0;
}