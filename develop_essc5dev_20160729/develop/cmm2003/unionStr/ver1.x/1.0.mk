#	add by Wolfgang Wang, 2003/09/09 UnionStr1.2.a

HOMEDIR = $(CMM2003DIR)/unionStr/
INCDIR = $(HOMEDIR)include/
INCL = -I $(HOMEDIR)include -I $(CMM2003INCDIR) 


DEFINES = $(OSSPEC)

all:	makeall		\
	UnionStr1.2.a	\
	Product	

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

allobjs=	UnionStr1.2.o				\
		unionExcuteExternalCmdFile.1.0.o	\
		sjl05HsmApiSample.1.0.o	\
		unionExcuteExternalCmdSample.1.0.o
makeall:	$(allobjs)
objs0112 = UnionStr1.2.o
UnionStr1.2.a:	$(objs0112)	
		ar $(OSSPECPACK) rv UnionStr1.2.a $(objs0112)

Product:
		cp $(INCDIR)UnionStr.h	$(CMM2003INCDIR)
		mv UnionStr1.2.a	$(CMM2003LIBDIR)
		mv *.o			$(CMM2003LIBDIR)

