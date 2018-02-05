#	Wolfgang Wang
#	2004/11/20

# 使用3.0版本的DESKey定义
DEF = -D _UnionDesKey_3_x_ -D _UnionPKDB_3_x_  $(OSSPEC)

HOMEDIR = $(CMM2003DIR)/unionKeyDBBackuper/
INCDIR = $(HOMEDIR)include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR) -D _unionDesKey_4_x_

all:	makeall					\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(INCL) $(DEF) -c $*.c

# Libs 01
allobjs = keyDBBackuper.20041120.o	\
	keyDBBackuperMain.20041120.o
makeall:	$(allobjs)

finalProducts:	$(null)
	mv	keyDBBackuper.20041120.o	$(CMM2003LIBDIR)keyDBBackuper.useDesKey4.x.20060814.o	
	mv	keyDBBackuperMain.20041120.o	$(CMM2003LIBDIR)keyDBBackuperMain.useDesKey4.x.20060814.o
