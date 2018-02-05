#	2003/09/19£¬Wolfgang Wang
#		unionEnv.20050804

HOMEDIR = $(CMM2003DIR)/unionEnv
INCDIR = $(HOMEDIR)/include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = -D _UNIX  $(OSSPEC)

all:	unionEnv.20050804.a	\
	finalProducts	\
	rmTmpFiles

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs0130 = unionEnv.20050804.o
unionEnv.20050804.a:	$(objs0130)	
		ar $(OSSPECPACK) rv unionEnv.20050804.a $(objs0130)

finalProducts:	$(null)
# includes
		cp $(INCDIR)/UnionEnv.h		$(CMM2003INCDIR)
# libs
		mv $(LIBDIR)unionEnv.20050804.a	$(CMM2003LIBDIR)

rmTmpFiles:	$(null)	
		rm *.o

