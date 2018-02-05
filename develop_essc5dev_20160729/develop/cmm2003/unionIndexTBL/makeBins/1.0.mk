#	2003/09/19	Wolfgang Wang
#	Version		3.0

#	bins
#		01	mngIndexTBL

#	2003/09/23
#	bins
#		mngIndexTBL2.0
#		msgBufMon1.0

HOMEDIR = $(HOME)/unionIndexTBL
BINDIR = $(HOMEDIR)/bin/
LIBDIR = $(HOMEDIR)/lib/
INCDIR = $(HOMEDIR)/include/
OBJDIR = $(HOMEDIR)/obj/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = -D _UNIX

all:	genProductTime		\
	mngIndexTBL1.0		\
	binProduct		\
	finalProduct

genProductTime:	$(null)
		genProductGenerateTime

libs0110 =	$(CMM2003LIBDIR)mngIndexTBL1.0.a			\
		$(CMM2003LIBDIR)unionIndexTBL1.0.a			\
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
mngIndexTBL1.0:	$(null)
		cc -o mngIndexTBL1.0 $(libs0110)

binProduct:	$(null)
		mv mngIndexTBL1.0				$(CMM2003BINDIR)

finalProduct:	$(null)
	# bins
		cp $(BINDIR)mngIndexTBL1.0		$(CMM2003BINDIR)mngIndexTBL
	# rmTmpFiles
		rm *.a

