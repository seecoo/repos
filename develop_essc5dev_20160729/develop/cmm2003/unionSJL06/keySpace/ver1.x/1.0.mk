#	Wolfgang Wang
#	2003/09/10
#	Version1.0

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

all:	sjl06KeySpace1.0.a		\
	mngKeySpace1.0.a		\
	sjl06SharedKeySpace1.0.a	\
	mngSharedKeySpace1.0.a		\
	sjl06LMK1.0.a			\
	rmTmpFiles			\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(INCL) $(DEF) -c $*.c

# Libs 01
objs0110 =	sjl06KeySpace1.0.o
sjl06KeySpace1.0.a:	$(objs0110)
			ar $(OSSPECPACK) rv sjl06KeySpace1.0.a $(objs0110)

# Libs 02
objs0210 =	mngKeySpace1.0.o
mngKeySpace1.0.a:	$(objs0210)
			ar $(OSSPECPACK) rv mngKeySpace1.0.a $(objs0210)

# Libs 03
objs0310 =	sjl06SharedKeySpace1.0.o
sjl06SharedKeySpace1.0.a:	$(objs0310)
			ar $(OSSPECPACK) rv sjl06SharedKeySpace1.0.a $(objs0310)

# Libs 04
objs0410 =	mngSharedKeySpace1.0.o
mngSharedKeySpace1.0.a:	$(objs0410)
			ar $(OSSPECPACK) rv mngSharedKeySpace1.0.a $(objs0410)

# Libs 05
objs0510 =	sjl06LMK1.0.o
sjl06LMK1.0.a:	$(objs0510)
			ar $(OSSPECPACK) rv sjl06LMK1.0.a $(objs0510)

# rmTmpFiles
rmTmpFiles:	$(null)
		rm	*.o

# finalProducts
finalProducts:	$(null)
	#include
		cp $(INCDIR)sjl06KeySpace.h		$(CMM2003INCDIR)
		cp $(INCDIR)sjl06SharedKeySpace.h	$(CMM2003INCDIR)
		cp $(INCDIR)sjl06LMK.h			$(CMM2003INCDIR)
	#libs
		mv sjl06KeySpace1.0.a		$(CMM2003LIBDIR)
		mv mngKeySpace1.0.a		$(CMM2003LIBDIR)
		mv sjl06SharedKeySpace1.0.a	$(CMM2003LIBDIR)
		mv mngSharedKeySpace1.0.a	$(CMM2003LIBDIR)
		mv sjl06LMK1.0.a		$(CMM2003LIBDIR)

