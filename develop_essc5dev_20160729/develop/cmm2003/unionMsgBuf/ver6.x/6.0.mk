# 定义开发目录
HOMEDIR = $(CMM2003DIR)/unionMsgBuf
INCDIR = $(HOMEDIR)/include/
OBJDIR = $(CMM2003LIBDIR)

# 定义头文件目录清单
# 顺序应该是：
# 通用模块头文件目录/当前开发环境头文件目录/本工程的头文件目录

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

# 这个编译开关必须增加
DEFINES = -D _UNIX  $(OSSPEC)

COMPILE = $(CC) $(DEFINES) $(INCL) -c

srcs =  $(wildcard *.c)
objs =  $(patsubst %.c,%.o,$(srcs))

all:    makeall         \
	tmp

makeall:        $(objs)

.SUFFIXES:.c.o
%.o:%.c
	@echo "CC $(DEFINES) $@ $<"
	@$(COMPILE) $< -o $@

tmp:
	cp      $(INCDIR)unionMsgBuf6.x.h       $(CMM2003INCDIR)
	cp      $(INCDIR)unionUnionMsgBuf.h     $(CMM2003INCDIR)
	mv      *.o				$(CMM2003LIBDIR)
