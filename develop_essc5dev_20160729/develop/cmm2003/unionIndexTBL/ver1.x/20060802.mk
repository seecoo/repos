#	2003/09/19	Wolfgang Wang

#	libs
#		01	unionIndexTBL
#		02	mngIndexTBL

HOMEDIR = $(CMM2003DIR)/unionIndexTBL
INCDIR = $(HOMEDIR)/include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = -D _UNIX  $(OSSPEC)

all:	unionIndexTBL.20060802.a	\
	finalProduct		\
	rmTmpFiles


.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs0110 = unionIndexTBL.20060802.o
unionIndexTBL.20060802.a:	$(objs0110)	
		ar $(OSSPECPACK) rv unionIndexTBL.20060802.a $(objs0110)

finalProduct:	$(null)
	# includes
		cp $(INCDIR)unionIndexTBL.h	$(CMM2003INCDIR)
	# libs
		mv unionIndexTBL.20060802.a		$(CMM2003LIBDIR)
		mv *.o				$(CMM2003LIBDIR)

rmTmpFiles:	$(null)
