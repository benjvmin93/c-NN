#include "../../src/utils/matrix.h"

#include <stdio.h>
#include <assert.h>

int main(void)
{
    struct Matrix *m = init_matrix(2, 2);
    assert(m->cols == 2);
    assert(m->lines == 2);

    for (size_t i = 0; i < m->lines; ++i)
    {
        for (size_t j = 0; j < m->cols; ++j)
        {
            assert(m->matrix[i][j] == 0);
        }
    }

    free_matrix(m);

    for (size_t i = 0; i < 15; ++i)
    {
        for (size_t j = 0; j < 15; ++j)
        {
            m = init_matrix(i, j);

            fill_matrix(m, 2);

            // printf("\n");
            // print_matrix(m);
            // printf("\n");

            for (size_t i = 0; i < m->lines; ++i)
            {
                for (size_t j = 0; j < m->cols; ++j)
                {
                    assert((size_t) m->matrix[i][j] == i * m->cols + j);
                }
            }
            free_matrix(m);
        }
    }
    

    return 0;
}