HOMEDIR = $(CMM2003DIR)/unionCommConf
INCDIR = $(HOMEDIR)/include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = -D _UNIX  $(OSSPEC) -D _longConnServerMustHaveConns_

all:	makeall						\
	finalProduct					\
	rmTmpFiles


.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

allobjs=unionCommConf.20051009.o			\
	mngCommConf.20051009.o				\
	commConfMon.20051009.o
makeall:	$(allobjs)


finalProduct:	$(null)
		mv *.o					$(CMM2003LIBDIR)
		cp $(INCDIR)unionCommConf.h		$(CMM2003INCDIR)

rmTmpFiles:	$(null)
