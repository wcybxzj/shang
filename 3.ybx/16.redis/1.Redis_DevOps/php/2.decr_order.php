<?php
//防止下过量的单
//step1:set key1 10
//step2:rm decr.log
//step3:ab -n100 -c100 http://localhost/redis_set/decr.php
$redis = new redis();
$redis->connect('127.0.0.1', 6379);

while (1) {
	//返回的是减少后的数量
	$re = $redis->decr('key1');
	if ($re<0) {
		echo "下单失败\n";
		$re = $redis->set('key1',0);//把-1 重置成 0
		exit(0);
	}else{
		echo "下单成功";
		echo "剩余数量".$re."\n";
	}
}

