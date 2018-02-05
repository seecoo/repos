#	2003/09/19£¬Wolfgang Wang
#	Libs
#		01	unionModule
#		02	mngModule
#	2003/09/23
#	Libs
#		unionModule1.0.a
#		mngModule1.0.a

HOMEDIR = $(CMM2003DIR)/unionModule
INCDIR = $(HOMEDIR)/include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = -D _UNIX  $(OSSPEC)

all:	unionModule1.0.a	\
	mngModule1.0.a		\
	finalProducts		\
	rmTmpFiles

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs0110 = unionModule1.0.o
unionModule1.0.a:	$(objs0110)	
		ar $(OSSPECPACK) rv unionModule1.0.a $(objs0110)

objs0210 = mngModule1.0.o
mngModule1.0.a:	$(objs0210)	
		ar $(OSSPECPACK) rv mngModule1.0.a $(objs0210)

finalProducts:	$(null)
	# includes
		cp $(INCDIR)/unionModule.h	$(CMM2003INCDIR)
	# libs
		mv mngModule1.0.a		$(CMM2003LIBDIR)
		mv unionModule1.0.a		$(CMM2003LIBDIR)

rmTmpFiles:	$(null)	
		rm *.o

