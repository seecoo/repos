HOMEDIR = $(CMM2003DIR)/unionTraceInfo/
INCDIR = $(HOMEDIR)include/
INCL = -I $(HOMEDIR)include -I $(CMM2003INCDIR) 


DEFINES = $(OSSPEC)

all:	UnionTraceInfo1.0.a \
	Product	

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c


objs0110 = UnionTraceInfo1.0.o
UnionTraceInfo1.0.a:	$(objs0110)
		ar $(OSSPECPACK) rv UnionTraceInfo1.0.a $(objs0110)

Product:
		cp $(INCDIR)unionTraceInfo.h	$(CMM2003INCDIR)
		mv UnionTraceInfo1.0.a		$(CMM2003LIBDIR)
		mv *.o				$(CMM2003LIBDIR)
