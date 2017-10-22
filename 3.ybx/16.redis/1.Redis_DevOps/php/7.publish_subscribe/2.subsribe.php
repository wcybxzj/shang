<?php
$redis = new redis();
$redis->connect('127.0.0.1', 6379);


function f($redis, $chan, $msg) {
	switch($chan) {
	case 'chan-1':
		echo $msg;
		break;
	case 'chan-2':
		echo $msg;
		break;
	case 'chan-3':
		echo $msg;
		break;
	}
}

$redis->subscribe(array('chan-1', 'chan-2', 'chan-3'), 'f'); // subscribe to 3 chans

while (1) {
	sleep(1);
}

?>
