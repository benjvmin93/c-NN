#ifndef FILTER_H
#define FILTER_H

#include <stdlib.h>

struct Filter
{
    struct Matrix **filters;
    size_t nbFilters;
};

struct Filter *init_filter();
void free_filter(struct Filter *filter);

#endif