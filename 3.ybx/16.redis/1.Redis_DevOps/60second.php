<?php
//连接本地的 Redis 服务
$redis = new Redis();
$redis->connect('127.0.0.1', 6379);

//如果key不存在set就放回TRUE
$result = $redis->set('phone_num', 1, Array('nx', 'ex'=>60));
//var_dump($result);
//echo "===================";

if ($result == FALSE) {
	$redis->incr('phone_num');
}
$num = $redis->get('phone_num');

if ($result or $num<=5) {
	echo "短信已经发送到您的手机";
	echo $num;
}else{
	echo "60秒内只能运行 5次";
}
?>
