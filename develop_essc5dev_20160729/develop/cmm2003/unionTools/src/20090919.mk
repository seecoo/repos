HOMEDIR = $(CMM2003DIR)/unionTools
INCDIR = $(HOMEDIR)/include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = -D _UNIX  $(OSSPEC)

all:	makeall			\
	genMKFileOfComplyAllSrc	\
	genMKFileOfLinkAllBins	\
	finalProduct


.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs0130 = batchComply.20090919.o	\
	genMKFileOfComplyAllSrc.o	\
	genMKFileOfLinkAllBins.o
makeall:	$(objs0130)

commLibs=	batchComply.20090919.o	\
		$(CMM2003LIBDIR)externLogFun.20080717.o\
		$(CMM2003LIBDIR)UnionStr1.2.o\
		$(CMM2003LIBDIR)unionSystemTime1.0.o\
		$(CMM2003LIBDIR)UnionLog1.2.withoutTimeFun.o

libs01=		genMKFileOfComplyAllSrc.o	\
		$(commLibs) $(OSSPECLIBS)
genMKFileOfComplyAllSrc:
		cc $(OSSPEC) -o genMKFileOfComplyAllSrc $(libs01)
		
libs02=		genMKFileOfLinkAllBins.o	\
		$(commLibs) $(OSSPECLIBS)
genMKFileOfLinkAllBins:
		cc $(OSSPEC) -o genMKFileOfLinkAllBins $(libs02)
		
finalProduct:	$(null)
		cp $(INCDIR)batchComply.h		$(CMM2003INCDIR)
		mv *.o					$(CMM2003LIBDIR)
		mv genMKFileOfComplyAllSrc		$(CMM2003BINDIR)
		mv genMKFileOfLinkAllBins		$(CMM2003BINDIR)
