#ifndef MATRIX_H
#define MATRIX_H

#include <stdlib.h>

struct Matrix
{
    size_t cols;
    size_t lines;
    float **matrix;
};

struct Matrix *init_matrix(size_t cols, size_t lines);
void print_matrix(struct Matrix* m);
void fill_matrix(struct Matrix *m, int dbgFlag);
int isEqual(struct Matrix *m1, struct Matrix *m2);
float *flatMatrix(struct Matrix *m, float *ptr);
float *flatMatrices(struct Matrix **matrices, size_t nbMatrices);
void clear_matrix(struct Matrix *m);
void free_matrix(struct Matrix *m);
float getElement(struct Matrix *m, int i, int j);
void setElement(struct Matrix *m, float element, int i, int j);
void free_inside_matrix(struct Matrix *m);
struct Matrix *transpose(struct Matrix *m);
// struct Matrix *copyFromFile(struct Matrix *m, const char *path, int index);
// void copyToFile(struct Matrix *m, const char *path, int index);

#endif