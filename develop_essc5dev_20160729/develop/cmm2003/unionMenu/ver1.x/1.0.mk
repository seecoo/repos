#	Wolfgang Wang
#	2003/05/01

#	Libs
#	01	unionCommand
#	02	unionInput
#	11	unionUserCmd
#	12	unionMenu
#	13	testMenu

#	Bins
#	01	unionMenu

#	2003/09/20	
#		unionCommand2.0.a
#		unionInput1.0.a
#		unionPreDefinedCmd1.0.a

DEF = -O -DUNIX  $(OSSPEC)
HOMEDIR = $(CMM2003DIR)/unionMenu
INCDIR = $(HOMEDIR)/include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

all:	unionCommand1.0.a	\
	unionUserCmd1.0.a	\
	unionMenu1.0.a		\
	testMenu1.0.a		\
	unionCommand2.0.a	\
	unionInput1.0.a		\
	unionPreDefinedCmd1.0.a	\
	rmTmpFiles		\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(INCL) $(DEF) -c $*.c

# Libs 01
objs0110 =	unionCommand1.0.o
unionCommand1.0.a:	$(objs0110)
			ar $(OSSPECPACK) rv unionCommand1.0.a $(objs0110)

# Libs 11
objs1110 =	unionUserCmd1.0.o
unionUserCmd1.0.a:	$(objs1110)
			ar $(OSSPECPACK) rv unionUserCmd1.0.a $(objs1110)

# Libs 12
objs1210 =	unionMenu1.0.o
unionMenu1.0.a:	$(objs1210)
			ar $(OSSPECPACK) rv unionMenu1.0.a $(objs1210)

# Libs 13
objs1310 =	testMenu1.0.o
testMenu1.0.a:	$(objs1310)
			ar $(OSSPECPACK) rv testMenu1.0.a $(objs1310)

# Added by Wolfgang Wang, 2003/09/20
# Libs 01
objs0120 =	unionCommand2.0.o
unionCommand2.0.a:	$(objs0120)
			ar $(OSSPECPACK) rv unionCommand2.0.a $(objs0120)


# Added by Wolfgang Wang, 2003/09/20
# Libs 02
objs0210 =	unionInput1.0.o
unionInput1.0.a:	$(objs0210)
			ar $(OSSPECPACK) rv unionInput1.0.a $(objs0210)

# Added by Wolfgang Wang, 2003/09/20
# Libs 03
objs0310 =	unionPreDefinedCmd1.0.o
unionPreDefinedCmd1.0.a:	$(objs0310)
			ar $(OSSPECPACK) rv unionPreDefinedCmd1.0.a $(objs0310)

# rmTmpFiles
rmTmpFiles:	$(null)

# finalProducts
finalProducts:	$(null)
	# libs
		mv unionCommand1.0.a			$(CMM2003LIBDIR)
		mv unionUserCmd1.0.a			$(CMM2003LIBDIR)
		mv unionMenu1.0.a			$(CMM2003LIBDIR)
		mv unionCommand2.0.a			$(CMM2003LIBDIR)
		mv unionInput1.0.a			$(CMM2003LIBDIR)
		mv unionPreDefinedCmd1.0.a		$(CMM2003LIBDIR)
		mv testMenu1.0.a			$(CMM2003LIBDIR)
		mv *.o					$(CMM2003LIBDIR)
	# includes
		cp	$(INCDIR)unionCommand.h		$(CMM2003INCDIR)
		cp	$(INCDIR)unionMenu.h		$(CMM2003INCDIR)

