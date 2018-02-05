# 2003/09/22	Wolfgang Wang

#	01	mngTask
#	02	taskMon
#	2003/09/22
#	bins:
#		mngTask3.2

TESTBINDIR = $(UNIONREC)/bin/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = -D _UNIX $(OSSPEC)

all:	genProductTime		\
	mngTask3.2		\
	taskMon3.2		\
	finalProducts

genProductTime:	$(null)
		genProductGenerateTime
# bins
# 01 mngTask
libs0132 =	$(CMM2003LIBDIR)mngTask3.0.a			\
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
		$(CMM2003VERLIB)unionPredefinedVersion3.2.a	\
		$(CMM2003LIBDIR)UnionStr1.2.a
mngTask3.2:	$(null)
		cc -o mngTask3.2 $(libs0132)

# 02 taskMon
libs0232 =	$(CMM2003LIBDIR)taskMon3.0.a			\
		$(CMM2003LIBDIR)unionTask3.2.a			\
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
		$(CMM2003VERLIB)unionPredefinedVersion3.2.a	\
		$(CMM2003LIBDIR)UnionStr1.2.a
taskMon3.2:	$(null)
		cc -o taskMon3.2 $(libs0232) -lcurses


finalProducts:	$(null)
	#bins
		mv	mngTask3.2	$(TESTBINDIR)mngTask
		mv	taskMon3.2	$(TESTBINDIR)taskMon
	# rmTmpFiles
		rm	unionProductGenerateTime.a


