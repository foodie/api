#include <api/conhash.h>

static api_int_t api_conhash_shm_init(api_shm_zone_t *shm_zone, void *data);
static api_int_t api_conhash_add_replicas(api_conhash_t *conhash, api_conhash_node_t *hnode);
static api_int_t api_conhash_del_replicas(api_conhash_t *conhash, api_conhash_node_t *hnode, api_uint_t replicas);
static api_int_t api_conhash_make_vnode_name(api_conhash_t *conhash, api_str_t *name,
    api_conhash_node_t *hnode, api_uint_t index);
static void api_conhash_rbtree_insert_value(api_rbtree_node_t *temp, api_rbtree_node_t *node,
    api_rbtree_node_t *sentinel);
static api_rbtree_node_t* api_conhash_rbtree_lookup(api_conhash_t *conhash, api_str_t *name,
    api_rbtree_key_t key);
static void api_conhash_tree_mid_traverse(api_rbtree_node_t *node, api_rbtree_node_t *sentinel,
    api_conhash_oper_pt func, void *data);

void
api_conhash_clear(api_conhash_t *conhash)
{
    api_rbtree_node_t    *node, *sentinel;
    api_conhash_vnode_t  *vnode;
    api_conhash_node_t   *hnode;
    api_queue_t          *q;
    
    if (conhash == NULL || conhash == API_CONF_UNSET_PTR) {
        return;
    }
    
    api_shmtx_lock(&conhash->shpool->mutex);
    
    if (api_queue_empty(&conhash->sh->hnode_queue)) {
        api_shmtx_unlock(&conhash->shpool->mutex);
        return;
    }
    
    for (q = api_queue_head(&conhash->sh->hnode_queue);
         q != api_queue_sentinel(&conhash->sh->hnode_queue);
         q = api_queue_next(q))
    {
        hnode = api_queue_data(q, api_conhash_node_t, queue);
        api_slab_free_locked(conhash->shpool, hnode->name.data);
        api_slab_free_locked(conhash->shpool, hnode);
        api_queue_remove(q);
    }
    
    sentinel = conhash->sh->vnode_tree.sentinel;
    
    while (conhash->sh->vnode_tree.root != sentinel) {
        
        node = conhash->sh->vnode_tree.root;
        
        vnode = (api_conhash_vnode_t *) node;
        
        api_rbtree_delete(&conhash->sh->vnode_tree, node);
        
        api_slab_free_locked(conhash->shpool, vnode->name.data);
        api_slab_free_locked(conhash->shpool, vnode);
        
        conhash->sh->vnodes--;
    }
    
    api_shmtx_unlock(&conhash->shpool->mutex);
}

api_int_t
api_conhash_add_node(api_conhash_t *conhash, u_char *name, size_t len, void *data)
{
    api_int_t               rc;
    api_queue_t            *q;
    api_conhash_node_t     *hnode;
    size_t                  size;
    
    if (conhash == NULL
        || conhash == API_CONF_UNSET_PTR
        || conhash->shpool == NULL 
        || conhash->sh == NULL)
    {
        return API_ERROR;
    }
    
    api_shmtx_lock(&conhash->shpool->mutex);
    
    for (q = api_queue_head(&conhash->sh->hnode_queue);
         q != api_queue_sentinel(&conhash->sh->hnode_queue);
         q = api_queue_next(q))
    {
        hnode = api_queue_data(q, api_conhash_node_t, queue);
        if (hnode) {
            rc = api_memn2cmp(hnode->name.data, name, hnode->name.len, len);
            if (rc == 0) {
                rc = API_DECLINED;
                goto done;
            }
        }
    }
    
    hnode = api_slab_alloc_locked(conhash->shpool, sizeof(api_conhash_node_t));
    if (hnode == NULL) {
        rc = API_ERROR;
        goto done;
    }
    
    size = (len < API_CONHASH_NAME_SIZE) ? len : API_CONHASH_NAME_SIZE - 1;
    
    hnode->name.len = size;
    hnode->name.data = api_slab_alloc_locked(conhash->shpool, size + 1);
    if (hnode->name.data == NULL) {
        api_slab_free_locked(conhash->shpool, hnode);
        rc = API_ERROR;
        goto done;
    }
    
    api_memcpy(hnode->name.data, name, size);
    hnode->name.data[size] = '\0';
    hnode->data = data;
    
    rc = api_conhash_add_replicas(conhash, hnode);
    if (rc != API_OK) {
        api_slab_free_locked(conhash->shpool, hnode->name.data);
        api_slab_free_locked(conhash->shpool, hnode);
        goto done;
    }
    
    api_queue_insert_tail(&conhash->sh->hnode_queue, &hnode->queue);

done:
    api_shmtx_unlock(&conhash->shpool->mutex);
    
    return rc;
}

