# 定义开发目录
HOMEDIR = $(CMM2003DIR)/unionRSAEuro
INCDIR = $(HOMEDIR)/include/

# 定义头文件目录清单
# 顺序应该是：
# 通用模块头文件目录/当前开发环境头文件目录/本工程的头文件目录

INCL =  -I $(INCDIR)  -I $(CMM2003INCDIR) 

all:    unionRSAEuro.a   \
        productLib              \
        rmtmp

DEFINES = $(OSSPEC) -DUNIX -DSoftAlgorithm

.SUFFIXES:.c.o
.c.o:
	$(CC) $(DEFINES) $(INCL) -c $*.c

unionRSAEuroBase =	desc.o \
			nn.o \
			digit.o	\
			r_dh.o \
			r_enhanc.o \
			r_random.o \
			r_stdlib.o \
			UnionRSAEuroFunc.o \
			md2c.o \
			md5c.o \
			prime.o \
			r_encode.o \
			r_keygen.o 



unionRSAEuro.a:$(unionRSAEuroBase)
	rm -rf unionRSAEuro.a
	ar  $(OSSPECPACK) crv unionRSAEuro.a $(unionRSAEuroBase)

productLib:
	cp unionRSAEuro.a $(CMM2003LIBDIR)/unionRSAEuro.a
rmtmp:
	rm      *.o
