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

commLibs     = 	$(CMM2003LIBDIR)unionTask3.2.20041125.a		\
		$(CMM2003LIBDIR)unionEnv3.0.a			\
		$(CMM2003LIBDIR)UnionLog3.0.a			\
		$(CMM2003LIBDIR)unionModule1.0.a			\
		$(CMM2003LIBDIR)unionEnv3.0.a			\
		$(CMM2003LIBDIR)unionInput1.0.a			\
		$(CMM2003LIBDIR)unionPreDefinedCmd1.0.a		\
		$(CMM2003LIBDIR)unionVersion.20041125.a		\
		$(CMM2003LIBDIR)unionLibName3.0.a			\
		unionProductGenerateTime.a			\
		$(CMM2003VERLIB)unionPredefinedVersion2.0.a	\
		$(CMM2003VERLIB)unionPredefinedVersion2.0.a	\
		$(CMM2003VERLIB)unionEffectiveDays.3650.a		\
		$(CMM2003LIBDIR)UnionStr1.2.a

desKeyDBLibs =	$(CMM2003LIBDIR)unionDesKeyDB3.0.20041125.a
pkDBLibs =	$(CMM2003LIBDIR)unionPKDB3.0.20041125.a
libs0410 =	$(CMM2003LIBDIR)keyDBBackuperMain.useDesKey3.0.20041125.a		\
		$(desKeyDBLibs)							\
		$(pkDBLibs)							\
		$(CMM2003LIBDIR)keyDBBackuper.useDesKey3.0.20041125.a		\
		$(CMM2003LIBDIR)UnionTCPIPSvr3.0.a					\
		$(commLibs)
keyDBBackuper:	$(null)
		cc -o keyDBBackuper $(libs0410) -lsocket

testProducts:	$(null)
		mv	keyDBBackuper	$(TESTBINDIR)
		rm	*.a
