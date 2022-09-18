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
    m->matrix = malloc(cols * lines * sizeof(int));
    if (!m->matrix)
        return NULL;
    
    m->cols = cols;
    m->lines = lines;
    printf("Initialized new matrix.\n");
    clear_matrix(m);

    return m;
}

void print_matrix(struct Matrix *m)
{
    if (!m)
        return;

    if (!m->matrix)
        return;

    for (size_t i = 0; i < m->lines; ++i)
    {
        for (size_t j = 0; j < m->cols; ++j)
        {
            int *matrix = m->matrix;
            printf("%d", matrix[i * m->lines + j]);
            if (j < m->lines - 1)
                printf(" ");
            else
                printf("\n");
        }
    }
    printf("\n");
}

void fill_matrix(struct Matrix *m, int dbgFlag)
{
    if (!m)
        return;
    if (!m->matrix)
        return;
    
    int *matrix = m->matrix;
    srand(time(NULL));
    for (size_t i = 0; i < m->cols; ++i)
    {
        for (size_t j = 0; j < m->cols; ++j)
        {
            if (dbgFlag == 1)
                matrix[i * m->cols + j] = rand() % 10;
            else
                matrix[i * m->cols + j] = rand() & 1;
        }
    }

    printf("Matrix randomly filled:\n");
    if (dbgFlag == 2)
        print_matrix(m);
}
void clear_matrix(struct Matrix *m)
{
    if (!m)
        return;
    if (!m->matrix)
        return;
    int *matrix = m->matrix;
    for (size_t i = 0; i < m->cols; ++i)
    {
        for (size_t j = 0; j < m->lines; ++j)
        {
            matrix[j * m->lines + i] = 0;
        }
    }
    printf("Matrix values set to 0.\n\n");
}

void free_matrix(struct Matrix *m)
{
    if (!m)
        return;

    if (!m->matrix)
        return;
    
    free(m->matrix);
    printf("Matrix freed.\n");
    free(m);
    printf("Structure freed.\n\n");
}
int getElement(struct Matrix *m, int i, int j)
{
    if (!m)
        err(1, "Matrix.getElement: struct Matrix is NULL.");
    if (!m->matrix)
        err(1, "Matrix.getElement: element m->matrix is NULL.");
    return m->matrix[i * m->cols + j];
}
void setElement(struct Matrix *m, int element, int i, int j)
{
    if (!m)
        return;
    if (!m->matrix)
        return;
    m->matrix[j * m->lines + i] = element;
}

char *toString(struct Matrix *m)
{
    if (!m)
        return NULL;
    if (!m->matrix)
        return NULL;

    int *matrix = m->matrix;
    char *str = malloc((3 * 3) + 1);
    if (!str)
        return NULL;

    for (size_t i = 0; i < m->cols; ++i)
    {
        for (size_t j = 0; j < m->lines; ++j)
        {
            printf("Filter: %d\n", matrix[i * m->cols + j]);
            str[i * m->cols + j] = matrix[i * m->cols + j] + '0';
        }
    }
    str[9] = 0;
    return str;
}

void copyToFile(struct Matrix *m, const char *path, int index)
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
}

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
