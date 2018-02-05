HOMEDIR = $(CMM2003DIR)/unionSJL06/
INCDIR = $(HOMEDIR)include/
INCL =  -I $(INCDIR) -I $(CMM2003INCDIR)

# 这个编译开关必须增加
DEFINES = -D _UnionSJL06_2_x_Above_ -D _KillHsmTaskAutomatically_ -D _nullTaskName_ $(OSSPEC)

all:	makeall			\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(INCL) $(DEFINES) -c $*.c


# 模块03 sjl06Grp
allobjs = sjl06GrpMon2.x.20060808.o	\
	  sjl06GrpMon2.x.20090421.o
makeall:	$(allobjs)

finalProducts:	$(null)
		mv sjl06GrpMon2.x.20060808.o		$(CMM2003LIBDIR)sjl06GrpMon2.x.20090424.o
		mv sjl06GrpMon2.x.20090421.o		$(CMM2003LIBDIR)sjl06GrpMon2.x.20090421.o

		
