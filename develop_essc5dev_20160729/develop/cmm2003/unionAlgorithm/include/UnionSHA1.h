/* crypto/sha/sha_locl.h */
#include <stdlib.h>
#include <string.h>

#ifdef undef
/* one or the other needs to be defined */
#ifndef SHA_1 /* FIPE 180-1 */
#define SHA_0 /* FIPS 180   */
#endif
#endif

#undef c2nl
#define c2nl(c,l)	(l =(((unsigned int)(*((c)++)))<<24), \
			 l|=(((unsigned int)(*((c)++)))<<16), \
			 l|=(((unsigned int)(*((c)++)))<< 8), \
			 l|=(((unsigned int)(*((c)++)))    ))

#undef p_c2nl
#define p_c2nl(c,l,n)	{ \
			switch (n) { \
			case 0: l =((unsigned int)(*((c)++)))<<24; \
			case 1: l|=((unsigned int)(*((c)++)))<<16; \
			case 2: l|=((unsigned int)(*((c)++)))<< 8; \
			case 3: l|=((unsigned int)(*((c)++))); \
				} \
			}

#undef c2nl_p
/* NOTE the pointer is not incremented at the end of this */
#define c2nl_p(c,l,n)	{ \
			l=0; \
			(c)+=n; \
			switch (n) { \
			case 3: l =((unsigned int)(*(--(c))))<< 8; \
			case 2: l|=((unsigned int)(*(--(c))))<<16; \
			case 1: l|=((unsigned int)(*(--(c))))<<24; \
				} \
			}

#undef p_c2nl_p
#define p_c2nl_p(c,l,sc,len) { \
			switch (sc) \
				{ \
			case 0: l =((unsigned int)(*((c)++)))<<24; \
				if (--len == 0) break; \
			case 1: l|=((unsigned int)(*((c)++)))<<16; \
				if (--len == 0) break; \
			case 2: l|=((unsigned int)(*((c)++)))<< 8; \
				} \
			}

#undef nl2c
#define nl2c(l,c)	(*((c)++)=(unsigned char)(((l)>>24)&0xff), \
			 *((c)++)=(unsigned char)(((l)>>16)&0xff), \
			 *((c)++)=(unsigned char)(((l)>> 8)&0xff), \
			 *((c)++)=(unsigned char)(((l)    )&0xff))

#undef c2l
#define c2l(c,l)	(l =(((unsigned int)(*((c)++)))    ), \
			 l|=(((unsigned int)(*((c)++)))<< 8), \
			 l|=(((unsigned int)(*((c)++)))<<16), \
			 l|=(((unsigned int)(*((c)++)))<<24))

#undef p_c2l
#define p_c2l(c,l,n)	{ \
			switch (n) { \
			case 0: l =((unsigned int)(*((c)++))); \
			case 1: l|=((unsigned int)(*((c)++)))<< 8; \
			case 2: l|=((unsigned int)(*((c)++)))<<16; \
			case 3: l|=((unsigned int)(*((c)++)))<<24; \
				} \
			}

#undef c2l_p
/* NOTE the pointer is not incremented at the end of this */
#define c2l_p(c,l,n)	{ \
			l=0; \
			(c)+=n; \
			switch (n) { \
			case 3: l =((unsigned int)(*(--(c))))<<16; \
			case 2: l|=((unsigned int)(*(--(c))))<< 8; \
			case 1: l|=((unsigned int)(*(--(c)))); \
				} \
			}

#undef p_c2l_p
#define p_c2l_p(c,l,sc,len) { \
			switch (sc) \
				{ \
			case 0: l =((unsigned int)(*((c)++))); \
				if (--len == 0) break; \
			case 1: l|=((unsigned int)(*((c)++)))<< 8; \
				if (--len == 0) break; \
			case 2: l|=((unsigned int)(*((c)++)))<<16; \
				} \
			}

#undef l2c
#define l2c(l,c)	(*((c)++)=(unsigned char)(((l)    )&0xff), \
			 *((c)++)=(unsigned char)(((l)>> 8)&0xff), \
			 *((c)++)=(unsigned char)(((l)>>16)&0xff), \
			 *((c)++)=(unsigned char)(((l)>>24)&0xff))

#ifndef SHA_LONG_LOG2
#define SHA_LONG_LOG2	2	/* default to 32 bits */
#endif

