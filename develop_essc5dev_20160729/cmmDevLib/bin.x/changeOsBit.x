#!/bin/sh

# ����1 Ϊ����λ��
###########################################################
# Ԥ�ȶ������±���
# UNIONLIBDIR         CMM�㽻��Ŀ¼��λ��cmmDevLibĿ¼·��
# PRODUCTHOUSEWARE    Ӧ�ò㽻��Ŀ¼λ��,��:kmsDevLib
# ORACLE_HOME         ���ʹ��oracle�趨��
# DIROFDB2            ���ʹ��db2�趨��
###########################################################

# **** ����������� ****
# ALL:          ��������
# NODB2         �����.sqc֮����ļ�
# ORACLE:       ֻ����.c��.qc
# DB2:          ֻ����.c��.sqc
# INFOMRIX:     ֻ����.c��.ec
# MYSQL         ֻ����.c��.myc
export COMPILEFLAG=ALL

# �������ϵͳ��λ��
if [ $1 -eq 64 ] ; then
	export UNION_USE_BITS=64
#	export UNION_DIR_SUFFIX=64
else
	export UNION_USE_BITS=32
#	export UNION_DIR_SUFFIX=32
fi

export APPBits=$UNION_USE_BITS

# �����Ƿ�ʹ��GCC����
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

# ����SHELL�ű���Ŀ¼
CMM2003BINXDIR=$UNIONLIBDIR/bin.x/
export CMM2003BINXDIR
# ͷ�ļ�������Ŀ¼
CMM2003INCDIR=$UNIONLIBDIR/include/
export CMM2003INCDIR

# ����CMM��������Ŀ¼
CMMPRODUCTLIB=$UNIONLIBDIR/lib$UNION_USE_BITS
export CMMPRODUCTLIB

# Ŀ����롢�⽻����Ŀ¼
CMM2003LIBDIR=$CMMPRODUCTLIB/lib/
export CMM2003LIBDIR
# ��ִ�г��򽻸���Ŀ¼
CMM2003BINDIR=$CMMPRODUCTLIB/bin/
export CMM2003BINDIR
# �汾�⽻����Ŀ¼
CMM2003VERLIB=$CMMPRODUCTLIB/scoVerLib/
export CMM2003VERLIB
# ��ִ�й��߽�����Ŀ¼
CMM2003TOOLSDIR=$CMMPRODUCTLIB/tools/
export CMM2003TOOLSDIR
# ��Դ�����Ŀ¼
CMM2003OPENDIR=$CMMPRODUCTLIB/openLib/
export CMM2003OPENDIR
# db2 bnd
CMM2003BNDDIR=$CMMPRODUCTLIB/bnd/
export CMM2003BNDDIR

# ����PATH����Ŀ¼��˳��
export PATH=.:$CMM2003OPENDIR/bin:$CMM2003BINXDIR:$CMM2003TOOLSDIR:$PATH

if [ $UNION_USE_BITS -eq 32 ] ; then
	export ORACLE_LIB_DIR=$ORACLE_HOME/lib32/
else
	export ORACLE_LIB_DIR=$ORACLE_HOME/lib/
fi

if [ -f $DIROFDB2/db2profile ] ; then
	. $DIROFDB2/db2profile
fi

# ����Ӧ�ò㽻������Ŀ¼
export PROINCDIR=$PRODUCTHOUSEWARE/include/
export PROLIBDIR=$PRODUCTHOUSEWARE/lib$UNION_USE_BITS/
export PROBINDIR=$PRODUCTHOUSEWARE/bin$UNION_USE_BITS/
export PROBNDDIR=$PRODUCTHOUSEWARE/bnd$UNION_USE_BITS/

export KMSINCDIR=$PROINCDIR
export KMSLIBDIR=$PROLIBDIR
export KMSBINDIR=$PROBINDIR
export KMSBNDDIR=$PROBNDDIR
