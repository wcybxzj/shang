<?php
define('DATA_NUM',100000);//10W

$obj_cluster = new RedisCluster(NULL, Array('127.0.0.1:6379', '127.0.0.1:6380', '127.0.0.1:6381'));



function store_data($obj_cluster)
{
	for ($j = 0; $j < DATA_NUM; $j++) {
		$key = "key".$j;
		$obj_cluster->set($key, $j);
	}
}


store_data($obj_cluster);

?>
