#	2001/09/19£¬Wolfgang Wang
#		01	unionREC
#		02	mngREC
#		03	recMon

HOMEDIR = $(CMM2003DIR)/unionREC
INCDIR = $(HOMEDIR)/include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = -D _UNIX $(OSSPEC)

all:	unionREC.20050916.a	\
	finalProducts	\
	rmTmpFiles

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs0110 = unionREC.20050916.o
unionREC.20050916.a:	$(objs0110)	
		ar $(OSSPECPACK) rv unionREC.20050916.a $(objs0110)

finalProducts:	$(null)
# includes
		cp $(INCDIR)/unionREC.h		$(CMM2003INCDIR)
# libs
		mv unionREC.20050916.a		$(CMM2003LIBDIR)
		mv *.o				$(CMM2003LIBDIR)

rmTmpFiles:	$(null)	
#		rm *.o

