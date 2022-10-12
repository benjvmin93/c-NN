#include "filter.h"
#include "../utils/matrix.h"

#include <err.h>
#include <time.h>

struct Filter *init_filter(enum ImageType imageType)
{
    struct Filter *filters = malloc(sizeof(struct Filter));
    if (!filters)
        err(1, "filter.init_filter(): Couldn't allocate filter.");

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

    filters->nbFilters = 6;

    filters->filters = malloc(filters->nbFilters * sizeof(struct Matrix));
    if (!filters->filters)
        err(1, "filter.init_filter(): Couldn't allocate filters->filters.");
    
    for (size_t i = 0; i < filters->nbFilters; ++i)
    {
        filters->filters[i] = init_matrix(3, 3);
        fill_matrix(filters->filters[i], -1);
        print_matrix(filters->filters[i]);
    }

    return filters;
}