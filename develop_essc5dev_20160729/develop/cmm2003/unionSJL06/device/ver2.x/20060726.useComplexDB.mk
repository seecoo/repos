HOMEDIR = $(CMM2003DIR)/unionSJL06/
INCDIR = $(HOMEDIR)include/
INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = $(OSSPEC) -D _UnionSJL06_2_x_Above_ -D _useComplexDB_

all:	makeall			\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

allobjs=sjl062.0.20060726.o
makeall:	$(allobjs)

finalProducts:	$(null)
		cp $(INCDIR)sjl06.h	$(CMM2003INCDIR)
		mv sjl062.0.20060726.o	$(CMM2003LIBDIR)sjl062.0.20060726.useComplexDB.o
