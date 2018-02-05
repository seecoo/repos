#!/bin/sh

#export UNIONREC=/cmmDevLib/bin/
export OSName=`uname`
export hostName=`hostname`
export useNewDBInterface=0
dbaccess_flag=esql
export RunningDBType=Oracle
ora_version=`sqlplus -v | grep Release | awk '{print $3}' | awk -F\. '{print $1}'`
export DBType=Oracle_${ora_version}g
if [ "$OSName" = "AIX" ] ; then
	export OsBits=`getconf KERNEL_BITMODE`
else
	export OsBits=`getconf LONG_BIT`
fi
makeBins_dir=${DEVDIR}/makeBins
makefile_list=${makeBins_dir}/makefile.lst
db_type_list="1 2 3 4 7 9"

OSType=""
makeName=mkOracleMK

SH_FLG=`echo -e XXX`
echoe=echo
auto_flag=0
if [ "$SH_FLG" = "XXX" ] ; then
	echoe="echo -e"
fi

read_key_continue()
{
	if [ $# -ge 1 ] ; then
		str="$@"
	else
		str="press any key to continue..."
		
	fi
	echo "$str"
	read any_key
}

config_compile_env()
{
	###############################################################
	#cert.def
	export CompileEnvOfCert="$OSName$UNION_USE_BITS"
	###############################################################
	

	export RunningDBType=`echo $DBType | awk -F'_' '{print $1}'`

	#echo "RunningDBType:"  $RunningDBType "DBType:" $DBType

	###############################################################
	#Oracle.def
	if [ "$OSName" = "AIX" ] ; then
		if [ "$DBType" = "Oracle_11g" ] ; then
			export ORACLE_HOME=/u2/ora11g/oracle11g/ora11gClient/ora11g_1_client/
		else
			export ORACLE_HOME=/opt/oracle/product/10.2.0/
		fi
	fi

	case "$hostName" in
		'aix43')
			if [ "$DBType" = "Oracle_10g" ] ; then
				export ORACLE_HOME=/opt/oracle/product/10g
			else
				export ORACLE_HOME=/opt/oracle/product/11gR2/db
			fi
		;;
		'linux-45')
			if [ "$DBType" = "Oracle_10g" ] ; then
				export ORACLE_HOME=/opt/oracle/product/10g
			else
				export ORACLE_HOME=/opt/oracle/product/11gR2/db
			fi
		;;
		'linux-47')
			if [ "$DBType" = "Oracle_10g" ] ; then
				export ORACLE_HOME=/opt/oracle/product/10g
			else
				export ORACLE_HOME=/opt/oracle/product/11g
			fi
		;;
		'linux-61')
			if [ "$DBType" = "Oracle_10g" ] ; then
				export ORACLE_HOME=/opt/oracle/product/10g
			else
				export ORACLE_HOME=/opt/oracle/product/11g
			fi

		;;
		'*')
			if [ "$DBType" = "Oracle_10g" ] ; then
				export ORACLE_HOME=/opt/oracle/product/10g
			else
				export ORACLE_HOME=/opt/oracle/product/11g
			fi
		;;
	esac

	if [ "$OsBits" =  "$UNION_USE_BITS" ] ; then
		export ORACLE_LIB_DIR=$ORACLE_HOME/lib
	else
		export ORACLE_LIB_DIR=$ORACLE_HOME/lib32
	fi
	
	###############################################################

	###############################################################
	#DB2
	export DB2DIR=$DIROFDB2/
	if [ "$OsBits" =  "$UNION_USE_BITS" ] ; then
		export DB2LIBDIR=${DIROFDB2}/lib
	else
		if [ "$DBType" = "DB2_v91" ] ; then
			export DIROFDB2=/opt/db2inst1/sqllib_v91/
			#. $DIROFDB2/db2profile
		fi 
		export DB2LIBDIR=${DIROFDB2}/lib32
	fi
	export DB2INCDIR=${DB2DIR}include
	###############################################################

	###############################################################
	#MYSQL
	#export DIROFMYSQL=/usr/
	if which mysql_config >/dev/null 2>&1; then
		if [ "$OsBits" =  "$UNION_USE_BITS" ] ; then
			#export MYSQLCONFIG=$(mysql_config --libs)
			export MYSQLCONFIG="/usr/lib64/mysql/libmysqlclient.a -lm -lz"
		else
			export MYSQLCONFIG="-L/usr/lib -lmysql"
		fi
	fi
	###############################################################

	###############################################################
	# informix
	if [ "$OsBits" !=  "$UNION_USE_BITS" ] ; then
		export INFORMIXDIR=/home/informix/informix32
	fi
	###############################################################

	#TESTBINDIR=$UNIONREC/bin-${OSName}-${DBType}-${UNION_USE_BITS}bit
	DBACCFLAG=''
	if [ $useNewDBInterface -eq 1 ] ; then
		if echo ${DBType} | grep -i db2 ; then
			DBACCFLAG='odbc-'
		fi
		if echo ${DBType} | grep -i oracle ; then
			DBACCFLAG='oci-'
		fi
	fi
	if [ $useNewDBInterface -eq 2 ] ; then
		DBACCFLAG='odbc-'
	fi
	TESTBINDIR=$UNIONREC/bin-${OSName}-${DBType}-$DBACCFLAG${UNION_USE_BITS}bit
	if ! [ -d "$TESTBINDIR" ] ; then
		mkdir "$TESTBINDIR"
	fi
	export TESTBINDIR=$TESTBINDIR/

	###############################################################
	if [ "$OSName" != "AIX" -a "$UNION_USE_BITS" = "64" ] ; then
		export LD_LIBRARY_PATH=$DB2LIBDIR:$ORACLE_LIB_DIR:/lib64:/usr/lib64:/usr/local/lib:.
	else
		export LD_LIBRARY_PATH=$DB2LIBDIR:$ORACLE_LIB_DIR:/lib:/usr/lib:/usr/local/lib:.
	fi
	###############################################################
	#if [ "$OSName" != "AIX" ] ; then
	#	OSSPECLIBS="-lncurses -lrt"
	#else
	#	#OSSPECLIBS="-lcurses -lrt -static -lpthread -liconv"
	#	OSSPECLIBS="/usr/lib/libncurses.a -lrt -static -lpthread -liconv"
	#fi
	
	#echo "useNewDBInterface=$useNewDBInterface"
	#echo oraclelib=$ORACLE_LIB_DIR=================
	#echo MYSQLCONFIG=$MYSQLCONFIG=================
	#echo db2lib=$DB2LIBDIR===============
	#echo LD_LIBRARY_PATH=$LD_LIBRARY_PATH===============
	#echo  "OsBits = $OsBits UNION_USE_BITS = $UNION_USE_BITS==========="
}

