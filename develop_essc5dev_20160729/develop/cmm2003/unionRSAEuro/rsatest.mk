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

LIBS	=	unionRSAEuro.a	
			

objs =		main.o
testGenDesKeyByPK:		$(objs)
			cc -o test $(objs) $(LIBS)

rmobjs:	
			rm *.o

