#	Wolfgang Wang
#	2004/11/20


DEF = -O -DUNIX  $(OSSPEC)
HOMEDIR = $(CMM2003DIR)/unionDesKeyDB/
INCDIR = $(HOMEDIR)include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

all:	makeall			\
	libProducts

DEFINES = -D _UnionDesKey_4_x_ -D _UnionDesKey_3_x_  $(OSSPEC)

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs=	unionDesKey.4.x.20060814.o	\
	unionDesKeyDB.4.x.20060814.o	\
	unionDesKeyDBLog.4.x.20060814.o	\
	mngDesKeyDB.4.x.20060814.o	\
	mngDesKeyDBLog.4.x.20060814.o \
	keyDBLocalBackuper.20060814.o \
	mngKeyDBLocalReLoader.20060814.o \
	mngKeyDBMirrorDiff.20060814.o
makeall:	$(objs)

libProducts:	$(null)
	mv keyDBLocalBackuper.20060814.o	$(CMM2003LIBDIR)keyDBLocalBackuper.4.x.20060814.o
	mv mngKeyDBLocalReLoader.20060814.o	$(CMM2003LIBDIR)mngKeyDBLocalReLoader.4.x.20060814.o
	mv mngKeyDBMirrorDiff.20060814.o	$(CMM2003LIBDIR)mngKeyDBMirrorDiff.4.x.20060814.o
	mv *.o					$(CMM2003LIBDIR)
	cp $(INCDIR)unionDesKey.h		$(CMM2003INCDIR)
	cp $(INCDIR)unionDesKeyDB.h		$(CMM2003INCDIR)
	cp $(INCDIR)unionDesKeyDBLog.h		$(CMM2003INCDIR)
