//哈希表在Libevent的使用：
//现在来讲一下event_io_map的应用。
//在event_base这个结构体中有一个event_io_map类型的成员变量io。
//它就是一个哈希表。当一个监听读或者写操作的event，调用event_add函数插入到event_base中时，就会调用evmap_io_add函数。
//evmap_io_add函数应用到这个event_io_map结构体。该函数的定义如下，其中使用到了一个宏定义，我已经展开了。
//GET_IO_SLOT_AND_CTOR宏的作用就是让ctx指向struct event_map_entry结构体中的TAILQ_HEAD。
//这样就可以使用TAILQ_INSERT_TAIL宏，把ev变量插入到队列中。
//如果有现成的event_map_entry就直接使用，没有的话就新建一个。

int
evmap_io_add(struct event_base *base, evutil_socket_t fd, struct event *ev)
{
    const struct eventop *evsel = base->evsel;
    struct event_io_map *io = &base->io;
    struct evmap_io *ctx = NULL;
    int nread, nwrite, retval = 0;
    short res = 0, old = 0;
    struct event *old_ev;

    EVUTIL_ASSERT(fd == ev->ev_fd);

    if (fd < 0)
        return 0;

    //GET_IO_SLOT_AND_CTOR(ctx, io, fd, evmap_io, evmap_io_init,
    //					 evsel->fdinfo_len);SLOT指的是fd
    //GET_IO_SLOT_AND_CTOR宏将展开成下面这个do{}while(0);
    do
    {
        struct event_map_entry _key, *_ent;
        _key.fd = fd;

        struct event_io_map *_ptr_head = io;
        struct event_map_entry **ptr;

        //哈希表扩容，减少冲突的可能性
        if (!_ptr_head->hth_table
                || _ptr_head->hth_n_entries >= _ptr_head->hth_load_limit)
        {
            event_io_map_HT_GROW(_ptr_head,
                                 _ptr_head->hth_n_entries + 1);
        }

#ifdef HT_CACHE_HASH_VALUES
        do{
            (&_key)->map_node.hte_hash = hashsocket((&_key));
        } while(0);
#endif

        //返回值ptr,是要查找节点的前驱节点的hte_next成员变量的地址.
        //所以返回值肯定不会为NULL,而*ptr就可能为NULL。说明hte_next
        //不指向任何节点。也正由于这个原因，所以即使*ptr 为NULL,但是可以
        //给*ptr赋值。此时，是修改前驱节点的hte_next成员变量的值，使之
        //指向另外一个节点。
        //这里调用_event_io_map_HT_FIND_P原因有二：1.查看该fd是否已经
        //插入过这个哈希表中。2.得到这个fd计算哈希位置。
        ptr = _event_io_map_HT_FIND_P(_ptr_head, (&_key));

        //在event_io_map这个哈希表中查找是否已经存在该fd的event_map_entry了
        //因为同一个fd可以调用event_new多次，然后event_add多次的。
        if (*ptr)
        {
            _ent = *ptr;
        }
        else
        {
            _ent = mm_calloc(1, sizeof(struct event_map_entry) + evsel->fdinfo_len);
            if (EVUTIL_UNLIKELY(_ent == NULL))
                return (-1);

            _ent->fd = fd;
			  //调用初始化函数初始化这个evmap_io
            (evmap_io_init)(&_ent->ent.evmap_io);

#ifdef HT_CACHE_HASH_VALUES
            do
            {
                ent->map_node.hte_hash = (&_key)->map_node.hte_hash;
            }while(0);
#endif
            _ent->map_node.hte_next = NULL;

            //把这个新建的节点插入到哈希表中。ptr已经包含了哈希位置
            *ptr = _ent;
            ++(io->hth_n_entries);
        }


        //这里是获取该event_map_entry的next和prev指针。因为
        //evmap_io含有next、prev变量。这样在之后就可以把这个
        //event_map_entry连起来。这个外do{}while(0)的功能是
        //为这个fd分配一个event_map_entry,并且插入到现有的哈希
        //表中。同时，这个fd还是结构体event的一部分。而event必须
        //插入到event队列中。
        (ctx) = &_ent->ent.evmap_io;

    } while (0);


 	....

    //ctx->events是一个TAILQ_HEAD。结合之前讲到的TAILQ_QUEUE队列，
    //就可以知道：同一个fd，可能有多个event结构体。这é就把这些结构体连
    //起来。依靠的链表是，event结构体中的ev_io_next。ev_io_next是
    //一个TAILQ_ENTRY,具有前驱和后驱指针。队列头部为event_map_entry
    //结构体中的evmap_io成员的events成员。
    TAILQ_INSERT_TAIL(&ctx->events, ev, ev_io_next);

    return (retval);
}
