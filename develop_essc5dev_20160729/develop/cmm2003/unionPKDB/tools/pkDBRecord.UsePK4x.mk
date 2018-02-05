DEF = -O -DUNIX  $(OSSPEC)
HOMEDIR = $(CMM2003DIR)/unionPKDB/
LIBDIR = $(HOMEDIR)lib/
INCDIR = $(HOMEDIR)include/
BINDIR = $(HOMEDIR)bin/
OBJDIR = $(HOMEDIR)obj/


INCLUDE = $(CMM2003DIR)/unionComplexDB/include/
INCLUDE2= $(CMM2003DIR)/unionMngSvr/include/

DEF = -D _UnionPKDB_2_x_ -D _UnionPK_3_x_ -D _UnionPK_4_x_ $(OSSPEC)

INCL = -I $(INCDIR) -I $(CMM2003INCDIR) -I $(INCLUDE) -I $(INCLUDE2)

all:	makeall				\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(INCL) $(DEF) -c $*.c

objs 	= 	pkDBRecord1.0.o	\
		pkDBSvrOperation1.0.o
		

makeall:	$(objs)

finalProducts:	$(null)
	mv	pkDBRecord1.0.o		$(CMM2003LIBDIR)pkDBRecord1.0.usePK4.x.o
	mv	pkDBSvrOperation1.0.o	$(CMM2003LIBDIR)pkDBSvrOperation1.0.usePK4.x.o	
