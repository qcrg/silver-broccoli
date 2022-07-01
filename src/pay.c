#include "pay.h"
#include "list.h"
#include <string.h>

typedef struct data_t_
{
    void *data;
    uint_ size;
    uint_ own;
} data_t;

typedef struct rtp_pay_ctx_t_
{
    rtp_list_t *data_list;
    uint_ data_offset;
    rtp_pkt_t *sample;
    rtp_pkt_t *small_pkt;
    uint_ curr_seq_num;
    rtp_alloc alloc;
    rtp_dealloc dealloc;
} rtp_pay_ctx_t;

rtp_err_t rtp_pay_push(rtp_pay_ctx_t *ctx, void *data, uint_ size, uint_ own)
{
    data_t *new = ctx->alloc(sizeof(data_t));
    new->data = data;
    new->size = size;
    new->own = own;
    return rtp_list_push_back(data_list, new);
}

rtp_err_t rtp_pay_push_copy(rtp_pay_ctx_t *ctx, void *data, uint_ size)
{
    data_t *new = ctx->alloc(sizeof(data_t));
    new->data = ctx->alloc(size);
    new->size = size;
    memcpy(new->data, data, size);
    new->own = 1;
    return rtp_list_push_back(data_list, new);
}

rtp_pkt_t *rtp_pay_pull(rtp_pay_ctx_t *ctx)
{
    data_t *data = rtp_list_get_front(ctx->data_list);
    rtp_pkt_t *pkt = ctx->sample;
    int remainder = pkt->payload_size - (data->size - ctx->data_offset);
    if (remainder > 0) {
        uint_ size = pkt->payload_size - remainder;
        if (!ctx->small_pkt || ctx->small_pkt->payload_size != size) {
            if (ctx->small_pkt)
                ctx->dealloc(ctx->small_pkt);
            rtp_pkt_alloc_info_t info = {
                    .csrc_count = pkt->csrc_count,
                    .ext = pkt->ext,
                    .ext_header_length = pkt->ext_size,
                    .padding = pkt->header->padding,
                    .payload_size = size
            };
            ctx->small_pkt = rtp_pkt_alloc(NULL, &info, ctx->alloc);
            pkt = ctx->small_pkt;
        }
    }
    memcpy(pkt->payload_begin, data->data + ctx->data_offset,
            pkt->payload_size);
    pkt->header->sequence_number = curr_seq_num++;
    if (remainder > 0) {
        ctx->data_offset = 0;
        rtp_list_destroy_front(ctx->data_list);
        if (data->own)
            ctx->dealloc(data->data);
    } else
        ctx->data_offset += pkt->payload_size;
    return pkt;
}

rtp_pkt_t *rtp_pay_pull_new(rtp_pay_ctx_t *ctx)
{
    return rtp_pkt_copy(rtp_pay_pull(ctx));
}
