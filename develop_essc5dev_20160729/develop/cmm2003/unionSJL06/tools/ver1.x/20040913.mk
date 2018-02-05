HOMEDIR = $(CMM2003DIR)/unionSJL06/
INCDIR = $(HOMEDIR)include/
INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

# 这个编译开关必须增加
DEFINES = -D _UnionSJL06_2_x_Above_ $(OSSPEC)


all:	checkVK.20040913.a				\
	backupVK.20040913.a				\
	restoreVK.20040913.a				\
	genKeyComponent.20040913.a			\
	rmTmpFiles					\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c


# Libs
# 模块02
objs0210 = checkVK.20040913.o
checkVK.20040913.a:	$(objs0210)	
			ar $(OSSPECPACK) rv checkVK.20040913.a 	$(objs0210)

# 模块03
objs0310 = backupVK.20040913.o
backupVK.20040913.a:	$(objs0310)	
			ar $(OSSPECPACK) rv backupVK.20040913.a 	$(objs0310)

# 模块04
objs0410 = restoreVK.20040913.o
restoreVK.20040913.a:	$(objs0410)	
			ar $(OSSPECPACK) rv restoreVK.20040913.a 	$(objs0410)

# 模块05
objs0510 = genKeyComponent1.0.o
genKeyComponent.20040913.a:	$(objs0510)	
			ar $(OSSPECPACK) rv genKeyComponent.20040913.a $(objs0510)

rmTmpFiles:	$(null)
		rm *.o

finalProducts:	$(null)
	#libs
		mv checkVK.20040913.a			$(CMM2003LIBDIR)
		mv backupVK.20040913.a			$(CMM2003LIBDIR)
		mv restoreVK.20040913.a			$(CMM2003LIBDIR)
		mv genKeyComponent.20040913.a		$(CMM2003LIBDIR)
