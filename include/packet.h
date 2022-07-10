#ifndef _SILVER_BROCCOLI_PACKET_H
#define _SILVER_BROCCOLI_PACKET_H
#include "common.h"
#include "allocator.h"
#include <stdint.h>

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
    void *data;
    uint_ size;

    uint32_t *csrc_begin;
    uint_ csrc_count;

    void *ext_begin;
    uint_ ext_size;

    void *payload_begin;
    uint_ payload_size;

    uint_ padding_size;

    rtp_pkt_header_t *header;

    rtp_allocator_t alctr;
} rtp_pkt_t;

typedef struct rtp_pkt_alloc_info_t_
{
    uint_ csrc_count;
    uint_ ext;
    uint16_t ext_header_length;
    uint_ padding;
    uint_ payload_size;
    rtp_allocator_t alctr;
} rtp_pkt_alloc_info_t;

typedef struct rtp_pkt_data_init_info_t_
{
    uint_ padding;
    uint_ ext;
    uint16_t ext_id;
    uint16_t ext_header_length;
    uint_ csrc_count;
} rtp_pkt_data_init_info_t;

#ifdef  __cplusplus
extern "C" {
#endif//__cplusplus

rtp_pkt_t *rtp_pkt_alloc(rtp_pkt_alloc_info_t *info);
rtp_err_t rtp_pkt_data_init(rtp_pkt_t *pkt, rtp_pkt_data_init_info_t *info);
void rtp_pkt_destroy(rtp_pkt_t *pkt);

rtp_pkt_t *rtp_pkt_copy(const rtp_pkt_t *src);

rtp_pkt_t *rtp_pkt_parse(data_t data, rtp_allocator_t alctr);


#ifdef  __cplusplus
}
#endif//__cplusplus

#endif//_SILVER_BROCCOLI_COMMON_H
