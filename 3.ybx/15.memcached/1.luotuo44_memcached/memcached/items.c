//获取1个item存了数据后的实际总长度
static size_t item_make_header(const uint8_t nkey, const int flags, const int nbytes,
                     char *suffix, uint8_t *nsuffix) {
    /* suffix is defined at 40 chars elsewhere.. */
    *nsuffix = (uint8_t) snprintf(suffix, 40, " %d %d\r\n", flags, nbytes - 2);
    return sizeof(item) + nkey + *nsuffix + nbytes;
}

//TODO
//key、flags、exptime三个参数是用户在使用set、add命令存储一条数据时输入的参数。  
//nkey是key字符串的长度。nbytes则是用户要存储的data长度+2,因为在data的结尾处还要加上"\r\n"  
//cur_hv则是根据键值key计算得到的哈希值。
item *do_item_alloc(char *key, const size_t nkey, const int flags,
                    const rel_time_t exptime, const int nbytes,
                    const uint32_t cur_hv) {
    uint8_t nsuffix;
    item *it = NULL;
    char suffix[40];
    size_t ntotal = item_make_header(nkey + 1, flags, nbytes, suffix, &nsuffix);
    if (settings.use_cas) {
        ntotal += sizeof(uint64_t);
    }

    unsigned int id = slabs_clsid(ntotal);
    if (id == 0)
        return 0;

    mutex_lock(&cache_lock);
    /* do a quick check if we have any expired items in the tail.. */
    int tries = 5;
    /* Avoid hangs if a slab has nothing but refcounted stuff in it. */
    int tries_lrutail_reflocked = 1000;
    int tried_alloc = 0;
    item *search;
    item *next_it;
    void *hold_lock = NULL;
    rel_time_t oldest_live = settings.oldest_live;

    search = tails[id];
    /* We walk up *only* for locked items. Never searching for expired.
     * Waste of CPU for almost all deployments */
    for (; tries > 0 && search != NULL; tries--, search=next_it) {
        /* we might relink search mid-loop, so search->prev isn't reliable */
        next_it = search->prev;
        if (search->nbytes == 0 && search->nkey == 0 && search->it_flags == 1) {
            /* We are a crawler, ignore it. */
            tries++;
            continue;
        }
        uint32_t hv = hash(ITEM_key(search), search->nkey);
        /* Attempt to hash item lock the "search" item. If locked, no
         * other callers can incr the refcount
         */
        /* Don't accidentally grab ourselves, or bail if we can't quicklock */
        if (hv == cur_hv || (hold_lock = item_trylock(hv)) == NULL)
            continue;
        /* Now see if the item is refcount locked */
        if (refcount_incr(&search->refcount) != 2) {
            /* Avoid pathological case with ref'ed items in tail */
            do_item_update_nolock(search);
            tries_lrutail_reflocked--;
            tries++;
            refcount_decr(&search->refcount);
            itemstats[id].lrutail_reflocked++;
            /* Old rare bug could cause a refcount leak. We haven't seen
             * it in years, but we leave this code in to prevent failures
             * just in case */
            if (settings.tail_repair_time &&
                    search->time + settings.tail_repair_time < current_time) {
                itemstats[id].tailrepairs++;
                search->refcount = 1;
                do_item_unlink_nolock(search, hv);
            }
            if (hold_lock)
                item_trylock_unlock(hold_lock);

            if (tries_lrutail_reflocked < 1)
                break;

            continue;
        }

        /* Expired or flushed */
        if ((search->exptime != 0 && search->exptime < current_time)
            || (search->time <= oldest_live && oldest_live <= current_time)) {
            itemstats[id].reclaimed++;
            if ((search->it_flags & ITEM_FETCHED) == 0) {
                itemstats[id].expired_unfetched++;
            }
            it = search;
            slabs_adjust_mem_requested(it->slabs_clsid, ITEM_ntotal(it), ntotal);
            do_item_unlink_nolock(it, hv);
            /* Initialize the item block: */
            it->slabs_clsid = 0;
        } else if ((it = slabs_alloc(ntotal, id)) == NULL) {
            tried_alloc = 1;
            if (settings.evict_to_free == 0) {
                itemstats[id].outofmemory++;
            } else {
                itemstats[id].evicted++;
                itemstats[id].evicted_time = current_time - search->time;
                if (search->exptime != 0)
                    itemstats[id].evicted_nonzero++;
                if ((search->it_flags & ITEM_FETCHED) == 0) {
                    itemstats[id].evicted_unfetched++;
                }
                it = search;
                slabs_adjust_mem_requested(it->slabs_clsid, ITEM_ntotal(it), ntotal);
                do_item_unlink_nolock(it, hv);
                /* Initialize the item block: */
                it->slabs_clsid = 0;

                /* If we've just evicted an item, and the automover is set to
                 * angry bird mode, attempt to rip memory into this slab class.
                 * TODO: Move valid object detection into a function, and on a
                 * "successful" memory pull, look behind and see if the next alloc
                 * would be an eviction. Then kick off the slab mover before the
                 * eviction happens.
                 */
                if (settings.slab_automove == 2)
                    slabs_reassign(-1, id);
            }
        }

        refcount_decr(&search->refcount);
        /* If hash values were equal, we don't grab a second lock */
        if (hold_lock)
            item_trylock_unlock(hold_lock);
        break;
    }

    if (!tried_alloc && (tries == 0 || search == NULL))
        it = slabs_alloc(ntotal, id);

    if (it == NULL) {
        itemstats[id].outofmemory++;
        mutex_unlock(&cache_lock);
        return NULL;
    }

    assert(it->slabs_clsid == 0);
    assert(it != heads[id]);

    /* Item initialization can happen outside of the lock; the item's already
     * been removed from the slab LRU.
     */
    it->refcount = 1;     /* the caller will have a reference */
    mutex_unlock(&cache_lock);
    it->next = it->prev = it->h_next = 0;
    it->slabs_clsid = id;

    DEBUG_REFCNT(it, '*');
    it->it_flags = settings.use_cas ? ITEM_CAS : 0;
    it->nkey = nkey;
    it->nbytes = nbytes;
    memcpy(ITEM_key(it), key, nkey);
    it->exptime = exptime;
    memcpy(ITEM_suffix(it), suffix, (size_t)nsuffix);
    it->nsuffix = nsuffix;
    return it;
}



