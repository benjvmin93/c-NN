#include "filter.h"
#include "../utils/matrix.h"
#include "../utils/xmalloc.h"

#include <err.h>
#include <time.h>

#define NB_FILTERS 5
#define SIZE_FILTER 3

struct Filter *init_filter(enum ImageType imageType)
{
    struct Filter *filters = xmalloc(1, sizeof(struct Filter));
   
    switch (imageType)
    {
        case ANIMAL:
            break;
        case VEHICULE:
            break;
        case WOMAN:
            break;
        case MAN:
            break;
        case CHILD:
            break;
        default:
            break;
    }

    filters->nbFilters = NB_FILTERS;

    filters->filters = xmalloc(filters->nbFilters, sizeof(struct Matrix *));
    
    for (size_t i = 0; i < filters->nbFilters; ++i)
    {
        filters->filters[i] = init_matrix(SIZE_FILTER, SIZE_FILTER);
        fill_matrix(filters->filters[i], -1);
    }

    return filters;
}

void free_filter(struct Filter *filter)
{
    if (!filter)
        return;

    if (filter->nbFilters == 0 && !filter->filters)
        return;
    for (size_t i = 0; i < filter->nbFilters; ++i)
        free_matrix(filter->filters[i]);
    free(filter->filters);
    free(filter);
}