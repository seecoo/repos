HOMEDIR = $(CMM2003DIR)/unionSJL06/
INCDIR = $(HOMEDIR)include/
INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

# ������뿪�ر�������
DEFINES = -D _UnionSJL06_2_x_Above_ $(OSSPEC) -D _spyHsmCmd_

all:	makeall				\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c


objs=sjl06Protocol2.x.20060808.o
makeall:	$(objs)

finalProducts:	$(null)
		cp $(INCDIR)sjl06Protocol.h		$(CMM2003INCDIR)
		mv sjl06Protocol2.x.20060808.o	sjl06Protocol2.x.spyHsmCmd.20060808.o
		mv *.o					$(CMM2003LIBDIR)

