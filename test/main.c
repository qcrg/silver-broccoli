#include "depay.h"
#include "pay.h"
#include <string.h>
#include <assert.h>

uint8_t test_frame[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

enum
{
    payload_size = 10,
    frame_size = sizeof(test_frame)
};


int main(int argc, char **argv)
{
    argc = argc; argv = argv;
    rtp_pkt_alloc_info_t pinfo;
    memset(&pinfo, 0, sizeof(pinfo));
    pinfo.payload_size = payload_size;
    pinfo.alctr = rtp_def_allocator;
    rtp_pkt_t *pkt = rtp_pkt_alloc(&pinfo);
    assert(pkt && "packet alloc error");
    rtp_pkt_data_init_info_t dinfo;
    memset(&dinfo, 0, sizeof(dinfo));
    assert(rtp_err_ok == rtp_pkt_data_init(pkt, &dinfo));
    rtp_pay_ctx_create_info_t pcinfo = {
        .pkt_sample = pkt,
        .take_sample_own = 1,
        .alctr = rtp_def_allocator
    };
    rtp_pay_ctx_t *pay_ctx = rtp_pay_ctx_create(&pcinfo);
    assert(pay_ctx && "pay context create error");
    rtp_depay_ctx_create_info_t dcinfo = {
        .frame_size = frame_size,
        .alctr = rtp_def_allocator
    };
    rtp_depay_ctx_t *depay_ctx = rtp_depay_ctx_create(&dcinfo);
    assert(depay_ctx && "depay context create error");

    uint_ pay_count = 0, depay_count = 0;
    data_t tmp_data = { .data = test_frame, .size = frame_size, 0};
    assert(rtp_err_ok == rtp_pay_push(pay_ctx, tmp_data));

    for (uint_ i = 0; i < 20; i++) {
        rtp_pkt_t *pkt = rtp_pay_pull(pay_ctx);
        if (pkt) {
            pay_count++;
        }
    }
    uint_ theoretical_pay_count = frame_size / payload_size +
        (frame_size % payload_size ? 1 : 0);
    assert(pay_count == theoretical_pay_count);

    return 0;
}
