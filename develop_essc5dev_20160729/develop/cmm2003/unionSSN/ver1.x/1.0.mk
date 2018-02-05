HOMEDIR = $(CMM2003DIR)/unionSSN/
INCDIR = $(HOMEDIR)include/

INCL = -I $(HOMEDIR)/include -I $(CMM2003INCDIR) $(OSSPEC)

all:	UnionSerialNo1.0.a	\
	UnionSerialNo2.0.a	\
	testSerialNo1.0.a	\
	MngSerialNo1.0.a	\
	Product			\
	rmobjs

.SUFFIXES:.c.o
.c.o:
	$(CC) $(INCL) -c $*.c

objs0110 = 	UnionSerialNo1.0.o
UnionSerialNo1.0.a:	$(objs0110)
		ar $(OSSPECPACK) rv UnionSerialNo1.0.a $(objs0110)

objs0120 = 	UnionSerialNo2.0.o
UnionSerialNo2.0.a:	$(objs0120)
		ar $(OSSPECPACK) rv UnionSerialNo2.0.a $(objs0120)

objs0210 = testSerialNo1.0.o
testSerialNo1.0.a:	$(objs0210)
		ar $(OSSPECPACK) rv testSerialNo1.0.a $(objs0210)

objs0310 = MngSerialNo1.0.o
MngSerialNo1.0.a:	$(objs0310)
		ar $(OSSPECPACK) rv MngSerialNo1.0.a $(objs0310)

Product:	$(null)
		cp $(INCDIR)UnionSerialNo.h	$(CMM2003INCDIR)
		mv UnionSerialNo1.0.a		$(CMM2003LIBDIR)
		mv testSerialNo1.0.a		$(CMM2003LIBDIR)
		mv MngSerialNo1.0.a		$(CMM2003LIBDIR)
		mv UnionSerialNo2.0.a		$(CMM2003LIBDIR)

rmobjs:		$(null)
		rm *.o
