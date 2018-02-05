#!/bin/sh 

err_log_file=$HOME/make.$$.log

read_confirm()
{
	echo "$@ [y/n]? "
	read __confirm_key__
	if [ "${__confirm_key__}" = "y" ] ;then
		return 1
	else
		return 0
	fi
}
check_err()
{
	curr_file="$1"
	if [ ! -f ${err_log_file} ] ; then
		>${err_log_file}
	fi
	flag=`grep -v '^ar' ${err_log_file} | grep -v 'In function' | grep -v '警告' | grep -v '在函数' | \
		grep -v '(W) Function' | grep -v 'warning' | wc | awk '{print $1}'`

        if [ $flag -gt 0 ] ; then
                echo "compiled stop at [${curr_file}]!!"
		read_confirm "continue"
		if [ $? -eq 1 ] ;then
			continue
		else
			break
		fi
        fi
}
exec_makefile()
{
	mk_dir=$1
	mk_file=$2
	old_dir=$PWD
	cd $mk_dir
	pwd
	echo "make -f $mk_file"
	#echo "Press any enter continue...."
	#read
	make -f $mk_file 2>${err_log_file}
	cd $old_dir
	check_err "$mk_dir/$mk_file"
}

compiled_dir()
{
	mk_root_dir=$1
	my_cur_dir=$PWD
	for fullname in `find ${mk_root_dir} -name "*.mk" | \
		grep -vw makeBins | grep -vw test | grep -vw tmp | grep -vw "db2" `
	do
        	mk_file=`basename $fullname`
        	mk_dir=`dirname $fullname`
		exec_makefile $mk_dir $mk_file
		cd ${my_cur_dir}
	done
#
	for fullname in `find ${mk_root_dir} -name "[mK]akefile" | \
		grep -vw makeBins | grep -vw test | grep -vw tmp `
	do
        	mk_file=`basename $fullname`
        	mk_dir=`dirname $fullname`
		exec_makefile $mk_dir $mk_file
		cd ${my_cur_dir}
	done
}

compiled_all()
{
	. set32.x
	echo =================================编译32位=====================================
	compiled_dir "$1"
	echo =================================编译64位=====================================
	. set64.x
	compiled_dir "$1"
	rm -f ${err_log_file}
}

##################
if [ $# -eq 1 ] ; then
	if [ -d "$1" ] ; then
		compiled_all "$1"
	else
		echo "dir[$1] not exist!!"
	fi
fi 
if [ $# -eq 0 ] ; then
	if ! ls -l | grep -q '^d' ; then
		compiled_all $PWD
	else
		echo "usage: `basename $0` dir"
	fi
fi 
##################

#