sw_db_env()
{
	if [ $# -ge 1 ] ; then
		db_type=$1
	else
		echo ""
		echo "      -----------------------------------------"
		echo "           s>> 切换DB编译环境 请选择序号"
		echo "      -----------------------------------------"
		echo ""
		echo "        1) Oarcle 10g"
		echo ""
		echo "        2) DB2   #v97"
		echo ""
		echo "        3) Mysql"
		echo ""
		echo "        4) Informix"
		echo ""
		echo "        7) Oarcle 11g"
		echo ""
		echo "        8) DB2_v95"
		echo ""
		echo "      ----------------------------------------"
		eval "$echoe '              <\\c'"
		read db_type
	fi
	case "$db_type" in
		'0')
			DBType=$DBType
			makeName=mkOracleMK
		;;
		'1')
			DBType=Oracle_10g
			makeName=mkOracleMK
		;;
		'2')
			DBType=DB2
			makeName=mkDB2MK
			DIROFDB2=/opt/db2inst1/sqllib

			if [ -r $DIROFDB2/db2profile ] ; then
				. $DIROFDB2/db2profile
			else
				echo "file[$DIROFDB2/db2profile] not found!!"
				read_key_continue
			fi
		;;
		'3')
			DBType=Mysql
			makeName=mkMysqlMK
		;;
		'4')
			DBType=Infromix
			makeName=mkInformixMK
		;;
		'7')
			DBType=Oracle_11g
			makeName=mkOracleMK
		;;
		'8')
			DBType=DB2_v95
			makeName=mkDB2MK
			if [ -r $DIROFDB2/db2profile ] ; then
				. $DIROFDB2/db2profile
			else
				echo "file[$DIROFDB2/db2profile] not found!!"
				read_key_continue
			fi
		;;
	esac

	export DBType; 
	
}

