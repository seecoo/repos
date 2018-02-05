#	Author:		Wolfgang Wang
#	Copyright:	Union Tech. Guangzhou
#	Date:		2003/09/12

#	Modules:
#		03	sjl06Grp
#		98	mngSJL06Grp
#		99	sjl06GrpMon

HOMEDIR = $(CMM2003DIR)/unionCustomer/
INCDIR = $(HOMEDIR)include/
INCL =  -I $(INCDIR) -I $(CMM2003INCDIR)

# 这个编译开关必须增加
DEFINES = $(OSSPEC)

all:	makeall			\
	finalProducts

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c


allobjs = unionEsscLibName.20060726.o	\
	unionEsscLibName.20060808.o   

makeall:	$(allobjs)

finalProducts:	$(null)
		mv *.o				$(CMM2003LIBDIR)

		
