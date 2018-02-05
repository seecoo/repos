HOMEDIR = $(CMM2003DIR)/unionComplexDB
INCDIR = $(HOMEDIR)/include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = -D _UNIX  $(OSSPEC) -D _UnionEnv_3_x_ -D _MaxVarNum_4096_

all:	makeall						\
	finalProduct

DEFINES = -D _UNIX $(OSSPEC)

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs=	unionInputTBLData.20090428.o		\
	unionOutputTBLData.20090428.o
makeall:	$(objs)

finalProduct:	$(null)
	mv unionInputTBLData.20090428.o				$(CMM2003LIBDIR)/InputTBLData.OnComplexDB.o
	mv unionOutputTBLData.20090428.o			$(CMM2003LIBDIR)/OutputTBLData.OnComplexDB.o
	cp $(INCDIR)unionTBLData.h				$(CMM2003INCDIR)

