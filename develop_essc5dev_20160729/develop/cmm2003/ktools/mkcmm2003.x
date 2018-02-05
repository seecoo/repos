#!/bin/sh

work_dir=$CMM2003DIR/ktools/
. $work_dir/set32.x
err_log_file=${work_dir}/compiled_err.log

first_compiled_list="unionMenu unionSQL unionWorkingDir unionStr"

is_confirm=0
if [ $# -eq 1 ] ; then
	if [ "$1" = "c" ] ; then
		is_confirm=1
	fi
fi

read_confirm()
{
	echo "$@ [y/n]?"
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
	flag=`grep -v '^ar' ${err_log_file} | grep -v 'In function' | \
		grep -v '(W) Function' | grep -v 'warning' | \
		grep -v 'In file included' | wc | awk '{print $1}'`

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
	cd $mk_dir
	pwd
	echo "make -f $mk_file"
	make -f $mk_file 2>${err_log_file}
	cd - >/dev/null
	check_err "$mk_dir/$mk_file"
}

compiled_dir()
{
	mk_root_dir=$1
	if [ $is_confirm -eq 1 ] ; then
		read_confirm "compile $mk_root_dir ..."
	fi
	for fullname in `find ${mk_root_dir} -name "*.mk" | \
		grep -v makeBins | grep -v test | grep -v tmp`
	do
		my_cur_dir=`pwd`
        	mk_file=`basename $fullname`
        	mk_dir=`dirname $fullname`
		exec_makefile $mk_dir $mk_file
		cd ${my_cur_dir}
	done
#	for fullname in `find ${mk_root_dir} -name makefile|grep -v makeBins`
#	do
#		my_cur_dir=`pwd`
#        	mk_file=`basename $fullname`
#        	mk_dir=`dirname $fullname`
#		exec_makefile $mk_dir $mk_file
#		cd ${my_cur_dir}
#	done
}

######## 开始编译cmm2003代码 ###############
echo " 开始编译底层代码 ...."
read_confirm "continue"
if [ $? -eq 1 ] ;then
	code_dir=$CMM2003DIR
	cd $code_dir
	for union_dir in ${first_compiled_list}
	do
		echo "compiled_dir ${code_dir}/${union_dir}"
		compiled_dir "${code_dir}/${union_dir}"
	done
	read_confirm "continue"
	
	for union_dir in `ls -l |grep '^d' | awk '{print $NF}' | grep '^union' `
	do
		
		if echo "${first_compiled_list}" | grep -wq "${union_dir}" ; then
			continue
		fi
		echo "compiled_dir ${code_dir}/${union_dir}"
		compiled_dir "${code_dir}/${union_dir}"
		#read_confirm "continue"
	done
fi
######## 完成编译cmm2003代码 ###############
echo "编译结束."
echo "press anykey to contine..."
read _tmp_key
#
