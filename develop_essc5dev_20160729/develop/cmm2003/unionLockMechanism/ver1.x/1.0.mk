HOMEDIR = $(CMM2003DIR)/unionLockMechanism/
INCDIR = $(HOMEDIR)include/
INCL = -I $(HOMEDIR)include -I $(CMM2003INCDIR) 


DEFINES = $(OSSPEC)

all:	makeall	\
	finalProduct
.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

allobjs=unionLockTBL.1.0.o	\
	mngLockTBL.1.0.o	\
	lockTBLMon.1.0.o	\
	unionOSLockMachinism.forUnix.useMsgQueue.1.0.o	
makeall:	$(allobjs)

finalProduct:
		cp $(INCDIR)unionLockTBL.h		$(CMM2003INCDIR)
		cp $(INCDIR)unionOSLockMachinism.h	$(CMM2003INCDIR)
		mv *.o					$(CMM2003LIBDIR)
