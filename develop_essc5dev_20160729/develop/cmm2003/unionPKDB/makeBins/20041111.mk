#	Wolfgang Wang
#	2004/11/11

#	Bins
#		02	mngPKDB
#		03	mngPKDBLog

TESTBINDIR = $(HOME)/bin/

all:	genProductTime		\
	mngPKDB.20041111	\
	mngPKDBLog.20041111	\
	testProducts		\
	rmTmpFiles

genProductTime:	$(null)
		genProductGenerateTime

.SUFFIXES:.c.o
.c.o:
	$(CC) $(INCL) $(DEF) -c $*.c

commLibs = 	$(CMM2003LIBDIR)unionTask3.2.a			\
		$(CMM2003LIBDIR)unionEnv3.0.a			\
		$(CMM2003LIBDIR)UnionSocket3.0.a			\
		$(CMM2003LIBDIR)UnionLog3.0.a			\
		$(CMM2003LIBDIR)unionModule1.0.a			\
		$(CMM2003LIBDIR)unionEnv3.0.a			\
		$(CMM2003LIBDIR)unionInput1.0.a			\
		$(CMM2003LIBDIR)unionPreDefinedCmd1.0.a		\
		$(CMM2003LIBDIR)unionVersion1.0.a			\
		$(CMM2003LIBDIR)unionLibName3.0.a			\
		unionProductGenerateTime.a			\
		$(CMM2003VERLIB)unionPredefinedVersion2.0.a	\
		$(CMM2003LIBDIR)UnionStr1.2.a

pkDBLibs = 	$(CMM2003LIBDIR)unionPKDB.20041111.a		\
		$(CMM2003LIBDIR)unionPK.20041111.a			\
		$(CMM2003LIBDIR)keyDBBackuper.20041120.a

# Bins 02
libs0220 =	$(CMM2003LIBDIR)mngPKDB.20041111.a			\
		$(pkDBLibs)					\
		$(commLibs)
mngPKDB.20041111:	$(null)
		cc -o mngPKDB.20041111 $(libs0220) -lsocket

# Added by Wolfgang Wang, 2003/09/09
# Bins 02
libs0310 =	$(CMM2003LIBDIR)mngPKDBLog.20041111.a		\
		$(pkDBLibs)					\
		$(commLibs)
mngPKDBLog.20041111:	$(null)
		cc -o mngPKDBLog.20041111 $(libs0310) -lsocket

# testProducts
testProducts:	$(null)
		mv	mngPKDB.20041111		$(TESTBINDIR)mngPKDB
		mv	mngPKDBLog.20041111		$(TESTBINDIR)mngPKDBLog

rmTmpFiles:
		rm *.a
