HOMEDIR = $(CMM2003DIR)/unionSocket/
INCDIR = $(HOMEDIR)include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = $(OSSPEC)

all:	UnionSocket3.0.a	\
	UnionTCPIPSvr3.0.a	\
	makeall			\
	finalProducts
	

.SUFFIXES:.c.o
.c.o:
	$(CC) $(INCL) $(DEFINES) -c $*.c

allobjs=UnionTCPIPSvr3.0.oldForTest.o	\
	UnionTCPIPSvr3.0.forTest.o
makeall:	$(allobjs)

objs0130 = UnionSocket3.0.o
UnionSocket3.0.a:	$(objs0130)
			ar $(OSSPECPACK) rv UnionSocket3.0.a $(objs0130)

objs0230 = UnionTCPIPSvr3.0.o UnionSocket3.0.o
UnionTCPIPSvr3.0.a:	$(objs0230)
			ar $(OSSPECPACK) rv UnionTCPIPSvr3.0.a $(objs0230)

finalProducts:		$(null)
	#includes
			cp $(INCDIR)UnionSocket.h	$(CMM2003INCDIR)
	#libs
			mv UnionSocket3.0.a	$(CMM2003LIBDIR)
			mv UnionTCPIPSvr3.0.a	$(CMM2003LIBDIR)
			mv *.o			$(CMM2003LIBDIR)
