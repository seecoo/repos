#	Wolfgang Wang
#	2004/11/11

#	Libs
#		01	unionPK
#		13	unionPKDB
#		14	mngPKDB
#		14	mngPKDBLog

DEF = -O -DUNIX $(OSSPEC)
HOMEDIR = $(CMM2003DIR)/unionPKDB/
INCDIR = $(HOMEDIR)include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

all:	unionPK.20041111.a	\
	unionPKDB.20041111.a	\
	mngPKDB.20041111.a	\
	mngPKDBLog.20041111.a		\
	rmTmpFiles		\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(INCL) $(DEF) -c $*.c

# Libs 01
objs0120 =	unionPK.20041111.o
unionPK.20041111.a:	$(objs0120)
			ar $(OSSPECPACK) rv unionPK.20041111.a $(objs0120)

# Libs 13
objs1320 =	unionPKDB.20041111.o unionPKDBLog.20041111.o
unionPKDB.20041111.a:	$(objs1320)
			ar $(OSSPECPACK) rv unionPKDB.20041111.a $(objs1320)

# Libs 14
objs1420 =	mngPKDB.20041111.o
mngPKDB.20041111.a:	$(objs1420)
			ar $(OSSPECPACK) rv mngPKDB.20041111.a $(objs1420)

# Libs 15
objs1510 =	mngPKDBLog.20041111.o
mngPKDBLog.20041111.a:	$(objs1510)
			ar $(OSSPECPACK) rv mngPKDBLog.20041111.a $(objs1510)

# rmTmpFiles
rmTmpFiles:	$(null)
		rm	*.o

# finalProducts
finalProducts:	$(null)
	# libs
		mv	unionPK.20041111.a			$(CMM2003LIBDIR)
		mv	unionPKDB.20041111.a			$(CMM2003LIBDIR)
		mv	mngPKDB.20041111.a			$(CMM2003LIBDIR)
		mv	mngPKDBLog.20041111.a			$(CMM2003LIBDIR)
	# includes
		cp	$(INCDIR)unionPK.h			$(CMM2003INCDIR)
		cp	$(INCDIR)unionPKDB.h			$(CMM2003INCDIR)
		cp	$(INCDIR)unionPKDBLog.h			$(CMM2003INCDIR)
