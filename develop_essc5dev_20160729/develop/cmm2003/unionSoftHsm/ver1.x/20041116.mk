HOMEDIR = $(CMM2003DIR)/unionSoftHsm
INCDIR = $(HOMEDIR)/include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = -D _UNIX $(OSSPEC)

all:	softHsmKeySpace.20041116.a	\
	mngSoftHsmKeySpace.20041116.a	\
	softHsmCmd.20041116.a		\
	softHsmMain.20041116.a		\
	finalProducts			\
	rmTmpFiles

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs0110 = softHsmKeySpace.20041116.o
softHsmKeySpace.20041116.a:	$(objs0110)	
		ar $(OSSPECPACK) rv softHsmKeySpace.20041116.a $(objs0110)

objs0210 = mngSoftHsmKeySpace.20041116.o
mngSoftHsmKeySpace.20041116.a:	$(objs0210)	
		ar $(OSSPECPACK) rv mngSoftHsmKeySpace.20041116.a $(objs0210)

objs0310 = softHsmCmd.20041116.o
softHsmCmd.20041116.a:	$(objs0310)	
		ar $(OSSPECPACK) rv softHsmCmd.20041116.a $(objs0310)

objs0410 = softHsmMain.20041116.o
softHsmMain.20041116.a:	$(objs0410)	
		ar $(OSSPECPACK) rv softHsmMain.20041116.a $(objs0410)

finalProducts:	$(null)
# includes
		cp $(INCDIR)/softHsmKeySpace.h			$(CMM2003INCDIR)
# libs
		mv softHsmKeySpace.20041116.a			$(CMM2003LIBDIR)
		mv mngSoftHsmKeySpace.20041116.a		$(CMM2003LIBDIR)
		mv softHsmCmd.20041116.a			$(CMM2003LIBDIR)
		mv softHsmMain.20041116.a				$(CMM2003LIBDIR)

rmTmpFiles:	$(null)	
		rm *.o

