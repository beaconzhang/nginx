
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_PALLOC_H_INCLUDED_
#define _NGX_PALLOC_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>


/*
 * NGX_MAX_ALLOC_FROM_POOL should be (ngx_pagesize - 1), i.e. 4095 on x86.
 * On Windows NT it decreases a number of locked pages in a kernel.
 */
#define NGX_MAX_ALLOC_FROM_POOL  (ngx_pagesize - 1)

#define NGX_DEFAULT_POOL_SIZE    (16 * 1024)

#define NGX_POOL_ALIGNMENT       16
#define NGX_MIN_POOL_SIZE                                                     \
    ngx_align((sizeof(ngx_pool_t) + 2 * sizeof(ngx_pool_large_t)),            \
              NGX_POOL_ALIGNMENT)


typedef void (*ngx_pool_cleanup_pt)(void *data);

typedef struct ngx_pool_cleanup_s  ngx_pool_cleanup_t;

struct ngx_pool_cleanup_s {//本质上注册回调函数，释放内存时，按照顺序调用
    ngx_pool_cleanup_pt   handler;//回调函数，在释放ngx_pool调用
    void                 *data;//回调函数调用时传入的数据
    ngx_pool_cleanup_t   *next;
};


typedef struct ngx_pool_large_s  ngx_pool_large_t;

struct ngx_pool_large_s {//大内存管理,分配完，即全部返回，不会预留空间待下次再分配
    ngx_pool_large_t     *next;
    void                 *alloc;//分配的内存指针
};


typedef struct {
    u_char               *last;//指向下一个要分配的地址
    u_char               *end;//指向内存的终止位置
    ngx_pool_t           *next;//指向下一个ngx_pool_t
    ngx_uint_t            failed;//何时会failed，作用是用于更新current的依据,如果调用ngx_palloc_block，表示当pool->current以及以后的节点分配内存失败，失败一次+1，达到4，直接更新pool->current为后面第一个不为4的节点或者是最后一个
} ngx_pool_data_t;


struct ngx_pool_s {
    ngx_pool_data_t       d;//分配小的内存
    size_t                max;//判断large和small的依据
    ngx_pool_t           *current;//每次分配内存从当前的指向的块进行分配
    ngx_chain_t          *chain;
    ngx_pool_large_t     *large;//分配大内存
    ngx_pool_cleanup_t   *cleanup;
    ngx_log_t            *log;//用于输出日志
};


typedef struct {
    ngx_fd_t              fd;//文件描述符
    u_char               *name;//文件name
    ngx_log_t            *log;//日志
} ngx_pool_cleanup_file_t;


ngx_pool_t *ngx_create_pool(size_t size, ngx_log_t *log);
void ngx_destroy_pool(ngx_pool_t *pool);
void ngx_reset_pool(ngx_pool_t *pool);

void *ngx_palloc(ngx_pool_t *pool, size_t size);
void *ngx_pnalloc(ngx_pool_t *pool, size_t size);
void *ngx_pcalloc(ngx_pool_t *pool, size_t size);
void *ngx_pmemalign(ngx_pool_t *pool, size_t size, size_t alignment);
ngx_int_t ngx_pfree(ngx_pool_t *pool, void *p);


ngx_pool_cleanup_t *ngx_pool_cleanup_add(ngx_pool_t *p, size_t size);
void ngx_pool_run_cleanup_file(ngx_pool_t *p, ngx_fd_t fd);
void ngx_pool_cleanup_file(void *data);
void ngx_pool_delete_file(void *data);


#endif /* _NGX_PALLOC_H_INCLUDED_ */
