#	2003/09/19£¬Wolfgang Wang
#		unionEnv3.0

HOMEDIR = $(CMM2003DIR)/unionEnv
INCDIR = $(HOMEDIR)/include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = -D _UNIX  $(OSSPEC)

all:	unionEnv3.0.a	\
	finalProducts	\
	rmTmpFiles

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs0130 = unionEnv3.0.o
unionEnv3.0.a:	$(objs0130)	
		ar $(OSSPECPACK) rv unionEnv3.0.a $(objs0130)

finalProducts:	$(null)
# includes
		cp $(INCDIR)/UnionEnv.h		$(CMM2003INCDIR)
# libs
		mv $(LIBDIR)unionEnv3.0.a	$(CMM2003LIBDIR)
		mv *.o				$(CMM2003LIBDIR)

rmTmpFiles:	$(null)	

