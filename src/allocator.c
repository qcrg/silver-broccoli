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

rtp_allocator_t rtp_def_allocator = {
    .alloc = rtp_def_alloc,
    .dealloc = rtp_def_dealloc
};
