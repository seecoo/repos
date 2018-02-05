#	Wolfgang Wang
#	2004/11/11

#	Libs
#		01	unionPK
#		13	unionPKDB
#		14	mngPKDB
#		14	mngPKDBLog

DEF = $(OSSPEC) -D _KDBClient_
HOMEDIR = $(CMM2003DIR)/unionPKDB/
INCDIR = $(HOMEDIR)include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

all:	unionPKDB3.0.client.20060414.a	\
	rmTmpFiles		\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(INCL) $(DEF) -c $*.c

# Libs 13
objs1320 =	unionPKDB3.0.20041125.o unionPKDBLog3.0.20060414.o unionPK3.0.20041125.o
unionPKDB3.0.client.20060414.a:	$(objs1320)
			ar $(OSSPECPACK) rv unionPKDB3.0.client.20060414.a $(objs1320)

# rmTmpFiles
rmTmpFiles:	$(null)
		rm	*.o

# finalProducts
finalProducts:	$(null)
		mv	unionPKDB3.0.client.20060414.a			$(CMM2003LIBDIR)
