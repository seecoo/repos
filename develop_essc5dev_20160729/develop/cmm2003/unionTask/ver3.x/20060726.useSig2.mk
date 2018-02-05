HOMEDIR = $(CMM2003DIR)/unionTask/
INCDIR = $(HOMEDIR)include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = -D _UNIX $(OSSPEC) -D _useSigUser2_

all:	makeall				\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

# 01 UnionTask
objs = 	unionTask.20060726.o	\
	unionProc1.0.o
makeall:	$(objs)

finalProducts:	$(null)
		cp	$(INCDIR)UnionTask.h		$(CMM2003INCDIR)
		mv	unionTask.20060726.o		unionTask.20060726.useSig2.o
		mv	*.o				$(CMM2003LIBDIR)
