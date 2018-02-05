#	Wolfgang Wang
#	2007/8/3

# 使用3.0版本的DESKey定义
DEF = -D _UnionDesKey_4_x_ -D _UnionPKDB_4_x_  $(OSSPEC) -D _useComplexDB_

HOMEDIR = $(CMM2003DIR)/unionKDBSvr/
INCDIR = $(HOMEDIR)include/

ESSC4XINCL=/esscProductHouseWare/2009/includeLibs/dataInterface

INCL = -I $(INCDIR) -I $(CMM2003INCDIR) -I $(ESSC4XINCL)

all:	makeall		\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(INCL) $(DEF) -c $*.c

allobjs=	kdbSvrService.20090323.useComplexDB.o

makeall:	$(allobjs)

finalProducts:  $(null)
		mv      kdbSvrService.20090323.useComplexDB.o                $(CMM2003LIBDIR)kdbSvrService.useComplexDB.20090323.o
		cp      $(INCDIR)kdbSvrService.h                $(CMM2003INCDIR)
