#include "list.h"
#include <stdlib.h>

typedef struct rtp_node_list_t_
{
    void *data;
    struct rtp_node_list_t_ *next;
} rtp_node_list_t;


rtp_list_t *rtp_list_create(rtp_list_create_info_t *info)
{
    rtp_list_t *res = info->alctr.alloc(sizeof(rtp_list_t));
    if (res) {
        res->begin = NULL;
        res->count = 0;
        res->alctr = info->alctr;
    }
    return res;
}

rtp_node_list_t *node_list_create(void *p, rtp_node_list_t *next, 
        rtp_alloc alloc)
{
    rtp_node_list_t *res = alloc(sizeof(rtp_node_list_t));
    if (res) { 
        res->data = p;
        res->next = next;
    }
    return res;
}

rtp_err_t rtp_list_push_front(rtp_list_t *list, void *p)
{
    rtp_node_list_t *new = node_list_create(p, list->begin,
            list->alctr.alloc);
    if (new) {
        list->begin = new;
        list->count++;
        return rtp_err_ok;
    } else {
        return rtp_err_memory;
    }
}

rtp_node_list_t *node_list_find_last(rtp_list_t *list)
{
    rtp_node_list_t *last = NULL, *curr = list->begin;
    while (curr) {
        last = curr;
        curr = curr->next;
    }
    return last;
}

rtp_err_t rtp_list_push_back(rtp_list_t *list, void *p)
{
    rtp_node_list_t *new = node_list_create(p, NULL,
            list->alctr.alloc);
    if (new) {
        if (list->count) {
            rtp_node_list_t *last = node_list_find_last(list);
            last->next = new;
        } else
            list->begin = new;
        list->count++;
        return rtp_err_ok;
    } else {
        return rtp_err_memory;
    }
}

void *rtp_list_pull_front(rtp_list_t *list)
{
    void *res;
    if (list->count) {
        rtp_node_list_t *tmp = list->begin;
        list->begin = tmp->next;
        list->count--;
        res = tmp->data;
        list->alctr.dealloc(tmp);
    } else {
        res = NULL;
    }
    return res;
}

void *rtp_list_pull_back(rtp_list_t *list)
{
    void *res;
    if (list->count > 1) {
        rtp_node_list_t *last = NULL;
        rtp_node_list_t *pred = list->begin;
        last = pred->next;
        while (last->next) {
            pred = last;
            last = last->next;
        }
        pred->next = NULL;
        list->count--;
        res = last->data;
        list->alctr.dealloc(last);
    } else if (list->count == 1) {
        res = list->begin->data;
        list->alctr.dealloc(list->begin);
        list->begin = NULL;
        list->count--;
    } else {
        res = NULL;
    }
    return res;
}

void *rtp_list_get_front(rtp_list_t *list)
{
    return list->begin ? list->begin->data : NULL;
}

void *rtp_list_get_back(rtp_list_t *list)
{
    rtp_node_list_t *last = node_list_find_last(list);
    return last ? last->data : NULL;
}

void rtp_list_destroy_front(rtp_list_t *list)
{
    void *front = rtp_list_pull_front(list);
    if (front)
        list->alctr.dealloc(front);
}

void rtp_list_destroy_back(rtp_list_t *list)
{
    void *back = rtp_list_pull_back(list);
    if (back) {
        list->alctr.dealloc(back);
    }
}

void rtp_list_clear(rtp_list_t *list)
{
    for (; list->count; rtp_list_destroy_front(list))
        ;
}

void rtp_list_destroy(rtp_list_t *list)
{
    rtp_list_clear(list);
    list->alctr.dealloc(list);
}
