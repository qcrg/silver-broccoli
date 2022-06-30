#include "pay.h"

typedef struct data_t_
{
    void *data;
    uint_ data_size;
    void *begin;
    uint_ size;
} data_t;

typedef struct rtp_pay_ctx_t_
{
    list_t *data_list;
} rtp_pay_ctx_t;


