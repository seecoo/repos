HOMEDIR = $(CMM2003DIR)/unionSJL06/
INCDIR = $(HOMEDIR)include/
INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

# 这个编译开关必须增加
DEFINES = $(OSSPEC)


all:	readBMK.useSJL06_1_x.20050616.a				\
	restoreBMK.useSJL06_1_x.20050616.a				\
	readTerminalKey.useSJL06_1_x.20050616.a			\
	restoreTerminalKey.useSJL06_1_x.20050616.a			\
	readTransKey.useSJL06_1_x.20050616.a				\
	restoreTransKey.useSJL06_1_x.20050616.a			\
	rmTmpFiles					\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c


# Libs
# 模块02
objs0210 = readBMK.20050616.o
readBMK.useSJL06_1_x.20050616.a:	$(objs0210)	
			ar $(OSSPECPACK) rv readBMK.useSJL06_1_x.20050616.a 	$(objs0210)

# 模块03
objs0310 = restoreBMK.20050616.o
restoreBMK.useSJL06_1_x.20050616.a:	$(objs0310)	
			ar $(OSSPECPACK) rv restoreBMK.useSJL06_1_x.20050616.a 	$(objs0310)

# 模块04
objs0410 = readTerminalKey.20050616.o
readTerminalKey.useSJL06_1_x.20050616.a:	$(objs0410)	
			ar $(OSSPECPACK) rv readTerminalKey.useSJL06_1_x.20050616.a 	$(objs0410)

# 模块05
objs0510 = restoreTerminalKey.20050616.o
restoreTerminalKey.useSJL06_1_x.20050616.a:	$(objs0510)	
			ar $(OSSPECPACK) rv restoreTerminalKey.useSJL06_1_x.20050616.a $(objs0510)

# 模块14
objs1410 = readTransKey.20050616.o
readTransKey.useSJL06_1_x.20050616.a:	$(objs1410)	
			ar $(OSSPECPACK) rv readTransKey.useSJL06_1_x.20050616.a 	$(objs1410)

# 模块15
objs1510 = restoreTransKey1.0.o
restoreTransKey.useSJL06_1_x.20050616.a:	$(objs1510)	
			ar $(OSSPECPACK) rv restoreTransKey.useSJL06_1_x.20050616.a $(objs1510)

rmTmpFiles:	$(null)
		rm *.o

finalProducts:	$(null)
	#libs
		mv readBMK.useSJL06_1_x.20050616.a				$(CMM2003LIBDIR)
		mv restoreBMK.useSJL06_1_x.20050616.a			$(CMM2003LIBDIR)
		mv readTerminalKey.useSJL06_1_x.20050616.a			$(CMM2003LIBDIR)
		mv restoreTerminalKey.useSJL06_1_x.20050616.a		$(CMM2003LIBDIR)
		mv readTransKey.useSJL06_1_x.20050616.a			$(CMM2003LIBDIR)
		mv restoreTransKey.useSJL06_1_x.20050616.a			$(CMM2003LIBDIR)
