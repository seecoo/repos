# 定义通用模块目录
CMM2003DIR = $(UNIONLIBDIR)
CMM2003LIBDIR = $(CMM2003DIR)/lib/
CMM2003INCDIR = $(CMM2003DIR)/include

# 定义产品模块目录
PROLIBDIR=$(UNIONLIBDIR)/libLibs/
PROINCDIR=$(UNIONLIBDIR)/includeLibs/

# 定义开发目录
HOMEDIR = $(UNIONLIBDIR)/unionReport
INCDIR = $(HOMEDIR)include/

# 定义头文件目录清单
# 顺序应该是：
# 通用模块头文件目录/当前开发环境头文件目录/本工程的头文件目录

INCL = -I $(INCDIR) -I $(PROINCDIR) -I $(CMM2003INCDIR)

all:	makeall			\
	libProducts

DEFINES = $(OSSPEC)

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

libProducts:	$(null)
	mv *.o						$(PROLIBDIR)
	cp $(INCDIR)unionReportModule.h			$(PROINCDIR)


objs=	unionReportModule.1.0.o

makeall:	$(objs)

