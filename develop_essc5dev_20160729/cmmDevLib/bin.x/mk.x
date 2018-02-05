#!/bin/bash

###############
#判断是否存在makefile文件
if [ ! -f makefile ] ; then
	echo "makefile not exit!"
	exit 11
fi

echo "------------ 编译32Bit程序-----------------------"
. set32.x
if [ -z "$1" ] ; then 
	make && make install
else
	make $@
fi

echo "------------ 编译64Bit程序-----------------------"
. set64.x
if [ -z "$1" ] ; then 
	make && make install
else
	make $@
fi

