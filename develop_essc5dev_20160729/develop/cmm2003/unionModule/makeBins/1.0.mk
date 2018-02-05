#	2003/09/19£¬Wolfgang Wang
#	Bins
#		01	mngModule

#	2003/09/23
#	Bins
#		mngModule1.0


TESTBINDIR = $(UNIONREC)/bin/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = -D _UNIX

all:	genProductTime		\
	mngModule1.0		\
	finalProducts

genProductTime:	$(null)
		genProductGenerateTime

# bins
# 01 mngModule
libs0130 =	$(CMM2003LIBDIR)mngModule1.0.a			\
		$(CMM2003LIBDIR)unionTask3.2.a			\
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
mngModule1.0:	$(null)
		cc -o mngModule1.0 $(libs0130)


finalProducts:	$(null)
	# bins
		mv $(BINDIR)mngModule1.0	$(TESTBINDIR)mngModule
	# rmTmpFiles
		rm *.a
