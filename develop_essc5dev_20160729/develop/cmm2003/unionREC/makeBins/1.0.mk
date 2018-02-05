#	2003/09/19	Wolfgang Wang
#	Version		1.0

#	bins
#		01	mngREC
#		02	recMon

#	2003/09/23
#	bins
#		mngREC1.0
#		recMon1.0

BINLIBS = $(UNIONLIBDIR)/binLibs

HOMEDIR = $(HOME)/unionLog

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = -D _UNIX

all:	genProductTime		\
	mngREC1.0		\
	recMon1.0		\
	finalProduct

genProductTime:	$(null)
		genProductGenerateTime

libs0110 =	$(CMM2003LIBDIR)mngREC1.0.a			\
		$(CMM2003LIBDIR)unionREC1.0.a			\
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
mngREC1.0:	$(null)
		cc -o mngREC1.0 $(OSSPECLINK) $(libs0110)

libs0210 =	$(CMM2003LIBDIR)recMon1.0.a			\
		$(CMM2003LIBDIR)unionMonitor1.0.a			\
		$(CMM2003LIBDIR)unionREC1.0.a			\
		$(CMM2003LIBDIR)unionTask3.2.a			\
		$(CMM2003LIBDIR)unionEnv3.0.a			\
		$(CMM2003LIBDIR)UnionLog3.0.a			\
		$(CMM2003LIBDIR)unionModule1.0.a			\
		$(CMM2003LIBDIR)unionEnv3.0.a			\
		$(CMM2003LIBDIR)unionInput1.0.a			\
		$(CMM2003LIBDIR)unionPreDefinedCmd1.0.a		\
		$(CMM2003LIBDIR)unionVersion1.0.a			\
		$(CMM2003LIBDIR)unionLibName3.0.a			\
		unionProductGenerateTime.a			\
		$(CMM2003VERLIB)unionPredefinedVersion1.0.a	\
		$(CMM2003LIBDIR)UnionStr1.2.a
recMon1.0:	$(null)
		cc -o recMon1.0 $(OSSPECLINK) $(libs0210)


finalProduct:	$(null)
		mv mngREC1.0		$(CMM2003BINDIR)mngREC
		mv recMon1.0		$(CMM2003BINDIR)recMon
		rm *.a
