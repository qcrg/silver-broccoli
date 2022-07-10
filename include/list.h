#ifndef _SILVER_BROCCOLI_LIST_H
#define _SILVER_BROCCOLI_LIST_H

#include "common.h"
#include "allocator.h"

typedef struct rtp_node_list_t_ rtp_node_list_t;

typedef struct rtp_list_t_
{
    rtp_node_list_t *begin;
    uint_ count;
    rtp_allocator_t alctr;
} rtp_list_t;

typedef struct rtp_list_create_info_t_
{
    rtp_allocator_t alctr;
} rtp_list_create_info_t;

#ifdef  __cplusplus
extern "C" {
#endif//__cplusplus

rtp_list_t *rtp_list_create(rtp_list_create_info_t *info);
void rtp_list_clear(rtp_list_t *list);
void rtp_list_destroy(rtp_list_t *list);

rtp_err_t rtp_list_push_front(rtp_list_t *list, void *p);
rtp_err_t rtp_list_push_back(rtp_list_t *list, void *p);

void *rtp_list_pull_front(rtp_list_t *list);
void *rtp_list_pull_back(rtp_list_t *list);

void *rtp_list_get_front(rtp_list_t *list);
void *rtp_list_get_back(rtp_list_t *list);

void rtp_list_destroy_front(rtp_list_t *list);
void rtp_list_destroy_back(rtp_list_t *list);

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif//_SILVER_BROCCOLI_LIST_H
