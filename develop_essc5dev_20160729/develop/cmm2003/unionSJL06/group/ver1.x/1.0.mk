HOMEDIR = $(CMM2003DIR)/unionSJL06/
INCDIR = $(HOMEDIR)include/
INCL =  -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = $(OSSPEC)

all:	sjl06Grp1.0.a		\
	mngSJL06Grp1.0.a	\
	sjl06GrpMon1.0.a	\
	rmTmpFiles		\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(INCL) $(DEFINES) -c $*.c


# Ä£¿é03 sjl06Grp
objs0310 = sjl06Grp1.0.o
sjl06Grp1.0.a:	$(objs0310)	
			ar $(OSSPECPACK) rv sjl06Grp1.0.a 	$(objs0310)

objs9810 = mngSJL06Grp1.0.o
mngSJL06Grp1.0.a:	$(objs9810)
			ar $(OSSPECPACK) rv mngSJL06Grp1.0.a $(objs9810)

objs9910 = sjl06GrpMon1.0.o
sjl06GrpMon1.0.a:	$(objs9910)
			ar $(OSSPECPACK) rv sjl06GrpMon1.0.a $(objs9910)

rmTmpFiles:	$(null)
		rm *.o

finalProducts:	$(null)
	#includes
		cp $(INCDIR)sjl06Grp.h		$(CMM2003INCDIR)
	#libs
		mv sjl06Grp1.0.a		$(CMM2003LIBDIR)
		mv sjl06GrpMon1.0.a		$(CMM2003LIBDIR)
		mv mngSJL06Grp1.0.a		$(CMM2003LIBDIR)
		
