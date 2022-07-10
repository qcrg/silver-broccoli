#ifndef _SILVER_BROCCOLI_DEPAY_H
#define _SILVER_BROCCOLI_DEPAY_H
#include "common.h"
#include "allocator.h"
#include "packet.h"

typedef struct rtp_depay_ctx_t_ rtp_depay_ctx_t;

typedef struct rtp_depay_ctx_create_info_t_
{
    uint_ frame_size;
    rtp_allocator_t alctr;
} rtp_depay_ctx_create_info_t;

#ifdef  __cplusplus
extern "C" {
#endif//__cplusplus

rtp_depay_ctx_t *rtp_depay_ctx_create(rtp_depay_ctx_create_info_t *info);
void *rtp_depay_ctx_destroy(rtp_depay_ctx_t *ctx);

rtp_err_t rtp_depay_push(rtp_depay_ctx_t *ctx, rtp_pkt_t *pkt);
rtp_err_t rtp_depay_pull(rtp_depay_ctx_t *ctx, data_t *frame);

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif//_SILVER_BROCCOLI_DEPAY_H
