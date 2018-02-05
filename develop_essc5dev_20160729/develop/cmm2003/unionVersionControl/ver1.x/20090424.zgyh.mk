# 定义开发目录
HOMEDIR = $(CMM2003DIR)/unionVersionControl/
INCDIR = $(HOMEDIR)include/

# 定义头文件目录清单
# 顺序应该是：
# 通用模块头文件目录/当前开发环境头文件目录/本工程的头文件目录

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = $(OSSPEC)

all:	makeall			\
	libProducts


.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs=	zgyh2006LibName.20080310.o
makeall:	$(objs)

libProducts:	$(null)
	mv *.o				$(CMM2003LIBDIR)
