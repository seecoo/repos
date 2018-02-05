HOMEDIR = $(CMM2003DIR)/unionAlgorithm/
INCDIR = $(HOMEDIR)include/
INCL = -I $(INCDIR) -I $(CMM2003INCDIR) 
ThirdPartyObjsDir=$(HOMEDIR)/thirdPartyObjs/

# 当操作系统是是高字节在前时，打开该编译开关。
# 浦发HP-UNIX即是这样的系统
# DEFINES = -D HIGHFIRST $(OSSPEC)
DEFINES = $(OSSPEC)

all:	unionAlgorithm.20040706.withoutBaseDes.a Product rmobjs

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

objs10 =	UnionDes.20050706.o		\
		UnionDataExch1.0.o	\
		UnionBaseDes.20050222.o	\
		UnionMAC1.0.o		\
		UnionXOR1.0.o		\
		UnionMD5_1.0.o		\
		UnionSHA1_1.0.o		\
		base64.o		\
		unionPBOCKeyGenerate.20050706.o

unionAlgorithm.20040706.withoutBaseDes.a:	$(objs10)
			ar $(OSSPECPACK) rv unionAlgorithm.20040706.withoutBaseDes.a $(objs10)

Product:
			mv unionAlgorithm.20040706.withoutBaseDes.a		$(CMM2003LIBDIR)
			cp $(INCDIR)UnionDes.h		$(CMM2003INCDIR)
			cp $(INCDIR)UnionDataExch.h	$(CMM2003INCDIR)
			cp $(INCDIR)UnionMAC.h		$(CMM2003INCDIR)
			cp $(INCDIR)UnionXOR.h		$(CMM2003INCDIR)
			cp $(INCDIR)UnionMD5.h		$(CMM2003INCDIR)
			cp $(INCDIR)UnionAlgorithm.h	$(CMM2003INCDIR)
			cp $(INCDIR)unionPBOCKeyGenerate.h $(CMM2003INCDIR)
			mv *.o				$(CMM2003LIBDIR)

rmobjs:	

