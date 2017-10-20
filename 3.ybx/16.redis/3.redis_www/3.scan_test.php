<?php
//1.创建模拟数据5000个wc 10个ybx

//2.redis-cli中scan MATCH *ybx* 可能会返回空结果集合但是实际后边还有*ybx*的内容没有返回
/*
127.0.0.1:6379> scan 0 count 30 MATCH *ybx*
1) "8176"
2) (empty list or set)
127.0.0.1:6379> scan 8176 count 30 MATCH *ybx*
1) "8184"
2) (empty list or set)
127.0.0.1:6379> scan 8184 count 30 MATCH *ybx*
1) "3060"
2) 1) "ybx3"
*/

//3.php中可以通过参数避免在本来后边有*ybx*但是当前结果却返回空的情况
/* Options for the SCAN family of commands, indicating whether to abstract
   empty results from the user.  If set to SCAN_NORETRY (the default), phpredis
   will just issue one SCAN command at a time, sometimes returning an empty
   array of results.  If set to SCAN_RETRY, phpredis will retry the scan command
   until keys come back OR Redis returns an iterator of zero
*/
//$redis->setOption(Redis::OPT_SCAN, Redis::SCAN_NORETRY);
//$redis->setOption(Redis::OPT_SCAN, Redis::SCAN_RETRY);

$redis = new Redis();
$redis->connect('127.0.0.1', 6379);

function build_data($redis)
{
	$redis->flushAll();

	for ($i = 0; $i < 500; $i++) {
		$key= "wc".$i;
		$result = $redis->set($key, 1);
	}

	for ($i = 0; $i < 10; $i++) {
		$key= "ybx".$i;
		$result = $redis->set($key, 1);
	}
}

//强制在后边有可能匹配的情况不返回空结果
function scan_retry($redis)
{
	$it = NULL;
	$redis->setOption(Redis::OPT_SCAN, Redis::SCAN_RETRY); /* don't return empty results until we're done */
	while($arr_mems = $redis->scan( $it, '*ybx*')) {
		foreach($arr_mems as $str_mem) {
			echo "Member: $str_mem\n";
		}
	}
}

//在后边有可能匹配的情况返回空结果
function scan_noretry($redis)
{
	$it = NULL;
	$redis->setOption(Redis::OPT_SCAN, Redis::SCAN_NORETRY); /* return after each iteration, even if empty */
	while(($arr_mems = $redis->scan( $it, '*ybx*'))!==FALSE) {
		if(count($arr_mems) > 0) {
			foreach($arr_mems as $str_mem) {
				echo "Member found: $str_mem\n";
			}
		} else {
			echo "No members in this iteration, iterator value: $it\n";
		}
	}
}

build_data($redis);
scan_retry($redis);
echo "\n=====================================\n";
scan_noretry($redis);


?>
