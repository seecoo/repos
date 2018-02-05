# 2003/09/22	Wolfgang Wang

#	01	UnionTask

#	2003/09/22
#	libs:
#		unionTask3.1.a

HOMEDIR = $(CMM2003DIR)/unionTask/
INCDIR = $(HOMEDIR)include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = -D _UNIX $(OSSPEC)

all:	unionTask3.1.a		\
	rmTmpFiles		\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

# 01 UnionTask
objs0130 = unionTask3.1.o unionProc1.0.o
unionTask3.1.a:	$(objs0130)	
		ar $(OSSPECPACK) rv unionTask3.1.a $(objs0130)

libProducts:	$(null)
		mv	unionTask3.1.a		$(LIBDIR)

finalProducts:	$(null)
	#includes
		cp $(INCDIR)UnionTask.h	$(CMM2003INCDIR)
	#libs
		mv unionTask3.1.a	$(CMM2003LIBDIR)

rmTmpFiles:	$(null)
		rm *.o
