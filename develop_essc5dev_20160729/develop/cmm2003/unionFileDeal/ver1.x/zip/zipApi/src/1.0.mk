# 定义开发目录
HOMEDIR = $(CMM2003DIR)/unionFileDeal/ver1.x/zip/zipApi/
INCDIR = $(HOMEDIR)include/

# 定义头文件目录清单
# 顺序应该是：
# 通用模块头文件目录/当前开发环境头文件目录/本工程的头文件目录

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

all:	makeall			\
	libUnionZipAPI.a	\
	libProducts

DEFINES = -D UNIX $(OSSPEC)

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs=	unionZipAPI.1.0.o

makeall:	$(objs)

libUnionZipAPI.a:$(shareobjs)
	ar $(OSSPECPACK) rv libUnionZipAPI.a $(objs)
	
libProducts:	$(null)
	mv libUnionZipAPI.a				$(CMM2003LIBDIR)
	cp ../include/unionZipAPI.h			$(CMM2003INCDIR)
	rm *.o
