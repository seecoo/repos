HOMEDIR = $(CMM2003DIR)/unionPackage
INCDIR = $(HOMEDIR)/include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = -D _UNIX  $(OSSPEC)

all:	makeall			\
	libProducts

DEFINES = $(OSSPEC)

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs=	unionPackageDef.1.0.o
makeall:	$(objs)


libProducts:	$(null)
	mv *.o					$(CMM2003LIBDIR)
	cp $(INCDIR)unionPackageDef.h		$(CMM2003INCDIR)
