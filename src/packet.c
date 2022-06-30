#include "packet.h"
#include "allocator.h"
#include <string.h>
#include <arpa/inet.h>

typedef enum fields_size_
{
    pkt_header_size = sizeof(rtp_pkt_header_t),
    sequence_number_size = 2,
    timestamp_size = 4,
    ssrc_size = 4,
    csrc_size = 4,
    ext_id_size = 2,
    ext_length_size = 2
} fields_size;

rtp_pkt_t *rtp_pkt_alloc(rtp_pkt_t *pkt, rtp_pkt_alloc_info_t *info,
        rtp_alloc *alloc)
{
    RTP_INIT_ALLOC(alloc);
    rtp_pkt_t *res;
    uint_ csrc_h_size, ext_h_size, padding_size, pkt_size;
    res = pkt ? pkt : alloc(sizeof(*res));
    csrc_h_size = info->csrc_count * csrc_size;
    ext_h_size = info->extension ? ext_id_size + 
        ext_length_size + info->ext_header_length : 0;
    padding_size = info->padding ? 32 - (pkt_header_size +
            csrc_h_size + ext_h_size + info->payload_size) % 32 : 0;
    pkt_size = pkt_header_size + csrc_h_size + ext_h_size +
            info->payload_size + padding_size;
    res->data = alloc(pkt_size);
    if (res->data) {
        res->size = pkt_size;
        res->header = (rtp_pkt_header_t *)res->data;

        uint_ offset = pkt_header_size;
        if (info->csrc_count) {
            res->csrc_begin = (uint32_t *)((uint8_t *)res->data + offset);
            res->csrc_count = info->csrc_count;
            offset += res->csrc_count * csrc_size;
        } else {
            res->csrc_begin = NULL;
            res->csrc_count = 0;
        }

        if (info->ext) {
            offset += ext_id_size + ext_length_size;
            res->ext_begin = (uint8_t *)res->data + offset;
            res->ext_size = info_ext_header_length;
            offset += res->ext_size;
        } else {
            res->ext_begin = NULL;
            res->ext_size = 0;
        }

        if (info->payload_size) {
            res->payload_begin = (uint8_t *)res->data + offset;
            res->payload_size = info->payload_size;
            offset += res->payload_size;
        } else {
            res->payload_begin = NULL;
            res->payload_size = 0;
        }

        if (info->padding) {
            memset(res->data + offset, 0, padding_size);
            offset += padding_size;
            res->data[offset - 1] = padding_size;
        }
    }

    return res;
}

void rtp_pkt_free(rtp_pkt_t *pkt, rtp_dealloc *dealloc)
{
    RTP_INIT_DEALLOC(dealloc);
    dealloc(pkt->data);
}

void rtp_pkt_destroy(rtp_pkt_t *pkt, rtp_dealloc *dealloc)
{
    RTP_INIT_DEALLOC(dealloc);
    dealloc(pkt->data);
    dealloc(pkt);
}

rtp_err_t rtp_pkt_init(rtp_pkt_t *pkt, rtp_pkt_init_info_t *info)
{
    rtp_pkt_header_t head = {
        .version = 0x2,
        .padding = info->padding ? 0x1 : 0x0;
        .extension = info->ext ? 0x1 : 0x0;
        .csrc_count = info->csrc_count;
    };
    memcpy(pkt->header, &head, sizeof(head));
    if (info->ext) {
        uint16_t *begin = (uint16_t *)((uint8_t *)pkt->ext_begin -
                ext_id_size - ext_length_size);
        begin[0] = htons(info->ext_id);
        begin[1] = htons(info->ext_header_length);
    }
    return rtp_err_ok;
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
