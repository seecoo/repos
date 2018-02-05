HOMEDIR = $(CMM2003DIR)/unionSJL06/
INCDIR = $(HOMEDIR)include/
INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

# 这个编译开关必须增加
DEFINES = -D _UnionSJL06_2_x_Above_ $(OSSPEC)


all:	makeall		\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs = racalSM2Cmd.20111117.o
makeall:	$(objs)

finalProducts:	$(null)
	#include
		cp $(INCDIR)racalSM2Cmd.h	$(CMM2003INCDIR)
	#libs
		mv *.o				$(CMM2003LIBDIR)
