#!/bin/sh

#######################
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
        mk_file=$1
        echo "make -C $mk_file"
        make -C $mk_file 2>${err_log_file}
#        check_err "$mk_file"
}

err_log_file=${DEVDIR}/compiled_err.log
exec_makefile $1
