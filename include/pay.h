#ifndef _SILVER_BROCCOLI_PAY_H
#define _SILVER_BROCCOLI_PAY_H

#include "common.h"
#include "packet.h"
#include "allocator.h"

typedef struct rtp_pay_ctx_t_ rtp_pay_ctx_t;

typedef struct rtp_pay_ctx_create_info_t_
{
    rtp_pkt_t *pkt_sample;
    uint_ take_sample_own;
    rtp_allocator_t alctr;
} rtp_pay_ctx_create_info_t;

#ifdef  __cplusplus
extern "C" {
#endif//__cplusplus

rtp_pay_ctx_t *rtp_pay_ctx_create(rtp_pay_ctx_create_info_t *info);
void rtp_pay_ctx_destroy(rtp_pay_ctx_t *ctx);

rtp_err_t rtp_pay_push(rtp_pay_ctx_t *ctx, data_t data);
rtp_pkt_t *rtp_pay_pull(rtp_pay_ctx_t *ctx);

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif//_SILVER_BROCCOLI_PAY_H
