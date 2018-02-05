HOMEDIR = $(CMM2003DIR)/unionSJL06/
INCDIR = $(HOMEDIR)include/

DEFINES = -D _UnionSJL06_2_x_Above_ -D _UnionTask_3_x_ $(OSSPEC)

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

all:	unionSJL06API.20040726.a	\
	sjl06APITest.20040726.a		\
	finalProducts			\
	rmTmpFiles

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c


# Libs
objs0210 = unionSJL06API.20040726.o
unionSJL06API.20040726.a:	$(objs0210)	
			ar $(OSSPECPACK) rv unionSJL06API.20040726.a 	$(objs0210)

objs0310 = sjl06APITest.20040726.o
sjl06APITest.20040726.a:	$(objs0310)	
			ar $(OSSPECPACK) rv sjl06APITest.20040726.a 	$(objs0310)

rmTmpFiles:	$(null)
		rm *.o

finalProducts:	$(null)
	#includes
		cp $(INCDIR)unionSJL06API.h		$(CMM2003INCDIR)
	#bins
		mv unionSJL06API.20040726.a		$(CMM2003LIBDIR)
		mv sjl06APITest.20040726.a		$(CMM2003LIBDIR)

