#	2003/09/19	Wolfgang Wang

#	libs
#		01	UnionLog
#		02	mngUnionLog
#		03	logMon

#	2003/09/23
#	libs
#		UnionLog3.0.20041209.a
#		mngUnionLog1.0.a
#		mngUnionLog2.0.20041209.a
#		logMon1.0.a

HOMEDIR = $(CMM2003DIR)/unionLog
INCDIR = $(HOMEDIR)/include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = $(OSSPEC)

all:	UnionLog3.0.20041209.a		\
	finalProduct			\
	rmTmpFiles


.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs0130 = UnionLog3.0.o unionSystemTime1.0.o unionLogTBL1.0.20041209.o
UnionLog3.0.20041209.a:	$(objs0130)	
		ar $(OSSPECPACK) rv UnionLog3.0.20041209.a $(objs0130)


finalProduct:	$(null)
	# includes
		cp $(INCDIR)UnionLog.h		$(CMM2003INCDIR)
	# libs
		mv UnionLog3.0.20041209.a	$(CMM2003LIBDIR)

rmTmpFiles:	$(null)
		rm *.o
