#	2003/09/19£¬Wolfgang Wang
#		01	unionLisence
#		02	genStaticLisenceAttr
#		03	mngLisence

HOMEDIR = $(CMM2003DIR)/unionCopyrightRegistry
INCDIR = $(HOMEDIR)/include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = -D _UNIX  $(OSSPEC)

all:	unionLisence3.0.a		\
	genStaticLisenceAttr3.0.a	\
	mngLisence3.0.a			\
	genStaticLisenceAttr3.0		\
	finalProducts			\
	rmTmpFiles

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

0130 = unionLisence3.0.o
unionLisence3.0.a:	$(0130)	
		ar $(OSSPECPACK) rv unionLisence3.0.a $(0130)

objs0230 = genStaticLisenceAttr3.0.o
genStaticLisenceAttr3.0.a:	$(objs0230)	
		ar $(OSSPECPACK) rv genStaticLisenceAttr3.0.a $(objs0230)

objs0330 = mngLisence3.0.o
mngLisence3.0.a:	$(objs0330)	
		ar $(OSSPECPACK) rv mngLisence3.0.a $(objs0330)

genStaticLisenceAttr3.0:	genStaticLisenceAttr3.0.o
		cc -o genStaticLisenceAttr3.0 $(OSSPECLINK) genStaticLisenceAttr3.0.o

finalProducts:	$(null)
	# includes
		cp $(INCDIR)/unionLisence.h		$(CMM2003INCDIR)
	# libs
		mv $(LIBDIR)unionLisence3.0.a		$(CMM2003LIBDIR)
		mv $(LIBDIR)mngLisence3.0.a		$(CMM2003LIBDIR)
		mv $(LIBDIR)genStaticLisenceAttr3.0.a	$(CMM2003LIBDIR)
	# bins
		mv $(BINDIR)genStaticLisenceAttr3.0	$(CMM2003TOOLSDIR)

rmTmpFiles:	$(null)	
		rm *.o

