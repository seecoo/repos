HOMEDIR = $(CMM2003DIR)/unionSJL06/
INCDIR = $(HOMEDIR)include/
INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = $(OSSPEC)

all:	genKeyComponent2.1.a		\
	rmTmpFiles			\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c


# Libs
objs0321 = genKeyComponent2.1.o
genKeyComponent2.1.a:	$(objs0321)	
			ar $(OSSPECPACK) rv genKeyComponent2.1.a 	$(objs0321)

rmTmpFiles:	$(null)
		rm *.o

finalProducts:	$(null)
	#libs
		mv genKeyComponent2.1.a			$(CMM2003LIBDIR)

