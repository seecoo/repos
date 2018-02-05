DEF = -O -DUNIX  $(OSSPEC)
HOMEDIR = $(CMM2003DIR)/unionDesKeyDB/
LIBDIR = $(HOMEDIR)lib/
INCDIR = $(HOMEDIR)include/
BINDIR = $(HOMEDIR)bin/
OBJDIR = $(HOMEDIR)obj/


INCLUDE = $(CMM2003DIR)/unionComplexDB/include/
INCLUDE2= $(CMM2003DIR)/unionMngSvr/include/

DEF = -D _UnionDesKeyDB_2_x_ -D _UnionDesKey_3_x_ -D _UnionDesKey_4_x_ $(OSSPEC)

INCL = -I $(INCDIR) -I $(CMM2003INCDIR) -I $(INCLUDE) -I $(INCLUDE2)

all:	makeall				\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(INCL) $(DEF) -c $*.c

objs 	= 	desKeyDBRecord1.0.o	\
		desKeyDBSvrOperation1.0.o
		

makeall:	$(objs)

finalProducts:	$(null)
	mv	desKeyDBRecord1.0.o		$(CMM2003LIBDIR)desKeyDBRecord1.0.useDesKey4.x.o
	mv	desKeyDBSvrOperation1.0.o	$(CMM2003LIBDIR)desKeyDBSvrOperation1.0.useDesKey4.x.o	
