#include "../../src/utils/matrix.h"
#include "../../src/utils/xmalloc.h"

#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    for (size_t i = 3; i < 15; ++i)
    {
        for (size_t j = 3; j < 15; ++j)
        {
            struct Matrix *m = init_matrix(j, i);
            fill_matrix(m, 0);
            struct Matrix *t = transpose(m);
            // printf("Original:\n");
            // print_matrix(m);
            // printf("\nTransposed:\n");
            // print_matrix(t);
            free_matrix(m);
            free_matrix(t);
        }
    }
}