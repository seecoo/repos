# ���忪��Ŀ¼
HOMEDIR = $(CMM2003DIR)/unionVersionControl/
INCDIR = $(HOMEDIR)include/

# ����ͷ�ļ�Ŀ¼�嵥
# ˳��Ӧ���ǣ�
# ͨ��ģ��ͷ�ļ�Ŀ¼/��ǰ��������ͷ�ļ�Ŀ¼/�����̵�ͷ�ļ�Ŀ¼

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)
DEFINES = $(OSSPEC)

all:	makeall			\
	libProducts


.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs=	zgyh2006LibName.20080310.o
makeall:	$(objs)

libProducts:	$(null)
	mv *.o				$(CMM2003LIBDIR)
