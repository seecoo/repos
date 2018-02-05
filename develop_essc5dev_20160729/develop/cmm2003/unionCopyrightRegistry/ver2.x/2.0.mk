#	Wolfgang Wang
#	2003/09/23

#	Libs
#	12	unionCopyrightRegistry
#	13	mngLisence

#	2003/09/23
#	libs	
#		unionCopyrightRegistry2.0.a
#		mngLisence2.0.a

DEFINES = -O -DUNIX  $(OSSPEC)
HOMEDIR = $(CMM2003DIR)/unionCopyrightRegistry/
INCDIR = $(HOMEDIR)include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

all:	unionCopyrightRegistry2.0.a	\
	mngLisence2.0.a			\
	rmTmpFiles			\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(INCL) $(DEFINES) -c $*.c

# Libs 12
objs1220 =	unionCopyrightRegistry2.0.o
unionCopyrightRegistry2.0.a:	$(objs1220)
			ar $(OSSPECPACK) rv unionCopyrightRegistry2.0.a $(objs1220)

# Libs 13
objs1320 =	mngLisence2.0.o
mngLisence2.0.a:	$(objs1320)
			ar $(OSSPECPACK) rv mngLisence2.0.a $(objs1320)

# rmTmpFiles
rmTmpFiles:	$(null)
		rm	*.o

# finalProducts
finalProducts:	$(null)
	# includes
		cp	$(INCDIR)unionCopyrightRegistry.h	$(CMM2003INCDIR)
	# libs
		mv	unionCopyrightRegistry2.0.a		$(CMM2003LIBDIR)
		mv	mngLisence2.0.a				$(CMM2003LIBDIR)
