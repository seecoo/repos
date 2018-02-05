#	Wolfgang Wang
#	2004/11/20

# 使用3.0版本的DESKey定义
DEF = -D _UnionDesKey_3_x_ -D _UnionPKDB_3_x_  $(OSSPEC)

HOMEDIR = $(CMM2003DIR)/unionKeyDBBackuper/
INCDIR = $(HOMEDIR)include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR) -D _unionDesKey_4_x_ -D _unionPK_4_x_ -D _keyDBBackuper_2_x_

all:	makeall					\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(INCL) $(DEF) -c $*.c

# Libs 01
allobjs = keyDBBackuper.2.x.20070807.o	\
	keyDBBackuperMain.2.x.20070807.o
makeall:	$(allobjs)

finalProducts:	$(null)
	mv	keyDBBackuper.2.x.20070807.o	$(CMM2003LIBDIR)keyDBBackuper.useDesKey4.x.PK4.x.20070807.o
	mv	*.o				$(CMM2003LIBDIR)
	cp	$(INCDIR)keyDBBackuper.h	$(CMM2003INCDIR)