api_int_t
api_conhash_del_node(api_conhash_t *conhash, u_char *name, size_t len)
{
    api_int_t               rc, ret;
    api_queue_t            *q;
    api_conhash_node_t     *hnode;
    
    if (conhash == NULL
        || conhash == API_CONF_UNSET_PTR
        || conhash->shpool == NULL
        || conhash->sh == NULL)
    {
        return API_ERROR;
    }
    
    rc = API_DECLINED;
    
    api_shmtx_lock(&conhash->shpool->mutex);
    
    for (q = api_queue_head(&conhash->sh->hnode_queue);
         q != api_queue_sentinel(&conhash->sh->hnode_queue);
         q = api_queue_next(q))
    {
        hnode = api_queue_data(q, api_conhash_node_t, queue);
        
        ret = api_memn2cmp(hnode->name.data, name, hnode->name.len, len);

        if (ret == 0) {
        
            rc = api_conhash_del_replicas(conhash, hnode, conhash->vnodecnt);
            if (rc != API_OK) {
                goto done;
            }
            
            api_slab_free_locked(conhash->shpool, hnode->name.data);
            api_slab_free_locked(conhash->shpool, hnode);
            api_queue_remove(q);
            break;
        }
    }

done:
    api_shmtx_unlock(&conhash->shpool->mutex);

    return rc;
}

api_int_t
api_conhash_lookup_node(api_conhash_t *conhash, u_char *name, size_t len, 
    api_conhash_oper_pt func, void *data)
{
    api_rbtree_key_t      node_key;
    api_rbtree_node_t    *node, *sentinel;
    api_conhash_vnode_t  *vnode;
    api_int_t             rc;
    
    vnode = NULL;
    node_key = conhash->hash_func(name, len);
    
    api_shmtx_lock(&conhash->shpool->mutex);
    
    node = conhash->sh->vnode_tree.root;
    sentinel = conhash->sh->vnode_tree.sentinel;
    
    if (node == sentinel) {
        rc = API_DECLINED;
        goto done;
    }
    
    while (node != sentinel) {
        
        if (node_key <= node->key) {
            vnode = (api_conhash_vnode_t *) node;
            node = node->left;
            continue;
        }
        
        if (node_key > node->key) {
            node = node->right;
            continue;
        }
    }
    
    if (vnode == NULL) {
        node = api_rbtree_min(conhash->sh->vnode_tree.root, sentinel);
        vnode = (api_conhash_vnode_t *) node;
    }
    
    func(vnode, data);
    
    rc = API_OK;

done:
    api_shmtx_unlock(&conhash->shpool->mutex);
    
    return rc;
}

static api_rbtree_node_t*
api_conhash_rbtree_lookup(api_conhash_t *conhash, api_str_t *name, api_rbtree_key_t key)
{
    api_rbtree_node_t    *node, *sentinel;
    api_conhash_vnode_t  *vnode;
    api_int_t             rc;
    
    node = conhash->sh->vnode_tree.root;
    sentinel = conhash->sh->vnode_tree.sentinel;
        
    while (node != sentinel) {
    
        if (key < node->key) {
            node = node->left;
            continue;
        }
        
        if (key > node->key) {
            node = node->right;
            continue;
        }
        
        vnode = (api_conhash_vnode_t *) node;
        
        rc = api_memn2cmp(name->data, vnode->name.data, name->len, vnode->name.len);
        if (rc == 0) {
            return node;
        }
        
        node = (rc < 0) ? node->left : node->right;
    }
    
    return NULL;
}

static api_int_t 
api_conhash_add_replicas(api_conhash_t *conhash, api_conhash_node_t *hnode)
{
    api_uint_t               i;
    api_rbtree_key_t         key;
    api_str_t                vnode_name;
    api_conhash_vnode_t     *vnode;
    api_rbtree_node_t       *node;
    api_int_t                rc = API_OK;
    
    for (i = 0; i < conhash->vnodecnt; i++) {
        
        vnode_name.data = NULL;
        
        rc = api_conhash_make_vnode_name(conhash, &vnode_name, hnode, i);
        if (rc == API_ERROR) {
            goto done;
        }
        
        key = conhash->hash_func(vnode_name.data, vnode_name.len);

        node = api_conhash_rbtree_lookup(conhash, &vnode_name, key);
        if (node != NULL) {
            api_slab_free_locked(conhash->shpool, vnode_name.data);
            continue;
        }
        
        vnode = api_slab_alloc_locked(conhash->shpool,
                                      sizeof(api_conhash_vnode_t));
        if (vnode == NULL) {
            api_slab_free_locked(conhash->shpool, vnode_name.data);
            rc = API_ERROR;
            goto done;
        }
        
        vnode->node.key = key;
        vnode->hnode = hnode;
        vnode->name = vnode_name;

        api_rbtree_insert(&conhash->sh->vnode_tree, &vnode->node);
        conhash->sh->vnodes++;
    }

done:

    if (rc != API_SUCCESS && i < conhash->vnodecnt) {
        
        rc = api_conhash_del_replicas(conhash, hnode, i);
        if (rc != API_SUCCESS) {
            return rc;
        }
        
        return API_EAGAIN;
    }
    
    return API_SUCCESS;
}

