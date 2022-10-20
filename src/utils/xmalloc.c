#include "xmalloc.h"

#include <err.h>
#include <errno.h>

void *xmalloc(size_t nbElt, size_t size)
{
    void *alloc = malloc(nbElt * size);
    if (!alloc)
    {
        err(errno, "Couldn't allocate memory.");
    }

    return alloc;
}