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

rtp_pay_ctx_t *rtp_pay_cxt_create(rtp_pay_ctx_create_info_t *info)
{
    rtp_pay_ctx_t *res = info->alloc(sizeof(rtp_pay_ctx_t));
    if (!res)
        return NULL;
    memset(res, 0, sizeof(rtp_pay_ctx_t));
    rtp_list_create_info_t list_info = {
        .alloc = info->alloc,
        .dealloc = info->dealloc
    };
    if (!(res->data_list = rtp_list_create(&list_info)))
        goto fail;
    res->data_offset = 0;
    res->sample = info->take_sample_own ? info->pkt_sample : rtp_pkt_copy(info->pkt_sample);
    if (!res->sample)
        goto fail;
    res->alloc = info->alloc;
    res->dealloc = info->dealloc;
    return res;
fail:
    if (res->sample && res->sample != info->pkt_sample)
        rtp_pkt_destroy(res->sample);
    if (res->data_list)
        rtp_list_destroy(res->data_list);
    info->dealloc(res);
    return NULL;
}

void rtp_pay_ctx_destroy(rtp_pay_ctx_t *ctx)
{
    rtp_list_destroy(ctx->data_list);
    rtp_pkt_destroy(ctx->sample);
    if (ctx->small_pkt)
        rtp_pkt_destroy(ctx->small_pkt);
    ctx->dealloc(ctx);
}

rtp_err_t rtp_pay_push(rtp_pay_ctx_t *ctx, void *data, uint_ size, uint_ own)
{
    data_t *new = ctx->alloc(sizeof(data_t));
    new->data = data;
    new->size = size;
    new->own = own;
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

#ifdef USE_TESTS
int main(int argc, char **argv)
{
    
    return 0;
}
#endif
