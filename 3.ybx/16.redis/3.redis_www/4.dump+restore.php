<?php
$redis1 = new Redis();
$redis1->connect('127.0.0.1', 6379);

$redis2 = new Redis();
$redis2->connect('127.0.0.1', 6380);

$redis2->delete("ybx");
//0 是ttl,不超时
$redis2->restore("ybx",0,$redis1->dump("ybx"));
echo $redis2->get("ybx");

?>
