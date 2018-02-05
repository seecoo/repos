# ���忪��Ŀ¼
HOMEDIR = $(CMM2003DIR)/unionCommCheck
INCDIR = $(HOMEDIR)/include/

# ����ͷ�ļ�Ŀ¼�嵥
# ˳��Ӧ���ǣ�
# ͨ��ģ��ͷ�ļ�Ŀ¼/��ǰ��������ͷ�ļ�Ŀ¼/�����̵�ͷ�ļ�Ŀ¼

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

DEFINES=$(OSSPEC)

all:	commCheck.20040714.a			\
	commCheckMon.20040714.a			\
	commCheckMain.20040714.a		\
	LibProducts				\
	rmobjs

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c


LibProducts:		$(null)
			mv commCheck.20040714.a		$(CMM2003LIBDIR)
			mv commCheckMon.20040714.a	$(CMM2003LIBDIR)
			mv commCheckMain.20040714.a	$(CMM2003LIBDIR)
			cp $(INCDIR)commCheck.h		$(CMM2003INCDIR)

rmobjs:			$(null)
			rm *.o

obj0110 = commCheck.20040714.o
commCheck.20040714.a:	$(obj0110)
			ar $(OSSPECPACK) rv commCheck.20040714.a $(obj0110)

obj0210 = commCheckMon.20040714.o
commCheckMon.20040714.a:	$(obj0210)
			ar $(OSSPECPACK) rv commCheckMon.20040714.a $(obj0210)

obj0310 = commCheckMain.20040714.o
commCheckMain.20040714.a:	$(obj0310)
			ar $(OSSPECPACK) rv commCheckMain.20040714.a $(obj0310)