#undef ROTATE
#undef Endian_Reverse32
#if defined(WIN32)
#define ROTATE(a,n)     _lrotl(a,n)
#elif defined(__GNUC__) && !defined(PEDANTIC)
/* some inline assembler templates by <appro@fy.chalmers.se> */
#if defined(__i386) && !defined(NO_ASM)
#define ROTATE(a,n)	({ register unsigned int ret;	\
				asm ("roll %1,%0"	\
				: "=r"(ret)		\
				: "I"(n), "0"(a)	\
				: "cc");		\
			   ret;				\
			})
#ifndef I386_ONLY
#define Endian_Reverse32(a) \
			{ register unsigned int ltmp=(a);	\
				asm ("bswapl %0"	\
				: "=r"(ltmp) : "0"(ltmp));	\
			  (a)=ltmp;			\
			}
#endif
#elif defined(__powerpc)
#define ROTATE(a,n)	({ register unsigned int ret;		\
				asm ("rlwinm %0,%1,%2,0,31"	\
				: "=r"(ret)			\
				: "r"(a), "I"(n));		\
			   ret;					\
			})
/* Endian_Reverse32 is not needed for PowerPC */
#endif
#endif

/* A nice byte order reversal from Wei Dai <weidai@eskimo.com> */
#ifdef ROTATE
#ifndef Endian_Reverse32
/* 5 instructions with rotate instruction, else 9 */
#define Endian_Reverse32(a) \
	{ \
	unsigned int t=(a); \
	(a)=((ROTATE(t,8)&0x00FF00FF)|(ROTATE((t&0x00FF00FF),24))); \
	}
#endif
#else
#define ROTATE(a,n)     (((a)<<(n))|(((a)&0xffffffff)>>(32-(n))))
#ifndef Endian_Reverse32
/* 6 instructions with rotate instruction, else 8 */
#define Endian_Reverse32(a) \
	{ \
	unsigned int t=(a); \
	t=(((t>>8)&0x00FF00FF)|((t&0x00FF00FF)<<8)); \
	(a)=ROTATE(t,16); \
	}
#endif
/*
 * Originally the middle line started with l=(((l&0xFF00FF00)>>8)|...
 * It's rewritten as above for two reasons:
 *	- RISCs aren't good at long constants and have to explicitely
 *	  compose 'em with several (well, usually 2) instructions in a
 *	  register before performing the actual operation and (as you
 *	  already realized:-) having same constant should inspire the
 *	  compiler to permanently allocate the only register for it;
 *	- most modern CPUs have two ALUs, but usually only one has
 *	  circuitry for shifts:-( this minor tweak inspires compiler
 *	  to schedule shift instructions in a better way...
 *
 *				<appro@fy.chalmers.se>
 */
#endif

/* As  pointed out by Wei Dai <weidai@eskimo.com>, F() below can be
 * simplified to the code in F_00_19.  Wei attributes these optimisations
 * to Peter Gutmann's SHS code, and he attributes it to Rich Schroeppel.
 * #define F(x,y,z) (((x) & (y))  |  ((~(x)) & (z)))
 * I've just become aware of another tweak to be made, again from Wei Dai,
 * in F_40_59, (x&a)|(y&a) -> (x|y)&a
 */
#define	F_00_19(b,c,d)	((((c) ^ (d)) & (b)) ^ (d)) 
#define	F_20_39(b,c,d)	((b) ^ (c) ^ (d))
#define F_40_59(b,c,d)	(((b) & (c)) | (((b)|(c)) & (d))) 
#define	F_60_79(b,c,d)	F_20_39(b,c,d)

#undef Xupdate
#ifdef SHA_0
#define Xupdate(a,i,ia,ib,ic,id) X[(i)&0x0f]=(a)=\
	(ia[(i)&0x0f]^ib[((i)+2)&0x0f]^ic[((i)+8)&0x0f]^id[((i)+13)&0x0f]);
#endif
#ifdef SHA_1
#define Xupdate(a,i,ia,ib,ic,id) (a)=\
	(ia[(i)&0x0f]^ib[((i)+2)&0x0f]^ic[((i)+8)&0x0f]^id[((i)+13)&0x0f]);\
	X[(i)&0x0f]=(a)=ROTATE((a),1);
#endif