sw_dbaccess_mode()
{
	if [ $# -ge 1 ] ; then
		dbacc_type=$1
	else
		echo ""
		echo "      -----------------------------------------"
		echo "           s>> 切换DB访问方式"
		echo "      -----------------------------------------"
		echo ""
		echo "        1) 原访问方式" 
		echo ""
		#echo "        2) 新访问方式(oracle oci/db2 odbc)"
		echo ""
		echo "        3) ODBC访问方式"
		echo ""
		echo "      ----------------------------------------"
		eval "$echoe '              <\\c'"
		read dbacc_type
	fi
	case "$dbacc_type" in
		'1')
			export useNewDBInterface=0
			dbaccess_flag=esql
		;;
	#	'2')
	#		export useNewDBInterface=1
	#		dbaccess_flag=new
	#	;;
		'3')
			export useNewDBInterface=2
			dbaccess_flag=odbc
		;;
	esac
}
sw_os_env()
{
	if [ $# -ge 1 ] ; then
		os_type=$1
	else
		echo ""
		echo "      -----------------------------------------"
		echo "           s>> 切换OS编译环境 请选择序号"
		echo "      -----------------------------------------"
		echo ""
		echo "        1) $OSName-32位"
		echo ""
		echo "        2) $OSName-64位"
		echo ""
		echo "      ----------------------------------------"
		eval "$echoe '              <\\c'"
		read os_type
	fi
	case "$os_type" in
		'1')
			. set32.x
		;;
		'2')
			. set64.x
		;;
	esac
	OSType="$OSName-$UNION_USE_BITS"
}