static api_int_t 
api_conhash_del_replicas(api_conhash_t *conhash, api_conhash_node_t *hnode, api_uint_t replicas)
{
    api_uint_t           i;
    api_int_t            rc;
    api_rbtree_key_t     key;
    api_str_t            vnode_name;
    api_uint8_t          name[1024];
    api_conhash_vnode_t *vnode;
    api_rbtree_node_t   *node;
    
    for (i = 0; i < replicas; i++) {
        
        api_memzero(name, sizeof(name));
        
        vnode_name.data = name;
        
        rc = api_conhash_make_vnode_name(conhash, &vnode_name, hnode, i);
        if (rc == API_ERROR) {
            return rc;
        }
        
        key = conhash->hash_func(vnode_name.data, vnode_name.len);

        node = api_conhash_rbtree_lookup(conhash, &vnode_name, key);
        if (node == NULL) {
            continue;
        }
        
        vnode = (api_conhash_vnode_t *) node;
        
        api_rbtree_delete(&conhash->sh->vnode_tree, node);
        api_slab_free_locked(conhash->shpool, vnode->name.data);
        api_slab_free_locked(conhash->shpool, vnode);
        
        conhash->sh->vnodes--;
    }
    
    return API_SUCCESS;
}

static api_int_t 
api_conhash_make_vnode_name(api_conhash_t *conhash, api_str_t *name,
    api_conhash_node_t *hnode, api_uint_t index)
{
    uint8_t      *p;
    name->len = hnode->name.len + 5;
    
    if (name->data == NULL) {
        name->data = api_slab_alloc_locked(conhash->shpool, name->len + 1);
        if (name->data == NULL) {
            return API_ERROR;
        }
    }
    
    p = name->data;
    p = api_snprintf(name->len, 0, "%V-%04ui", &hnode->name, index);
    *p++ = '\0';
	
    return API_SUCCESS;
}

static void
api_conhash_rbtree_insert_value(api_rbtree_node_t *temp, api_rbtree_node_t *node,
    api_rbtree_node_t *sentinel)
{
    api_rbtree_node_t    **p;
    api_conhash_vnode_t   *vnode, *vnode_temp;

    for ( ;; ) {

        if (node->key < temp->key) {

            p = &temp->left;

        } else if (node->key > temp->key) {

            p = &temp->right;

        } else {

            vnode = (api_conhash_vnode_t *) node;
            vnode_temp = (api_conhash_vnode_t *) temp;

            p = (api_memn2cmp(vnode->name.data, vnode_temp->name.data, vnode->name.len, 
                              vnode_temp->name.len) < 0) ? &temp->left : &temp->right;
        }

        if (*p == sentinel) {
            break;
        }

        temp = *p;
    }

    *p = node;
    node->parent = temp;
    node->left = sentinel;
    node->right = sentinel;
    api_rbt_red(node);
}

