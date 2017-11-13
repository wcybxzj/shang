<?php
//测试集群中 主6379-->从6382
//           主6380-->从6383
//           主6381-->从6384
//此时将6379 6380 都下线，
//然后剩下的机器开启部分集群节点也工作的模式 也可以取出还没关机的节点数据
//配置项为cluster-require-full-coverage no

$obj_cluster = new RedisCluster(NULL, Array('127.0.0.1:6379', '127.0.0.1:6380', '127.0.0.1:6381'));
$keys=array("key81447","key8630");
$arr = $obj_cluster->mget($keys);
print_r($arr);
