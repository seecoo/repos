#	Wolfgang Wang
#	2004/11/20


DEF = -O -DUNIX  $(OSSPEC)
HOMEDIR = $(CMM2003DIR)/unionDesKeyDB/
INCDIR = $(HOMEDIR)include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

all:	unionDesKey3.0.20041125.a		\
	unionDesKeyDB3.0.20041125.a		\
	mngDesKeyDB3.0.20041125.a		\
	mngDesKeyDBLog3.0.20041125.a		\
	finalProducts				\
	rmTmpFiles

.SUFFIXES:.c.o
.c.o:
	$(CC) $(INCL) $(DEF) -c $*.c

# Libs 01
objs0220 =	mngDesKeyDB3.0.20041125.o
mngDesKeyDB3.0.20041125.a:	$(objs0220)
			ar $(OSSPECPACK) rv mngDesKeyDB3.0.20041125.a $(objs0220)

# Libs 01
objs1220 =	mngDesKeyDBLog3.0.20041125.o
mngDesKeyDBLog3.0.20041125.a:	$(objs1220)
			ar $(OSSPECPACK) rv mngDesKeyDBLog3.0.20041125.a $(objs1220)

objs0420 =	unionDesKeyDB3.0.20041125.o unionDesKeyDBLog3.0.20041125.o unionDesKey3.0.20041125.o
unionDesKeyDB3.0.20041125.a:	$(objs0420)
			ar $(OSSPECPACK) rv unionDesKeyDB3.0.20041125.a $(objs0420)

objs0520 =	unionDesKey3.0.20041125.o
unionDesKey3.0.20041125.a:	$(objs0420)
			ar $(OSSPECPACK) rv unionDesKey3.0.20041125.a $(objs0520)

# rmTmpFiles
rmTmpFiles:	$(null)
		rm	*.o

# finalProducts
finalProducts:	$(null)
	# libs
		mv	mngDesKeyDB3.0.20041125.a		$(CMM2003LIBDIR)
		mv	mngDesKeyDBLog3.0.20041125.a		$(CMM2003LIBDIR)
		mv	unionDesKey3.0.20041125.a		$(CMM2003LIBDIR)
		mv	unionDesKeyDB3.0.20041125.a		$(CMM2003LIBDIR)
	# includes
		cp	$(INCDIR)unionDesKeyDBLog.h		$(CMM2003INCDIR)
		cp	$(INCDIR)unionDesKeyDB.h		$(CMM2003INCDIR)
		cp	$(INCDIR)unionDesKey.h			$(CMM2003INCDIR)
