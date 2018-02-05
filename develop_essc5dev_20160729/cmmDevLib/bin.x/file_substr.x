#bin/bash

#file_substr.sh
#本脚本用于被全编译makefile调用


if [ $# -lt 3 ]; then
	echo "in $0 arg err!!" 
	exit 11
fi

#定义临时文件
temp_file="/tmp/script.$$.tmp"

ori_ptn="$1"
dest_ptn="$2"
file_name="$3"

sed "s,$ori_ptn,$dest_ptn,g"<$file_name>$temp_file
mv $temp_file $file_name
#
