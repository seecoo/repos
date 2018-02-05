#	Wolfgang Wang
#	2003/09/23

#	Libs
#	12	unionMenu
#	13	testMenu

#	2003/09/23
#	libs	
#		unionMenu2.0.a
#		testMenu2.0.a

DEF = -O -DUNIX  $(OSSPEC)
HOMEDIR = $(CMM2003DIR)/unionMenu/
INCDIR = $(HOMEDIR)/include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

all:	unionMenu2.0.a		\
	testMenu2.0.a		\
	testMenu.20060515.o	\
	finalProducts		\
	rmTmpFiles

.SUFFIXES:.c.o
.c.o:
	$(CC) $(INCL) $(DEF) -c $*.c

# Libs 12
objs1220 =	unionMenu2.0.o
unionMenu2.0.a:	$(objs1220)
			ar $(OSSPECPACK) rv unionMenu2.0.a $(objs1220)

# Libs 13
objs1320 =	testMenu2.0.o
testMenu2.0.a:	$(objs1320)
			ar $(OSSPECPACK) rv testMenu2.0.a $(objs1320)

# rmTmpFiles
rmTmpFiles:	$(null)

# finalProducts
finalProducts:	$(null)
	# includes
		cp	$(INCDIR)unionMenu.h			$(CMM2003INCDIR)
	# libs
		mv	unionMenu2.0.a				$(CMM2003LIBDIR)
		mv	testMenu2.0.a				$(CMM2003LIBDIR)mngMenu2.0.a
		mv	testMenu.20060515.o			$(CMM2003LIBDIR)mngMenu.20060515.o
		mv	*.o					$(CMM2003LIBDIR)
