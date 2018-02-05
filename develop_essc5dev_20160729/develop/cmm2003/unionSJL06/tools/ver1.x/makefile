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

objs =	synchronizeBMK.useSJL06_2_x.20050616.o	\
	synchronizeVK.useSJL06_2_x.20050616.o	\
	restoreVK.useSJL06_2_x.20080226.o	\
	authRacal1.0.a		\
	genKeyComponent1.0.a	\
	readBMK1.0.a		\
	readTransKey1.0.a	\
	genKeyComponent2.0.a	\
	genKeyComponent2.1.a	\
	authRacal.20040729.a	\
	genKeyComponent.20040729.a	\
	readBMK.20040729.a	\
	readTransKey.20040729.a	\
	checkVK.20040913.a	\
	backupVK.20040913.a	\
	restoreVK.20040913.a	\
	genKeyComponent.20040913.a	\
	readBMK.useSJL06_1_x.20050616.a	\
	restoreBMK.useSJL06_1_x.20050616.a	\
	readTerminalKey.useSJL06_1_x.20050616.a	\
	restoreTerminalKey.useSJL06_1_x.20050616.a	\
	readTransKey.useSJL06_1_x.20050616.a	\
	restoreTransKey.useSJL06_1_x.20050616.a	\
	readBMK.useSJL06_2_x.20050616.a	\
	restoreBMK.useSJL06_2_x.20050616.a	\
	readTerminalKey.useSJL06_2_x.20050616.a	\
	restoreTerminalKey.useSJL06_2_x.20050616.a	\
	readTransKey.useSJL06_2_x.20050616.a	\
	restoreTransKey.useSJL06_2_x.20050616.a	\
	generateKeyComponent.SJL06_2_x.20050718.a	\
	generateKeyComponent.SJL06_2_x.20050725.a	\
	keyPrintFormat.20060614.a

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
synchronizeBMK.useSJL06_2_x.20050616.o:  synchronizeBMK.20050616.c synchronizeBMK.20050616.d
else
synchronizeBMK.useSJL06_2_x.20050616.o:  synchronizeBMK.20050616.c
endif
	@echo "CC $(SPEC_DEFINES) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
synchronizeVK.useSJL06_2_x.20050616.o:  synchronizeVK.20050616.c synchronizeVK.20050616.d
else
synchronizeVK.useSJL06_2_x.20050616.o:  synchronizeVK.20050616.c
endif
	@echo "CC $(SPEC_DEFINES) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE) $< -o $(OBJDIR)$@

ifeq ($(UNION_USE_GCC),1)
restoreVK.useSJL06_2_x.20080226.o:  restoreVK.20080226.c restoreVK.20080226.d
else
restoreVK.useSJL06_2_x.20080226.o:  restoreVK.20080226.c
endif
	@echo "CC $(SPEC_DEFINES) $(OBJDIR)$@      $<"
	@$(SPEC_COMPILE) $< -o $(OBJDIR)$@

objs07 = authRacal1.0.o
$(OBJDIR)authRacal.20040729.a:
	@$(SPEC_COMPILE) authRacal1.0.c -o $(objs07)
	$(AR) $(OSSPECPACK) rv $@ $(objs07)
	$(RM) $(objs07)

objs08 = genKeyComponent2.1.o
$(OBJDIR)genKeyComponent.20040729.a:
	@$(SPEC_COMPILE) genKeyComponent2.1.c -o $(objs08)
	$(AR) $(OSSPECPACK) rv $@ $(objs08)
	$(RM) $(objs08)

objs09 = readBMK1.0.o
$(OBJDIR)readBMK.20040729.a:
	@$(SPEC_COMPILE) readBMK1.0.c -o $(objs09)
	$(AR) $(OSSPECPACK) rv $@ $(objs09)
	$(RM) $(objs09)

objs10 = readTransKey1.0.o
$(OBJDIR)readTransKey.20040729.a:
	@$(SPEC_COMPILE) readTransKey1.0.c -o $(objs10)
	$(AR) $(OSSPECPACK) rv $@ $(objs10)
	$(RM) $(objs10)

objs11 = checkVK.20040913.o
$(OBJDIR)checkVK.20040913.a:
	@$(SPEC_COMPILE) checkVK.20040913.c -o $(objs11)
	$(AR) $(OSSPECPACK) rv $@ $(objs11)
	$(RM) $(objs11)

objs12 = backupVK.20040913.o
$(OBJDIR)backupVK.20040913.a:
	@$(SPEC_COMPILE) backupVK.20040913.c -o $(objs12)
	$(AR) $(OSSPECPACK) rv $@ $(objs12)
	$(RM) $(objs12)

objs13 = restoreVK.20040913.o
$(OBJDIR)restoreVK.20040913.a:
	@$(SPEC_COMPILE) restoreVK.20040913.c -o $(objs13)
	$(AR) $(OSSPECPACK) rv $@ $(objs13)
	$(RM) $(objs13)

objs14 = genKeyComponent1.0.o
$(OBJDIR)genKeyComponent.20040913.a:
	@$(SPEC_COMPILE) genKeyComponent1.0.c -o $(objs14)
	$(AR) $(OSSPECPACK) rv $@ $(objs14)
	$(RM) $(objs14)

