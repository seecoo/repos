#	2001/09/19£¬Wolfgang Wang
#		01	unionREC
#		02	mngREC
#		03	recMon

HOMEDIR = $(CMM2003DIR)/unionREC
INCDIR = $(HOMEDIR)/include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = -D _UNIX $(OSSPEC)

all:	makeall		\
	unionREC1.0.a	\
	mngREC1.0.a	\
	recMon1.0.a	\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs=	unionREC1.0.o	\
	mngREC1.0.o	\
	recMon1.0.o	\
	unionVarListDef.1.0.o

makeall:	$(objs)

objs0110 = unionREC1.0.o
unionREC1.0.a:	$(objs0110)	
		ar $(OSSPECPACK) rv unionREC1.0.a $(objs0110)

objs0210 = mngREC1.0.o
mngREC1.0.a:	$(objs0210)	
		ar $(OSSPECPACK) rv mngREC1.0.a $(objs0210)

objs0310 = recMon1.0.o
recMon1.0.a:	$(objs0310)	
		ar $(OSSPECPACK) rv recMon1.0.a $(objs0310)

finalProducts:	$(null)
# includes
		cp $(INCDIR)/unionREC.h		$(CMM2003INCDIR)
		cp $(INCDIR)/unionVarListDef.h		$(CMM2003INCDIR)
# libs
		mv unionREC1.0.a		$(CMM2003LIBDIR)
		mv mngREC1.0.a			$(CMM2003LIBDIR)
		mv recMon1.0.a			$(CMM2003LIBDIR)
		mv *.o				$(CMM2003LIBDIR)
