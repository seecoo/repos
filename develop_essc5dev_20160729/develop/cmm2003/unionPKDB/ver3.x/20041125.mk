#	Wolfgang Wang
#	2004/11/11

#	Libs
#		01	unionPK
#		13	unionPKDB
#		14	mngPKDB
#		14	mngPKDBLog

DEF = $(OSSPEC)
HOMEDIR = $(CMM2003DIR)/unionPKDB/
INCDIR = $(HOMEDIR)include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

all:	unionPK3.0.20041125.a	\
	unionPKDB3.0.20041125.a	\
	mngPKDB3.0.20041125.a	\
	mngPKDBLog3.0.20041125.a		\
	rmTmpFiles		\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(INCL) $(DEF) -c $*.c

# Libs 01
objs0120 =	unionPK3.0.20041125.o
unionPK3.0.20041125.a:	$(objs0120)
			ar $(OSSPECPACK) rv unionPK3.0.20041125.a $(objs0120)

# Libs 13
objs1320 =	unionPKDB3.0.20041125.o unionPKDBLog3.0.20041125.o unionPK3.0.20041125.o
unionPKDB3.0.20041125.a:	$(objs1320)
			ar $(OSSPECPACK) rv unionPKDB3.0.20041125.a $(objs1320)

# Libs 14
objs1420 =	mngPKDB3.0.20041125.o
mngPKDB3.0.20041125.a:	$(objs1420)
			ar $(OSSPECPACK) rv mngPKDB3.0.20041125.a $(objs1420)

# Libs 15
objs1510 =	mngPKDBLog3.0.20041125.o
mngPKDBLog3.0.20041125.a:	$(objs1510)
			ar $(OSSPECPACK) rv mngPKDBLog3.0.20041125.a $(objs1510)

# rmTmpFiles
rmTmpFiles:	$(null)
		rm	*.o

# finalProducts
finalProducts:	$(null)
	# libs
		mv	unionPK3.0.20041125.a			$(CMM2003LIBDIR)
		mv	unionPKDB3.0.20041125.a			$(CMM2003LIBDIR)
		mv	mngPKDB3.0.20041125.a			$(CMM2003LIBDIR)
		mv	mngPKDBLog3.0.20041125.a		$(CMM2003LIBDIR)
	# includes
		cp	$(INCDIR)unionPK.h			$(CMM2003INCDIR)
		cp	$(INCDIR)unionPKDB.h			$(CMM2003INCDIR)
		cp	$(INCDIR)unionPKDBLog.h			$(CMM2003INCDIR)
