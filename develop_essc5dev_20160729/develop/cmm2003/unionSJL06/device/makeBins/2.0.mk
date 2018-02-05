HOMEDIR = $(HOME)/unionSJL06/
LIBDIR = $(HOMEDIR)lib/
BINDIR = $(HOMEDIR)bin/
INCDIR = $(HOMEDIR)include/
INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

all:	genProductTime			\
	mngSJL062.0			\
	finalProducts			\
	rmTmpFiles

genProductTime:	$(null)
		genProductGenerateTime

# Bins
libs0110 = 	$(CMM2003LIBDIR)mngSJL062.0.a			\
		$(CMM2003LIBDIR)sjl062.0.a				\
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
		$(CMM2003VERLIB)unionPredefinedVersion2.0.a	\
		$(CMM2003LIBDIR)UnionStr1.2.a			\
		$(CMM2003LIBDIR)unionREC1.0.a
mngSJL062.0:	$(null)
		cc -o mngSJL062.0 $(libs0110) $(OSSPEC)

finalProducts:	$(null)
	#bins
		mv mngSJL062.0		$(CMM2003BINDIR)mngSJL06

rmTmpFiles:
		rm *.a
