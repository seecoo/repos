#	2008/12/30£¬chenliang
#		unionComplexDBAPIOnUnionEnv.20081230

HOMEDIR = $(CMM2003DIR)/unionEnv
INCDIR = $(HOMEDIR)/include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = -D _UNIX  $(OSSPEC) -D _MaxVarNum_4096_ -D _UnionEnv_3_x_ -D _LargeEnvVar_

all:	makeall	\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs0130 = unioncomplexDBAPIOnUnionEnv.1.0.o	\
	unioncomplexDBAPIOnUnionEnv.2.0.o
makeall:	$(objs0130)

finalProducts:	$(null)
# includes
		cp $(INCDIR)/UnionEnv.h		$(CMM2003INCDIR)
# libs
		mv *.o				$(CMM2003LIBDIR)

