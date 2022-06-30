#include "allocator.h"
#include <stdlib.h>

void *rtp_def_alloc(uint_ size)
{
    return malloc(size);
}

void rtp_def_dealloc(void *p)
{
    free(p);
}
