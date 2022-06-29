#ifndef _SILVER_BROCCOLI_COMMON_H
#define _SILVER_BROCCOLI_COMMON_H
#include <stdint.h>

typedef unsigned int uint_;

typedef enum rtp_err_t_
{
    rtp_err_ok,
    rtp_err_undefined
} rtp_err_t;

typedef struct rtp_pkt_header_t_
{
    const uint_ version:2;
    const uint_ padding:1;
    const uint_ extension:1;
    const uint_ csrc_count:4;
    uint_ marker:1;
    uint_ payload_type:7;
    uint16_t sequence_number;
    uint32_t timestamp;
    uint32_t ssrc;
} rtp_pkt_header_t;

typedef struct rtp_pkt_t_
{
    uint8_t *data;
    uint_ size;
} rtp_pkt_t;

typedef struct rtp_pkt_ctx_t_
{
    uint32_t *csrc_ids_begin;
    uint_ csrc_count;
    uint8_t *ext_header_begin;
    uint16_t ext_header_id;
    uint16_t ext_header_length;
    uint8_t *payload_begin;
    uint_ payload_size;

    rtp_pkt_t pkt;
    rtp_pkt_header_t header;
} rtp_pkt_ctx_t;

typedef struct rtp_pkt_ctx_create_info_t_
{
    uint_ payload_size;
    uint_ padding;
    uint_ extension;
    uint_ csrc_count;
    uint_ ext_header_id;
    uint_ ext_header_length;
} rtp_pkt_ctx_create_info_t;

#ifdef  __cplusplus
extern "C" {
#endif//__cplusplus

rtp_pkt_ctx_t *rtp_pkt_ctx_create(rtp_pkt_ctx_create_info_t *info);
void rtp_pkt_ctx_free(rtp_pkt_ctx_t *ctx);

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif//_SILVER_BROCCOLI_COMMON_H