#define BODY_00_15(i,a,b,c,d,e,f,xa) \
	(f)=xa[i]+(e)+K_00_19+ROTATE((a),5)+F_00_19((b),(c),(d)); \
	(b)=ROTATE((b),30);

#define BODY_16_19(i,a,b,c,d,e,f,xa,xb,xc,xd) \
	Xupdate(f,i,xa,xb,xc,xd); \
	(f)+=(e)+K_00_19+ROTATE((a),5)+F_00_19((b),(c),(d)); \
	(b)=ROTATE((b),30);

#define BODY_20_31(i,a,b,c,d,e,f,xa,xb,xc,xd) \
	Xupdate(f,i,xa,xb,xc,xd); \
	(f)+=(e)+K_20_39+ROTATE((a),5)+F_20_39((b),(c),(d)); \
	(b)=ROTATE((b),30);

#define BODY_32_39(i,a,b,c,d,e,f,xa) \
	Xupdate(f,i,xa,xa,xa,xa); \
	(f)+=(e)+K_20_39+ROTATE((a),5)+F_20_39((b),(c),(d)); \
	(b)=ROTATE((b),30);

#define BODY_40_59(i,a,b,c,d,e,f,xa) \
	Xupdate(f,i,xa,xa,xa,xa); \
	(f)+=(e)+K_40_59+ROTATE((a),5)+F_40_59((b),(c),(d)); \
	(b)=ROTATE((b),30);

#define BODY_60_79(i,a,b,c,d,e,f,xa) \
	Xupdate(f,i,xa,xa,xa,xa); \
	(f)=X[(i)&0x0f]+(e)+K_60_79+ROTATE((a),5)+F_60_79((b),(c),(d)); \
	(b)=ROTATE((b),30);

/* crypto/sha/sha.h */
#ifndef HEADER_SHA_H
#define HEADER_SHA_H

#ifdef  __cplusplus
extern "C" {
#endif

#ifdef NO_SHA
#error SHA is disabled.
#endif

/*
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * ! SHA_LONG has to be at least 32 bits wide. If it's wider, then !
 * ! SHA_LONG_LOG2 has to be defined along.                        !
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 */

#if defined(WIN16) || defined(__LP32__)
#define SHA_LONG unsigned int
#elif defined(_CRAY) || defined(__ILP64__)
#define SHA_LONG unsigned int
#define SHA_LONG_LOG2 3
#else
#define SHA_LONG unsigned int
#endif

#define SHA_LBLOCK	16
#define SHA_CBLOCK	(SHA_LBLOCK*4)	/* SHA treats input data as a
					 * contiguous array of 32 bit
					 * wide big-endian values. */
#define SHA_LAST_BLOCK  (SHA_CBLOCK-8)
#define SHA_DIGEST_LENGTH 20

typedef struct SHAstate_st
	{
	SHA_LONG h0,h1,h2,h3,h4;
	SHA_LONG Nl,Nh;
	SHA_LONG data[SHA_LBLOCK];
	int num;
	} SHA_CTX;

#ifndef NO_SHA0
void SHA_Init(SHA_CTX *c);
void SHA_Update(SHA_CTX *c, const unsigned char *data, unsigned int len);
void SHA_Final(unsigned char *md, SHA_CTX *c);
unsigned char *SHA(const unsigned char *d, unsigned int n,unsigned char *md);
void SHA_Transform(SHA_CTX *c, unsigned char *data);

#endif

#ifndef NO_SHA1
void XXSHA1_Init(SHA_CTX *c);
void XXSHA1_Update(SHA_CTX *c, const unsigned char *data, unsigned int len);
void XXSHA1_Final(unsigned char *md, SHA_CTX *c);
unsigned char *XXSHA1(const unsigned char *d, unsigned int n,unsigned char *md);
void XXSHA1_Transform(SHA_CTX *c, unsigned char *data);

void XXhmac_sha1(unsigned char*  text, int text_len, unsigned char* key, int key_len, unsigned char* digest);

#endif
#ifdef  __cplusplus
}
#endif

unsigned char *UnionSHA1(const unsigned char *d,	// 要进行HASH的数据
	unsigned int n,					// 要进行HASH的数据长度
	unsigned char *md);				// HASH结果

void UnionSHA(unsigned char *plaintexts,unsigned int block_number,unsigned char *hash);

#endif
