#ifndef ESBMD5_H
#define ESBMD5_H

typedef unsigned char * POINTER;
typedef unsigned short int UINT2;
typedef unsigned int UINT4;

typedef struct Md5_ctx {
 UINT4 state[4];
 UINT4 count[2];
 unsigned char buffer[64];
} Md5_ctx;

void md5Init (Md5_ctx *context);
void md5Update(Md5_ctx *context, unsigned char *input,unsigned int inputLen);

void md5Final (unsigned char digest[16], Md5_ctx *context);
void md5Transform (UINT4 [4], unsigned char [64]) ;
void Encode(unsigned char *, UINT4 *, unsigned int);
void Decode (UINT4 *, unsigned char *, unsigned int);
void md5Memcpy(POINTER, POINTER, unsigned int);
void md5Memset(POINTER, int, unsigned int);

char * getMd5Str(char *string,int len,char *output);
char * getMd5File (char *filename,char *output);
void md5Final32(char * md5str, Md5_ctx *context);
int convMd5To32Byte(unsigned char digest[16],char * md5Str);
#endif
