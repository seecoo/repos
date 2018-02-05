#	2003/09/19£¬Wolfgang Wang
#	Libs
#		01	unionModule
#		02	mngModule
#	2003/09/23
#	Libs
#		unionModule2.0.20050915.a
#		mngModule2.0.20050915.a

HOMEDIR = $(CMM2003DIR)/unionModule
INCDIR = $(HOMEDIR)/include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = -D _UNIX  $(OSSPEC)

all:	unionModule2.0.20050915.a	\
	mngModule2.0.20050915.a		\
	finalProducts		\
	rmTmpFiles

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs0110 = unionModule2.0.20050915.o
unionModule2.0.20050915.a:	$(objs0110)	
		ar $(OSSPECPACK) rv unionModule2.0.20050915.a $(objs0110)

objs0210 = mngModule2.0.20050915.o
mngModule2.0.20050915.a:	$(objs0210)	
		ar $(OSSPECPACK) rv mngModule2.0.20050915.a $(objs0210)

finalProducts:	$(null)
	# includes
		cp $(INCDIR)/unionModule.h	$(CMM2003INCDIR)
	# libs
		mv mngModule2.0.20050915.a		$(CMM2003LIBDIR)
		mv unionModule2.0.20050915.a		$(CMM2003LIBDIR)

rmTmpFiles:	$(null)	
		rm *.o

