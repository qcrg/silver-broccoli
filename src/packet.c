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
        rtp_alloc alloc)
{
    RTP_INIT_ALLOC(alloc);
    rtp_pkt_t *res;
    uint_ csrc_h_size, ext_h_size, padding_size, pkt_size;
    res = pkt ? pkt : (rtp_pkt_t *)alloc(sizeof(*res));
    csrc_h_size = info->csrc_count * csrc_size;
    ext_h_size = info->ext ? ext_id_size + 
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
            res->ext_size = info->ext_header_length;
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
            memset((uint8_t*)(res->data) + offset, 0, padding_size);
            offset += padding_size;
            ((uint8_t *)(res->data))[offset - 1] = padding_size;
        }
    }

    return res;
}

void rtp_pkt_clear(rtp_pkt_t *pkt, rtp_dealloc dealloc)
{
    RTP_INIT_DEALLOC(dealloc);
    dealloc(pkt->data);
}

void rtp_pkt_destroy(rtp_pkt_t *pkt, rtp_dealloc dealloc)
{
    RTP_INIT_DEALLOC(dealloc);
    dealloc(pkt->data);
    dealloc(pkt);
}

rtp_err_t rtp_pkt_init(rtp_pkt_t *pkt, rtp_pkt_init_info_t *info)
{
    rtp_pkt_header_t head = {
        .version = 0x2,
        .padding = info->padding ? 0x1 : 0x0,
        .extension = info->ext ? 0x1 : 0x0,
        .csrc_count = info->csrc_count
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

rtp_pkt_t  *rtp_pkt_copy(const rtp_pkt_t *src, rtp_alloc alloc)
{
    rtp_pkt_t *res = alloc(sizeof(rtp_pkt_t));
    memcpy(res, src, sizeof(rtp_pkt_t));
    res->header = res->data = alloc(src->size);
    return res;
}



#ifdef USE_TESTS
#include <unistd.h>
#include <stdio.h>

int main()
{

    return 0;
}

#endif
