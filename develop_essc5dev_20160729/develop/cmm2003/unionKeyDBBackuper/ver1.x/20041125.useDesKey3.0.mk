#	Wolfgang Wang
#	2004/11/20

# 使用3.0版本的DESKey定义
DEF = -D _UnionDesKey_3_x_ -D _UnionPKDB_3_x_  $(OSSPEC)

HOMEDIR = $(CMM2003DIR)/unionKeyDBBackuper/
INCDIR = $(HOMEDIR)include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

all:	keyDBBackuper.useDesKey3.0.20041125.a			\
	keyDBBackuperMain.useDesKey3.0.20041125.a		\
	finalProducts						\
	rmTmpFiles

.SUFFIXES:.c.o
.c.o:
	$(CC) $(INCL) $(DEF) -c $*.c

# Libs 01
objs0220 =	keyDBBackuper.20041120.o
keyDBBackuper.useDesKey3.0.20041125.a:	$(objs0220)
			ar $(OSSPECPACK) rv keyDBBackuper.useDesKey3.0.20041125.a $(objs0220)

objs0420 =	keyDBBackuperMain.20041120.o
keyDBBackuperMain.useDesKey3.0.20041125.a:	$(objs0420)
			ar $(OSSPECPACK) rv keyDBBackuperMain.useDesKey3.0.20041125.a $(objs0420)

# rmTmpFiles
rmTmpFiles:	$(null)
		rm	*.o

# finalProducts
finalProducts:	$(null)
	# libs
		mv	keyDBBackuper.useDesKey3.0.20041125.a		$(CMM2003LIBDIR)
		mv	keyDBBackuperMain.useDesKey3.0.20041125.a	$(CMM2003LIBDIR)
