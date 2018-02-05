# 定义开发目录
HOMEDIR = $(CMM2003DIR)/unionSckClient/
LIBDIR = $(HOMEDIR)lib/
INCDIR = $(HOMEDIR)include/

# 定义头文件目录清单
# 顺序应该是：
# 通用模块头文件目录/当前开发环境头文件目录/本工程的头文件目录

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

all:	makeall			\
	libProducts

DEFINES = $(OSSPEC)

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs=	commWithTcpipSvr.useSocket.forThread.20081127.o	\
	commWithTcpipSvr.useSocket.balance.20081127.o	\
	commWithPackSvr.20081127.o			\
	centerREC.useProfile.20081127.o			\
	defaultClientErrCodeTranslater.20081127.o	\
	centerREC.20081127.o				\
	unionCommWithEsscSvr.1.0.o			\
	unionCenterREC.useprofile.1.0.o			\
	UnionAPIForEssc.1.0.o
	
makeall:	$(objs)

libProducts:	$(null)
	mv *.o					$(CMM2003LIBDIR)
	cp $(INCDIR)/commWithPackSvr.h		$(CMM2003INCDIR)
	cp $(INCDIR)/commWithTcpipSvr.h		$(CMM2003INCDIR)
	cp $(INCDIR)/centerREC.h		$(CMM2003INCDIR)
	cp $(INCDIR)/UnionCenterREC.h		$(CMM2003INCDIR)
	
