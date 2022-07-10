#ifndef _SILVER_BROCCOLI_ALLOCATOR_H
#define _SILVER_BROCCOLI_ALLOCATOR_H
#include "common.h"

typedef void *(*rtp_alloc)(uint_);
typedef void (*rtp_dealloc)(void *);

typedef struct rtp_allocator_t_
{
    rtp_alloc alloc;
    rtp_dealloc dealloc;
} rtp_allocator_t;

extern rtp_allocator_t rtp_def_allocator;

#endif//_SILVER_BROCCOLI_ALLOCATOR_H
