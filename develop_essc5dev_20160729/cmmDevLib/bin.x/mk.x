#!/bin/bash

###############
#�ж��Ƿ����makefile�ļ�
if [ ! -f makefile ] ; then
	echo "makefile not exit!"
	exit 11
fi

echo "------------ ����32Bit����-----------------------"
. set32.x
if [ -z "$1" ] ; then 
	make && make install
else
	make $@
fi

echo "------------ ����64Bit����-----------------------"
. set64.x
if [ -z "$1" ] ; then 
	make && make install
else
	make $@
fi

