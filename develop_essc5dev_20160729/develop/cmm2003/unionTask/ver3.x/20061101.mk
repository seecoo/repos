# 2003/09/22	Wolfgang Wang

#	01	UnionTask

#	2003/09/22
#	libs:
#		unionTask.20061101.a

HOMEDIR = $(CMM2003DIR)/unionTask/
INCDIR = $(HOMEDIR)include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = -D _UNIX $(OSSPEC)

all:	unionTask.20061101.a		\
	makeall				\
	rmTmpFiles			\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

# 01 UnionTask
objs0130 = unionTask.20061101.o unionProc1.0.o
unionTask.20061101.a:	$(objs0130)	
		ar $(OSSPECPACK) rv unionTask.20061101.a $(objs0130)

objs=mngTask.20061101.o
makeall:	$(objs)
		
finalProducts:	$(null)
	#includes
		cp	$(INCDIR)UnionTask.h	$(CMM2003INCDIR)
	#libs
		mv	unionTask.20061101.a	$(CMM2003LIBDIR)
		mv	*.o			$(CMM2003LIBDIR)

rmTmpFiles:	$(null)
