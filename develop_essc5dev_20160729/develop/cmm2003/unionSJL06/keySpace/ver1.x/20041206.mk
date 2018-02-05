#	Wolfgang Wang
#	2003/09/10
#	Version.20041206

#	Libs
#	02	mngKeySpace

DEF = -O -DUNIX $(OSSPEC)
HOMEDIR = $(CMM2003DIR)/unionSJL06/
INCDIR = $(HOMEDIR)include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

all:	mngKeySpace.20041206.a		\
	rmTmpFiles			\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(INCL) $(DEF) -c $*.c

# Libs 02
objs0210 =	mngKeySpace.20041206.o
mngKeySpace.20041206.a:	$(objs0210)
			ar $(OSSPECPACK) rv mngKeySpace.20041206.a $(objs0210)

# rmTmpFiles
rmTmpFiles:	$(null)
		rm	*.o

# finalProducts
finalProducts:	$(null)
	#libs
		mv mngKeySpace.20041206.a		$(CMM2003LIBDIR)

