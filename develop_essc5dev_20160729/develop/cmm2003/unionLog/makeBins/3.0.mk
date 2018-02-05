#	2003/09/19	Wolfgang Wang
#	Version		3.0

#	bins
#		01	mngUnionLog
#		02	logMon

#	2003/09/23
#	bins
#		mngUnionLog1.0
#		mngUnionLog2.0
#		logMon1.0

HOMEDIR = $(HOME)/unionLog
BINDIR = $(HOMEDIR)/bin/
LIBDIR = $(HOMEDIR)/lib/
INCDIR = $(HOMEDIR)/include/
OBJDIR = $(HOMEDIR)/obj/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = -D _UNIX

all:	genProductTime		\
	mngUnionLog2.0		\
	logMon1.0		\
	binProduct		\
	finalProduct

genProductTime:	$(null)
		genProductGenerateTime

libs0120 =	$(CMM2003LIBDIR)/mngUnionLog2.0.a			\
		$(CMM2003LIBDIR)/unionTask3.2.a			\
		$(CMM2003LIBDIR)/unionEnv3.0.a			\
		$(CMM2003LIBDIR)/UnionLog3.0.a			\
		$(CMM2003LIBDIR)/unionModule1.0.a			\
		$(CMM2003LIBDIR)/unionEnv3.0.a			\
		$(CMM2003LIBDIR)/unionInput1.0.a			\
		$(CMM2003LIBDIR)/unionPreDefinedCmd1.0.a		\
		$(CMM2003LIBDIR)/unionVersion1.0.a			\
		$(CMM2003LIBDIR)/unionLibName3.0.a			\
		unionProductGenerateTime.a			\
		$(CMM2003VERLIB)unionPredefinedVersion2.0.a	\
		$(CMM2003LIBDIR)/UnionStr1.2.a
mngUnionLog2.0:	$(null)
		cc $(OSSPEC) -o mngUnionLog2.0 $(libs0120) $(OSSPECLIBS)

libs0210 =	$(CMM2003LIBDIR)/logMon1.0.a			\
		$(CMM2003LIBDIR)/unionTask3.2.a			\
		$(CMM2003LIBDIR)/unionEnv3.0.a			\
		$(CMM2003LIBDIR)/UnionLog3.0.a			\
		$(CMM2003LIBDIR)/unionModule1.0.a			\
		$(CMM2003LIBDIR)/unionEnv3.0.a			\
		$(CMM2003LIBDIR)/unionInput1.0.a			\
		$(CMM2003LIBDIR)/unionPreDefinedCmd1.0.a		\
		$(CMM2003LIBDIR)/unionVersion1.0.a			\
		$(CMM2003LIBDIR)/unionMonitor1.0.a			\
		$(CMM2003LIBDIR)/unionLibName3.0.a			\
		unionProductGenerateTime.a			\
		$(CMM2003VERLIB)unionPredefinedVersion1.0.a	\
		$(CMM2003LIBDIR)/UnionStr1.2.a
logMon1.0:	$(null)
		cc $(OSSPEC) -o logMon1.0 $(libs0210) $(OSSPECLIBS)

binProduct:	$(null)
		mv mngUnionLog2.0			$(CMM2003BINDIR)
		mv logMon1.0				$(CMM2003BINDIR)

finalProduct:	$(null)
		rm *.a

