HOMEDIR = $(CMM2003DIR)/unionSJL06/
INCDIR = $(HOMEDIR)include/
INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

# ������뿪�ر�������
DEFINES = -D _UnionSJL06_2_x_Above_ $(OSSPEC)

all:	sjl06Protocol.20050620.useSJL06_2_x.a		\
	rmTmpFiles			\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c


# Libs
# ģ��02 sjl06Protocol
objs0210 = sjl06Protocol.20050620.o
sjl06Protocol.20050620.useSJL06_2_x.a:	$(objs0210)	
			ar $(OSSPECPACK) rv sjl06Protocol.20050620.useSJL06_2_x.a 	$(objs0210)

rmTmpFiles:	$(null)
	# ����Ŀ���ļ�
		rm *.o

finalProducts:	$(null)
	#includes
		cp $(INCDIR)sjl06Protocol.h		$(CMM2003INCDIR)
	#bins
		mv sjl06Protocol.20050620.useSJL06_2_x.a			$(CMM2003LIBDIR)

