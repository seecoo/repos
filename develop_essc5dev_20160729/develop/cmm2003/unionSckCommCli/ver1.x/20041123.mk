HOMEDIR = $(CMM2003DIR)/unionSckCommCli/
INCDIR = $(HOMEDIR)include/

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = -D _UnionTask_3_x_  $(OSSPEC)
.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c


all:	unionSckCommCli.20041123.selfBalance.a	\
	unionSckCommCli.20041123.a	\
	mngCommSvr.20041123.a		\
	unionCommSvrSample.20041123.a	\
	commSvrMon.20041123.a		\
	finalProducts			\
	rmTmpFiles


# Libs
objs0110 = unionSckCommCli.20041123.selfBalance.o
unionSckCommCli.20041123.selfBalance.a:	$(unionSckCommCli.20041123.c)
			cc -c $(INCL) $(DEFINES) -D _selfBalanceSckSvr_ unionSckCommCli.20041123.c
			mv unionSckCommCli.20041123.o unionSckCommCli.20041123.selfBalance.o	
			ar $(OSSPECPACK) rv unionSckCommCli.20041123.selfBalance.a 	$(objs0110)

objs0210 = unionSckCommCli.20041123.o
unionSckCommCli.20041123.a:	$(objs0210)	
			ar $(OSSPECPACK) rv unionSckCommCli.20041123.a 	$(objs0210)

objs0310 = mngCommSvr.20041123.o
mngCommSvr.20041123.a:	$(objs0310)	
			ar $(OSSPECPACK) rv mngCommSvr.20041123.a 	$(objs0310)

objs0410 = unionCommSvrSample.20041123.o
unionCommSvrSample.20041123.a:	$(objs0410)	
			ar $(OSSPECPACK) rv unionCommSvrSample.20041123.a 	$(objs0410)

objs0510 = commSvrMon.20041123.o
commSvrMon.20041123.a:	$(objs0510)	
			ar $(OSSPECPACK) rv commSvrMon.20041123.a 	$(objs0510)

rmTmpFiles:	$(null)
		rm *.o

finalProducts:	$(null)
	#includes
		cp $(INCDIR)unionSckCommCli.h			$(CMM2003INCDIR)
	#libs
		mv unionSckCommCli.20041123.a			$(CMM2003LIBDIR)
		mv mngCommSvr.20041123.a			$(CMM2003LIBDIR)
		mv unionCommSvrSample.20041123.a		$(CMM2003LIBDIR)
		mv commSvrMon.20041123.a			$(CMM2003LIBDIR)
		mv unionSckCommCli.20041123.selfBalance.a	$(CMM2003LIBDIR)
