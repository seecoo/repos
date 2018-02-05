HOMEDIR = $(CMM2003DIR)/unionSJL06/
INCDIR = $(HOMEDIR)include/
INCL = -I $(INCDIR) -I $(CMM2003INCDIR)

# 这个编译开关必须增加
DEFINES = -D _UnionSJL06_2_x_Above_ -D _Suport3Des_ $(OSSPEC)


all:	sjl06CmdForJK.IC.RSA.for3Des.20050712.useDesKey3.xnonSJL06Server.a		\
	rmTmpFiles								\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c


objs0310 = sjl06CmdForJK.IC.RSA.for3Des.20050712.nonSJL06Server.o
sjl06CmdForJK.IC.RSA.for3Des.20050712.useDesKey3.xnonSJL06Server.a:	$(objs0310)	
			ar $(OSSPECPACK) rv sjl06CmdForJK.IC.RSA.for3Des.20050712.useDesKey3.xnonSJL06Server.a 	$(objs0310)

rmTmpFiles:	$(null)
		rm *.o

finalProducts:	$(null)
	#include
		cp $(INCDIR)sjl06Cmd.h								$(CMM2003INCDIR)
	#libs
		mv sjl06CmdForJK.IC.RSA.for3Des.20050712.useDesKey3.xnonSJL06Server.a		$(CMM2003LIBDIR)
