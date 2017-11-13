<?php
define('DATA_NUM',100000);//10W

$obj_cluster = new RedisCluster(NULL, Array('127.0.0.1:6379', '127.0.0.1:6380', '127.0.0.1:6381'));

//随机取得5000个key(key0-key99999)
function store_data($obj_cluster)
{
	for ($j = 0; $j < DATA_NUM/2; $j++) {
		$key = "key".(rand(0,99999));
		echo $obj_cluster->get($key) ;
	}
}

store_data($obj_cluster);
