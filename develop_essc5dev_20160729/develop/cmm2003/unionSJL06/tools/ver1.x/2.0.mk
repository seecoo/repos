HOMEDIR = $(CMM2003DIR)/unionSJL06/
INCDIR = $(HOMEDIR)include/
INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = $(OSSPEC)

all:	genKeyComponent2.0.a		\
	rmTmpFiles			\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c


# Libs
objs0320 = genKeyComponent2.0.o
genKeyComponent2.0.a:	$(objs0320)	
			ar $(OSSPECPACK) rv genKeyComponent2.0.a 	$(objs0320)

rmTmpFiles:	$(null)
		rm *.o

finalProducts:	$(null)
	#libs
		mv genKeyComponent2.0.a		$(CMM2003LIBDIR)

