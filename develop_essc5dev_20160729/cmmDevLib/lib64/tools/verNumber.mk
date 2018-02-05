#	2003/09/19£¬Wolfgang Wang
#		unionPredefinedVersion

HOMEDIR = $(UNIONLIBDIR)/unionVersion
LIBDIR = $(HOMEDIR)/lib/
INCDIR = $(HOMEDIR)/include/
OBJDIR = $(HOMEDIR)/obj/

COMMLIBDIR = $(UNIONLIBDIR)/lib/
COMMOBJDIR = $(UNIONLIBDIR)/obj/
COMMINCDIR = $(UNIONLIBDIR)/include/
COMMCPPDIR = $(UNIONLIBDIR)/cppFiles/

INCL = -I $(INCDIR) -I $(COMMINCDIR)
DEFINES = -D _UNIX $(OSSPEC)

all:	unionPredefinedVersion.a	\
	rmTmpFiles

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs0110 = unionPredefinedVersion.o
unionPredefinedVersion.a:	$(objs0110)
		ar rv unionPredefinedVersion.a $(objs0110)

rmTmpFiles:	$(null)	
		rm *.o

