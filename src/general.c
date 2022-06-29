#include "general.h"
#include <stdlib.h>
#include <string.h>

rtp_pkt_ctx_t *plug__;

enum
{
    pkt_ctx_sizeof = sizeof(rtp_pkt_ctx_t),
    pkt_header_sizeof = sizeof(rtp_pkt_header_t),
    ssrc_sizeof = sizeof(plug__->header.ssrc),
    ext_header_id_sizeof = sizeof(plug__->ext_header_id),
    ext_header_length_sizeof = sizeof(plug__->ext_header_length),
    csrc_sizeof = sizeof(*plug__->csrc_ids_begin)
};

rtp_err_t rtp_pkt_ctx_init(rtp_pkt_ctx_t *ctx, rtp_pkt_ctx_create_info_t *info,
        uint_ csrc_size, uint_ extension_size, uint_ padding_size)
{
    rtp_pkt_header_t *head = &ctx->header;
    rtp_pkt_header_t tmp_pkt_header = {
        .version = 0x2,
        .padding = info->padding ? 0x1 : 0x0,
        .extension = info->extension ? 0x1 : 0x0,
        .csrc_count = info->csrc_count
    };
    memcpy(head, &tmp_pkt_header, sizeof(rtp_pkt_header_t));

    ctx->pkt.data = (uint8_t *)head;
    ctx->pkt.size = pkt_header_sizeof + csrc_size +
        extension_size + padding_size;

    uint8_t *pkt_begin = ctx->pkt.data;
    uint_ offset = pkt_header_sizeof;

    ctx->csrc_ids_begin = info->csrc_count ? pkt_begin + offset : NULL;
    ctx->csrc_count = info->csrc_count;
    offset += csrc_size;

    ctx->ext_header_begin = info->extension ? pkt_begin + offset +
        ext_header_id_sizeof + ext_header_length_sizeof : NULL;
    ctx->ext_header_id = info->ext_header_id;
    ctx->ext_header_length = info->ext_header_length;
    offset += extension_size;

    ctx->payload_begin = pkt_begin + offset;
    ctx->payload_size = info->payload_size;
    offset += info->payload_size;

    memset(pkt_begin + offset, 0, padding_size);
    offset += padding_size;

    ctx->pkt.size = offset;
    pkt_begin[offset - 1] = padding_size - 1;

    return rtp_err_ok;
}

rtp_pkt_ctx_t *rtp_pkt_ctx_create(rtp_pkt_ctx_create_info_t *info)
{
    rtp_pkt_ctx_t *res;
    uint_ csrc_size = info->csrc_count * csrc_sizeof;
    uint_ extension_size = info->extension ? ext_header_id_sizeof +
        ext_header_length_sizeof + info->ext_header_length : 0;
    uint_ padding_size = info->padding ? 32 - (pkt_header_sizeof +
            csrc_size + extension_size + info->payload_size) % 32 : 0;

    res = (rtp_pkt_ctx_t *)malloc(pkt_ctx_sizeof +
            csrc_size + extension_size + info->payload_size + padding_size);

    if (res)
        if (rtp_err_ok != rtp_pkt_ctx_init(res, info,
                csrc_size, extension_size, padding_size)) {
            free(res);
            res = NULL;
        }

    return res;
}

void rtp_pkt_ctx_free(rtp_pkt_ctx_t *ctx)
{
    free(ctx);
}


#ifdef USE_TESTS
#include <unistd.h>
#include <stdio.h>

#define h(v) printf(#v": %d\n", head->v)
void print_pkt_header(rtp_pkt_header_t *head)
{
    h(version);
    h(padding);
    h(extension);
    h(csrc_count);
    h(marker);
    h(payload_type);
    h(sequence_number);
    h(timestamp);
    h(ssrc);
}

#define p(v) printf(#v": %d\n", ctx->v)
#define pp(v) printf(#v": %ld\n", (uint8_t *)ctx->v - (uint8_t *)ctx->pkt.data)
void print_pkt_ctx(rtp_pkt_ctx_t *ctx)
{
    pp(csrc_ids_begin);
    p(csrc_count);
    pp(ext_header_begin);
    p(ext_header_id);
    p(ext_header_length);
    pp(payload_begin);
    p(payload_size);
    printf("pkt.data: %ld\n", ctx->pkt.data - ctx->pkt.data);
    printf("pkt.size: %d\n", ctx->pkt.size);
}

int main()
{
    rtp_pkt_ctx_create_info_t info = {
        .payload_size = 8,
        .padding = 1,
        .extension = 0,
        .csrc_count = 0,
        .ext_header_id = 100,
        .ext_header_length = 10
    };
    rtp_pkt_ctx_t *ctx = rtp_pkt_ctx_create(&info);

    memset(ctx->payload_begin, 0xAA, ctx->payload_size);

    print_pkt_ctx(ctx);
    printf("\n");
    print_pkt_header(&ctx->header);
    printf("\n");

    write(2, ctx->pkt.data, ctx->pkt.size);
    printf("\n");
    return 0;
}

#endif
