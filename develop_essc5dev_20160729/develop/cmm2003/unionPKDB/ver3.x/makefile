# 定义开发目录
HOMEDIR = $(CMM2003DIR)/unionPKDB/
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
SPEC_DEFINES = $(OSSPEC) -D _KDBClient_

COMPILE	= $(CC) $(DEFINES) $(INCL) -c
SPEC_COMPILE	= $(CC) $(SPEC_DEFINES) $(INCL) -c
DEPEND	= $(CC) $(INCL) -MM -c

VPATH	= $(OBJDIR)

srcs	= $(wildcard *.c)
deps =	$(patsubst %.c,%.d,$(srcs)) 

objs = unionPK3.0.20041125.a	\
	unionPKDB3.0.20041125.a	\
	mngPKDB3.0.20041125.a	\
	mngPKDBLog3.0.20041125.a	\
	unionPKDB3.0.client.20060414.a	\
	unionPKDB3.0.20060414.a	\
	unionPKDB3.x.NoIndex.20060421.a

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

objs01 = unionPKDB3.0.20041125.o unionPKDBLog3.0.20041125.o unionPK3.0.20041125.o
unionPKDB3.0.20041125.a:	$(objs01)
	@$(COMPILE) unionPKDB3.0.20041125.c -o unionPKDB3.0.20041125.o
	@$(COMPILE) unionPKDBLog3.0.20041125.c -o unionPKDBLog3.0.20041125.o
	@$(COMPILE) unionPK3.0.20041125.c -o unionPK3.0.20041125.o 
	$(AR) $(OSSPECPACK) rv $(OBJDIR)$@ $(objs01)
	$(RM) $(objs01) 

objs02 = unionPKDB3.0.20041125.client.o unionPKDBLog3.0.20060414.client.o unionPK3.0.20041125.client.o
unionPKDB3.0.client.20060414.a:	$(objs01)	
	@$(SPEC_COMPILE) unionPKDB3.0.20041125.c -o unionPKDB3.0.20041125.client.o
	@$(SPEC_COMPILE) unionPKDBLog3.0.20060414.c -o unionPKDBLog3.0.20060414.client.o
	@$(SPEC_COMPILE) unionPK3.0.20041125.c -o unionPK3.0.20041125.client.o
	$(AR) $(OSSPECPACK) rv $(OBJDIR)$@ $(objs02)
	$(RM) $(objs02)

objs03= unionPKDB3.0.20041125.o unionPKDBLog3.0.20060414.o unionPK3.0.20041125.o
unionPKDB3.0.20060414.a:	$(objs03)
	@$(COMPILE) unionPKDB3.0.20041125.c -o unionPKDB3.0.20041125.o
	@$(COMPILE) unionPKDBLog3.0.20060414.c -o unionPKDBLog3.0.20060414.o
	@$(COMPILE) unionPK3.0.20041125.c -o unionPK3.0.20041125.o
	$(AR) $(OSSPECPACK) rv $(OBJDIR)$@ $(objs03)
	$(RM) $(objs03) 

objs04 = unionPKDB3.x.NoIndex.20060421.o unionPKDBLog3.0.20060414.o unionPK3.0.20041125.o
unionPKDB3.x.NoIndex.20060421.a:	$(objs04)
	@$(COMPILE) unionPKDB3.x.NoIndex.20060421.c -o unionPKDB3.x.NoIndex.20060421.o
	@$(COMPILE) unionPKDBLog3.0.20060414.c -o unionPKDBLog3.0.20060414.o
	@$(COMPILE) unionPK3.0.20041125.c -o unionPK3.0.20041125.o
	$(AR) $(OSSPECPACK) rv $(OBJDIR)$@ $(objs04)
	$(RM) $(objs04)

ifeq ($(filter $(MAKECMDGOALS),clean install show),) 
ifeq ($(UNION_USE_GCC),1)
sinclude $(addprefix $(OBJDIR),$(deps))
endif
endif

includes = $(INCDIR)unionPK.h	\
	$(INCDIR)unionPKDB.h	\
	$(INCDIR)unionPKDBLog.h

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
	
