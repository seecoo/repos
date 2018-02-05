# 2003/09/22	Wolfgang Wang

#	01	UnionTask

#	2003/09/22
#	libs:
#		unionTask.20050919.noIgnoreSigCLD.a

HOMEDIR = $(CMM2003DIR)/unionTask/
INCDIR = $(HOMEDIR)include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = -D _UNIX $(OSSPEC)

all:	unionTask.20050919.noIgnoreSigCLD.a		\
	rmTmpFiles			\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

# 01 UnionTask
objs0130 = unionTask.20050919.o unionProc.20061116.o
unionTask.20050919.noIgnoreSigCLD.a:	$(objs0130)	
		ar $(OSSPECPACK) rv unionTask.20050919.noIgnoreSigCLD.a $(objs0130)

finalProducts:	$(null)
	#includes
		cp	$(INCDIR)UnionTask.h	$(CMM2003INCDIR)
	#libs
		mv	unionTask.20050919.noIgnoreSigCLD.a	$(CMM2003LIBDIR)

rmTmpFiles:	$(null)
		mv *.o						$(CMM2003LIBDIR)
