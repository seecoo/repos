#bin/bash

#file_substr.sh
#���ű����ڱ�ȫ����makefile����


if [ $# -lt 3 ]; then
	echo "in $0 arg err!!" 
	exit 11
fi

#������ʱ�ļ�
temp_file="/tmp/script.$$.tmp"

ori_ptn="$1"
dest_ptn="$2"
file_name="$3"

sed "s,$ori_ptn,$dest_ptn,g"<$file_name>$temp_file
mv $temp_file $file_name
#