//对于memcached来说几乎所有的操作时间复杂度都是常数级的。
//插入操作：
//假设我们有一个item要插入到LRU链表中，那么可以通过调用item_link_q函数把item插入到LRU队列中。
//将item插入到LRU队列的头部  
static void item_link_q(item *it) { /* item is the new head */  
    item **head, **tail;  
    assert(it->slabs_clsid < LARGEST_ID);  
	//确保这个item已经从slab分配出去
    assert((it->it_flags & ITEM_SLABBED) == 0);  
  
    head = &heads[it->slabs_clsid];  
    tail = &tails[it->slabs_clsid];  
    assert(it != *head);  
    assert((*head && *tail) || (*head == 0 && *tail == 0));  
  
    //头插法插入该item  
    it->prev = 0;  
    it->next = *head;  
    if (it->next) it->next->prev = it;  
    *head = it;//该item作为对应链表的第一个节点  
  
    //如果该item是对应id上的第一个item，那么还会被认为是该id链上的最后一个item  
    //因为在head那里使用头插法，所以第一个插入的item，到了后面确实成了最后一个item  
    if (*tail == 0) *tail = it;  
    sizes[it->slabs_clsid]++;//个数加一  
    return;  
}//end  item_link_q()  

//删除操作：
//有了插入函数，肯定有对应的删除函数。
//删除函数是蛮简单，主要是处理删除这个节点后，该节点的前后驱节点怎么拼接在一起。
//将it从对应的LRU队列中删除  
static void item_unlink_q(item *it) {  
    item **head, **tail;  
    assert(it->slabs_clsid < LARGEST_ID);  
    head = &heads[it->slabs_clsid];  
    tail = &tails[it->slabs_clsid];  
  
      
    if (*head == it) {//链表上的第一个节点  
        assert(it->prev == 0);  
        *head = it->next;  
    }  
    if (*tail == it) {//链表上的最后一个节点  
        assert(it->next == 0);  
        *tail = it->prev;  
    }  
    assert(it->next != it);  
    assert(it->prev != it);  
  
    //把item的前驱节点和后驱节点连接起来  
    if (it->next) it->next->prev = it->prev;  
    if (it->prev) it->prev->next = it->next;  
    sizes[it->slabs_clsid]--;//个数减一  
    return;  
}//end  item_unlink_q  