linklist()
{
	clear
	echo ""
	echo "        ************* $LOGNAME@$hostName ****************"
	echo "      ======================================================"
	echo "           编译$DBType $OSType程序 DB_ACCESS=$dbaccess_flag  请选择序号"
	echo "      ------------------------------------------------------"
	echo "      b) 切换程序位数（32/64Bit）环境"
	echo "      d) 切换数据库（oracle/db2/...）环境"
	echo "      v) 查看环境状态"
	echo "      c) 切换新数据库访问方式"
	echo ""
	line_num=0
	grep '^\[' ${makefile_list} | while read line
	do
		prompt_name=`echo $line | awk -F\] '{print $1}' | awk -F\[ '{print $2}'`
		line_num=`expr $line_num + 1`
		if [ "$prompt_name" = "null" -o "$prompt_name" = "NULL" ] ; then
			echo ""
		else
			#echo "      $line_num) 链接 ${prompt_name}"
			printf "      %2d|%da) 链接|链接所有 %s\n" $line_num $line_num "${prompt_name}"
		fi
	done
	echo ""
	echo "      a|aa) 链接 所有程序|链接 所有数据库32/64位程序"
	echo ""
	echo "      q) 退出."
	echo "      ------------------------------------------------------"
	eval "$echoe '                <\\c'"
}
makeCmd()
{
	if [ $# -lt 2 ] ; then
		return
	fi
	t_dir=$1
	t_list_file=$2
	
	t_prompt="press any to continue..."
	if [ $# -gt 2 ] ; then
		t_prompt="$3"
	fi
	cd $t_dir || return 72
	rm -f esscMngSvrCmdLibs.a esscMngSvrShareLibs.a unionProductGenerateTime.a

	config_compile_env
	echo "::::::::::::::::: useNewDBInterface=$useNewDBInterface db=$dbaccess_flag  DBType=$DBType BITS=$UNION_USE_BITS ::::::::::::::"

	for t_file in `echo $t_list_file | sed 's/,/ /g'`
	do
		if ! [ -r $t_file ] ; then
			echo "file[$t_file] not exist!!"
			read_key_continue "$t_prompt"
			return
		fi
		#which genProductGenerateTime
		if [ -x $makeName ] ; then
			./$makeName $t_file
		else
			make -f $t_file
		fi
	done

}
view_env_conf()
{
	config_compile_env
	
	echo "OS:   [$OSName]"
	echo "DB:   [$DBType]"
	echo "OsBit = [$OsBits] UNION_USE_BITS = [$UNION_USE_BITS]"
	echo "UNION_DIR_SUFFIX = [$UNION_DIR_SUFFIX]"
	echo ""
	echo "\$CC = [$CC]"
	echo ""
	echo "genTime app = `which genProductGenerateTime`"
	echo ""
	echo "oraclelib = [$ORACLE_LIB_DIR]"
	echo "db2lib = [$DB2LIBDIR]"
	echo "LD_LIBRARY_PATH = [$LD_LIBRARY_PATH]"
	echo ""
	echo "cmmLibDir = [$UNIONLIBDIR]"
	echo "kmsLibDir = [$KMSPRODUCTHOUSEWARE]"
	echo ""
	echo "bin dir = [$TESTBINDIR]"
	echo "makeBins dir = [${makeBins_dir}]"

	echo "::::::::::::::::: useNewDBInterface=$useNewDBInterface db=$dbaccess_flag  DBType=$DBType BITS=$UNION_USE_BITS ::::::::::::::"
	
	#echo "useNewDBInterface=$useNewDBInterface"
	#echo oraclelib=$ORACLE_LIB_DIR=================
	#echo MYSQLCONFIG=$MYSQLCONFIG=================
	#echo db2lib=$DB2LIBDIR===============
	#echo LD_LIBRARY_PATH=$LD_LIBRARY_PATH===============
	#echo  "OsBits = $OsBits UNION_USE_BITS = $UNION_USE_BITS==========="
}

link_all()
{
	#view_env_conf
	config_compile_env
	line_num=0
	rm -f $TESTBINDIR/*
	
	while read line
	do
		prompt_name=`echo $line | awk -F\] '{print $1}' | awk -F\[ '{print $2}'`
		if echo "$line" | grep -q '^\[' ; then
			line_num=`expr $line_num + 1`
			makefile_name=`echo $line | awk '{print $NF}'`
			if [ "${makefile_name}" = '[null]' ] ; then
				continue
			fi
			#if [ -f "${makeBins_dir}/${makefile_name}" ] ; then
				echo "链接 $prompt_name 程序..."
				makeCmd ${makeBins_dir} ${makefile_name}
			#fi
		fi
	done<${makefile_list}
}

auto_link_all_list()
{
	#$1=db_flag
	#$2=app_bit

	if [ $# -ne 2 ] ; then
		echo "auto_run arg err!!"
		return
	fi
	
	sw_db_env $1
	sw_os_env $2
	#view_env_conf
	link_all
}

auto_link_all_type()
{
	makeBins_dir="$1"
	makefile_name="$2"
	for i in 1 2
	do
		for j in $db_type_list
		do
			sw_db_env $j
			sw_os_env $i
			makeCmd ${makeBins_dir} ${makefile_name}
		done
	done
}

link_all_app()
{
	for i in 1 2
	do
		for j in $db_type_list
		do
			auto_link_all_list $j $i
		done
	done
}

if [ $# -ge 3 ] ; then
	if [ "$1" = "-auto" ] ; then
		auto_flag=1
		auto_link_all_list $2 $3
		exit
	fi

fi


sw_db_env 7
sw_os_env 2
input_key=999
while [ true ]
do
	case ${input_key} in
	'aa'|'AA')
		link_all_app
		read_key_continue
	;;
	'a'|'A')
		link_all
		read_key_continue
	;;
	'D'|'d')
		sw_db_env
		echo "  已切换为[$DBType]数据库环境!"
		read_key_continue
	;;
	'b'|'B')
		sw_os_env
		echo "  已切换为[$OSType]Bit程序环境!"
		read_key_continue
	;;
	'v'|'V')
		view_env_conf
		read_key_continue
	;;
	'c'|'C')
		sw_dbaccess_mode
		read_key_continue
	;;

	'q'|'Q'|'exit'|'quit')
		sw_db_env 1
		sw_os_env 1
		exit 0
	;;
	'999')
		:
	;;
	*)
		line_num=0
		while read line
		do
			if echo "$line" | grep -q '^\[' ; then
				line_num=`expr $line_num + 1`
				if [ "$line_num" = "$input_key" ] ; then
					makefile_name=`echo $line | awk '{print $NF}'`
					if [ "${makefile_name}" = '[null]' ] ; then
						continue
					fi
					#echo "makefile_name:" $makefile_name "makeBins_dir:" $makeBins_dir
					#echo "useNewDBInterface" $useNewDBInterface " dbaccess_flag" $dbaccess_flag
					makeCmd ${makeBins_dir} ${makefile_name}
				fi
				if [ "${line_num}a" = "$input_key" -o "${line_num}A" = "$input_key" ] ; then
					makefile_name=`echo $line | awk '{print $NF}'`
					if [ "${makefile_name}" = '[null]' ] ; then
						continue
					fi
					#echo "makefile_name:" $makefile_name "makeBins_dir:" $makeBins_dir
					auto_link_all_type ${makeBins_dir} ${makefile_name}
				fi
			fi
		done<${makefile_list}
		read_key_continue

		
	;;
	esac
	if [ ${auto_flag} -eq 1 ]  ; then
		break;
	fi
	linklist	
	read input_key
	done
#
