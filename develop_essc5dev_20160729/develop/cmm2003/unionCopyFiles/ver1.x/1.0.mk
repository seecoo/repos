#	2003/12/25£¬Wolfgang Wang
#		unionCopyFiles1.0

HOMEDIR = $(CMM2003DIR)/unionCopyFiles
LIBDIR = $(HOMEDIR)/lib/
INCDIR = $(HOMEDIR)/include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = -D _UNIX $(OSSPEC)

all:	unionCopyFiles1.0.a	\
	finalProducts	\
	rmTmpFiless

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs0110 = unionCopyFiles1.0.o
unionCopyFiles1.0.a:	$(objs0110)	
		ar $(OSSPECPACK) rv unionCopyFiles1.0.a $(objs0110)
finalProducts:	$(null)
	# libs
		mv unionCopyFiles1.0.a	$(CMM2003LIBDIR)

rmTmpFiless:	$(null)	
		rm *.o

