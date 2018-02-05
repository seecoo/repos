# 2003/09/22	Wolfgang Wang

#	01	mngTask
#	02	taskMon

#	2003/09/22
#	bins:
#		mngTask4.0

BINLIBS = $(UNIONLIBDIR)/binLibs

HOMEDIR = $(HOME)/unionTask/
LIBDIR = $(HOMEDIR)lib/
BINDIR = $(HOMEDIR)bin/
INCDIR = $(HOMEDIR)include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = -D _UNIX $(OSSPEC)

all:	genProductTime		\
	mngTask4.0		\
	taskMon4.0		\
	binProducts		\
	finalProducts

genProductTime:	$(null)
		genProductGenerateTime
# bins
# 01 mngTask
libs0140 =	$(CMM2003LIBDIR)mngTask3.0.a			\
		$(CMM2003LIBDIR)unionTask4.0.a			\
		$(CMM2003LIBDIR)unionLisence3.0.a			\
		unionFixedLisence.a				\
		$(CMM2003LIBDIR)unionEnv3.0.a			\
		$(CMM2003LIBDIR)UnionLog3.0.a			\
		$(CMM2003LIBDIR)unionModule1.0.a			\
		$(CMM2003LIBDIR)unionEnv3.0.a			\
		$(CMM2003LIBDIR)unionInput1.0.a			\
		$(CMM2003LIBDIR)unionPreDefinedCmd1.0.a		\
		$(CMM2003LIBDIR)unionVersion1.0.a			\
		unionProductGenerateTime.a			\
		$(CMM2003LIBDIR)UnionAlgorithm1.0.a		\
		$(CMM2003LIBDIR)unionLibName3.0.a			\
		$(CMM2003VERLIB)unionPredefinedVersion4.0.a	\
		$(CMM2003LIBDIR)UnionStr1.2.a
mngTask4.0:	$(null)
		cc -o mngTask4.0 $(libs0140)

# 02 taskMon
libs0240 =	$(CMM2003LIBDIR)taskMon3.0.a			\
		$(CMM2003LIBDIR)unionTask4.0.a				\
		$(CMM2003LIBDIR)unionLisence3.0.a			\
		unionFixedLisence.a				\
		$(CMM2003LIBDIR)unionEnv3.0.a			\
		$(CMM2003LIBDIR)UnionLog3.0.a			\
		$(CMM2003LIBDIR)unionModule1.0.a			\
		$(CMM2003LIBDIR)unionEnv3.0.a			\
		$(CMM2003LIBDIR)unionInput1.0.a			\
		$(CMM2003LIBDIR)unionPreDefinedCmd1.0.a		\
		$(CMM2003LIBDIR)unionVersion1.0.a			\
		unionProductGenerateTime.a			\
		$(CMM2003LIBDIR)UnionAlgorithm1.0.a		\
		$(CMM2003LIBDIR)unionMonitor1.0.a			\
		$(CMM2003LIBDIR)unionLibName3.0.a			\
		$(CMM2003VERLIB)unionPredefinedVersion4.0.a	\
		$(CMM2003LIBDIR)UnionStr1.2.a
taskMon4.0:	$(null)
		cc -o taskMon4.0 $(libs0240) -lcurses

binProducts:	$(null)
		cp	mngTask4.0		$(BINLIBS)
		mv	mngTask4.0		$(BINDIR)
		cp	taskMon4.0		$(BINLIBS)
		mv	taskMon4.0		$(BINDIR)

finalProducts:	$(null)
	#bins
		cp	$(BINDIR)mngTask4.0	$(CMM2003BINDIR)mngTask
		cp	$(BINDIR)taskMon4.0	$(CMM2003BINDIR)taskMon
	# rmTmpFiles
		rm	unionProductGenerateTime.a


