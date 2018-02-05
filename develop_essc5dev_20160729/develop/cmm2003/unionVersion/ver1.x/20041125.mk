HOMEDIR = $(CMM2003DIR)/unionVersion
INCDIR = $(HOMEDIR)/include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = -D _UNIX $(OSSPEC)

all:	makeall				\
	unionVersion.20041125.a		\
	mvLibs				\
	genEffectiveDaysLib		\
	rmTmpFiles			\
	finalProducts

objs=	unionVersion.20041125.o	\
	genEffectiveDaysLib.20041125.o
makeall:	$(objs)

mvLibs:
		mv unionVersion.20041125.a		$(CMM2003LIBDIR)
		mv *.o					$(CMM2003LIBDIR)
	
.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs0110 = unionVersion.20041125.o
unionVersion.20041125.a:	$(objs0110)	
		ar $(OSSPECPACK) rv unionVersion.20041125.a $(objs0110)

libs0210 =	$(CMM2003LIBDIR)genEffectiveDaysLib.20041125.o	\
		$(CMM2003LIBDIR)unionInput1.0.a	\
		$(CMM2003LIBDIR)UnionStr1.2.a
genEffectiveDaysLib:	
		$(CC) $(OSSPEC) -o genEffectiveDaysLib $(OSSPECLINK) $(objs0210) $(libs0210) $(OSSPECLIBS)

finalProducts:	$(null)
	# includes
		cp $(INCDIR)/unionVersion.h		$(CMM2003INCDIR)
	# libs
#		mv unionVersion.20041125.a		$(CMM2003LIBDIR)
		mv genEffectiveDaysLib			$(CMM2003TOOLSDIR)

rmTmpFiles:	$(null)	
#		rm *.o

