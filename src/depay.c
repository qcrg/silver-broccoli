#include "depay.h"
#include "list.h"
#include "packet.h"

typedef struct rtp_depay_ctx_t_
{
    rtp_list_t *data_list;
    data_t frame;
    uint_ pkt_counter;
    uint_ current_seq_num;
    rtp_alloc alloc;
    rtp_dealloc dealloc;
} rtp_depay_ctx_t;

rtp_depay_ctx_t *rtp_depay_ctx_create(rtp_depay_ctx_create_info_t *info)
{
    rtp_depay_ctx_t *res = info->alloc(sizeof(*res));
    if (!res)
        return NULL;
    memset(res, 0, sizeof(*res));
    rtp_list_create_info_t linfo = {
        .alloc = info->alloc,
        .dealloc = info->dealloc
    };
    if (!(res->data_list = rtp_list_create(&list_info)))
        goto fail;
    if (!(res->frame.data = info->alloc(info->frame_size)))
        goto fail;
    res->frame.size = info->frame_size;
    res->frame.own = 1;
    res->alloc = info->alloc;
    res->dealloc = info->dealloc;
    return res;
fail:
    if (res->frame->data)
        info->dealloc(res->frame->data);
    if (res->data_list)
        rtp_list_destroy(res->data_list);
    info->dealloc(res);
    return NULL;
}

rtp_err_t rtp_depay_push(rtp_depay_ctx_t *ctx, data_t *data)
{
    return rtp_list_push_front(ctx->data_list, data);
}

rtp_err_t rtp_depay_pull(rtp_depay_ctx_t *ctx, data_t **frame)
{
    data_t *curr_data;
    while ((curr_data = rtp_list_pull_front(ctx->data_list))) {
        rtp_pkt
    }
}
