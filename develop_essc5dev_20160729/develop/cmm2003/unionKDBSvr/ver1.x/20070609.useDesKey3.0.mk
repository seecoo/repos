#	Wolfgang Wang
#	2004/11/20

# 使用3.0版本的DESKey定义
DEF = -D _UnionDesKey_3_x_ -D _UnionPKDB_3_x_  $(OSSPEC)

HOMEDIR = $(CMM2003DIR)/unionKDBSvr/
INCDIR = $(HOMEDIR)include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

all:	makeall		\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(INCL) $(DEF) -c $*.c

allobjs=	keyDBSvr.20070609.o	\
		keyDBSvrMain.20070609.o	\
		keyDBMaintainer.20070609.o
makeall:	$(allobjs)

finalProducts:	$(null)
		mv	keyDBSvr.20070609.o		$(CMM2003LIBDIR)keyDBSvr.useDesKey3.xPK3.x.20070609.o
		mv	keyDBSvrMain.20070609.o		$(CMM2003LIBDIR)keyDBSvrMain.useDesKey3.xPK3.x.20070609.o
		mv	keyDBMaintainer.20070609.o	$(CMM2003LIBDIR)keyDBMaintainer.useDesKey3.xPK3.x.20070609.o
		cp	$(INCDIR)keyDBSvr.h		$(CMM2003INCDIR)

