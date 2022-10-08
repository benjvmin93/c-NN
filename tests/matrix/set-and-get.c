#include "../../src/utils/matrix.h"

#include <stdio.h>
#include <assert.h>

int main(void)
{
    for (size_t y = 1; y < 100; ++y) 
    {
        for (size_t x = 1; x < 100; ++x)
        {
            struct Matrix *m = init_matrix(x, y);
            for (size_t i = 0; i < m->lines; ++i)
            {
                for (size_t j = 0; j < m->cols; ++j)
                {                    
                    setElement(m, i, i, j);
                    assert(getElement(m, i, j) == m->matrix[i][j]);
                }
            }

            int *ptr = flatMatrix(m);
            for (size_t i = 0; i < m->lines; ++i)
            {
                for (size_t j = 0; j < m->cols; ++j)
                {
                    if (getElement(m, i, j) != ptr[i * m->cols + j])
                    {
                        printf("ERROR: different elements.\nMatrix:\n");
                        print_matrix(m);
                        printf("\npos: %ld\n", i * m->cols + j);
                        printf("\n%d != %d\n", getElement(m, i, j), ptr[i * m->cols + j]);
                        exit(1);
                    }
                }
            }
            free(ptr);
            free_matrix(m);
        }
    }
    
    return 0;
}