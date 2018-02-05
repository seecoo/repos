HOMEDIR = $(CMM2003DIR)/unionSJL06/
INCDIR = $(HOMEDIR)include/
INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

# ������뿪�ر�������
DEFINES = -D _UnionSJL06_2_x_Above_ $(OSSPEC)


all:	makeall		\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs = racalRsaCmd.20070828.o
makeall:	$(objs)

finalProducts:	$(null)
	#include
		cp $(INCDIR)racalRsaCmd.h	$(CMM2003INCDIR)
	#libs
		mv *.o				$(CMM2003LIBDIR)
