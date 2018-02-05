HOMEDIR = $(CMM2003DIR)/unionAlgorithm/
INCDIR = $(HOMEDIR)include/
INCL = -I $(INCDIR) -I $(CMM2003INCDIR) 
ThirdPartyObjsDir=$(HOMEDIR)/thirdPartyObjs/

# 当操作系统是是高字节在前时，打开该编译开关。
# 浦发HP-UNIX即是这样的系统
# DEFINES = -D HIGHFIRST $(OSSPEC)
DEFINES = $(OSSPEC)  

all:	testDes testSHA1 testSHA11 testSHA256 rmobjs

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

#LIBS	=	$(CMM2003LIBDIR)/UnionAlgorithm1.0.a		\
#		$(CMM2003LIBDIR)/UnionLog1.2.a
LIBS 	=	$(CMM2003LIBDIR)/unionAlgorithm.20040706.withoutBaseDes.a

objs =		testDes.o UnionBaseDes.20050222.o
testDes:		$(objs)
			cc $(OSSPEC)  -o testDes $(objs) $(LIBS) $(OSSPECLIBS)

objs1 =		testSHA1.o
testSHA1:		$(objs1)
			cc $(OSSPEC)  -o testSHA1 $(objs1) $(LIBS) $(OSSPECLIBS)

objs2 =		testSHA11.o
testSHA11:		$(objs2)
			cc $(OSSPEC)  -o testSHA11 $(objs2) $(LIBS) $(OSSPECLIBS)

objs3 =		testSHA256.o
testSHA256:		$(objs3)
			cc $(OSSPEC)  -o testSHA256 $(objs3) $(LIBS) $(OSSPECLIBS) /cmmDevLib/lib64/openLib/lib/libcrypto.a

rmobjs:	
			rm *.o

