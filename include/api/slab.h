#ifndef __API_SLAB_H_INCLUDED__
#define __API_SLAB_H_INCLUDED__

#include "api/lib.h"
#include "api/atomic.h"
#include "api/shmtx.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct shm_zone_s  api_shm_zone_t;

typedef api_int_t (*api_shm_zone_init_pt) (api_shm_zone_t *zone, void *data);

struct shm_zone_s {
    void                     *data;
    ngx_shm_t                 shm;
    api_shm_zone_init_pt      init;
    void                     *tag;
};

typedef struct api_slab_page_s  api_slab_page_t;

struct api_slab_page_s {
    uintptr_t         slab;
    api_slab_page_t  *next;
    uintptr_t         prev;
};


typedef struct {
    api_shmtx_sh_t    lock;

    size_t            min_size;
    size_t            min_shift;

    api_slab_page_t  *pages;
    api_slab_page_t  *last;
    api_slab_page_t   free;

    uint8_t           *start;
    uint8_t           *end;

    api_shmtx_t       mutex;

    uint8_t           *log_ctx;
    uint8_t            zero;

    unsigned          log_nomem:1;

    void             *data;
    void             *addr;
} api_slab_pool_t;


API api_slab_pool_t *api_slab_init(void *addr, size_t size);
API void *api_slab_alloc(api_slab_pool_t *pool, size_t size);
API void *api_slab_alloc_locked(api_slab_pool_t *pool, size_t size);
API void *api_slab_calloc(api_slab_pool_t *pool, size_t size);
API void *api_slab_calloc_locked(api_slab_pool_t *pool, size_t size);
API void api_slab_free(api_slab_pool_t *pool, void *p);
API void api_slab_free_locked(api_slab_pool_t *pool, void *p);

#ifdef __cplusplus
}
#endif

#endif /* ! __API_SLAB_H_INCLUDED__ */