objs15 = readBMK.20050616.o
$(OBJDIR)readBMK.useSJL06_1_x.20050616.a:	$(objs15)	
	$(AR) $(OSSPECPACK) rv $@ $(addprefix $(OBJDIR),$(objs15))
	$(RM) $(addprefix $(OBJDIR),$(objs15))

objs16 = restoreBMK.20050616.o
$(OBJDIR)restoreBMK.useSJL06_1_x.20050616.a:	$(objs16)	
	$(AR) $(OSSPECPACK) rv $@ $(addprefix $(OBJDIR),$(objs16))
	$(RM) $(addprefix $(OBJDIR),$(objs16))

objs17 = readTerminalKey.20050616.o
$(OBJDIR)readTerminalKey.useSJL06_1_x.20050616.a:	$(objs17)	
	$(AR) $(OSSPECPACK) rv $@ $(addprefix $(OBJDIR),$(objs17))
	$(RM) $(addprefix $(OBJDIR),$(objs17))

objs18 = restoreTerminalKey.20050616.o
$(OBJDIR)restoreTerminalKey.useSJL06_1_x.20050616.a:	$(objs18)	
	$(AR) $(OSSPECPACK) rv $@ $(addprefix $(OBJDIR),$(objs18))
	$(RM) $(addprefix $(OBJDIR),$(objs18))

objs19 = readTransKey.20050616.o
$(OBJDIR)readTransKey.useSJL06_1_x.20050616.a:	$(objs19)	
	$(AR) $(OSSPECPACK) rv $@ $(addprefix $(OBJDIR),$(objs19))
	$(RM) $(addprefix $(OBJDIR),$(objs19))

objs20 = restoreTransKey1.0.o
$(OBJDIR)restoreTransKey.useSJL06_1_x.20050616.a:	$(objs20)	
	$(AR) $(OSSPECPACK) rv $@ $(addprefix $(OBJDIR),$(objs20))
	$(RM) $(addprefix $(OBJDIR),$(objs20))

objs21 = readBMK.20050616.o
$(OBJDIR)readBMK.useSJL06_2_x.20050616.a:
	@$(SPEC_COMPILE) readBMK.20050616.c -o $(objs21)
	$(AR) $(OSSPECPACK) rv $@ $(objs21)
	$(RM) $(objs21)

objs22 = restoreBMK.20050616.o
$(OBJDIR)restoreBMK.useSJL06_2_x.20050616.a:
	@$(SPEC_COMPILE) restoreBMK.20050616.c -o $(objs22)
	$(AR) $(OSSPECPACK) rv $@ $(objs22)
	$(RM) $(objs22)

objs23 = readTerminalKey.20050616.o
$(OBJDIR)readTerminalKey.useSJL06_2_x.20050616.a:
	@$(SPEC_COMPILE) readTerminalKey.20050616.c -o $(objs23)
	$(AR) $(OSSPECPACK) rv $@ $(objs23)
	$(RM) $(objs23)

objs24 = restoreTerminalKey.20050616.o
$(OBJDIR)restoreTerminalKey.useSJL06_2_x.20050616.a:
	@$(SPEC_COMPILE) restoreTerminalKey.20050616.c -o $(objs24)
	$(AR) $(OSSPECPACK) rv $@ $(objs24)
	$(RM) $(objs24)

objs25 = readTransKey.20050616.o
$(OBJDIR)readTransKey.useSJL06_2_x.20050616.a:
	@$(SPEC_COMPILE) readTransKey.20050616.c -o $(objs25)
	$(AR) $(OSSPECPACK) rv $@ $(objs25)
	$(RM) $(objs25)

objs26 = restoreTransKey1.0.o
$(OBJDIR)restoreTransKey.useSJL06_2_x.20050616.a:
	@$(SPEC_COMPILE) restoreTransKey1.0.c -o $(objs26)
	$(AR) $(OSSPECPACK) rv $@ $(objs26)
	$(RM) $(objs26)

objs27 =	generateKeyComponent.20050718.o
$(OBJDIR)generateKeyComponent.SJL06_2_x.20050718.a:
	@$(SPEC_COMPILE) generateKeyComponent.20050718.c -o $(objs27)
	$(AR) $(OSSPECPACK) rv $@ $(objs27)
	$(RM) $(objs27)

objs28 =	generateKeyComponent.20050725.o
$(OBJDIR)generateKeyComponent.SJL06_2_x.20050725.a:
	@$(SPEC_COMPILE) generateKeyComponent.20050725.c -o $(objs28)
	$(AR) $(OSSPECPACK) rv $@ $(objs28)
	$(RM) $(objs28)

objs29 =	keyPrintFormat.20060614.o
$(OBJDIR)keyPrintFormat.20060614.a:
	@$(SPEC_COMPILE) keyPrintFormat.20060614.c -o $(objs29)
	$(AR) $(OSSPECPACK) rv $@ $(objs29)
	$(RM) $(objs29)

ifeq ($(filter $(MAKECMDGOALS),clean install show),) 
ifeq ($(UNION_USE_GCC),1)
sinclude $(addprefix $(OBJDIR),$(deps))
endif
endif
	
.PHONY: clean install
install:

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
	
