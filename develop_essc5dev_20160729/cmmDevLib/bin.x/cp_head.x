#!/bin/sh 

####################
flag=0

#�жϴ������Ĳ����Ƿ�Ŀ¼
if [ -d "$1" ] ; then
	echo $1
	dir=`find $1 -name *.h`
#	echo $dir
else
	dir=$1
fi


#����ͷ�ļ�
for i in $dir
do
	file=`basename $i`
#	echo "$2$file"
	if [ ! -f $2$file ] ; then
		echo "cp $i $2"
		cp $i $2
		flag=1
	fi
	
	diff $i $2
	if [ $? -ne 0 ] ; then
		echo "diff $i $2"
		cp $i $2
		flag=1
	fi
done

#�ж��Ƿ��и��Ʋ���
if [ $flag -eq 0 ] ; then
	echo "make: Nothing to be done for install"
fi
####################

