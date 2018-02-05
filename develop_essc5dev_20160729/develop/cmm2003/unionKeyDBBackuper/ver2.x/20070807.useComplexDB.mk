#	Wolfgang Wang
#	2004/11/20

# 使用3.0版本的DESKey定义
DEF = -D _UnionDesKey_3_x_ -D _UnionPKDB_3_x_  $(OSSPEC)

HOMEDIR = $(CMM2003DIR)/unionKeyDBBackuper/
INCDIR = $(HOMEDIR)include/

ESSC4XINCL=/esscProductHouseWare/2009/includeLibs/dataInterface

INCL = -I $(INCDIR) -I $(CMM2003INCDIR) -I $(ESSC4XINCL) -D _unionDesKey_4_x_ -D _unionPK_4_x_ -D _keyDBBackuper_2_x_ -D _useComplexDB_

all:	makeall					\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(INCL) $(DEF) -c $*.c

# Libs 01
allobjs = keyDBBackuper.2.x.20070807.useComplexDB.o
makeall:	$(allobjs)

finalProducts:  $(null)
	mv      keyDBBackuper.2.x.20070807.useComplexDB.o    $(CMM2003LIBDIR)keyDBBackuper.useComplexDB.20070807.o
	cp      $(INCDIR)keyDBBackuper.h        $(CMM2003INCDIR)
