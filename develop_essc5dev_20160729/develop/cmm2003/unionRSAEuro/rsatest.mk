COMMDIR = $(UNIONLIBDIR)
COMMLIBDIR = $(COMMDIR)/lib/
COMMINCDIR = $(COMMDIR)/include

HOMEDIR = $(CMM2003DIR)/unionAlgorithm/rsa/
INCDIR = $(HOMEDIR)include/
INCL = -I $(INCDIR) -I $(COMMINCDIR) 
ThirdPartyObjsDir=$(HOMEDIR)/thirdPartyObjs/

# 当操作系统是是高字节在前时，打开该编译开关。
# 浦发HP-UNIX即是这样的系统
# DEFINES = -D HIGHFIRST $(OSSPEC)
DEFINES = $(OSSPEC)

all:	testGenDesKeyByPK rmobjs

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

LIBS	=	unionRSAEuro.a	
			

objs =		main.o
testGenDesKeyByPK:		$(objs)
			cc -o test $(objs) $(LIBS)

rmobjs:	
			rm *.o

