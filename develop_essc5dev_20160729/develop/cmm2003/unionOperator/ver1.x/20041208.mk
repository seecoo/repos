#	Wolfgang Wang
#	2004/11/20


DEF = -O -DUNIX  $(OSSPEC)
HOMEDIR = $(CMM2003DIR)/unionOperator/
INCDIR = $(HOMEDIR)include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR) 

all:	unionOperator.20041208.a		\
	mngOperator.20041208.a			\
	finalProducts				\
	rmTmpFiles

.SUFFIXES:.c.o
.c.o:
	$(CC) $(INCL) $(DEF) -c $*.c

# Libs 01
objs0220 =	mngOperator.20041208.o
mngOperator.20041208.a:	$(objs0220)
			ar $(OSSPECPACK) rv mngOperator.20041208.a $(objs0220)

objs0420 =	unionOperator.20041208.o
unionOperator.20041208.a:	$(objs0420)
			ar $(OSSPECPACK) rv unionOperator.20041208.a $(objs0420)

# rmTmpFiles
rmTmpFiles:	$(null)
		rm	*.o

# finalProducts
finalProducts:	$(null)
	# libs
		mv	mngOperator.20041208.a			$(CMM2003LIBDIR)
		mv	unionOperator.20041208.a		$(CMM2003LIBDIR)
	# includes
		cp	$(INCDIR)unionOperator.h		$(CMM2003INCDIR)
