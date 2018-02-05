#!/bin/sh

c_dir="$PWD"
filename=""
dirname=""


#########################################
if [ $# -gt 0 ] ; then
	if [ -f "$1" ] ; then
		echo "------------ ±‡“Î32Bit≥Ã–Ú-----------------------"
		. set32.x
		make -f $1
		read
		echo "------------ ±‡“Î64Bit≥Ã–Ú-----------------------"
		. set64.x
		make -f $1
	else
		echo "makefile[$1] not exist!!"
	fi
	exit 0
fi
##########################################
for fullname in `find . -name "*.mk"|grep -v makeBins|grep -v test|grep -v tmp| grep -v bak`
do
	dirname=`dirname $fullname`
	filename=`basename $fullname`
        cd $dirname || continue
	if grep -qw mv $filename ; then
		if ! grep -qw db2 $filename ; then
			make -f $filename
		fi
	fi
	cd $c_dir 
done
#
