# ����ͨ��ģ��Ŀ¼
CMM2003DIR = $(UNIONLIBDIR)
CMM2003LIBDIR = $(CMM2003DIR)/lib/
CMM2003INCDIR = $(CMM2003DIR)/include

# �����Ʒģ��Ŀ¼
PROLIBDIR=$(UNIONLIBDIR)/libLibs/
PROINCDIR=$(UNIONLIBDIR)/includeLibs/

# ���忪��Ŀ¼
HOMEDIR = $(UNIONLIBDIR)/unionReport
INCDIR = $(HOMEDIR)include/

# ����ͷ�ļ�Ŀ¼�嵥
# ˳��Ӧ���ǣ�
# ͨ��ģ��ͷ�ļ�Ŀ¼/��ǰ��������ͷ�ļ�Ŀ¼/�����̵�ͷ�ļ�Ŀ¼

INCL = -I $(INCDIR) -I $(PROINCDIR) -I $(CMM2003INCDIR)

all:	makeall			\
	libProducts

DEFINES = $(OSSPEC)

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

libProducts:	$(null)
	mv *.o						$(PROLIBDIR)
	cp $(INCDIR)unionReportModule.h			$(PROINCDIR)


objs=	unionReportModule.1.0.o

makeall:	$(objs)

