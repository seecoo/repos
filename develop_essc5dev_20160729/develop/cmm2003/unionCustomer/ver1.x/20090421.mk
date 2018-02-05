#	Copyright:	Union Tech. Guangzhou
#	Date:		2012/05/24

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


allobjs =	unionKmsLibName.20090421.o	\
		unionSftLibName.20120521.o	\
		cmm2003LibName.20080310.o

makeall:	$(allobjs)

finalProducts:	$(null)
		mv *.o				$(CMM2003LIBDIR)

