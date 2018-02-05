HOMEDIR = $(CMM2003DIR)/unionSJL06/
INCDIR = $(HOMEDIR)include/
INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = $(OSSPEC)

all:	sjl06CmdForJK.IC.RSA.1.0.a			\
	sjl06CmdForNewRacal.1.0.a			\
	rmTmpFiles					\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c


# Libs
# Ä£¿é02 sjl06CmdForJK.IC.RSA.
objs0210 = sjl06CmdForJK.IC.RSA.1.0.o
sjl06CmdForJK.IC.RSA.1.0.a:	$(objs0210)	
			ar $(OSSPECPACK) rv sjl06CmdForJK.IC.RSA.1.0.a 	$(objs0210)

objs0110 = sjl06CmdForNewRacal.1.0.o 3DesRacalCmd1.0.o 3DesRacalSyntaxRules1.0.o
sjl06CmdForNewRacal.1.0.a:	$(objs0110)	
			ar $(OSSPECPACK) rv sjl06CmdForNewRacal.1.0.a 	$(objs0110)


rmTmpFiles:	$(null)
#		rm *.o

finalProducts:	$(null)
	#include
		cp $(INCDIR)sjl06Cmd.h				$(CMM2003INCDIR)
		cp $(INCDIR)3DesRacalCmd.h			$(CMM2003INCDIR)
		cp $(INCDIR)3DesRacalSyntaxRules.h		$(CMM2003INCDIR)
	#libs
		mv sjl06CmdForJK.IC.RSA.1.0.a		$(CMM2003LIBDIR)
		mv sjl06CmdForNewRacal.1.0.a		$(CMM2003LIBDIR)
		mv *.o					$(CMM2003LIBDIR)
