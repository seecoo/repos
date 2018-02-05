#	Wolfgang Wang
#	2003/09/10
#	Version1.0

#	Bins
#	01	mngKeySpace
#	02	mngSharedKeySpace

#BINLIBS = $(UNIONLIBDIR)/binLibs
BINLIBS=$(HOME)/bin/

DEF = -O -DUNIX $(OSSPEC)
HOMEDIR = $(HOME)/unionSJL06/
CMM2003LIBDIR = $(HOMEDIR)lib/
INCDIR = $(HOMEDIR)include/
BINDIR = $(HOMEDIR)bin/
OBJDIR = $(HOMEDIR)obj/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

all:	genProductTime			\
	mngKeySpace1.0			\
	mngSharedKeySpace1.0		\
	binProducts			\
	rmTmpFiles

genProductTime:	$(null)
		genProductGenerateTime

# Bins 01
libs0110 =	$(CMM2003LIBDIR)mngKeySpace.20041206.a		\
		$(CMM2003LIBDIR)sjl06KeySpace1.0.a			\
		$(CMM2003LIBDIR)sjl06SharedKeySpace1.0.a		\
		$(CMM2003LIBDIR)unionTask3.2.a			\
		$(CMM2003LIBDIR)unionEnv3.0.a			\
		$(CMM2003LIBDIR)UnionLog3.0.a			\
		$(CMM2003LIBDIR)unionModule1.0.a			\
		$(CMM2003LIBDIR)unionEnv3.0.a			\
		$(CMM2003LIBDIR)unionInput1.0.a			\
		$(CMM2003LIBDIR)unionPreDefinedCmd1.0.a		\
		$(CMM2003LIBDIR)unionVersion1.0.a			\
		$(CMM2003LIBDIR)sjl06LMK1.0.a			\
		unionProductGenerateTime.a			\
		$(CMM2003LIBDIR)unionLibName3.0.a			\
		$(CMM2003VERLIB)unionPredefinedVersion1.0.a	\
		$(CMM2003LIBDIR)UnionStr1.2.a
mngKeySpace1.0:	$(null)
		cc -o mngKeySpace1.0 $(libs0110)

# Bins 02
libs0210 =	$(CMM2003LIBDIR)mngSharedKeySpace1.0.a			\
		$(CMM2003LIBDIR)sjl06KeySpace1.0.a			\
		$(CMM2003LIBDIR)sjl06SharedKeySpace1.0.a		\
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
mngSharedKeySpace1.0:	$(null)
		cc -o mngSharedKeySpace1.0 $(libs0210)

# binProducts
binProducts:	$(null)
		mv	mngKeySpace1.0			$(CMM2003BINDIR)mngKeySpace
		mv	mngSharedKeySpace1.0		$(CMM2003BINDIR)mngSharedKeySpace
rmTmpFiles:
		rm *.a

