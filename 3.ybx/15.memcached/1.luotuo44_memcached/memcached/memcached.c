static void settings_init(void) {  
    //开启CAS业务，如果开启了那么在item里面就会多一个用于CAS的字段。可以在启动memcached的时候通过-C选项禁用  
    settings.use_cas = true;  
      
    settings.access = 0700; //unix socket的权限位信息  
    settings.port = 11211;//memcached监听的tcp端口  
    settings.udpport = 11211;//memcached监听的udp端口  
    //memcached绑定的ip地址。如果该值为NULL，那么就是INADDR_ANY。否则该值指向一个ip字符串  
    settings.inter = NULL;  
      
    settings.maxbytes = 64 * 1024 * 1024; //memcached能够使用的最大内存  
    settings.maxconns = 1024; //最多允许多少个客户端同时在线。不同于settings.backlog  
     
    settings.verbose = 0;//运行信息的输出级别.该值越大输出的信息就越详细  
    settings.oldest_live = 0; //flush_all命令的时间界限。插入时间小于这个时间的item删除。  
    settings.evict_to_free = 1;  //标记memcached是否允许LRU淘汰机制。默认是可以的。可以通过-M选项禁止    
    settings.socketpath = NULL;//unix socket监听的socket路径.默认不使用unix socket   
    settings.factor = 1.25; //item的扩容因子  
    settings.chunk_size = 48; //最小的一个item能存储多少字节的数据(set、add命令中的数据)  
    settings.num_threads = 4; //worker线程的个数  
     //多少个worker线程为一个udp socket服务 number of worker threads serving each udp socket  
    settings.num_threads_per_udp = 0;  
      
    settings.prefix_delimiter = ':'; //分隔符  
    settings.detail_enabled = 0;//是否自动收集状态信息  
  
    //worker线程连续为某个客户端执行命令的最大命令数。这主要是为了防止一个客户端霸占整个worker线程  
    //，而该worker线程的其他客户端的命令无法得到处理  
    settings.reqs_per_event = 20;  
      
    settings.backlog = 1024;//listen函数的第二个参数，不同于settings.maxconns  
    //用户命令的协议，有文件和二进制两种。negotiating_prot是协商，自动根据命令内容判断  
    settings.binding_protocol = negotiating_prot;  
    settings.item_size_max = 1024 * 1024;//slab内存页的大小。单位是字节  
    settings.maxconns_fast = false;//如果连接数超过了最大同时在线数(由-c选项指定)，是否立即关闭新连接上的客户端。  
  
    //用于指明memcached是否启动了LRU爬虫线程。默认值为false，不启动LRU爬虫线程。  
    //可以在启动memcached时通过-o lru_crawler将变量的值赋值为true，启动LRU爬虫线程  
    settings.lru_crawler = false;  
    settings.lru_crawler_sleep = 100;//LRU爬虫线程工作时的休眠间隔。单位为微秒   
    settings.lru_crawler_tocrawl = 0; //LRU爬虫检查每条LRU队列中的多少个item,如果想让LRU爬虫工作必须修改这个值  
  
    //哈希表的长度是2^n。这个值就是n的初始值。可以在启动memcached的时候通过-o hashpower_init  
    //设置。设置的值要在[12, 64]之间。如果不设置，该值为0。哈希表的幂将取默认值16  
    settings.hashpower_init = 0;  /* Starting hash power level */  
  
    settings.slab_reassign = false;//是否开启调节不同类型item所占的内存数。可以通过 -o slab_reassign选项开启  
    settings.slab_automove = 0;//自动检测是否需要进行不同类型item的内存调整，依赖于settings.slab_reassign的开启  
  
    settings.shutdown_command = false;//是否支持客户端的关闭命令，该命令会关闭memcached进程  
  
    //用于修复item的引用数。如果一个worker线程引用了某个item，还没来得及解除引用这个线程就挂了  
    //那么这个item就永远被这个已死的线程所引用而不能释放。memcached用这个值来检测是否出现这种  
    //情况。因为这种情况很少发生，所以该变量的默认值为0(即不进行检测)。  
    //在启动memcached时，通过-o tail_repair_time xxx设置。设置的值要大于10(单位为秒)  
    //TAIL_REPAIR_TIME_DEFAULT 等于 0。  
    settings.tail_repair_time = TAIL_REPAIR_TIME_DEFAULT;   
    settings.flush_enabled = true;//是否运行客户端使用flush_all命令  
}   
