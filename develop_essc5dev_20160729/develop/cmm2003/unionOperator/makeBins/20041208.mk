#	2003/09/19	Wolfgang Wang
#	Version		1.0
#	bins
#		mngOperator.20041208

TESTBINDIR = $(UNIONREC)/bin/

all:	genProductTime			\
	mngOperator.20041208		\
	finalProduct

genProductTime:	$(null)
		genProductGenerateTime

libs0110 =	$(CMM2003LIBDIR)mngOperator.20041208.a		\
		$(CMM2003LIBDIR)unionOperator.20041208.a		\
		$(CMM2003LIBDIR)unionTask3.2.a			\
		$(CMM2003LIBDIR)unionEnv3.0.a			\
		$(CMM2003LIBDIR)UnionAlgorithm1.0.a		\
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
mngOperator.20041208:	$(null)
		cc -o mngOperator.20041208 $(libs0110)
finalProduct:	$(null)
# bins
		mv mngOperator.20041208		$(TESTBINDIR)mngOperator
	# rmTmpFiles
		rm *.a

