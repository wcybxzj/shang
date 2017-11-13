<?php
//page386(12.5寻找热点key)
//随机访问产生数据
//php 2.cluster_get.php

//收集mointer数据，还没统计
//redis-cli monitor|head -n 100|php 4.seek_hotkey.php
while (!feof(STDIN)) {
	echo $name = trim(fgets(STDIN));
	echo PHP_EOL;
}
