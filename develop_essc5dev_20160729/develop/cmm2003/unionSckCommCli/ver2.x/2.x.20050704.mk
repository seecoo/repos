HOMEDIR = $(CMM2003DIR)/unionSckCommCli/
INCDIR = $(HOMEDIR)include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = -D _UnionTask_3_x_  $(OSSPEC) -D _UnionSckCli_2_x_
.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c


all:	unionSckCommCli.2.x.20050704.a	\
	commSvrMon.2.x.20050704.a		\
	finalProducts			\
	rmTmpFiles


# Libs
objs0210 = unionSckCommCli.20050704.o
unionSckCommCli.2.x.20050704.a:	$(objs0210)	
			ar $(OSSPECPACK) rv unionSckCommCli.2.x.20050704.a 	$(objs0210)

objs0510 = commSvrMon.20050704.o
commSvrMon.2.x.20050704.a:	$(objs0510)	
			ar $(OSSPECPACK) rv commSvrMon.2.x.20050704.a 	$(objs0510)

rmTmpFiles:	$(null)
		rm *.o

finalProducts:	$(null)
	#includes
		cp $(INCDIR)unionSckCommCli.h		$(CMM2003INCDIR)
	#libs
		mv unionSckCommCli.2.x.20050704.a		$(CMM2003LIBDIR)
		mv commSvrMon.2.x.20050704.a		$(CMM2003LIBDIR)

