# ���忪��Ŀ¼
HOMEDIR = $(CMM2003DIR)/unionFileDeal/ver1.x/zip/zipApi/
INCDIR = $(HOMEDIR)include/

# ����ͷ�ļ�Ŀ¼�嵥
# ˳��Ӧ���ǣ�
# ͨ��ģ��ͷ�ļ�Ŀ¼/��ǰ��������ͷ�ļ�Ŀ¼/�����̵�ͷ�ļ�Ŀ¼

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

all:	makeall			\
	libUnionZipAPI.a	\
	libProducts

DEFINES = -D UNIX $(OSSPEC)

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs=	unionZipAPI.1.0.o

makeall:	$(objs)

libUnionZipAPI.a:$(shareobjs)
	ar $(OSSPECPACK) rv libUnionZipAPI.a $(objs)
	
libProducts:	$(null)
	mv libUnionZipAPI.a				$(CMM2003LIBDIR)
	cp ../include/unionZipAPI.h			$(CMM2003INCDIR)
	rm *.o
