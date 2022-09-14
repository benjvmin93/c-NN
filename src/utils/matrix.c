#define _GNU_SOURCE

#include "matrix.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <err.h>
#include <errno.h>

int **matrix = NULL;

int **init_matrix()
{
    matrix = malloc(3 * sizeof(int*));
    if (!matrix)
        return NULL;
    for (int i = 0; i < 3; ++i)
    {
        matrix[i] = malloc(3 * sizeof(int));
        if (!matrix[i])
            return NULL;
    }

    return matrix;
}

void print_matrix()
{
    if (!matrix)
        return;
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            printf("%d", matrix[i][j]);
            if (j < 2)
                printf(" ");
            else
                printf("\n");
        }
    }
    printf("\n");
}
void fill_matrix()
{
    srand(time(0));
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            matrix[i][j] = rand() & 1;
        }
    }
    printf("Matrix randomly filled:\n");
    print_matrix();
}
void clear_matrix()
{
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            matrix[i][j] = 0;
        }
    }
    printf("Matrix cleared.\n");
    print_matrix();
}
void free_matrix()
{
    if (!matrix)
        return;
    for (int i = 0; i < 3; ++i)
    {
        free(matrix[i]);
    }
    free(matrix);
}
int getElement(int i, int j)
{
    if (!matrix)
        return -1;
    return matrix[i][j];
}
void setElement(int element, int i, int j)
{
    if (!matrix)
        return;
    matrix[i][j] = element;
}

char *toString()
{
    if (!matrix)
        return NULL;
    char *str = malloc((3 * 3) + 1);
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            printf("Filter: %d\n", matrix[i][j]);
            str[i * 3 + j] = matrix[i][j] + '0';
        }
    }
    str[9] = 0;
    return str;
}

void copyToFile(const char *path, int index)
{
    char *str = toString();

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

void copyFromFile(const char *path, int index)
{
    if (index > 10 || index < 1) 
        return;
    
    FILE *file = fopen(path, "r");

    if (!file)
        err(1, "Cannot open file %s", path);
    
    if (fseek(file, 10 * (index - 1), SEEK_SET) != 0)
        err(errno, "Fseek failed at index %d.", index);
    
    char *line = NULL;
    size_t len = 0;
    int i = 0;
    int j = 0;
    if (getline(&line, &len, file) != -1)
    {
        clear_matrix();
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
                matrix[i][j] = 1;
            j++;
        }
    }

    print_matrix();
}
