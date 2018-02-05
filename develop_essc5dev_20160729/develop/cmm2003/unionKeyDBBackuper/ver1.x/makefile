# 定义开发目录
HOMEDIR = $(CMM2003DIR)/unionKeyDBBackuper/
INCDIR = $(HOMEDIR)/include/
OBJDIR = $(CMM2003LIBDIR)

# 定义头文件目录清单
# 顺序应该是：
# 通用模块头文件目录/当前开发环境头文件目录/本工程的头文件目录

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

#传给脚本的参数
SLP1 = "$*.o"	"$@ $*.o $*.a"	$@

# 这个编译开关必须增加
DEFINES = $(OSSPEC)
SPEC_DEFINES = -D _UnionDesKey_3_x_ -D _UnionPKDB_3_x_  $(OSSPEC)
SPEC_DEFINES1 = -D _UnionDesKey_3_x_ -D _UnionPKDB_3_x_  -D _unionDesKey_4_x_ $(OSSPEC)

COMPILE	= $(CC) $(DEFINES) $(INCL) -c
SPEC_COMPILE	= $(CC) $(SPEC_DEFINES) $(INCL) -c
SPEC_COMPILE1	= $(CC) $(SPEC_DEFINES1) $(INCL) -c
DEPEND	= $(CC) $(INCL) -MM -c

VPATH	= $(OBJDIR)

srcs	= $(wildcard *.c)
deps =	$(patsubst %.c,%.d,$(srcs)) 

objs = keyDBBackuper.useDesKey4.x.20060814.o	\
	keyDBBackuperMain.useDesKey4.x.20060814.o	\
	keyDBBackuper.20041120.a	\
	keyDBBackuperMain.20041120.a	\
	keyDBBackuper.useDesKey3.0.20041125.a	\
	keyDBBackuperMain.useDesKey3.0.20041125.a
	
all:	makeall

makeall:	$(objs)

.SUFFIXES:.c.o
%.o:%.c
	@echo "CC $(DEFINES) $(OBJDIR)$@	$<"
	@$(COMPILE) $< -o $(OBJDIR)$@

.c.a:
	@echo "CC AND $(AR) $(DEFINES) $(OBJDIR)$@	$<"
	@$(COMPILE) $< -o $*.o
	@$(AR) $(OSSPECPACK) rv $(OBJDIR)$@ $*.o 
	@$(RM) $*.o

$(OBJDIR)%.d:%.c
	@echo "DEP $@	$<"
	@$(DEPEND) $< > $@
	@sh file_substr.x $(SLP1)

ifeq ($(UNION_USE_GCC),1)
keyDBBackuper.useDesKey4.x.20060814.o:  keyDBBackuper.20041120.c keyDBBackuper.20041120.d
else
keyDBBackuper.useDesKey4.x.20060814.o:  keyDBBackuper.20041120.c
endif
	@echo "CC $(SPEC_DEFINES1) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE1) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
keyDBBackuperMain.useDesKey4.x.20060814.o:  keyDBBackuperMain.20041120.c keyDBBackuperMain.20041120.d
else
keyDBBackuperMain.useDesKey4.x.20060814.o:  keyDBBackuperMain.20041120.c
endif
	@echo "CC $(SPEC_DEFINES) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE1) $< -o $(OBJDIR)$@

keyDBBackuper.useDesKey3.0.20041125.a: keyDBBackuper.20041120.c
	@$(SPEC_COMPILE) keyDBBackuper.20041120.c -o $*.o
	@$(AR) $(OSSPECPACK) rv $(OBJDIR)$@ $*.o
	@$(RM) $*.o

keyDBBackuperMain.useDesKey3.0.20041125.a:	keyDBBackuperMain.20041120.c
	@$(SPEC_COMPILE) keyDBBackuperMain.20041120.c -o $*.o
	@$(AR) $(OSSPECPACK) rv $(OBJDIR)$@ $*.o
	@$(RM) $*.o

ifeq ($(filter $(MAKECMDGOALS),clean install show),) 
ifeq ($(UNION_USE_GCC),1)
sinclude $(addprefix $(OBJDIR),$(deps))
endif
endif

.PHONY: clean install
install:
	@sh cp_head.x  "$(INCDIR)keyDBBackuper.h" $(CMM2003INCDIR)
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
	
