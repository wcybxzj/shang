<?php
//8.3.5 最后的例子
//本例子是set使用intset和hashtable保存数字内容的比较

/*
php 10.intset_hashtable_ziplist.php 
set-max_intset_entries:1
1509357346.3734
1509357352.2602
耗时微秒:5.886864900589
数据类型:hashtable
used:3.8507080078125MB
=================
set-max_intset_entries:200000
1509357352.2715
1509357358.5625
耗时微秒:6.2910580635071
数据类型:intset
used:0.20391845703125MB
 */

define('DATA_NUM',100);

$redis1 = new Redis();
$redis1->connect('127.0.0.1', 6379);


//1.100个集合,每个集合里元素1000个 类型hashtable
//集合key 20字节 集合value 7字节
//set-max-intset-entries为1,set元素超过1个就用hashtable做内部编码
test($redis1, 1);

echo "\n=================\n";

//2.100个集合,每个集合里元素1000个 类型intset
//集合key 20字节 集合value 7字节
//set-max-intset-entries为2000000,set元素没超过20w个就用intset做内部编码
test($redis1, 200000);

function store_data($redis)
{
	$arr = array();
	for ($i = 0; $i < DATA_NUM; $i++) {
		for ($j = 0; $j < 1000; $j++) {
			$key = sprintf("%020s",$i);
			$redis->sAdd($key, $j);
		}
	}
}

function test($redis1, $num)
{
	//初始化DB
	$redis1->flushDb();
	//配置
	$redis1->config("SET", "set-max-intset-entries", $num);
	$arr = $redis1->config("GET", "set-max-intset-entries");
	echo "set-max_intset_entries:".$arr['set-max-intset-entries']."\n";
	$memory1= $redis1->info("memory");
	echo $s1 = microtime(1);
	echo "\n";
	store_data($redis1);
	echo $s2 = microtime(1);
	echo "\n";
	echo "耗时微秒:";
	echo $s2-$s1;
	echo "\n";
	//查看类型
	$type = $redis1->object("encoding", "00000000000000000001");
	echo "数据类型:". $type."\n";
	//内存情况
	$memory2= $redis1->info("memory");
	$bytes = $memory2['used_memory'] - $memory1['used_memory'];//字节
	echo "used:". ($bytes/1024/1024)."MB";
}


?>
