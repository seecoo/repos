# ���忪��Ŀ¼
HOMEDIR = $(CMM2003DIR)/unionMsgBuf
INCDIR = $(HOMEDIR)/include/
OBJDIR = $(CMM2003LIBDIR)

# ����ͷ�ļ�Ŀ¼�嵥
# ˳��Ӧ���ǣ�
# ͨ��ģ��ͷ�ļ�Ŀ¼/��ǰ��������ͷ�ļ�Ŀ¼/�����̵�ͷ�ļ�Ŀ¼

INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

# ������뿪�ر�������
DEFINES = -D _UNIX  $(OSSPEC)

COMPILE = $(CC) $(DEFINES) $(INCL) -c

srcs =  $(wildcard *.c)
objs =  $(patsubst %.c,%.o,$(srcs))

all:    makeall         \
	tmp

makeall:        $(objs)

.SUFFIXES:.c.o
%.o:%.c
	@echo "CC $(DEFINES) $@ $<"
	@$(COMPILE) $< -o $@

tmp:
	cp      $(INCDIR)unionMsgBuf6.x.h       $(CMM2003INCDIR)
	cp      $(INCDIR)unionUnionMsgBuf.h     $(CMM2003INCDIR)
	mv      *.o				$(CMM2003LIBDIR)
