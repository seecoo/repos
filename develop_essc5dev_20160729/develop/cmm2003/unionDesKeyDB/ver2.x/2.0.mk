#	Wolfgang Wang
#	2003/09/24

#	Libs
#		01	unionDesKey
#		13	unionDesKeyDB
#		14	mngDesKeyDB
#		14	mngDesKeyDBLog

DEF = -O -DUNIX  $(OSSPEC)
HOMEDIR = $(CMM2003DIR)/unionDesKeyDB/
INCDIR = $(HOMEDIR)include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

all:	unionDesKey2.0.a	\
	unionDesKeyDB2.0.a	\
	mngDesKeyDB2.0.a	\
	mngDesKeyDBLog1.0.a	\
	rmTmpFiles		\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(INCL) $(DEF) -c $*.c

# Libs 01
objs0120 =	unionDesKey2.0.o unionDesKeyDBLog1.0.o
unionDesKey2.0.a:	$(objs0120)
			ar $(OSSPECPACK) rv unionDesKey2.0.a $(objs0120)

# Libs 13
objs1320 =	unionDesKeyDB2.0.o
unionDesKeyDB2.0.a:	$(objs1320)
			ar $(OSSPECPACK) rv unionDesKeyDB2.0.a $(objs1320)

# Libs 14
objs1420 =	mngDesKeyDB2.0.o
mngDesKeyDB2.0.a:	$(objs1420)
			ar $(OSSPECPACK) rv mngDesKeyDB2.0.a $(objs1420)

# Libs 15
objs1510 =	mngDesKeyDBLog1.0.o
mngDesKeyDBLog1.0.a:	$(objs1510)
			ar $(OSSPECPACK) rv mngDesKeyDBLog1.0.a $(objs1510)

# rmTmpFiles
rmTmpFiles:	$(null)
		rm	*.o

# finalProducts
finalProducts:	$(null)
	# libs
		mv	unionDesKey2.0.a				$(CMM2003LIBDIR)
		mv	unionDesKeyDB2.0.a				$(CMM2003LIBDIR)
		mv	mngDesKeyDB2.0.a				$(CMM2003LIBDIR)
		mv	mngDesKeyDBLog1.0.a				$(CMM2003LIBDIR)
	# includes
		cp	$(INCDIR)unionDesKey.h				$(CMM2003INCDIR)
		cp	$(INCDIR)unionDesKeyDB.h			$(CMM2003INCDIR)
		cp	$(INCDIR)unionDesKeyDBLog.h			$(CMM2003INCDIR)
