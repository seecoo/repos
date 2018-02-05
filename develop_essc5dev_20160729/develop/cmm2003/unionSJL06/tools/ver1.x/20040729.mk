HOMEDIR = $(CMM2003DIR)/unionSJL06/
INCDIR = $(HOMEDIR)include/
INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

all:	authRacal.20040729.a			\
	genKeyComponent.20040729.a		\
	readBMK.20040729.a			\
	readTransKey.20040729.a			\
	rmTmpFiles				\
	finalProducts

DEFINES = -D _UnionSJL06_2_x_Above_ $(OSSPEC)
.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c


# Libs
# 模块02 authRacal
objs0210 = authRacal1.0.o
authRacal.20040729.a:	$(objs0210)	
			ar $(OSSPECPACK) rv authRacal.20040729.a 	$(objs0210)

# 模块03 genKeyComponent
objs0310 = genKeyComponent2.1.o
genKeyComponent.20040729.a:	$(objs0310)	
			ar $(OSSPECPACK) rv genKeyComponent.20040729.a 	$(objs0310)
# 模块04 readBMK
objs0410 = readBMK1.0.o
readBMK.20040729.a:	$(objs0410)	
			ar $(OSSPECPACK) rv readBMK.20040729.a 	$(objs0410)

# 模块05 readTransKey
objs0510 = readTransKey1.0.o
readTransKey.20040729.a:	$(objs0510)	
			ar $(OSSPECPACK) rv readTransKey.20040729.a 	$(objs0510)


rmTmpFiles:	$(null)
		rm *.o

finalProducts:	$(null)
	#libs
		mv authRacal.20040729.a			$(CMM2003LIBDIR)
		mv genKeyComponent.20040729.a		$(CMM2003LIBDIR)
		mv readBMK.20040729.a			$(CMM2003LIBDIR)
		mv readTransKey.20040729.a		$(CMM2003LIBDIR)
