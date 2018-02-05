# 定义开发目录
HOMEDIR = $(CMM2003DIR)/unionSJL06/
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
SPEC_DEFINES = -D _UnionSJL06_2_x_Above_ $(OSSPEC)

COMPILE	= $(CC) $(DEFINES) $(INCL) -c
SPEC_COMPILE	= $(CC) $(SPEC_DEFINES) $(INCL) -c
DEPEND	= $(CC) $(INCL) -MM -c

VPATH	= $(OBJDIR)

srcs	= $(wildcard *.c)
deps =	$(patsubst %.c,%.d,$(srcs)) 

objs = sjl06Protocol1.0.a	\
	sjl06Protocol2.0.a	\
	sjl06Protocol.20050620.useSJL06_2_x.a	\
	sjl06Protocol.20050822.useSJL06_2_x.a

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

objs02 = sjl06Protocol1.0.o
$(OBJDIR)sjl06Protocol2.0.a:
	@$(SPEC_COMPILE) sjl06Protocol1.0.c -o $(objs02)
	$(AR) $(OSSPECPACK) rv $@ $(objs02)
	$(RM) $(objs02)

objs03 = sjl06Protocol.20050620.o
$(OBJDIR)sjl06Protocol.20050620.useSJL06_2_x.a:
	@$(SPEC_COMPILE) sjl06Protocol.20050620.c -o $(objs03)
	$(AR) $(OSSPECPACK) rv $@ $(objs03)
	$(RM) $(objs03)

objs04 = sjl06Protocol.20050822.o
$(OBJDIR)sjl06Protocol.20050822.useSJL06_2_x.a:
	@$(SPEC_COMPILE) sjl06Protocol.20050822.c -o $(objs04)
	$(AR) $(OSSPECPACK) rv $@ $(objs04)
	$(RM) $(objs04)

ifeq ($(filter $(MAKECMDGOALS),clean install show),) 
ifeq ($(UNION_USE_GCC),1)
sinclude $(addprefix $(OBJDIR),$(deps))
endif
endif

.PHONY: clean install
install:
	@sh cp_head.x  "$(INCDIR)sjl06Protocol.h" $(CMM2003INCDIR)
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
	
