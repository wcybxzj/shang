<?php

$redis1 = new Redis();
$redis1->connect('127.0.0.1', 6379);
//$redis1->flushDb();

if ($argc< 3) {
	echo "php 1.php start end\n";
	die();
}

$s_num = $argv[1];
$e_num = $argv[2];
echo "s_num:$argv[1]";
echo "e_num:$argv[2]";

function store_data($redis,$s_num ,$e_num)
{
	for ($i = $s_num; $i < $e_num; $i++) {
		$content = rand(1,100000);
		$key= "ybx".$i;
		echo $key."  ".$content."\n";
		$redis->hSet('h', $key, $content);
}
}

store_data($redis1,$s_num, $e_num);
?>
