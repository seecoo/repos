HOMEDIR = $(CMM2003DIR)/unionSJL06/
INCDIR = $(HOMEDIR)include/
INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = $(OSSPEC)

all:	sjl061.0.a			\
	mngSJL061.0.a			\
	rmTmpFiles			\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c


# Libs
# Ä£¿é02 sjl06
objs0210 = sjl061.0.o
sjl061.0.a:	$(objs0210)	
			ar $(OSSPECPACK) rv sjl061.0.a 	$(objs0210)

# Ä£¿é04 mngSJL06
objs0410 = mngSJL061.0.o
mngSJL061.0.a:	$(objs0410)	
			ar $(OSSPECPACK) rv mngSJL061.0.a 	$(objs0410)

rmTmpFiles:	$(null)
		rm *.o

finalProducts:	$(null)
	#include
		cp $(INCDIR)sjl06.h		$(CMM2003INCDIR)
	#libs
		mv sjl061.0.a			$(CMM2003LIBDIR)
		mv mngSJL061.0.a		$(CMM2003LIBDIR)
