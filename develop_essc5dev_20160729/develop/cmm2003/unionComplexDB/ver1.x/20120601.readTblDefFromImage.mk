HOMEDIR = $(CMM2003DIR)/unionComplexDB
INCDIR = $(HOMEDIR)/include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = -D _UNIX  $(OSSPEC)

all:	makeall						\
	finalProduct


.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs=	unionComplexDBObjectDef.readTblDefFromImage.20120601.o

makeall:	$(objs)

finalProduct:	$(null)
	mv *.o							$(CMM2003LIBDIR)
