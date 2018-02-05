#	2003/09/19	Wolfgang Wang

#	libs
#		01	unionIndexTBL
#		02	mngIndexTBL

HOMEDIR = $(CMM2003DIR)/unionIndexTBL
INCDIR = $(HOMEDIR)/include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = -D _UNIX  $(OSSPEC)

all:	unionIndexTBL1.0.a	\
	mngIndexTBL1.0.a	\
	finalProduct		\
	rmTmpFiles


.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs0110 = unionIndexTBL1.0.o
unionIndexTBL1.0.a:	$(objs0110)	
		ar $(OSSPECPACK) rv unionIndexTBL1.0.a $(objs0110)

objs0210 = mngIndexTBL1.0.o
mngIndexTBL1.0.a:	$(objs0210)	
		ar $(OSSPECPACK) rv mngIndexTBL1.0.a $(objs0210)

finalProduct:	$(null)
	# includes
		cp $(INCDIR)unionIndexTBL.h	$(CMM2003INCDIR)
	# libs
		mv unionIndexTBL1.0.a		$(CMM2003LIBDIR)
		mv mngIndexTBL1.0.a		$(CMM2003LIBDIR)
		mv *.o				$(CMM2003LIBDIR)

rmTmpFiles:	$(null)
