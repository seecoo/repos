# 定义开发目录
HOMEDIR = $(CMM2003DIR)/unionFileTransfer/
INCDIR = $(HOMEDIR)/include/
OBJDIR = $(CMM2003LIBDIR)

# 定义头文件目录清单
# 顺序应该是：
# 通用模块头文件目录/当前开发环境头文件目录/本工程的头文件目录

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)	

#传给脚本的参数
SLP1 = "$*.o"	"$@ $*.o"	$@
# 这个编译开关必须增加
DEFINES = $(OSSPEC) -D _UnionSocket_3_x_ 
SPEC_DEFINES = $(OSSPEC) -D _UnionSocket_3_x_ -D _unionFileTransfer_2_x_

COMPILE	= $(CC) $(DEFINES) $(INCL) -c
SPEC_COMPILE	= $(CC) $(SPEC_DEFINES) $(INCL) -c
DEPEND	= $(CC) $(INCL) -MM -c

VPATH	= $(OBJDIR)

srcs	= $(wildcard *.c)
deps =	$(patsubst %.c,%.d,$(srcs)) 

objs = 	unionFileTransfer.20040723.o    \
	unionFileSender.20040723.o	\
	unionFileReceiver.20040723.o	\
	unionTmpFileDir.20040723.o	\
	unionFileTransfer.2.0.o \
	unionFileSender.2.0.o   \
        unionFileReceiver.2.0.o		\
	unionIO.20120521.o

all:	makeall

makeall:	$(objs)

.SUFFIXES:.c.o
%.o:%.c
	@echo "CC $(DEFINES) $(OBJDIR)$@	$<"
	@$(COMPILE) $< -o $(OBJDIR)$@

$(OBJDIR)%.d:%.c
	@echo "DEP $@	$<"
	@$(DEPEND) $< > $@
	@sh file_substr.x $(SLP1)
	
ifeq ($(UNION_USE_GCC),1)
unionFileTransfer.2.0.o:	unionFileTransfer.20040723.c unionFileTransfer.20040723.d
else
unionFileTransfer.2.0.o:	unionFileTransfer.20040723.c
endif
	@echo "CC $(SPEC_DEFINES) $(OBJDIR)$@	$<"
	@$(SPEC_COMPILE) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
unionFileSender.2.0.o:	unionFileSender.20040723.c unionFileSender.20040723.d
else
unionFileSender.2.0.o:	unionFileSender.20040723.c
endif
	@echo "CC $(SPEC_DEFINES) $(OBJDIR)$@	$<"
	@$(SPEC_COMPILE) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
unionFileReceiver.2.0.o:	unionFileReceiver.20040723.c unionFileReceiver.20040723.d
else
unionFileReceiver.2.0.o:	unionFileReceiver.20040723.c
endif
	@echo "CC $(SPEC_DEFINES) $(OBJDIR)$@	$<"
	@$(SPEC_COMPILE) $< -o $(OBJDIR)$@

ifeq ($(filter $(MAKECMDGOALS),clean install show),) 
ifeq ($(UNION_USE_GCC),1)
sinclude $(addprefix $(OBJDIR),$(deps))
endif
endif
	
includes =      $(INCDIR)unionFileTransfer.h	\
                $(INCDIR)UnionIO.h
.PHONY: clean install
install:
	@sh cp_head.x  "$(includes)" $(CMM2003INCDIR)

clean:
	$(RM) $(addprefix $(OBJDIR),$(objs)) $(addprefix $(OBJDIR),$(deps))

show:
	make -v
	@echo '=============================================================='
	@echo 'srcs	:' $(srcs)
	@echo 'objs	:' $(objs)
	@echo 'deps	:' $(deps)
	@echo 'INCL	:' $(INCL)
	@echo 'COMPILE	:' $(COMPILE)
	@echo 'DEPEND	:' $(DEPEND)
	
