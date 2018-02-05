HOMEDIR = $(CMM2003DIR)/unionSJL06/
INCDIR = $(HOMEDIR)include/
INCL =  -I $(INCDIR) -I $(CMM2003INCDIR)

# ������뿪�ر�������
DEFINES = -D _UnionSJL06_2_x_Above_  $(OSSPEC)

all:	sjl06Grp2.0.a		\
	mngSJL06Grp2.0.a	\
	sjl06GrpMon2.0.a	\
	rmTmpFiles		\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(INCL) $(DEFINES) -c $*.c


# ģ��03 sjl06Grp
objs0310 = sjl06Grp2.0.o
sjl06Grp2.0.a:	$(objs0310)	
			ar $(OSSPECPACK) rv sjl06Grp2.0.a 	$(objs0310)

objs9810 = mngSJL06Grp1.0.o
mngSJL06Grp2.0.a:	$(objs9810)
			ar $(OSSPECPACK) rv mngSJL06Grp2.0.a $(objs9810)

objs9910 = sjl06GrpMon2.0.o
sjl06GrpMon2.0.a:	$(objs9910)
			ar $(OSSPECPACK) rv sjl06GrpMon2.0.a $(objs9910)

rmTmpFiles:	$(null)
	# ����Ŀ���ļ�
		rm *.o

finalProducts:	$(null)
	#includes
		cp $(INCDIR)sjl06Grp.h		$(CMM2003INCDIR)
	#libs
		mv sjl06Grp2.0.a		$(CMM2003LIBDIR)
		mv sjl06GrpMon2.0.a		$(CMM2003LIBDIR)
		mv mngSJL06Grp2.0.a		$(CMM2003LIBDIR)
		
		
