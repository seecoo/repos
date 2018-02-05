#	HuangBaoxin	
#	2009/02/09

####### FOR UNION DEF ##########
DEF = -O -DUNIX $(OSSPEC)
HOMEDIR = $(CMM2003DIR)/unionDesKeyDB/
INCDIR = $(HOMEDIR)include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

###### FOR oracle path DEFINE
PRECOMDIR=$(ORACLE_HOME)/precomp/public 
RDBMDIR=$(ORACLE_HOME)/rdbms/public 
PLSQLDIR=$(ORACLE_HOME)/plsql/public 
NETWORKDIR=$(ORACLE_HOME)/network/public
ESQLINCL = $(ORACLE_HOME)/precomp/lib
ERRORLIB = $(ORACLE_HOME)/lib32
#### FOR C COMPILER ####
CC     =  cc
CFLAGS = -I . -I $(ESQLINCL) -I $(PRECOMDIR) -I $(RDBMDIR) -I $(PLSQLDIR) -I $(NETWORKDIR) -I $(INCDIR) -I $(CMM2003INCDIR)
CLINK  = $(CC) $(CFLAGS)
CLINKOBJ = $(CC) -L$(ORACLE_HOME)/lib/ -L $(ERRORLIB) -lclntsh `cat $(ORACLE_HOME)/lib/ldflags` \
           `cat $(ORACLE_HOME)/lib/sysliblist` -lm -lpthread 

####### FOR ORACLE COMPILER ######## 
PROC     = proc 
PCFLAG   =  include=. include=$(PRECOMDIR) include=$(RDBMDIR) include=$(PLSQLDIR) include=$(NETWORKDIR) include=$(ESQLINCL)\
	include=$(INCDIR) include=$(CMM2003INCDIR) LINES=NO IRECLEN=400 oreclen=400 oraca=yes ltype=none parse=none
PCLINK   = $(PROC) $(PCFLAG)

all:	makeall unionDesKeyDB.4.x.useOracle.20090209.o\
	libProducts

DEFINES = -D _UnionDesKey_4_x_ -D _UnionDesKey_3_x_ $(OSSPEC)

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs=	unionDesKey.4.x.20060814.o	\
	unionDesKeyDB.4.x.20090209.o	\
	unionDesKeyDBLog.4.x.20060814.o	\
	mngDesKeyDB.4.x.20060814.o	\
	mngDesKeyDBLog.4.x.20060814.o \
	keyDBLocalBackuper.20060814.o \
	mngKeyDBLocalReLoader.20060814.o \
	mngKeyDBMirrorDiff.20060814.o
makeall:	$(objs)

unionDesKeyDB.4.x.useOracle.20090209.o : unionDesKeyDB.4.x.useOracle.20090209.pc
	$(PCLINK) iname=unionDesKeyDB.4.x.useOracle.20090209.pc
	$(CLINK) -c $(DEFINES) unionDesKeyDB.4.x.useOracle.20090209.c
	
libProducts:	$(null)
	mv keyDBLocalBackuper.20060814.o	$(CMM2003LIBDIR)keyDBLocalBackuper.4.x.20060814.o
	mv mngKeyDBLocalReLoader.20060814.o	$(CMM2003LIBDIR)mngKeyDBLocalReLoader.4.x.20060814.o
	mv mngKeyDBMirrorDiff.20060814.o	$(CMM2003LIBDIR)mngKeyDBMirrorDiff.4.x.20060814.o
	mv unionDesKeyDB.4.x.useOracle.20090209.o	$(CMM2003LIBDIR)unionDesKeyDB.4.x.useOracle.20090209.o
	mv *.o					$(CMM2003LIBDIR)
	cp $(INCDIR)unionDesKey.h		$(CMM2003INCDIR)
	cp $(INCDIR)unionDesKeyDB.h		$(CMM2003INCDIR)
	cp $(INCDIR)unionDesKeyDBLog.h		$(CMM2003INCDIR)
	cp $(INCDIR)unionDesKeyDB.useOracle.h	$(CMM2003INCDIR)
	rm unionDesKeyDB.4.x.useOracle.20090209.c
	rm tp*
