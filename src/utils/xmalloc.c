#include "xmalloc.h"

#include <err.h>
#include <errno.h>
#include <stdlib.h>

void *xmalloc(size_t nbElt, size_t size)
{
    void *alloc = malloc(nbElt * size);
    if (!alloc)
    {
        err(errno, "Malloc failed.");
    }

    return alloc;
}

void *xcalloc(size_t nbElt, size_t size)
{
    void *alloc = calloc(nbElt, size);
    if (!alloc)
        err(errno, "Calloc failed.");

    return alloc;
}
