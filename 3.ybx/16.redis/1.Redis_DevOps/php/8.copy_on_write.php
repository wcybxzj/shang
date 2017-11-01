<?php

/*
<redis开发与运维8.1.3子进程内存消耗>
想要测试在AOF重写过程中如果父进程还要接受客户请求，在父子进程间引起
子进程需要把原先和父进程共享的内存拷贝出来的情况
C 28 Oct 21:48:17.642 * AOF rewrite: 1 MB of memory used by copy-o

终端1:插入数据让aof_buf 有内容
php 8.php 0 100

终端1完成后几乎同时
终端2:
redis-cli berewritreaof
终端3:
php 8.php 0 100
终端4:
php 8.php 0 100

n-write

*/

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
		$result = $redis->set($key, $content);
	}
}

for ($i = 0; $i < 30000; $i++) {
	store_data($redis1,$s_num, $e_num);
	echo "\n";
}
?>
