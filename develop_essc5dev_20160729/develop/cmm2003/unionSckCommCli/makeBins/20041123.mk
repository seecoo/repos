
TESTBINDIR = $(HOME)/bin/

all:	genProductTime			\
	mngCommSvr.20041123		\
	commSvrMon.20041123		\
	finalProduct			\
	rmTmpFiles
	

genProductTime:	$(null)
		genProductGenerateTime

commLibs = 	$(CMM2003LIBDIR)unionTask3.2.a			\
		$(CMM2003LIBDIR)UnionSocket3.0.a			\
		$(CMM2003LIBDIR)unionEnv3.0.a			\
		$(CMM2003LIBDIR)UnionLog3.0.a			\
		$(CMM2003LIBDIR)unionModule1.0.a			\
		$(CMM2003LIBDIR)unionEnv3.0.a			\
		$(CMM2003LIBDIR)unionInput1.0.a			\
		$(CMM2003LIBDIR)unionPreDefinedCmd1.0.a		\
		$(CMM2003LIBDIR)unionVersion1.0.a			\
		unionProductGenerateTime.a			\
		$(CMM2003LIBDIR)unionLibName3.0.a			\
		$(CMM2003VERLIB)unionPredefinedVersion1.0.a	\
		$(CMM2003LIBDIR)UnionStr1.2.a

commSvrLibs =	$(CMM2003LIBDIR)unionSckCommCli.20041123.a		\
		$(CMM2003LIBDIR)unionCommSvrSample.20041123.a

libs0110 =	$(CMM2003LIBDIR)mngCommSvr.20041123.a		\
		$(commSvrLibs)					\
		$(commLibs)
mngCommSvr.20041123:	$(null)
		cc -o mngCommSvr.20041123 $(libs0110) -lsocket

libs0210 =	$(CMM2003LIBDIR)commSvrMon.20041123.a		\
		$(commSvrLibs)					\
		$(CMM2003LIBDIR)unionMonitor1.0.a			\
		$(commLibs)
commSvrMon.20041123:	$(null)
		cc -o commSvrMon.20041123 $(libs0210) -lcurses -lsocket


finalProduct:	$(null)
		mv mngCommSvr.20041123		$(TESTBINDIR)mngCommSvr
		mv commSvrMon.20041123		$(TESTBINDIR)commSvrMon

rmTmpFiles:	$(null)
		rm *.a

