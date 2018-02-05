# 2003/09/22	Wolfgang Wang

#	01	UnionTask
#	02	mngTask
#	03	taskMon

#	2003/09/22
#	libs:
#		unionTask3.0.a
#		mngTask3.0.a
#		taskMon3.0.a

HOMEDIR = $(CMM2003DIR)/unionTask/
INCDIR = $(HOMEDIR)include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = -D _UNIX $(OSSPEC)

all:	unionTask3.0.a		\
	mngTask3.0.a		\
	taskMon3.0.a		\
	unionProc1.0.a		\
	rmTmpFiles		\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

# 01 UnionTask
objs4030 = unionProc1.0.o
unionProc1.0.a:	$(objs4030)	
		ar $(OSSPECPACK) rv unionProc1.0.a $(objs4030)

# 01 UnionTask
objs0130 = unionTask3.0.o unionProc1.0.o
unionTask3.0.a:	$(objs0130)	
		ar $(OSSPECPACK) rv unionTask3.0.a $(objs0130)

# 02 mngTask
objs0230 = mngTask3.0.o
mngTask3.0.a:	$(objs0230)	
		ar $(OSSPECPACK) rv mngTask3.0.a $(objs0230)

# 02 taskMon
objs0330 = taskMon3.0.o
taskMon3.0.a:	$(objs0330)	
		ar $(OSSPECPACK) rv taskMon3.0.a $(objs0330)

finalProducts:	$(null)
	#includes
		cp $(INCDIR)UnionTask.h	$(CMM2003INCDIR)
		cp $(INCDIR)UnionProc.h	$(CMM2003INCDIR)
	#libs
		mv unionTask3.0.a	$(CMM2003LIBDIR)
		mv taskMon3.0.a		$(CMM2003LIBDIR)
		mv mngTask3.0.a		$(CMM2003LIBDIR)
		mv unionProc1.0.a	$(CMM2003LIBDIR)

rmTmpFiles:	$(null)
		mv *.o			$(CMM2003LIBDIR)
