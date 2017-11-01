<?php
//创建数据,据目的是测试在migrate过程中出现问题

$redis1 = new Redis();
$redis1->connect('127.0.0.1', 6379);
//$redis1->flushDb();

//$redis2 = new Redis();
//$redis2->connect('127.0.0.1', 6380);
//$redis2->flushDb();
if ($argc< 3) {
	echo "php 1.php start end\n";
	die();
}
$s_num = $argv[1];
$e_num = $argv[2];
echo "s_num:$argv[1]";
echo "e_num:$argv[2]";
//从媒体文件获取45M大字符串
$fp1 = fopen("/root/www/video_sound_data/3.h264_data/1.avi","r");
//$fp2 = fopen("1.avi","a+");
$contents = '';
while (!feof($fp1)) {
	$contents .= fread($fp1, 8192);
}
//fwrite($fp2, $contents);
//die();

function store_data($redis,$s_num ,$e_num, $contents)
{
	for ($i = $s_num; $i < $e_num; $i++) {
		$key= "ybx".$i;
		$result = $redis->set($key, $contents);
		sleep(1);//根据需求调整
	}
}

store_data($redis1,$s_num, $e_num,$contents);



?>
