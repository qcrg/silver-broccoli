#ifndef _SILVER_BROCCOLI_ALLOCATOR_H
#define _SILVER_BROCCOLI_ALLOCATOR_H
#include "common.h"

#define RTP_INIT_ALLOC(p) if (!p) p = rtp_def_alloc
#define RTP_INIT_DEALLOC(p) if (!p) p = rtp_def_dealloc

typedef void *(*rtp_alloc)(uint_ size);
typedef void (*rtp_dealloc)(void *ptr);

void *rtp_def_alloc(uint_ size);
void rtp_def_dealloc(void *p);

#endif//_SILVER_BROCCOLI_ALLOCATOR_H
