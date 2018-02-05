#	2003/09/19	Wolfgang Wang

#	libs
#		01	UnionLog
#		02	mngUnionLog
#		03	logMon

#	2003/09/23
#	libs
#		UnionLog3.0.a
#		mngUnionLog1.0.a
#		mngUnionLog2.0.a
#		logMon1.0.a

HOMEDIR = $(CMM2003DIR)/unionLog
INCDIR = $(HOMEDIR)/include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = $(OSSPEC)

all:	makeall			\
	UnionLog3.0.a		\
	mngUnionLog1.0.a	\
	mngUnionLog2.0.a	\
	unionSystemTime1.0.a	\
	logMon1.0.a		\
	finalProduct		\
	rmTmpFiles


.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

allobjs=unionLogTBL.withoutLogTBL.o
makeall:	$(allobjs)

objs0130 = UnionLog3.0.o unionSystemTime1.0.o unionLogTBL1.0.o
UnionLog3.0.a:	$(objs0130)	
		ar $(OSSPECPACK) rv UnionLog3.0.a $(objs0130)

objs0210 = mngUnionLog1.0.o
mngUnionLog1.0.a:	$(objs0210)	
		ar $(OSSPECPACK) rv mngUnionLog1.0.a $(objs0210)

objs0220 = mngUnionLog2.0.o
mngUnionLog2.0.a:	$(objs0220)	
		ar $(OSSPECPACK) rv mngUnionLog2.0.a $(objs0220)

objs0310 = logMon1.0.o
logMon1.0.a:	$(objs0310)	
		ar $(OSSPECPACK) rv logMon1.0.a $(objs0310)

objs0410 = unionSystemTime1.0.o
unionSystemTime1.0.a:	$(objs0410)	
		ar $(OSSPECPACK) rv unionSystemTime1.0.a $(objs0410)

finalProduct:	$(null)
	# includes
		cp $(INCDIR)UnionLog.h		$(CMM2003INCDIR)
	# libs
		mv UnionLog3.0.a		$(CMM2003LIBDIR)
		mv mngUnionLog1.0.a		$(CMM2003LIBDIR)
		mv mngUnionLog2.0.a		$(CMM2003LIBDIR)
		mv logMon1.0.a			$(CMM2003LIBDIR)
		mv unionSystemTime1.0.a		$(CMM2003LIBDIR)
		mv *.o				$(CMM2003LIBDIR)

rmTmpFiles:	$(null)
#		rm *.o
