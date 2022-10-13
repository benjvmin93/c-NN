#ifndef FILTER_H
#define FILTER_H

#include <stdlib.h>

enum ImageType
{
    NONE = -1,
    ANIMAL,
    VEHICULE,
    WOMAN,
    MAN,
    CHILD
};

struct Filter
{
    enum ImageType imageType;
    struct Matrix **filters;
    size_t nbFilters;
};

struct Filter *init_filter(enum ImageType imageType);
void free_filter(struct Filter *filter);

#endif