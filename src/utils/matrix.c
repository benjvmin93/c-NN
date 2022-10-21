#define _GNU_SOURCE
#define _DEFAULT_SOURCE

#include "matrix.h"
#include "xmalloc.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <err.h>
#include <errno.h>
#include <sys/time.h>

struct Matrix *init_matrix(size_t cols, size_t lines)
{
    struct Matrix *m = xmalloc(1, sizeof(struct Matrix));
    m->matrix = xmalloc(lines, sizeof(float*));

    for (size_t i = 0; i < lines; ++i)
        m->matrix[i] = xmalloc(cols, sizeof(float));
    
    m->cols = cols;
    m->lines = lines;
    clear_matrix(m);

    return m;
}

void print_matrix(struct Matrix *m)
{
    if (!m)
        return;

    if (!m->matrix)
        return;

    for (size_t i = 0; i != m->lines; ++i)
    {
        for (size_t j = 0; j < m->cols; ++j)
        {
            float **matrix = m->matrix;
            printf("%f", matrix[i][j]);
            if (j == m->cols - 1)
                printf("\n");
            else
                printf("  ");
        }
    }
}

void free_inside_matrix(struct Matrix *m)
{
    for (size_t i = 0; i < m->lines; ++i)
    {
        free(m->matrix[i]);
    }
    free(m->matrix);
    m->matrix = NULL;
}


void fill_matrix(struct Matrix *m, int dbgFlag)
{
    if (!m)
        return;
    if (!m->matrix)
        return;
    
    float **matrix = m->matrix;
    // int t;

    struct timeval tm;
    gettimeofday(&tm, NULL);
    srandom(tm.tv_sec + tm.tv_usec * 1000000ul);
    for (size_t i = 0; i < m->lines; ++i)
    {
        for (size_t j = 0; j < m->cols; ++j)
        {      
            if (dbgFlag == 0)
                matrix[i][j] = 0;
            else if (dbgFlag == 1)
                matrix[i][j] = 1;
            else if (dbgFlag == 2)
                matrix[i][j] = i * m->cols + j;
            else
            {
                int rand = random() % 3 + (-1);


                matrix[i][j] = rand;
            }
        }
    }
}

void clear_matrix(struct Matrix *m)
{
    if (!m)
        return;
    if (!m->matrix)
        return;
    float **matrix = m->matrix;
    for (size_t i = 0; i < m->lines; ++i)
    {
        for (size_t j = 0; j < m->cols; ++j)
        {
            matrix[i][j] = 0;
        }
    }
}

void free_matrix(struct Matrix *m)
{
    if (!m)
        return;

    if (!m->matrix)
        return;

    for (size_t i = 0; i < m->lines; ++i)
    {
        free(m->matrix[i]);
    }
    
    free(m->matrix);
    free(m);
}

int getElement(struct Matrix *m, int i, int j)
{
    if (!m)
        err(1, "Matrix.getElement: struct Matrix is NULL.");
    if (!m->matrix)
        err(1, "Matrix.getElement: element m->matrix is NULL.");
    return m->matrix[i][j];
}
void setElement(struct Matrix *m, float element, int i, int j)
{
    if (!m)
        return;
    if (!m->matrix)
        return;
    m->matrix[i][j] = element;
}

int isEqual(struct Matrix *m1, struct Matrix *m2)
{
    if (m1->cols != m2->cols || m1->lines != m2->lines)
        return 0;
    for (size_t i = 0; i < m1->lines; ++i)
    {
        for (size_t j = 0; j < m1->cols; ++j)
        {
            if (m1->matrix[i][j] != m2->matrix[i][j])
                return 0;
        }
    }
    return 1;
}

float *flatMatrices(struct Matrix **matrices, size_t nbMatrices)
{
    size_t size = 0;
    for (size_t i = 0; i < nbMatrices; ++i)
    {
        size += matrices[i]->cols * matrices[i]->lines;
    }

    float *flat = calloc(size, sizeof(float));
    if (!flat)
        err(1, "matrix.flatMatrices(): Couldn't allocate flat.");
    
    size_t offset = 0;
    for (size_t i = 0; i < nbMatrices; ++i)
    {
        struct Matrix *m = matrices[i];
        flatMatrix(m, flat + offset);
        offset += m->cols * m->lines;
    }

    return flat;
}

float *flatMatrix(struct Matrix *m, float *ptr)
{
    if (!m)
        return NULL;
    if (!m->matrix)
        return NULL;

    for (size_t i = 0; i < m->lines; ++i)
    {
        for (size_t j = 0; j < m->cols; ++j)
        {
            ptr[i * m->cols + j] = m->matrix[i][j];
            // printf("flatMatrix: %f\n", ptr[i * m->cols + j]);
        }
    }
    return ptr;
}

/* void copyToFile(struct Matrix *m, const char *path, int index)
{
    char *str = toString(m);

    if (index > 10)
    {
        free(str);
        return;
    }

    FILE *file = fopen(path, "r+");
    
    if (!file)
        err(1, "Cannot open file %s", path);
    
    if (index != -1 && index <= 10)
    {
        if (fseek(file, 10 * (index - 1), SEEK_SET) != 0)
            err(errno, "Fseek failed at index %d.", index);

        fprintf(file, "%s\n", str);
    }
    else if (index - 1 < 10)
    {
        if (fseek(file, 0, SEEK_END) != 0)
            err(errno, "Fseek at end failed.");

        fprintf(file, "%s\n", str);
    }
    fclose(file);
    free(str);
} */

/* struct Matrix *copyFromFile(const char *path, int index)
{
    if (index > 10 || index < 1) 
        return NULL;
    
    FILE *file = fopen(path, "r");

    if (!file)
        err(1, "Cannot open file %s", path);
    
    if (fseek(file, 10 * (index - 1), SEEK_SET) != 0)
        err(errno, "Fseek failed at index %d.", index);
    
    char *line = NULL;
    size_t len = 0;
    int i = 0;
    int j = 0;
    struct Matrix *m = NULL;
    if (getline(&line, &len, file) != -1)
    {
        m = init_
        clear_matrix(m);
        int *matrix = m->matrix;
        for (int k = 0; line[k]; ++k)
        {
            if (j == 3)
            {
                j = 0;
                i++;
            }
            if (i == 3)
                break; // Never goes here.
            if (line[k] == '1')
                matrix[i * m->cols + j] = 1;
            j++;
        }
    }

    print_matrix(m);

    return m;
}*/
