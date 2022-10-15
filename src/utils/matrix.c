#include <stdint.h>
#define _GNU_SOURCE

#include "matrix.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <err.h>
#include <errno.h>

struct Matrix *init_matrix(size_t cols, size_t lines)
{
    struct Matrix *m = malloc(sizeof(struct Matrix));
    if (!m)
        return NULL;
    m->matrix = malloc(lines * sizeof(int*));
    if (!m->matrix)
        return NULL;

    for (size_t i = 0; i < lines; ++i)
    {
        m->matrix[i] = malloc(cols * sizeof(int));
        if (!m->matrix[i])
            return NULL;
    }
    
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
            int **matrix = m->matrix;
            printf("%d", matrix[i][j]);
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
    
    int **matrix = m->matrix;
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
                matrix[i][j] = rand() % 2;
        }
    }
}
void clear_matrix(struct Matrix *m)
{
    if (!m)
        return;
    if (!m->matrix)
        return;
    int **matrix = m->matrix;
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
void setElement(struct Matrix *m, int element, int i, int j)
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

double *flatMatrices(struct Matrix **matrices, size_t nbMatrices)
{
    size_t size = 0;
    for (size_t i = 0; i < nbMatrices; ++i)
    {
        size += matrices[i]->cols * matrices[i]->lines;
    }

    double *flat = calloc(size, sizeof(double));
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

double *flatMatrix(struct Matrix *m, double *ptr)
{
    if (!m)
        return NULL;
    if (!m->matrix)
        return NULL;

    for (size_t i = 0; i < m->lines; ++i)
    {
        for (size_t j = 0; j < m->cols; ++j)
        {
            ptr[i * m->cols + j] = (double) m->matrix[i][j];
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
