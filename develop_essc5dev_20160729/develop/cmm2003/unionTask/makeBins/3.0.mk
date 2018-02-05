# 2003/09/22	Wolfgang Wang

#	01	mngTask
#	02	taskMon

#	2003/09/22
#	bins:
#		mngTask3.0

BINLIBS = $(UNIONLIBDIR)/binLibs

HOMEDIR = $(HOME)/unionTask/
LIBDIR = $(HOMEDIR)lib/
BINDIR = $(HOMEDIR)bin/
INCDIR = $(HOMEDIR)include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = -D _UNIX $(OSSPEC)

all:	genProductTime		\
	mngTask3.0		\
	taskMon3.0		\
	binProducts		\
	finalProducts

genProductTime:	$(null)
		genProductGenerateTime
# bins
# 01 mngTask
libs0130 =	$(CMM2003LIBDIR)mngTask3.0.a			\
		$(CMM2003LIBDIR)unionTask3.0.a			\
		$(CMM2003LIBDIR)unionEnv3.0.a			\
		$(CMM2003LIBDIR)UnionLog3.0.a			\
		$(CMM2003LIBDIR)unionModule1.0.a			\
		$(CMM2003LIBDIR)unionEnv3.0.a			\
		$(CMM2003LIBDIR)unionInput1.0.a			\
		$(CMM2003LIBDIR)unionPreDefinedCmd1.0.a		\
		$(CMM2003LIBDIR)unionVersion1.0.a			\
		unionProductGenerateTime.a			\
		$(CMM2003LIBDIR)unionLibName3.0.a			\
		$(CMM2003VERLIB)unionPredefinedVersion3.0.a	\
		$(CMM2003LIBDIR)UnionStr1.2.a
mngTask3.0:	$(null)
		cc -o mngTask3.0 $(OSSPECLINK) $(libs0130)

# 02 taskMon
libs0230 =	$(CMM2003LIBDIR)taskMon3.0.a			\
		$(CMM2003LIBDIR)unionTask3.0.a			\
		$(CMM2003LIBDIR)unionEnv3.0.a			\
		$(CMM2003LIBDIR)UnionLog3.0.a			\
		$(CMM2003LIBDIR)unionModule1.0.a			\
		$(CMM2003LIBDIR)unionEnv3.0.a			\
		$(CMM2003LIBDIR)unionInput1.0.a			\
		$(CMM2003LIBDIR)unionPreDefinedCmd1.0.a		\
		$(CMM2003LIBDIR)unionVersion1.0.a			\
		unionProductGenerateTime.a			\
		$(CMM2003LIBDIR)unionMonitor1.0.a			\
		$(CMM2003LIBDIR)unionLibName3.0.a			\
		$(CMM2003VERLIB)unionPredefinedVersion3.0.a	\
		$(CMM2003LIBDIR)UnionStr1.2.a
taskMon3.0:	$(null)
		cc -o taskMon3.0 $(OSSPECLINK) $(libs0230) -lcurses

binProducts:	$(null)
		cp	mngTask3.0		$(BINLIBS)
		mv	mngTask3.0		$(BINDIR)
		cp	taskMon3.0		$(BINLIBS)
		mv	taskMon3.0		$(BINDIR)

finalProducts:	$(null)
	#bins
		cp	$(BINDIR)mngTask3.0	$(CMM2003BINDIR)mngTask
		cp	$(BINDIR)taskMon3.0	$(CMM2003BINDIR)taskMon
	# rmTmpFiles
		rm	unionProductGenerateTime.a


