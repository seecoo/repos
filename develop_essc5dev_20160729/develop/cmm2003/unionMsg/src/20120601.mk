HOMEDIR = $(CMM2003DIR)/unionMsg/
INCDIR = $(HOMEDIR)include/
INCL = -I $(HOMEDIR)include -I $(CMM2003INCDIR) 


DEFINES = $(OSSPEC)

all:	makeall			\
	Product	

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

allobjs=	unionMsg.20120601.o

makeall:	$(allobjs)

Product:
		cp $(INCDIR)/*.h	$(CMM2003INCDIR)
		mv *.o			$(CMM2003LIBDIR)

