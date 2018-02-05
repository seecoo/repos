HOMEDIR = $(CMM2003DIR)/unionSJL06/
INCDIR = $(HOMEDIR)include/
INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

# 这个编译开关必须增加
DEFINES = -D _UnionSJL06_2_x_Above_ -D _client_use_ebcdic_ $(OSSPEC)

all:	makeall				\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c


objs=sjl06Protocol2.x.20060808.o
makeall:	$(objs)

finalProducts:	$(null)
		cp $(INCDIR)sjl06Protocol.h		$(CMM2003INCDIR)
		mv *.o					$(CMM2003LIBDIR)sjl06Protocol2.x.useEbcdic.20060808.o

