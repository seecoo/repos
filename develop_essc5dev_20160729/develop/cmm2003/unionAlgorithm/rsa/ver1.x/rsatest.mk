COMMDIR = $(UNIONLIBDIR)
COMMLIBDIR = $(COMMDIR)/lib/
COMMINCDIR = $(COMMDIR)/include

HOMEDIR = $(CMM2003DIR)/unionAlgorithm/rsa/
INCDIR = $(HOMEDIR)include/
INCL = -I $(INCDIR) -I $(COMMINCDIR) 
ThirdPartyObjsDir=$(HOMEDIR)/thirdPartyObjs/

# ������ϵͳ���Ǹ��ֽ���ǰʱ���򿪸ñ��뿪�ء�
# �ַ�HP-UNIX����������ϵͳ
# DEFINES = -D HIGHFIRST $(OSSPEC)
DEFINES = $(OSSPEC)

all:	testGenDesKeyByPK rmobjs

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

LIBS	=	$(COMMLIBDIR)/unionRSA.20050118.a	
			

objs =		testGenDesKeyByPK.o
testGenDesKeyByPK:		$(objs)
			cc -o testGenDesKeyByPK $(objs) $(LIBS)

rmobjs:	
			rm *.o

