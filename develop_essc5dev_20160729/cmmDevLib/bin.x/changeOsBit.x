#!/bin/sh

# 参数1 为程序位数
###########################################################
# 预先定义以下变量
# UNIONLIBDIR         CMM层交付目录的位置cmmDevLib目录路径
# PRODUCTHOUSEWARE    应用层交付目录位置,如:kmsDevLib
# ORACLE_HOME         如果使用oracle需定义
# DIROFDB2            如果使用db2需定义
###########################################################

# **** 定义编译类型 ****
# ALL:          编译所有
# NODB2         编译除.sqc之外的文件
# ORACLE:       只编译.c和.qc
# DB2:          只编译.c和.sqc
# INFOMRIX:     只编译.c和.ec
# MYSQL         只编译.c和.myc
export COMPILEFLAG=ALL

# 定义操作系统的位数
if [ $1 -eq 64 ] ; then
	export UNION_USE_BITS=64
#	export UNION_DIR_SUFFIX=64
else
	export UNION_USE_BITS=32
#	export UNION_DIR_SUFFIX=32
fi

export APPBits=$UNION_USE_BITS

# 定义是否使用GCC编译
export UNION_USE_GCC=1
export CC=gcc

OSName=`uname`
if [ $OSName = "AIX" ] ; then
	gcc --version > /dev/null 2>&1
	if [ $? -eq 0 ] ;then
		export UNION_USE_GCC=1;
	else
		export UNION_USE_GCC=0; 
	fi

	if [ $UNION_USE_GCC -eq 1 ] ; then
		export CC=gcc
		export OSSPEC="-fPIC -maix$UNION_USE_BITS -g -O2 -Wall -D _AIX_ -D __64BIT__"
		export OSSPECLINK="-maix$UNION_USE_BITS -lmsaa"
		export BIGTOCLINK="-Xlinker -bbigtoc"
	else
		export CC=cc
		export OSSPEC="-q$UNION_USE_BITS -qcpluscmt -g -D _AIX_ -D __64BIT__"
		export OSSPECLINK="-q$UNION_USE_BITS -lmsaa"
		export BIGTOCLINK=""
	fi

	export OSSPECPACK=""
	if [ "$UNION_USE_BITS" = "64" ] ; then
		export OSSPECPACK="-X64"
	else
		export OSSPECPACK="-X32"
	fi
	export OSSPECLIBS="-lcurses -lrt -lpthread -liconv"
	export LC_CTYPE=C
	export COMPILECMD="$CC -g -O2 $OSSPECLINK -o"
	if [ $UNION_USE_GCC -eq 1 ] ; then
		#export COMPILESO="$CC -g -O2 -fPIC -shared -Wl,-G -Wl,-bdynamic -Wl,-bexpall $OSSPECLINK -o"
		#export DLSPECLINK="-fPIC -Wl,-G -Wl,-bexpall -ldl"
		export COMPILESO="$CC -g -O2 -fPIC -shared -Wl,-G -Wl,-bdynamic -Wl,-bexpall $OSSPECLINK -o"
		export DLSPECLINK="-fPIC -Wl,-G -Wl,-bdynamic -Wl,-brtl -Wl,-bexpall -Wl,-berok -ldl"
	else
		export COMPILESO="$CC -g -O2 -G -bdynamic -bexpall $OSSPECLINK -o"
		export DLSPECLINK="-G -bdynamic -brtl -bexpall -berok -ldl"
	fi
else
	export OSSPEC="-fPIC -m$UNION_USE_BITS -g -O2 -Wall -D _LINUX_ -D __64BIT__"
	export OSSPECLINK="-m$UNION_USE_BITS"
	export OSSPECPACK=""
	#export OSSPECLIBS="-lncurses -lrt -lpthread -lz"
	#export OSSPECLIBS="-lncurses -lrt -lpthread -lsasl2"
	export OSSPECLIBS="-lncurses -lrt -lpthread"
	export BIGTOCLINK=""
	export COMPILECMD="$CC -g -O2 $OSSPECLINK -o"
	export COMPILESO="$CC -g -O2 -fPIC -shared $OSSPECLINK -o"
	export DLSPECLINK="-fPIC -rdynamic -ldl"
fi

# 定义SHELL脚本的目录
CMM2003BINXDIR=$UNIONLIBDIR/bin.x/
export CMM2003BINXDIR
# 头文件交付件目录
CMM2003INCDIR=$UNIONLIBDIR/include/
export CMM2003INCDIR

# 定义CMM交付对象目录
CMMPRODUCTLIB=$UNIONLIBDIR/lib$UNION_USE_BITS
export CMMPRODUCTLIB

# 目标代码、库交付件目录
CMM2003LIBDIR=$CMMPRODUCTLIB/lib/
export CMM2003LIBDIR
# 可执行程序交付件目录
CMM2003BINDIR=$CMMPRODUCTLIB/bin/
export CMM2003BINDIR
# 版本库交付件目录
CMM2003VERLIB=$CMMPRODUCTLIB/scoVerLib/
export CMM2003VERLIB
# 可执行工具交付件目录
CMM2003TOOLSDIR=$CMMPRODUCTLIB/tools/
export CMM2003TOOLSDIR
# 开源代码库目录
CMM2003OPENDIR=$CMMPRODUCTLIB/openLib/
export CMM2003OPENDIR
# db2 bnd
CMM2003BNDDIR=$CMMPRODUCTLIB/bnd/
export CMM2003BNDDIR

# 定义PATH查找目录的顺序
export PATH=.:$CMM2003OPENDIR/bin:$CMM2003BINXDIR:$CMM2003TOOLSDIR:$PATH

if [ $UNION_USE_BITS -eq 32 ] ; then
	export ORACLE_LIB_DIR=$ORACLE_HOME/lib32/
else
	export ORACLE_LIB_DIR=$ORACLE_HOME/lib/
fi

if [ -f $DIROFDB2/db2profile ] ; then
	. $DIROFDB2/db2profile
fi

# 定义应用层交付对象目录
export PROINCDIR=$PRODUCTHOUSEWARE/include/
export PROLIBDIR=$PRODUCTHOUSEWARE/lib$UNION_USE_BITS/
export PROBINDIR=$PRODUCTHOUSEWARE/bin$UNION_USE_BITS/
export PROBNDDIR=$PRODUCTHOUSEWARE/bnd$UNION_USE_BITS/

export KMSINCDIR=$PROINCDIR
export KMSLIBDIR=$PROLIBDIR
export KMSBINDIR=$PROBINDIR
export KMSBNDDIR=$PROBNDDIR
