#include "depay.h"
#include "list.h"
#include "packet.h"
#include <string.h>

typedef struct rtp_depay_ctx_t_
{
    rtp_list_t *pkt_list;
    data_t frame;
    uint_ pkt_counter;
    uint_ current_seq_num;
    uint_ timestamp;
    rtp_allocator_t alctr;
} rtp_depay_ctx_t;

rtp_depay_ctx_t *rtp_depay_ctx_create(rtp_depay_ctx_create_info_t *info)
{
    rtp_depay_ctx_t *res = info->alctr.alloc(sizeof(*res));
    if (!res)
        return NULL;
    memset(res, 0, sizeof(rtp_depay_ctx_t));
    rtp_list_create_info_t linfo = {
        .alctr = info->alctr
    };
    if (!(res->pkt_list = rtp_list_create(&linfo)))
        goto fail;
    if (!(res->frame.data = info->alctr.alloc(info->frame_size)))
        goto fail;
    res->frame.size = info->frame_size;
    res->frame.own = 1;
    res->alctr = info->alctr;
    return res;
fail:
    if (res->frame.data)
        info->alctr.dealloc(res->frame.data);
    if (res->pkt_list)
        rtp_list_destroy(res->pkt_list);
    info->alctr.dealloc(res);
    return NULL;
}

rtp_err_t rtp_depay_push(rtp_depay_ctx_t *ctx, rtp_pkt_t *pkt)
{
    return rtp_list_push_front(ctx->pkt_list, pkt);
}

rtp_err_t new_frame(rtp_depay_ctx_t *ctx)
{
    ctx->frame.data = ctx->alctr.alloc(ctx->frame.size);
    return ctx->frame.data ? rtp_err_ok : rtp_err_memory;
}

rtp_err_t rtp_depay_pull(rtp_depay_ctx_t *ctx, data_t *frame)
{
    rtp_pkt_t *pkt = rtp_list_pull_front(ctx->pkt_list);
    if (!pkt) {
        return rtp_err_data;
    }
    if (ctx->timestamp != pkt->header->timestamp) {
        *frame = ctx->frame;
        rtp_err_t res = new_frame(ctx);
        if (res != rtp_err_ok)
            return res;
    }
    ctx->timestamp = pkt->header->timestamp;
    memcpy((uint8_t *)ctx->frame.data +
            ctx->frame.size * pkt->header->sequence_number,
            pkt->payload_begin, pkt->payload_size);
    return rtp_err_ok;
}
