#	Libs:
#		01	unionCopyrightRegistry
#		02	mngLisence

HOMEDIR = $(CMM2003DIR)/unionCopyrightRegistry
INCDIR = $(HOMEDIR)/include

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = -D _UNIX  $(OSSPEC)

all:	unionCopyrightRegistry1.0.a	\
	mngLisence1.0.a			\
	LibProducts			\
	rmobjs


.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs0110 = unionCopyrightRegistry1.0.o
unionCopyrightRegistry1.0.a:	$(objs0110)	
		ar $(OSSPECPACK) rv unionCopyrightRegistry1.0.a $(objs0110)

objs0210 = mngLisence1.0.o
mngLisence1.0.a:	$(objs0210)	
		ar $(OSSPECPACK) rv mngLisence1.0.a $(objs0210)

LibProducts:	$(null)
		mv unionCopyrightRegistry1.0.a		$(CMM2003LIBDIR)
		mv mngLisence1.0.a			$(CMM2003LIBDIR)
		cp $(INCDIR)/unionCopyrightRegistry.h 	$(CMM2003INCDIR)

rmobjs:		$(null)
		rm *.o
