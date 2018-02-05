HOMEDIR = $(CMM2003DIR)/unionSJL06/
INCDIR = $(HOMEDIR)include/
INCL =  -I $(INCDIR) -I $(CMM2003INCDIR)

# 这个编译开关必须增加
DEFINES = -D _UnionSJL06_2_x_Above_ -D _KillHsmTaskAutomatically_ $(OSSPEC)

all:	sjl06Grp2.0.20050816.killHsmTaskAutomatically.a		\
	rmTmpFiles		\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(INCL) $(DEFINES) -c $*.c


# 模块03 sjl06Grp
objs0310 = sjl06Grp2.0.20050816.o
sjl06Grp2.0.20050816.killHsmTaskAutomatically.a:	$(objs0310)	
			ar $(OSSPECPACK) rv sjl06Grp2.0.20050816.killHsmTaskAutomatically.a 	$(objs0310)

rmTmpFiles:	$(null)
	# 拷贝目标文件
		rm *.o

finalProducts:	$(null)
	#includes
		cp $(INCDIR)sjl06Grp.h		$(CMM2003INCDIR)
	#libs
		mv sjl06Grp2.0.20050816.killHsmTaskAutomatically.a	$(CMM2003LIBDIR)
		
		
