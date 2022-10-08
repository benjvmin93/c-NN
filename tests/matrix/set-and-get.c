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
            for (size_t i = 0; i < m->cols; ++i)
            {
                for (size_t j = 0; j < m->lines; ++j)
                {
                    // printf("i: %ld, j: %ld\n", i, j);
                    // printf("cols: %ld, lines: %ld\n", m->cols, m->lines);
                    setElement(m, i, i, j);
                    assert(getElement(m, i, j) == m->matrix[i][j]);
                }
            }

            int *ptr = getPtr(m);
            for (size_t i = 0; i < m->cols; ++i)
            {
                for (size_t j = 0; j < m->lines; ++j)
                {
                    if (getElement(m, i, j) != ptr[i * m->lines + j])
                    {
                        printf("ERROR: different elements.\nMatrix:\n");
                        print_matrix(m);
                        printf("\npos: %ld\n", i * m->lines + j);
                        printf("\n%d != %d\n", getElement(m, i, j), ptr[i * m->lines + j]);
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