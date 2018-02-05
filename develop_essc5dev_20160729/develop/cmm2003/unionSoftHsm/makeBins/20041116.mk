TESTBINDIR = $(HOME)/bin/

all:	genProductTime			\
	mngSoftHsmKeySpace.20041116	\
	softHsm.20041116		\
	testProduct			\
	rmTmpFiles

genProductTime:	$(null)
		genProductGenerateTime

commLibs  =	$(CMM2003LIBDIR)unionTask3.2.a			\
		$(CMM2003LIBDIR)unionEnv3.0.a			\
		$(CMM2003LIBDIR)UnionLog3.0.a			\
		$(CMM2003LIBDIR)unionModule1.0.a			\
		$(CMM2003LIBDIR)unionEnv3.0.a			\
		$(CMM2003LIBDIR)unionInput1.0.a			\
		$(CMM2003LIBDIR)unionPreDefinedCmd1.0.a		\
		$(CMM2003LIBDIR)unionVersion1.0.a			\
		unionProductGenerateTime.a			\
		$(CMM2003LIBDIR)unionLibName3.0.a			\
		$(CMM2003LIBDIR)UnionAlgorithm1.0.a		\
		$(CMM2003VERLIB)unionPredefinedVersion1.0.a	\
		$(CMM2003LIBDIR)UnionStr1.2.a

libs0110 =	$(CMM2003LIBDIR)mngSoftHsmKeySpace.20041116.a	\
		$(CMM2003LIBDIR)softHsmKeySpace.20041116.a		\
		$(commLibs)
mngSoftHsmKeySpace.20041116:	$(null)
		cc -o mngSoftHsmKeySpace.20041116 $(libs0110)

libs0210 = 	$(CMM2003LIBDIR)softHsmMain.20041116.a		\
		$(CMM2003LIBDIR)softHsmCmd.20041116.a		\
		$(CMM2003LIBDIR)softHsmKeySpace.20041116.a		\
		$(CMM2003LIBDIR)UnionTCPIPSvr3.0.a			\
		$(commLibs)
softHsm.20041116:	$(null)
		cc -o softHsm.20041116 $(libs0210) -lsocket 

testProduct:	$(null)
		mv mngSoftHsmKeySpace.20041116	$(TESTBINDIR)mngSoftHsmKeySpace
		mv softHsm.20041116		$(TESTBINDIR)softHsm

rmTmpFiles:	$(null)
		rm *.a

