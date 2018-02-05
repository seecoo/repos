HOMEDIR = $(CMM2003DIR)/unionSocket/
INCDIR = $(HOMEDIR)include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = $(OSSPEC)

all:	UnionTCPIPSvr4.0.20061108.a	\
	finalProducts
	

.SUFFIXES:.c.o
.c.o:
	$(CC) $(INCL) $(DEFINES) -c $*.c

objs0230 = UnionTCPIPSvr4.0.20061108.o UnionSocket3.0.o
UnionTCPIPSvr4.0.20061108.a:	$(objs0230)
			ar $(OSSPECPACK) rv UnionTCPIPSvr4.0.20061108.a $(objs0230)

finalProducts:		$(null)
			mv UnionTCPIPSvr4.0.20061108.a	$(CMM2003LIBDIR)
			mv *.o				$(CMM2003LIBDIR)
