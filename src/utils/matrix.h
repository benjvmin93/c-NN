#ifndef MATRIX_H
#define MATRIX_H

#include <stdlib.h>

struct Matrix
{
    size_t cols;
    size_t lines;
    int *matrix;
};

struct Matrix *init_matrix(size_t cols, size_t lines);
void print_matrix(struct Matrix* m);
void fill_matrix(struct Matrix *m, int dbgFlag);
char *toString(struct Matrix *m);
void clear_matrix(struct Matrix *m);
void free_matrix(struct Matrix *m);
int getElement(struct Matrix *m, int i, int j);
void setElement(struct Matrix *m, int element, int i, int j);
struct Matrix *copyFromFile(struct Matrix *m, const char *path, int index);
void copyToFile(struct Matrix *m, const char *path, int index);

#endif