<?php
//创建数据,据目的是测试在migrate过程中出现问题

$redis1 = new Redis();
$redis1->connect('127.0.0.1', 6379);
$redis1->flushDb();

//$redis2 = new Redis();
//$redis2->connect('127.0.0.1', 6380);
//$redis2->flushDb();

//$num = 500000;
$num = 5000;

//从媒体文件获取45M大字符串
$fp1 = fopen("/root/www/video_sound_data/3.h264_data/1.avi","r");
//$fp2 = fopen("1.avi","a+");
$contents = '';
while (!feof($fp1)) {
	$contents .= fread($fp1, 8192);
}
//fwrite($fp2, $contents);
//die();

function store_data($redis, $num, $contents)
{
	for ($i = 0; $i < $num; $i++) {
		$key= "ybx".$i;
		$result = $redis->set($key, $contents);
	}
}

store_data($redis1,$num, $contents);



?>
