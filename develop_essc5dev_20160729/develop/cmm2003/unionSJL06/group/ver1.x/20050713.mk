HOMEDIR = $(CMM2003DIR)/unionSJL06/
INCDIR = $(HOMEDIR)include/
INCL =  -I $(INCDIR) -I $(CMM2003INCDIR)

# ������뿪�ر�������
DEFINES = -D _UnionSJL06_2_x_Above_ $(OSSPEC)

all:	mngSJL06Grp.20050713.a		\
	rmTmpFiles		\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(INCL) $(DEFINES) -c $*.c


# ģ��03 sjl06Grp
objs0310 = mngSJL06Grp.20050713.o
mngSJL06Grp.20050713.a:	$(objs0310)	
			ar $(OSSPECPACK) rv mngSJL06Grp.20050713.a 	$(objs0310)

rmTmpFiles:	$(null)
	# ����Ŀ���ļ�
		rm *.o

finalProducts:	$(null)
	#libs
		mv mngSJL06Grp.20050713.a	$(CMM2003LIBDIR)
		
		
