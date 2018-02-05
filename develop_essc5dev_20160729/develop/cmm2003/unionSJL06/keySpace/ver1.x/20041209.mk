#	Wolfgang Wang
#	2003/09/10
#	Version1.0.20041209

#	Libs
#	01	sjl06KeySpace
#	02	mngKeySpace
#	03	sjl06SharedKeySpace
#	04	mngSharedKeySpace
#	05	sjl06LMK

DEF = -O -DUNIX $(OSSPEC)
HOMEDIR = $(CMM2003DIR)/unionSJL06/
INCDIR = $(HOMEDIR)include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

all:	sjl06SharedKeySpace1.0.20041209.a	\
	sjl06LMK1.0.20041209.a			\
	rmTmpFiles				\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(INCL) $(DEF) -c $*.c

# Libs 03
objs0310 =	sjl06SharedKeySpace1.0.20041209.o
sjl06SharedKeySpace1.0.20041209.a:	$(objs0310)
			ar $(OSSPECPACK) rv sjl06SharedKeySpace1.0.20041209.a $(objs0310)

# Libs 05
objs0510 =	sjl06LMK1.0.20041209.o
sjl06LMK1.0.20041209.a:	$(objs0510)
			ar $(OSSPECPACK) rv sjl06LMK1.0.20041209.a $(objs0510)

# rmTmpFiles
rmTmpFiles:	$(null)
		rm	*.o

# finalProducts
finalProducts:	$(null)
	#include
		cp $(INCDIR)sjl06SharedKeySpace.h	$(CMM2003INCDIR)
		cp $(INCDIR)sjl06LMK.h			$(CMM2003INCDIR)
	#libs
		mv sjl06SharedKeySpace1.0.20041209.a	$(CMM2003LIBDIR)
		mv sjl06LMK1.0.20041209.a		$(CMM2003LIBDIR)

