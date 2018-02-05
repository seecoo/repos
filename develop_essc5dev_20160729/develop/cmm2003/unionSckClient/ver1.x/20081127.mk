# ���忪��Ŀ¼
HOMEDIR = $(CMM2003DIR)/unionSckClient/
LIBDIR = $(HOMEDIR)lib/
INCDIR = $(HOMEDIR)include/

# ����ͷ�ļ�Ŀ¼�嵥
# ˳��Ӧ���ǣ�
# ͨ��ģ��ͷ�ļ�Ŀ¼/��ǰ��������ͷ�ļ�Ŀ¼/�����̵�ͷ�ļ�Ŀ¼

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

all:	makeall			\
	libProducts

DEFINES = $(OSSPEC)

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs=	commWithTcpipSvr.useSocket.forThread.20081127.o	\
	commWithTcpipSvr.useSocket.balance.20081127.o	\
	commWithPackSvr.20081127.o			\
	centerREC.useProfile.20081127.o			\
	defaultClientErrCodeTranslater.20081127.o	\
	centerREC.20081127.o				\
	unionCommWithEsscSvr.1.0.o			\
	unionCenterREC.useprofile.1.0.o			\
	UnionAPIForEssc.1.0.o
	
makeall:	$(objs)

libProducts:	$(null)
	mv *.o					$(CMM2003LIBDIR)
	cp $(INCDIR)/commWithPackSvr.h		$(CMM2003INCDIR)
	cp $(INCDIR)/commWithTcpipSvr.h		$(CMM2003INCDIR)
	cp $(INCDIR)/centerREC.h		$(CMM2003INCDIR)
	cp $(INCDIR)/UnionCenterREC.h		$(CMM2003INCDIR)
	
