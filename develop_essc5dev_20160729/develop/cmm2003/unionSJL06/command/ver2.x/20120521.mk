HOMEDIR = $(CMM2003DIR)/unionSJL06/
INCDIR = $(HOMEDIR)include/
INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

# 这个编译开关必须增加
DEFINES = $(OSSPEC)

all:	makeall				\
	libProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs =	commWithHsmSvr.20120521.o		\
	commWithHsm.20120521.o			\
	emv2000AndPbocSyntaxRules.20120521.o	\
	unionHsmCmd.20120521.o			\
	unionHsmCmd.sjj1127.20120521.o		\
	hsmDefaultFunction.20120521.o
	
makeall:	$(objs)

libProducts:	$(null)
	mv *.o					$(CMM2003LIBDIR)
	cp $(INCDIR)commWithHsmSvr.h		$(CMM2003INCDIR)
	cp $(INCDIR)unionHsmCmd.h		$(CMM2003INCDIR)
	cp $(INCDIR)unionSJJ1127Cmd.h		$(CMM2003INCDIR)
	cp $(INCDIR)hsmDefaultFunction.h	$(CMM2003INCDIR)

