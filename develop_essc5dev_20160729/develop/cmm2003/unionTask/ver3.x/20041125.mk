# 2003/09/22	Wolfgang Wang

#	01	UnionTask

#	2003/09/22
#	libs:
#		unionTask3.2.20041125.a

HOMEDIR = $(CMM2003DIR)/unionTask/
INCDIR = $(HOMEDIR)include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = -D _UNIX $(OSSPEC)

all:	unionTask3.2.20041125.a		\
	mngTask.20050706.a		\
	rmTmpFiles			\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

# 01 UnionTask
objs0130 = unionTask3.2.20041125.o unionProc1.0.o
unionTask3.2.20041125.a:	$(objs0130)	
		ar $(OSSPECPACK) rv unionTask3.2.20041125.a $(objs0130)

objs0230 = mngTask.20050706.o
mngTask.20050706.a:	$(objs0230)	
		ar $(OSSPECPACK) rv mngTask.20050706.a $(objs0230)

finalProducts:	$(null)
	#includes
		cp	$(INCDIR)UnionTask.h	$(CMM2003INCDIR)
	#libs
		mv	unionTask3.2.20041125.a	$(CMM2003LIBDIR)
		mv	mngTask.20050706.a	$(CMM2003LIBDIR)

rmTmpFiles:	$(null)
		rm *.o
