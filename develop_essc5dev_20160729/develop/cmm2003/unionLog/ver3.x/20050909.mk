#	2003/09/19	Wolfgang Wang

#	libs
#		01	UnionLog
#		02	mngUnionLog
#		03	logMon

#	2003/09/23
#	libs
#		UnionLog.20050909.a
#		mngUnionLog1.0.a
#		mngUnionLog2.0.a
#		logMon1.0.a

HOMEDIR = $(CMM2003DIR)/unionLog
INCDIR = $(HOMEDIR)/include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = $(OSSPEC)

all:	UnionLog.20050909.a		\
	finalProduct		\
	rmTmpFiles


.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs0130 = UnionLog.20050909.o unionSystemTime1.0.o unionLogTBL.20050909.o
UnionLog.20050909.a:	$(objs0130)	
		ar $(OSSPECPACK) rv UnionLog.20050909.a $(objs0130)

finalProduct:	$(null)
	# includes
		cp $(INCDIR)UnionLog.h		$(CMM2003INCDIR)
	# libs
		mv UnionLog.20050909.a		$(CMM2003LIBDIR)

rmTmpFiles:	$(null)
		rm *.o
