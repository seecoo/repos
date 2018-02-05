HOMEDIR = $(CMM2003DIR)/unionSJL06/
INCDIR = $(HOMEDIR)include/
INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

# 这个编译开关必须增加
DEFINES = -D _UnionSJL06_2_x_Above_ $(OSSPEC)

all:	sjl06Protocol2.0.a		\
	rmTmpFiles			\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c


# Libs
# 模块02 sjl06Protocol
objs0210 = sjl06Protocol1.0.o
sjl06Protocol2.0.a:	$(objs0210)	
			ar $(OSSPECPACK) rv sjl06Protocol2.0.a 	$(objs0210)

rmTmpFiles:	$(null)
	# 拷贝目标文件
		rm *.o

finalProducts:	$(null)
	#includes
		cp $(INCDIR)sjl06Protocol.h		$(CMM2003INCDIR)
	#bins
		mv sjl06Protocol2.0.a			$(CMM2003LIBDIR)

