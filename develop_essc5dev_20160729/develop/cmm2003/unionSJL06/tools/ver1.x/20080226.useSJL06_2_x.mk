HOMEDIR = $(CMM2003DIR)/unionSJL06/
INCDIR = $(HOMEDIR)include/
INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

# 这个编译开关必须增加
DEFINES = -D _UnionSJL06_2_x_Above_ $(OSSPEC)


all:	makeall				\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c


allobjs=restoreVK.20080226.o
makeall:	$(allobjs)

rmTmpFiles:	$(null)
		rm *.o

finalProducts:	$(null)
	mv restoreVK.20080226.o $(CMM2003LIBDIR)restoreVK.useSJL06_2_x.20080226.o
