
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_LIST_H_INCLUDED_
#define _NGX_LIST_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>


typedef struct ngx_list_part_s  ngx_list_part_t;

struct ngx_list_part_s {
    void             *elts;//放链表数据的内存块
    ngx_uint_t        nelts;//当前elts中实际有的element个数
    ngx_list_part_t  *next;//下一个链表
};


typedef struct {
    ngx_list_part_t  *last;//指向最后一个ngx_list_part_t的地址
    ngx_list_part_t   part;
    size_t            size;//每个element占的内存
    ngx_uint_t        nalloc;//每个ngx_list_part_t中最多可以有多少个elment
    ngx_pool_t       *pool;//内存管理,链表节点和存储数据的空间都从这里分配
} ngx_list_t;


ngx_list_t *ngx_list_create(ngx_pool_t *pool, ngx_uint_t n, size_t size);

static ngx_inline ngx_int_t
ngx_list_init(ngx_list_t *list, ngx_pool_t *pool, ngx_uint_t n, size_t size)
{
    list->part.elts = ngx_palloc(pool, n * size);//每个元素占size空间，最多可以有n个元素
    if (list->part.elts == NULL) {
        return NGX_ERROR;
    }

    list->part.nelts = 0;//初始化为0个元素
    list->part.next = NULL;
    list->last = &list->part;//指向链表的最后一个节点
    list->size = size;//链表中每个元素占size个字节
    list->nalloc = n;//每个节点可以放n个元素
    list->pool = pool;//内存管理器

    return NGX_OK;
}


/*
 *
 *  the iteration through the list:
 *
 *  part = &list.part;
 *  data = part->elts;
 *
 *  for (i = 0 ;; i++) {
 *
 *      if (i >= part->nelts) {
 *          if (part->next == NULL) {
 *              break;
 *          }
 *
 *          part = part->next;
 *          data = part->elts;
 *          i = 0;
 *      }
 *
 *      ...  data[i] ...
 *
 *  }
 */


void *ngx_list_push(ngx_list_t *list);


#endif /* _NGX_LIST_H_INCLUDED_ */
