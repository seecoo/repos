#	Wolfgang Wang
#	2007/8/3

# 使用3.0版本的DESKey定义
DEF = -D _UnionDesKey_4_x_ -D _UnionPKDB_4_x_  $(OSSPEC)

HOMEDIR = $(CMM2003DIR)/unionKDBSvr/
INCDIR = $(HOMEDIR)include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

all:	makeall		\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(INCL) $(DEF) -c $*.c

allobjs=	kdbSvrService.20090323.o

makeall:	$(allobjs)

finalProducts:	$(null)
		mv	kdbSvrService.20090323.o		$(CMM2003LIBDIR)kdbSvrService.useDesKey4.xPK4.x.20090323.o
#		mv	*.o					$(CMM2003LIBDIR)
		cp	$(INCDIR)kdbSvrService.h		$(CMM2003INCDIR)

