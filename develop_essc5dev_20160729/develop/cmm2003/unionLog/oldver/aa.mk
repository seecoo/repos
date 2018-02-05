HOMEDIR = $(CMM2003DIR)/unionLog
BINDIR = $(HOMEDIR)/bin/
LIBDIR = $(HOMEDIR)/lib/
INCDIR = $(HOMEDIR)/include

INCL = -I $(HOMEDIR)/include
DEFINES =$(OSSPEC)

all:	UnionLog1.2.a	\
	UnionLog2.0.a	\
	UnionLog2.1.a	\
	UnionLog1.2.withoutTimeFun.o	\
	externLogFun.20080717.o		\
	product 	\
	rmobjs


.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs0112 = UnionLog1.2.o
UnionLog1.2.a:	$(objs0112)	
		ar $(OSSPECPACK) rv UnionLog1.2.a $(objs0112)

objs0120 = UnionLog2.0.o
UnionLog2.0.a:	$(objs0120)	
		ar $(OSSPECPACK) rv UnionLog2.0.a $(objs0120)

objs0121 = UnionLog2.1.o
UnionLog2.1.a:	$(objs0121)	
		ar $(OSSPECPACK) rv UnionLog2.1.a $(objs0121)

product:	$(null)
		mv UnionLog1.2.a 	$(CMM2003LIBDIR)
		mv UnionLog2.0.a	$(CMM2003LIBDIR)
		mv UnionLog2.1.a	$(CMM2003LIBDIR)
		mv *.o			$(CMM2003LIBDIR)
		cp $(INCDIR)/UnionLog.h $(CMM2003INCDIR)

rmobjs:		$(null)
		#rm *.o

