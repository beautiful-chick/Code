#!/bin/bash

if [ $1 -eq 0 ];then
	echo $0
	echo $$
	echo "hello world 1"
	echo $HOME
	echo `pwd`    #这里的`pwd`和$PWD等效，PWD是一个环境变量！而pwd是命令
	echo $PWD
else
	echo $0
	echo $$
	echo "hello world 2"
fi
