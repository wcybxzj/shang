#!/usr/bin/php
<?php

class A{
	private $a;
	private $b;

	public function __construct($a, $b){
		$this->a = $a;
		$this->b = $b;
	}

	public function say($obj){
		echo $obj->a;
		echo $obj->b;
	}
}

class B{
	private $a;
	private $b;

	public function __construct($a, $b){
		$this->a = $a;
		$this->b = $b;
	}

	public function say($obj){
		echo $obj->a;
		echo $obj->b;
	}
}

//php和c++一样
//同class对象可以通过方法,访问同类对象执行private数据属性
$obj1 = new A(11, 22);
$obj2 = new A(33, 44);
$obj3 = new B(55, 66);

//echo $obj1->a;//error
$obj1->say($obj2);//ok

//$obj3->say($obj2);//error
?>
