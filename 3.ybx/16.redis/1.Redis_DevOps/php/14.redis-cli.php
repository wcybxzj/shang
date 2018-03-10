<?php
$redis = new Redis();
$redis->connect('127.0.0.1', 6379);

$key="ybx".date("H:i:s");
for ($i = 0; $i < 90000000; $i++) {
	$str = date("H:i:s");
	$str = $str.'-'.$i;
	$k=$key;
	$k= $k.'-'.$i;


	echo $k;echo "\n";
	echo $str;echo "\n";

	$redis->set($k,$str);
	usleep(10);
}
