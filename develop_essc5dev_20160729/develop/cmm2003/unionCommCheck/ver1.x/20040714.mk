# 定义开发目录
HOMEDIR = $(CMM2003DIR)/unionCommCheck
INCDIR = $(HOMEDIR)/include/

# 定义头文件目录清单
# 顺序应该是：
# 通用模块头文件目录/当前开发环境头文件目录/本工程的头文件目录

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

DEFINES=$(OSSPEC)

all:	commCheck.20040714.a			\
	commCheckMon.20040714.a			\
	commCheckMain.20040714.a		\
	LibProducts				\
	rmobjs

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c


LibProducts:		$(null)
			mv commCheck.20040714.a		$(CMM2003LIBDIR)
			mv commCheckMon.20040714.a	$(CMM2003LIBDIR)
			mv commCheckMain.20040714.a	$(CMM2003LIBDIR)
			cp $(INCDIR)commCheck.h		$(CMM2003INCDIR)

rmobjs:			$(null)
			rm *.o

obj0110 = commCheck.20040714.o
commCheck.20040714.a:	$(obj0110)
			ar $(OSSPECPACK) rv commCheck.20040714.a $(obj0110)

obj0210 = commCheckMon.20040714.o
commCheckMon.20040714.a:	$(obj0210)
			ar $(OSSPECPACK) rv commCheckMon.20040714.a $(obj0210)

obj0310 = commCheckMain.20040714.o
commCheckMain.20040714.a:	$(obj0310)
			ar $(OSSPECPACK) rv commCheckMain.20040714.a $(obj0310)

