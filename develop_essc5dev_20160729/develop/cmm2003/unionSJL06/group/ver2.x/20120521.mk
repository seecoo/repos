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

objs =	unionHsm.20120521.o		\
	unionHsmGroup.20120521.o	\
	hsmMon.o
	
makeall:	$(objs)

libProducts:	$(null)
	mv *.o				$(CMM2003LIBDIR)
	cp $(INCDIR)unionHsm.h		$(CMM2003INCDIR)
	cp $(INCDIR)unionHsmGroup.h	$(CMM2003INCDIR)

