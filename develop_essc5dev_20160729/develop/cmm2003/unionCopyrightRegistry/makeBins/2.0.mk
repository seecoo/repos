#	Wolfgang Wang
#	2003/09/23

#	Bins
#	01	mngLisence

#	2003/09/23
#	bins
#		mngLisence2.0

BINLIBS = $(UNIONLIBDIR)/binLibs

DEF = -O -DUNIX
HOMEDIR = $(HOME)/unionCopyrightRegistry/
LIBDIR = $(HOMEDIR)lib/
INCDIR = $(HOMEDIR)include/
BINDIR = $(HOMEDIR)bin/
OBJDIR = $(HOMEDIR)obj/


INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

all:	genProductTime			\
	mngLisence2.0			\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(INCL) $(DEF) -c $*.c

genProductTime:	$(null)
		genProductGenerateTime

# Bins
libs0120 =	$(CMM2003LIBDIR)mngLisence2.0.a			\
		$(CMM2003LIBDIR)unionCopyrightRegistry2.0.a	\
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
		$(CMM2003LIBDIR)UnionAlgorithm1.0.a		\
		$(CMM2003LIBDIR)UnionStr1.2.a
mngLisence2.0:	$(null)
		cc $(OSSPECLINK) -o mngLisence2.0 $(libs0120) $(OSSPECLIBS)

# finalProducts
finalProducts:	$(null)
	# bins
		mv	mngLisence2.0		$(CMM2003BINDIR)mngLisence
	# rmTmpFiles
		rm *.a

