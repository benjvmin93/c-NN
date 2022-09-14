#ifndef MATRIX_H
#define MATRIX_H

int **init_matrix();
void print_matrix();
void fill_matrix();
char *toString();
void clear_matrix();
void free_matrix();
int getElement(int i, int j);
void setElement(int element, int i, int j);
void copyFromFile(const char *path, int index);
void copyToFile(const char *path, int index);

#endif