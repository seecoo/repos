#	Wolfgang Wang
#	2004/11/11

#	Bins
#		02	mngPKDB
#		03	mngPKDBLog

TESTBINDIR = $(HOME)/bin/

all:	genProductTime		\
	mngPKDB3.0.20041125	\
	mngPKDBLog3.0.20041125	\
	testProducts		\
	rmTmpFiles

genProductTime:	$(null)
		genProductGenerateTime

.SUFFIXES:.c.o
.c.o:
	$(CC) $(INCL) $(DEF) -c $*.c

commLibs = 	$(CMM2003LIBDIR)unionTask3.2.20041125.a		\
		$(CMM2003LIBDIR)unionEnv3.0.a			\
		$(CMM2003LIBDIR)UnionSocket3.0.a			\
		$(CMM2003LIBDIR)UnionLog3.0.a			\
		$(CMM2003LIBDIR)unionModule1.0.a			\
		$(CMM2003LIBDIR)unionEnv3.0.a			\
		$(CMM2003LIBDIR)unionInput1.0.a			\
		$(CMM2003LIBDIR)unionPreDefinedCmd1.0.a		\
		$(CMM2003LIBDIR)unionVersion.20041125.a		\
		$(CMM2003LIBDIR)unionLibName3.0.a			\
		unionProductGenerateTime.a			\
		$(CMM2003VERLIB)unionPredefinedVersion2.0.a	\
		$(CMM2003VERLIB)unionEffectiveDays.3650.a		\
		$(CMM2003LIBDIR)UnionStr1.2.a

pkDBLibs = 	$(CMM2003LIBDIR)unionPKDB3.0.20041125.a		\
		$(CMM2003LIBDIR)keyDBBackuper.useDesKey3.0.20041125.a

# Bins 02
libs0220 =	$(CMM2003LIBDIR)mngPKDB3.0.20041125.a			\
		$(pkDBLibs)					\
		$(commLibs)
mngPKDB3.0.20041125:	$(null)
		cc -o mngPKDB3.0.20041125 $(libs0220) -lsocket

# Added by Wolfgang Wang, 2003/09/09
# Bins 02
libs0310 =	$(CMM2003LIBDIR)mngPKDBLog3.0.20041125.a		\
		$(pkDBLibs)					\
		$(commLibs)
mngPKDBLog3.0.20041125:	$(null)
		cc -o mngPKDBLog3.0.20041125 $(libs0310) -lsocket

# testProducts
testProducts:	$(null)
		mv	mngPKDB3.0.20041125		$(TESTBINDIR)mngPKDB
		mv	mngPKDBLog3.0.20041125		$(TESTBINDIR)mngPKDBLog

rmTmpFiles:
		rm *.a
