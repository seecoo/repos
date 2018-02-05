#	2003/09/19£¬Wolfgang Wang
#		unionEnv.20050804

HOMEDIR = $(CMM2003DIR)/unionEnv
INCDIR = $(HOMEDIR)/include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = -D _UNIX  $(OSSPEC) -D _MaxVarNum_4096_

all:	makeall	\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs0130 = unionEnv.20050804.o
makeall:	$(objs0130)

finalProducts:	$(null)
# includes
		cp $(INCDIR)/UnionEnv.h		$(CMM2003INCDIR)
# libs
		mv unionEnv.20050804.o		$(CMM2003LIBDIR)unionEnv.20050804.4096.o

