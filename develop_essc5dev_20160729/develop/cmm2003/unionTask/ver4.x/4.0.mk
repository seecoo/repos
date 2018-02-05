# 2003/09/22	Wolfgang Wang

#	01	UnionTask

#	2003/09/22
#	libs:
#		unionTask4.0.a

HOMEDIR = $(CMM2003DIR)/unionTask/
INCDIR = $(HOMEDIR)include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = -D _UNIX $(OSSPEC)

all:	unionTask4.0.a		\
	rmTmpFiles		\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

# 01 UnionTask
objs0140 = unionTask4.0.o unionProc1.0.o
unionProc1.0.o:	$(HOMEDIR)/ver3.x/unionProc1.0.c
		cc -c $(DEFINES) $(INCL) $(HOMEDIR)/ver3.x/unionProc1.0.c
unionTask4.0.a:	$(objs0140)	
		ar $(OSSPECPACK) rv unionTask4.0.a $(objs0140)

finalProducts:	$(null)
	#includes
		cp	$(INCDIR)UnionTask.h	$(CMM2003INCDIR)
	#libs
		mv	unionTask4.0.a		$(CMM2003LIBDIR)

rmTmpFiles:	$(null)
		rm *.o