char *
api_conhash_shm_set_slot(api_conf_t *cf, api_command_t *cmd, void *conf)
{
    char  *p = conf;
    
    ssize_t                 size;
    api_str_t               name, *value, s;
    api_conhash_t          *conhash, **conhash_p;
    api_conhash_ctx_t      *conhash_ctx;
    api_int_t               vnode_cnt;
    u_char                 *ptr;
    
    conhash_p = (api_conhash_t **) (p + cmd->offset);
    
    if (*conhash_p != API_CONF_UNSET_PTR) {
        return "is duplicate";
    }
    
    vnode_cnt = 100;
    value = cf->args->elts;
    
    if (api_strncmp(value[1].data, "keys_zone=", 10) != 0) {
        api_conf_log_error(API_LOG_EMERG, cf, 0,
                           "invalid parameter \"%V\"", &value[1]);
        return API_CONF_ERROR;
    }
    
    name.data = value[1].data + 10;
    
    ptr = (u_char *) api_strchr(name.data, ':');
    if (!ptr) {
        api_conf_log_error(API_LOG_EMERG, cf, 0,
                           "invalid parameter \"%V\"", &value[1]);
        return API_CONF_ERROR;
    }
    
    name.len = ptr - name.data;
    ptr++;
    
    s.len = value[1].data + value[1].len - ptr;
    s.data = ptr;

    size = api_parse_size(&s);

    if (size < (api_int_t) (2 * api_pagesize)) {
        api_conf_log_error(API_LOG_EMERG, cf, 0,
                            "invalid keys zone size \"%V\"", &value[1]);
        return API_CONF_ERROR;
    }
    
    if (cf->args->nelts > 2) {
        if (api_strncmp(value[2].data, "vnodecnt=", 9) != 0) {
            api_conf_log_error(API_LOG_EMERG, cf, 0,
                           "invalid parameter \"%V\"", &value[2]);
            return API_CONF_ERROR;
        }
        
        s.len = value[2].len - 9;
        s.data = value[2].data + 9;
        
        vnode_cnt = api_atoi(s.data, s.len);
        if (vnode_cnt == API_ERROR || vnode_cnt > 10000) {
            api_conf_log_error(API_LOG_EMERG, cf, 0,
                    "invalid vnode count \"%V\", is not greater than 10000", &value[2]);
            return API_CONF_ERROR;
        }
    }
    
    conhash = (api_conhash_t *) api_pcalloc(cf->pool, sizeof(api_conhash_t));
    if (conhash == NULL) {
        return API_CONF_ERROR;
    }
    
    conhash_ctx = (api_conhash_ctx_t *) cmd->post;
    if (conhash_ctx == NULL) {
        return API_CONF_ERROR;
    }
    
    if (conhash_ctx->hash_func != NULL) {
        conhash->hash_func = conhash_ctx->hash_func;
    } else {
        conhash->hash_func = api_murmur_hash2;
    }
    
    conhash->shm_zone = api_shared_memory_add(cf, &name, size, conhash_ctx->data);
    if (conhash->shm_zone == NULL) {
        return API_CONF_ERROR;
    }

    if (conhash->shm_zone->data) {
        api_conf_log_error(API_LOG_EMERG, cf, 0, "duplicate zone \"%V\"", &name);
        return API_CONF_ERROR;
    }
    
    conhash->shm_zone->init = api_conhash_shm_init;
    conhash->shm_zone->data = conhash;
    conhash->vnodecnt = vnode_cnt;
    
    *conhash_p = conhash;
    
    return API_CONF_OK;
}


static api_int_t
api_conhash_shm_init(api_shm_zone_t *shm_zone, void *data)
{
    api_conhash_t   *o_conhash = data;
    
    api_conhash_t   *conhash;
    size_t           len;

    conhash = shm_zone->data;
    
    if (o_conhash) {
        conhash->sh = o_conhash->sh;
        conhash->shpool = o_conhash->shpool;
        return API_OK;
    }
    
    conhash->shpool = (api_slab_pool_t *) shm_zone->shm.addr;
    
    conhash->sh = api_slab_alloc(conhash->shpool, sizeof(api_conhash_sh_t));
    if (conhash->sh == NULL) {
        return API_ERROR;
    }
    
    conhash->shpool->data = conhash->sh;

    api_rbtree_init(&conhash->sh->vnode_tree, &conhash->sh->vnode_sentinel,
                    api_conhash_rbtree_insert_value);
                    
    api_queue_init(&conhash->sh->hnode_queue);
    
    len = sizeof(" in conhash zone \"\"") + shm_zone->shm.name.len;

    conhash->shpool->log_ctx = api_slab_alloc(conhash->shpool, len);
    if (conhash->shpool->log_ctx == NULL) {
        return API_ERROR;
    }

    api_sprintf(conhash->shpool->log_ctx, " in conhash zone \"%V\"%Z",
                &shm_zone->shm.name);
    
    return API_OK;
}

api_int_t
api_conhash_node_traverse(api_conhash_t *conhash, api_conhash_oper_pt func, void *data)
{
    api_rbtree_node_t    *node, *sentinel;
    api_int_t             rc;

    rc = API_OK;
    
    api_shmtx_lock(&conhash->shpool->mutex);

    node = conhash->sh->vnode_tree.root;
    sentinel = conhash->sh->vnode_tree.sentinel;
    
    if (node == sentinel) {
        rc = API_DECLINED;
        goto done;
    }
    
    api_conhash_tree_mid_traverse(node, sentinel, func, data);

done:

    api_shmtx_unlock(&conhash->shpool->mutex);
    
    return rc;
}

static void
api_conhash_tree_mid_traverse(api_rbtree_node_t *node, api_rbtree_node_t *sentinel, 
    api_conhash_oper_pt func, void *data)
{
    api_conhash_vnode_t  *vnode;
    
    if (node->left != sentinel) {
        api_conhash_tree_mid_traverse(node->left, sentinel, func, data);
    }
    
    vnode = (api_conhash_vnode_t *) node;
    
    func(vnode, data);
    
    if (node->right != sentinel) {
        api_conhash_tree_mid_traverse(node->right, sentinel, func, data);
    }
}

