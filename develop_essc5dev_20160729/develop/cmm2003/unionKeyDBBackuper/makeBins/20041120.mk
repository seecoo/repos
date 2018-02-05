#	Wolfgang Wang
#	2004/11/20

#	Bins
#		02	mngDesKeyDB
#		03	mngDesKeyDBLog
#		04	keyDBBackuper

TESTBINDIR=$(HOME)/bin/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

all:	genProductTime		\
	keyDBBackuper		\
	testProducts

genProductTime:	$(null)
		genProductGenerateTime

commLibs     = 	$(CMM2003LIBDIR)unionTask3.2.a			\
		$(CMM2003LIBDIR)unionEnv3.0.a			\
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

desKeyDBLibs =	$(CMM2003LIBDIR)unionDesKeyDB.20041120.a
pkDBLibs =	$(CMM2003LIBDIR)unionPKDB.20041111.a		\
		$(CMM2003LIBDIR)unionPK.20041111.a
libs0410 =	$(CMM2003LIBDIR)keyDBBackuperMain.20041120.a	\
		$(desKeyDBLibs)					\
		$(pkDBLibs)					\
		$(CMM2003LIBDIR)keyDBBackuper.20041120.a		\
		$(CMM2003LIBDIR)UnionTCPIPSvr3.0.a			\
		$(commLibs)
keyDBBackuper:	$(null)
		cc -o keyDBBackuper $(libs0410) -lsocket

testProducts:	$(null)
		mv	keyDBBackuper	$(TESTBINDIR)
		rm	*.a
