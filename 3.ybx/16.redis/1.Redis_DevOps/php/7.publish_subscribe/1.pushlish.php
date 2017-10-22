<?php
$redis = new redis();
$redis->connect('127.0.0.1', 6379);


for ($i = 0; $i < 10000; $i++) {
	$redis->publish('chan-1', 'c1:'.$i); // send message.
	$redis->publish('chan-3', 'c3:'.$i); // send message.
	sleep(1);
}


?>
