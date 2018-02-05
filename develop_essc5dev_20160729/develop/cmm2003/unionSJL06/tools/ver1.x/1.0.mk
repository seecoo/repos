HOMEDIR = $(CMM2003DIR)/unionSJL06/
INCDIR = $(HOMEDIR)include/
INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = $(OSSPEC)

all:	authRacal1.0.a			\
	genKeyComponent1.0.a		\
	readBMK1.0.a			\
	readTransKey1.0.a		\
	LibProducts			\
	rmTmpFiles

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c


# Libs
# 模块02 authRacal
objs0210 = authRacal1.0.o
authRacal1.0.a:	$(objs0210)	
			ar $(OSSPECPACK) rv authRacal1.0.a 	$(objs0210)

# 模块03 genKeyComponent
objs0310 = genKeyComponent1.0.o
genKeyComponent1.0.a:	$(objs0310)	
			ar $(OSSPECPACK) rv genKeyComponent1.0.a 	$(objs0310)
# 模块04 readBMK
objs0410 = readBMK1.0.o
readBMK1.0.a:	$(objs0410)	
			ar $(OSSPECPACK) rv readBMK1.0.a 	$(objs0410)

# 模块05 readTransKey
objs0510 = readTransKey1.0.o
readTransKey1.0.a:	$(objs0510)	
			ar $(OSSPECPACK) rv readTransKey1.0.a 	$(objs0510)


LibProducts:	$(null)
		mv authRacal1.0.a 			$(CMM2003LIBDIR)
		mv genKeyComponent1.0.a 		$(CMM2003LIBDIR)
		mv readBMK1.0.a				$(CMM2003LIBDIR)
		mv readTransKey1.0.a			$(CMM2003LIBDIR)

rmTmpFiles:	$(null)
		rm *.o
