#	Wolfgang Wang
#	2003/09/23

#	Bins
#	01	unionMenu

#	2003/09/23
#	bins
#		unionMenu2.0

BINLIBS = $(UNIONLIBDIR)/binLibs

DEF = -O -DUNIX
HOMEDIR = $(HOME)/unionMenu/
LIBDIR = $(HOMEDIR)/lib/
INCDIR = $(HOMEDIR)/include/
BINDIR = $(HOMEDIR)/bin/
OBJDIR = $(HOMEDIR)obj/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

all:	genProductTime		\
	unionMenu2.0		\
	binProducts		\
	finalProducts

genProductTime:	$(null)
		genProductGenerateTime

# Bins
libs0120 =	$(CMM2003LIBDIR)mngMenu2.0.a			\
		$(CMM2003LIBDIR)unionMenu2.0.a			\
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
		$(CMM2003LIBDIR)UnionStr1.2.a
unionMenu2.0:	$(null)
		cc -o unionMenu2.0 $(libs0120)

# binProudcts
binProducts:	$(null)
		cp	unionMenu2.0		$(BINLIBS)
		mv	unionMenu2.0		$(BINDIR)

# finalProducts
finalProducts:	$(null)
	# bins
		cp	$(BINDIR)unionMenu2.0				$(CMM2003BINDIR)unionMenu
	# rmTmpFiles
		rm *.a
