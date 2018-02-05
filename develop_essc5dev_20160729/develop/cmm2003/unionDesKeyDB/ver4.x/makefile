# 定义开发目录
HOMEDIR = $(CMM2003DIR)/unionDesKeyDB/
INCDIR = $(HOMEDIR)/include/
OBJDIR = $(CMM2003LIBDIR)

# 定义头文件目录清单
# 顺序应该是：
# 通用模块头文件目录/当前开发环境头文件目录/本工程的头文件目录

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)	

#传给脚本的参数
SLP1 = "$*.o"	"$@ $*.o"	$@
# 这个编译开关必须增加
DEFINES = -D _UnionDesKey_4_x_ -D _UnionDesKey_3_x_ $(OSSPEC)
SPEC_DEFINES = -D _UnionDesKey_4_x_ -D _UnionDesKey_3_x_ $(OSSPEC) -D _KDBClient_
SPEC_DEFINES1 = -D _KMC_USE_ -D _UnionDesKey_4_x_ -D _UnionDesKey_3_x_  $(OSSPEC)
SPEC_DEFINES2 = -D _UnionDesKey_4_x_ -D _UnionDesKey_3_x_ $(OSSPEC) -D _useKeySynchronizer_
SPEC_DEFINES3 = -D _UnionDesKey_4_x_ -D _UnionDesKey_3_x_ $(OSSPEC) -D _useKeySynchronizer_ -D _useKeyRecStr_

COMPILE	= $(CC) $(DEFINES) $(INCL) -c
SPEC_COMPILE	= $(CC) $(SPEC_DEFINES) $(INCL) -c
SPEC_COMPILE1	= $(CC) $(SPEC_DEFINES1) $(INCL) -c
SPEC_COMPILE2	= $(CC) $(SPEC_DEFINES2) $(INCL) -c
SPEC_COMPILE3	= $(CC) $(SPEC_DEFINES3) $(INCL) -c
DEPEND	= $(CC) $(INCL) -MM -c

VPATH	= $(OBJDIR)

srcs	= $(wildcard *.c)
deps =	$(patsubst %.c,%.d,$(srcs)) 

objs = unionDesKey.4.x.20060814.o	\
	unionDesKeyDB.4.x.20060814.o	\
	unionDesKeyDBLog.4.x.20060814.o	\
	mngDesKeyDB.4.x.20060814.o	\
	mngDesKeyDBLog.4.x.20060814.o \
	keyDBLocalBackuper.20060814.o \
	mngKeyDBLocalReLoader.20060814.o \
	mngKeyDBMirrorDiff.20060814.o	\
	autoMirrorDesKeyDB.20100706.o	\
	unionDesKeyDB.4.x.DBClient.20060814.o	\
	unionDesKey.4.x.20060814.KMC_USE.o	\
	unionDesKeyDBLog.4.x.useKeySynchronizer.20060814.o	\
	unionDesKeyDBLog.4.x.useKeySynchronizer.useKeyRecStr.20060814.o	\
	keyDBLocalBackuper.4.x.20060814.o	\
	mngKeyDBLocalReLoader.4.x.20060814.o	\
	mngKeyDBMirrorDiff.4.x.20060814.o

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
keyDBLocalBackuper.4.x.20060814.o:  keyDBLocalBackuper.20060814.c keyDBLocalBackuper.20060814.d
else
keyDBLocalBackuper.4.x.20060814.o:  keyDBLocalBackuper.20060814.c
endif
	@echo "CC $(DEFINES) $(OBJDIR)$@      $<"
	@$(COMPILE) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
mngKeyDBLocalReLoader.4.x.20060814.o:  mngKeyDBLocalReLoader.20060814.c mngKeyDBLocalReLoader.20060814.d
else
mngKeyDBLocalReLoader.4.x.20060814.o:  mngKeyDBLocalReLoader.20060814.c
endif
	@echo "CC $(DEFINES) $(OBJDIR)$@      $<"
	@$(COMPILE) $< -o $(OBJDIR)$@
	
ifeq ($(UNION_USE_GCC),1)
mngKeyDBMirrorDiff.4.x.20060814.o:  mngKeyDBMirrorDiff.20060814.c mngKeyDBMirrorDiff.20060814.d
else
mngKeyDBMirrorDiff.4.x.20060814.o:  mngKeyDBMirrorDiff.20060814.c
endif
	@echo "CC $(DEFINES) $(OBJDIR)$@      $<"
	@$(COMPILE) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
unionDesKeyDB.4.x.DBClient.20060814.o:  unionDesKeyDB.4.x.20060814.c unionDesKeyDB.4.x.20060814.d
else
unionDesKeyDB.4.x.DBClient.20060814.o:  unionDesKeyDB.4.x.20060814.c
endif
	@echo "CC $(SPEC_DEFINES) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
unionDesKey.4.x.20060814.KMC_USE.o:  unionDesKey.4.x.20060814.c unionDesKey.4.x.20060814.d
else
unionDesKey.4.x.20060814.KMC_USE.o:  unionDesKey.4.x.20060814.c
endif
	@echo "CC $(SPEC_DEFINES1) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE1) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
unionDesKeyDBLog.4.x.useKeySynchronizer.20060814.o:  unionDesKeyDBLog.4.x.20060814.c unionDesKeyDBLog.4.x.20060814.d
else
unionDesKeyDBLog.4.x.useKeySynchronizer.20060814.o:  unionDesKeyDBLog.4.x.20060814.c
endif
	@echo "CC $(SPEC_DEFINES2) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE2) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
unionDesKeyDBLog.4.x.useKeySynchronizer.useKeyRecStr.20060814.o:  unionDesKeyDBLog.4.x.20060814.c unionDesKeyDBLog.4.x.20060814.d
else
unionDesKeyDBLog.4.x.useKeySynchronizer.useKeyRecStr.20060814.o:  unionDesKeyDBLog.4.x.20060814.c
endif
	@echo "CC $(SPEC_DEFINES3) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE3) $< -o $(OBJDIR)$@

ifeq ($(filter $(MAKECMDGOALS),clean install show),) 
ifeq ($(UNION_USE_GCC),1)
sinclude $(addprefix $(OBJDIR),$(deps))
endif
endif
	
includes = $(INCDIR)unionDesKey.h	\
	$(INCDIR)unionDesKeyDB.h	\
	$(INCDIR)unionDesKeyDBLog.h	\
	$(INCDIR)unionKeyDB.h
	
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
	
