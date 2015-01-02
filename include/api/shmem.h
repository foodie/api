#ifndef _API_SHMEM_H_INCLUDED_
#define _API_SHMEM_H_INCLUDED_

#include <api/lib.h>

typedef struct shm_zone_s  api_shm_zone_t;

typedef api_int_t (*api_shm_zone_init_pt) (api_shm_zone_t *zone, void *data);

typedef struct {
    uint8_t     *addr;
    size_t       size;
    api_str_t    name;
    //api_log_t   *log;
    api_uint_t   exists;   /* unsigned  exists:1;  */
} api_shm_t;

struct shm_zone_s {
    void                     *data;
    api_shm_t                 shm;
    api_shm_zone_init_pt      init;
    void                     *tag;
};

api_int_t api_shm_alloc(api_shm_t *shm);
void api_shm_free(api_shm_t *shm);


#endif /* _API_SHMEM_H_INCLUDED_ */
