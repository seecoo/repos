# 定义开发目录
HOMEDIR = $(CMM2003DIR)/unionVarType/
INCDIR = $(HOMEDIR)include/

# 定义头文件目录清单
# 顺序应该是：
# 通用模块头文件目录/当前开发环境头文件目录/本工程的头文件目录

INCL =  -I $(INCDIR) -I $(CMM2003INCDIR)

all:	makeall			\
	mvLibs			\
	libProducts
	

DEFINES = $(OSSPEC)

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

mvLibs:	$(null)
	cp *.o							$(CMM2003LIBDIR)
libProducts:	$(null)
	mv *.o							$(CMM2003LIBDIR)
	cp $(INCDIR)/unionTree.h				$(CMM2003INCDIR)
	cp $(INCDIR)/unionUserInterfaceItemGrp.h		$(CMM2003INCDIR)
	cp $(INCDIR)/unionUserInterfaceMaintainer.h		$(CMM2003INCDIR)
	cp $(INCDIR)/unionQueueDef.h				$(CMM2003INCDIR)
	

objs =	unionUserInterfaceMaintainer.1.0.o	\
	unionUserInterfaceItemGrp.1.0.o	\
	unionQueueDef.1.0.o	\
	unionTree.1.0.o
makeall:	$(objs)

