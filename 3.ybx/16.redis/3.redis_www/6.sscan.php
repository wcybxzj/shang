<?php
//当前集合有两种类型的元素，例如分别以old：user和new：user开头，先需要将old：user开头的元素全部删除
/*
String key = "myset";
String pattern = "old:user*";
// 游标
String cursor = "0";
while (true) {
	// 获取扫描结果
	ScanResult scanResult = redis.sscan(key, cursor, pattern);
	List elements = scanResult.getResult();
	if (elements != null && elements.size() > 0) {
		// 批量删除
		redis.srem(key, elements);
	}
	// 获取新的游标
	cursor = scanResult.getStringCursor();
	// 如果游标为
	//0表示遍历结束
	if ("0".equals(cursor)) {
		break;
	}
}
*/

$redis = new Redis();
$redis->connect('127.0.0.1', 6379);
$redis->flushDb();

//创建测试数据
$key ="myset";
for ($i = 0; $i < 50; $i++) {
	$v= "new:user".$i;
	$result = $redis->sAdd($key, $v);
	$v= "old:user".$i;
	$result = $redis->sAdd($key, $v);
}

$pattern = "old:user*";
$it = NULL;
$redis->setOption(Redis::OPT_SCAN, Redis::SCAN_RETRY);//保证结果不为空，在没到数据结尾的情况下
while(($arr_mems = $redis->sScan($key, $it, $pattern))!==FALSE) {
	if(count($arr_mems) > 0) {
		foreach($arr_mems as $str_mem) {
			echo "Member found: $str_mem\n";
			//$redis->sRem($key, $str_mem);
		}
		echo "\n============================\n";
	} else {
		echo "No members in this iteration, iterator value: $it\n";
	}
}



?>
