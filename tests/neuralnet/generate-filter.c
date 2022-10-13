#include "../../src/NeuralNetwork/neuralNet.h"
#include "../../src/NeuralNetwork/filter.h"
#include "../../src/utils/matrix.h"

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    for (size_t i = 0; i < 1; ++i)
    {
        printf("Filter %ld:\n", i);
        struct Filter *filter = init_filter(NONE);
        for(size_t i = 0; i < filter->nbFilters; ++i)
        {
            print_matrix(filter->filters[i]);
            printf("\n\n");
        }
        free_filter(filter);
    }

    return 0;
}