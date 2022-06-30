#include "list.h"
#include <stdlib.h>

rtp_list_t *rtp_list_create(rtp_list_create_info_t *info)
{
    rtp_list_t *res = info->alloc(sizeof(rtp_list_t));
    res->begin = NULL;
    res->count = 0;
    res->alloc = info->alloc;
    res->dealloc = info->dealloc;
    return res;
}

rtp_node_list_t *rtp_node_list_create(void *p, rtp_node_list_t *next, 
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
    rtp_node_list_t *new = rtp_node_list_create(p, list->begin, list->alloc);
    if (new) {
        list->begin = new;
        list->count++;
        return rtp_err_ok;
    } else {
        return rtp_err_undefined;
    }
}

rtp_node_list_t *rtp_node_list_find_last(rtp_list_t *list)
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
    rtp_node_list_t *new = rtp_node_list_create(p, NULL, list->alloc);
    if (new) {
        if (list->count) {
            rtp_node_list_t *last = rtp_node_list_find_last(list);
            last->next = new;
        } else
            list->begin = new;
        list->count++;
        return rtp_err_ok;
    } else {
        return rtp_err_undefined;
    }
}

void *rtp_list_pull_front(rtp_list_t *list)
{
    if (list->count) {
        rtp_node_list_t *tmp = list->begin;
        list->begin = tmp->next;
        list->count--;
        void *data = tmp->data;
        list->dealloc(tmp);
        return data;
    } else {
        return NULL;
    }
}

void *rtp_list_pull_back(rtp_list_t *list)
{
    void *res = NULL;
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
        list->dealloc(last);
    } else if (list->count == 1) {
        res = list->begin->data;
        list->begin = NULL;
        list->count--;
    }
    return res;
}

void *rtp_list_get_front(rtp_list_t *list)
{
    return list->begin->data;
}

void *rtp_list_get_back(rtp_list_t *list)
{
    rtp_node_list_t *last = rtp_node_list_find_last(list);
    return last ? last->data : NULL;
}

void rtp_list_destroy_front(rtp_list_t *list)
{
    list->dealloc(rtp_list_pull_front(list));
}

void rtp_list_destroy_back(rtp_list_t *list)
{
    list->dealloc(rtp_list_pull_back(list));
}

void rtp_list_free(rtp_list_t *list)
{
    rtp_node_list_t *curr = list->begin, *next;
    while (curr) {
        next = curr->next;
        list->dealloc(curr);
        curr = next;
    }
    list->count = 0;
    list->begin = NULL;
}

void rtp_list_destroy(rtp_list_t *list)
{
    rtp_list_free(list);
    list->dealloc(list);
}

#ifdef USE_TESTS
#include <stdio.h>
#include <assert.h>

int main(int argc, char **argv)
{
    rtp_list_create_info_t info = {rtp_def_alloc, rtp_def_dealloc};
    rtp_list_t *list = rtp_list_create(&info);
    assert(list->count == 0 && list->begin == NULL && "list create");
    assert(rtp_err_ok == rtp_list_push_front(list, "hello, first"));
    assert(rtp_err_ok == rtp_list_push_front(list, "hello, second"));
    assert(rtp_err_ok == rtp_list_push_back(list, "hello, third"));
    while (list->count)
        printf("%s\n", rtp_list_pull_front(list));

    rtp_list_destroy(list);
    return 0;
}
#endif
