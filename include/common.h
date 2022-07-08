#ifndef _SILVER_BROCCOLI_COMMON_H
#define _SILVER_BROCCOLI_COMMON_H

typedef enum rtp_err_t_
{
    rtp_err_ok,
    rtp_err_undefined
} rtp_err_t;

typedef unsigned int uint_;

typedef struct data_t_
{
    void *data;
    uint_ size;
    uint_ own;
} data_t;

#endif//_SILVER_BROCCOLI_COMMON_H
