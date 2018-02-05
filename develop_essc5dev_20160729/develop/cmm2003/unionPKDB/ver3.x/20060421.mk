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

all:	unionPKDB3.x.NoIndex.20060421.a	\
	rmTmpFiles		\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(INCL) $(DEF) -c $*.c

# Libs 13
objs1320 =	unionPKDB3.x.NoIndex.20060421.o unionPKDBLog3.0.20060414.o unionPK3.0.20041125.o
unionPKDB3.x.NoIndex.20060421.a:	$(objs1320)
			ar $(OSSPECPACK) rv unionPKDB3.x.NoIndex.20060421.a $(objs1320)

# rmTmpFiles
rmTmpFiles:	$(null)
		rm	*.o

# finalProducts
finalProducts:	$(null)
		mv	unionPKDB3.x.NoIndex.20060421.a			$(CMM2003LIBDIR)
