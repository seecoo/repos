# 定义开发目录
HOMEDIR = $(CMM2003DIR)/unionAlgorithm/
INCDIR = $(HOMEDIR)/include/
OBJDIR = $(CMM2003LIBDIR)
OPENSOURCEDIR = $(CMM2003OPENDIR)/include/

# 定义头文件目录清单
# 顺序应该是：
# 通用模块头文件目录/当前开发环境头文件目录/本工程的头文件目录

INCL = -I $(INCDIR) -I $(CMM2003INCDIR) -I $(OPENSOURCEDIR)

#传给脚本的参数
SLP1 = "$*.o"	"$@ $*.o $*.a"	$@

# 当操作系统是是高字节在前时，打开该编译开关。
# 浦发HP-UNIX即是这样的系统
# DEFINES = -D HIGHFIRST $(OSSPEC)
DEFINES = $(OSSPEC)

COMPILE	= $(CC) $(DEFINES) $(INCL) -c
DEPEND	= $(CC) $(INCL) -MM -c

VPATH	= $(OBJDIR)

srcs	= $(wildcard *.c)
deps =	$(patsubst %.c,%.d,$(srcs)) 

objs = UnionDes.20050706.o		\
	UnionDataExch1.0.o	\
	UnionBaseDes.20050222.o	\
	UnionMAC1.0.o		\
	UnionXOR1.0.o		\
	UnionMD5_1.0.o		\
	UnionSHA1_1.0.o		\
	base64.o		\
	unionPBOCKeyGenerate.20050706.o	\
	unionAlgorithm.20040706.withoutBaseDes.a

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

objs10 =	UnionDes.20050706.o		\
	UnionDataExch1.0.o	\
	UnionBaseDes.20050222.o	\
	UnionMAC1.0.o		\
	UnionXOR1.0.o		\
	UnionMD5_1.0.o		\
	UnionSHA1_1.0.o		\
	UnionSHA256.o		\
	base64.o		\
	unionPBOCKeyGenerate.20050706.o

unionAlgorithm.20040706.withoutBaseDes.a:	$(objs10)
	$(AR) $(OSSPECPACK) rv $(OBJDIR)$@ $(addprefix $(OBJDIR),$(objs10))

ifeq ($(filter $(MAKECMDGOALS),clean install show),) 
ifeq ($(UNION_USE_GCC),1)
sinclude $(addprefix $(OBJDIR),$(deps))
endif
endif

includes = $(INCDIR)UnionDes.h	\
	$(INCDIR)UnionDataExch.h	\
	$(INCDIR)UnionMAC.h	\
	$(INCDIR)UnionXOR.h	\
	$(INCDIR)UnionMD5.h	\
	$(INCDIR)UnionSHA1.h	\
	$(INCDIR)UnionSHA256.h	\
	$(INCDIR)base64.h	\
	$(INCDIR)UnionAlgorithm.h	\
	$(INCDIR)unionPBOCKeyGenerate.h

.PHONY: clean install
install:
	@sh cp_head.x  "$(includes)" $(CMM2003INCDIR)
#	@sh cp_head.x  ../include/ $(CMM2003INCDIR)

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
	
