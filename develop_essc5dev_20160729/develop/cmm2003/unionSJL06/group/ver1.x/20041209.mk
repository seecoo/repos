HOMEDIR = $(CMM2003DIR)/unionSJL06/
INCDIR = $(HOMEDIR)include/
INCL =  -I $(INCDIR) -I $(CMM2003INCDIR)

# 这个编译开关必须增加
DEFINES = -D _UnionSJL06_2_x_Above_ $(OSSPEC)

all:	sjl06Grp2.0.20041209.a		\
	rmTmpFiles		\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(INCL) $(DEFINES) -c $*.c


# 模块03 sjl06Grp
objs0310 = sjl06Grp2.0.20041209.o
sjl06Grp2.0.20041209.a:	$(objs0310)	
			ar $(OSSPECPACK) rv sjl06Grp2.0.20041209.a 	$(objs0310)

rmTmpFiles:	$(null)
	# 拷贝目标文件
		rm *.o

finalProducts:	$(null)
	#includes
		cp $(INCDIR)sjl06Grp.h		$(CMM2003INCDIR)
	#libs
		mv sjl06Grp2.0.20041209.a	$(CMM2003LIBDIR)
		
		