//更新操作：
//为什么要把item插入到LRU队列头部呢？
//当然实现简单是其中一个原因。
//但更重要的是这是一个LRU队列！！
//还记得操作系统里面的LRU吧。这是一种淘汰机制。
//在LRU队列中，排得越靠后就认为是越少使用的item，此时被淘汰的几率就越大。
//所以新鲜item(访问时间新)，要排在不那么新鲜item的前面，所以插入LRU队列的头部是不二选择。
//下面的do_item_update函数佐证了这一点。
//do_item_update函数是先把旧的item从LRU队列中删除，然后再插入到LRU队列中(此时它在LRU队列中排得最前)。
//除了更新item在队列中的位置外，还会更新item的time成员，该成员指明上一次访问的时间(绝对时间)。
//如果不是为了LRU，那么do_item_update函数最简单的实现就是直接更新time成员即可。
void do_item_update(item *it) {  
    //下面的代码可以看到update操作是耗时的。如果这个item频繁被访问，  
    //那么会导致过多的update，过多的一系列费时操作。此时更新间隔就应运而生  
    //了。如果上一次的访问时间(也可以说是update时间)距离现在(current_time)  
    //还在更新间隔内的，就不更新。超出了才更新。  
    if (it->time < current_time - ITEM_UPDATE_INTERVAL) {  
  
        mutex_lock(&cache_lock);  
        if ((it->it_flags & ITEM_LINKED) != 0) {  
            item_unlink_q(it);//从LRU队列中删除  
            it->time = current_time;//更新访问时间  
            item_link_q(it);//插入到LRU队列的头部  
        }  
        mutex_unlock(&cache_lock);  
    }  
}//end do_item_update()

/** wrapper around assoc_find which does the lazy expiration logic */
item *do_item_get(const char *key, const size_t nkey, const uint32_t hv) {
    //mutex_lock(&cache_lock);
    item *it = assoc_find(key, nkey, hv);
    if (it != NULL) {
        refcount_incr(&it->refcount);
        /* Optimization for slab reassignment. prevents popular items from
         * jamming in busy wait. Can only do this here to satisfy lock order
         * of item_lock, cache_lock, slabs_lock. */
        if (slab_rebalance_signal &&
            ((void *)it >= slab_rebal.slab_start && (void *)it < slab_rebal.slab_end)) {
            do_item_unlink_nolock(it, hv);
            do_item_remove(it);
            it = NULL;
        }
    }
    //mutex_unlock(&cache_lock);
    int was_found = 0;

    if (settings.verbose > 2) {
        int ii;
        if (it == NULL) {
            fprintf(stderr, "> NOT FOUND ");
        } else {
            fprintf(stderr, "> FOUND KEY ");
            was_found++;
        }
        for (ii = 0; ii < nkey; ++ii) {
            fprintf(stderr, "%c", key[ii]);
        }
    }

    if (it != NULL) {
        if (settings.oldest_live != 0 && settings.oldest_live <= current_time &&
            it->time <= settings.oldest_live) {
            do_item_unlink(it, hv);
            do_item_remove(it);
            it = NULL;
            if (was_found) {
                fprintf(stderr, " -nuked by flush");
            }
        } else if (it->exptime != 0 && it->exptime <= current_time) {
            do_item_unlink(it, hv);
            do_item_remove(it);
            it = NULL;
            if (was_found) {
                fprintf(stderr, " -nuked by expire");
            }
        } else {
            it->it_flags |= ITEM_FETCHED;
            DEBUG_REFCNT(it, '+');
        }
    }

    if (settings.verbose > 2)
        fprintf(stderr, "\n");

    return it;
}//end do_item_get()

