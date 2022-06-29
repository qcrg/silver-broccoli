#ifndef _SILVER_BROCCOLI_PAY_H
#define _SILVER_BROCCOLI_PAY_H

#include "general.h"

typedef struct rtp_pay_ctx_t_ rtp_pay_ctx_t;

#ifdef  __cplusplus
extern "C" {
#endif//__cplusplus

rtp_pay_cxt_t *rtp_pay_ctx_create(rtp_pkt_ctx_t *pkt_ctx);
void rtp_pay_ctx_free();

rtp_err_t rtp_pay_ctx_push(void *data, uint_ size);
rtp_err_t rtp_pay_ctx_push_own(void *data, uint_ size);

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif//_SILVER_BROCCOLI_PAY_H
