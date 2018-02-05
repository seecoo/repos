#	UnionEnv2.0	2003/09/12£¬Wolfgang Wang
HOMEDIR = $(CMM2003DIR)/unionEnv
INCDIR = $(HOMEDIR)/include/

INCL = -I $(HOMEDIR)/include
DEFINES = -D _UNIX  $(OSSPEC)

all:	UnionEnv2.0.a	\
	finalProducts	\
	rmTmpFiles

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs0120 = UnionEnv2.0.o
UnionEnv2.0.a:	$(objs0120)	
		ar $(OSSPECPACK) rv UnionEnv2.0.a $(objs0120)

finalProducts:	$(null)
# includes
		cp $(INCDIR)/UnionEnv.h		$(CMM2003INCDIR)
# libs
		mv $(LIBDIR)UnionEnv2.0.a	$(CMM2003LIBDIR)
		
rmTmpFiles:	$(null)	
		rm *.o

