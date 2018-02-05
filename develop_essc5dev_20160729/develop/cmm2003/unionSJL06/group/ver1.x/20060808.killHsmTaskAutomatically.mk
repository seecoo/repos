HOMEDIR = $(CMM2003DIR)/unionSJL06/
INCDIR = $(HOMEDIR)include/
INCL =  -I $(INCDIR) -I $(CMM2003INCDIR)

# 这个编译开关必须增加
DEFINES = -D _UnionSJL06_2_x_Above_ -D _KillHsmTaskAutomatically_ $(OSSPEC)

all:	makeall			\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(INCL) $(DEFINES) -c $*.c


# 模块03 sjl06Grp
allobjs = sjl06Grp2.x.20060808.o	\
	sjl06GrpMon2.x.20060808.o
makeall:	$(allobjs)

finalProducts:	$(null)
		cp $(INCDIR)sjl06Grp.h		$(CMM2003INCDIR)
		mv sjl06Grp2.x.20060808.o	sjl06Grp2.x.20060808.o.killHsmTaskAutomatically.o
		mv *.o				$(CMM2003LIBDIR)

		
