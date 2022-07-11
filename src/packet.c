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

rtp_pkt_t *rtp_pkt_alloc(rtp_pkt_alloc_info_t *info)
{
    rtp_pkt_t *res;
    uint_ csrc_h_size, ext_h_size, padding_size, pkt_size;
    rtp_alloc alloc = info->alctr.alloc;
    res = alloc(sizeof(*res));
    if (!res)
        return NULL;
    csrc_h_size = info->csrc_count * csrc_size;
    ext_h_size = info->ext ? ext_id_size + 
        ext_length_size + info->ext_header_length : 0;
    padding_size = info->padding ? 32 - (pkt_header_size +
            csrc_h_size + ext_h_size + info->payload_size) % 32 : 0;
    pkt_size = pkt_header_size + csrc_h_size + ext_h_size +
            info->payload_size + padding_size;
    if (!(res->data = alloc(pkt_size)))
        goto fail;
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
        ((uint8_t *)(res->data))[offset - 1] = padding_size - 1;
        res->padding_size = padding_size - 1;
    }
    res->alctr = info->alctr;
    return res;

fail:
    if (res->data)
        info->alctr.dealloc(res->data);
    info->alctr.dealloc(res);
    return NULL;
}

void rtp_pkt_destroy(rtp_pkt_t *pkt)
{
    rtp_dealloc dealloc = pkt->alctr.dealloc;
    dealloc(pkt->data);
    dealloc(pkt);
}

rtp_err_t rtp_pkt_data_init(rtp_pkt_t *pkt,
        rtp_pkt_data_init_info_t *info)
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

rtp_pkt_t *rtp_pkt_copy(const rtp_pkt_t *src)
{
    rtp_pkt_alloc_info_t ainfo = {
        .csrc_count = src->csrc_count,
        .ext = src->ext_begin ? 1 : 0,
        .ext_header_length = src->ext_size,
        .padding = src->padding_size,
        .payload_size = src->payload_size,
        .alctr = src->alctr
    };
    rtp_pkt_t *res = rtp_pkt_alloc(&ainfo);
    if (!res)
        return NULL;
    memcpy(res->data, src->data, src->size);
    return res;
}

rtp_pkt_t *rtp_pkt_parse(data_t data, rtp_allocator_t alctr)
{
    rtp_pkt_t *res;
    if (!(res = alctr.alloc(sizeof(rtp_pkt_t *))))
        return NULL;
    if (data.own)
        res->data = data.data;
    else {
        res->data = alctr.alloc(data.size);
        if (!res->data)
            goto fail;
        memcpy(res->data, data.data, data.size);
    }
    res->size = data.size;
    uint8_t *begin = res->data;

    rtp_pkt_header_t *header = res->data;
    res->header = header;

    uint_ offset = pkt_header_size;

    if ((res->csrc_count = header->csrc_count)) {
        res->csrc_begin = (uint32_t *)(begin + offset);
        offset += res->csrc_count * csrc_size;
    } else {
        res->csrc_begin = NULL;
    }

    if (header->extension) {
        offset += ext_id_size;
        res->ext_size = ntohs(*(uint16_t *)(begin + offset));
        offset += ext_length_size;
        res->ext_begin = begin + offset;
        offset += res->ext_size;
    } else {
        res->ext_begin = NULL;
        res->ext_size = 0;
    }

    uint_ padding_size = header->padding ?
        *(begin + res->size - 1) : 0;
    res->padding_size = padding_size;

    res->payload_begin = begin + offset;
    res->payload_size = res->size - offset - padding_size;

    return res;
fail:
    if (!data.own && res->data)
        alctr.dealloc(res->data);
    alctr.dealloc(res);
    return NULL;
}
