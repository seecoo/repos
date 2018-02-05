//des.h
#ifndef __DES__H
#define __DES__H
//#ifndef __STDAFX__H
//#define __STDAFX__H
//#include"StdAfx.h"
/*
#define APP_EXPORT
#ifdef APP_EXPORT
#define DES_APP __declspec(dllexport)
#else
#define DES_APP __declspec(dllimport)
#endif
*/
//Define the Function()
//void Debug_p(char *filename,int lineno,char *fmt,...);
//APIENTRY is for .dll PB use;
//int APIENTRY Des(unsigned char *key,unsigned char *text,unsigned char *mtext);
//int APIENTRY _Des(unsigned char *key,unsigned char *text,unsigned char *mtext);
int Des(unsigned char *text,unsigned char *mtext,unsigned char *key);
int _Des(unsigned char *text,unsigned char *mtext,unsigned char *key);

int ASCDES(char *tag,char *src,char *key);
int ASC_DES(char *tag,char *src,char *key);

int CaculateMac(char *MacKey,char *MacVal,char *MacBuf);
void BcdAsc(char *buf, int len, char *result);
void AscBcd(char *buf, int len, char *result);
int  CaculatePin(char *PinKeyPos,char *PinKeyGnet,char *Pin,char *Account,char *OutPin);

int expand0(unsigned char *in,unsigned char *out);
int encrypt0(unsigned char *text,unsigned char *mtext);
int compress0(unsigned char *out,unsigned char *in);
int discrypt0(unsigned char *mtext,unsigned char *text);
int compress016(unsigned char *out,unsigned char *in);
int setkeystar(unsigned char *bits);
int LS(unsigned char *bits,unsigned char *buffer,int count);
int son(unsigned char *cc,unsigned char *dd,unsigned char *kk);
int ip(unsigned char *text,unsigned char *ll,unsigned char *rr);
int _ip(unsigned char *text,unsigned char *ll,unsigned char *rr);
int F(int n,unsigned char *ll,unsigned char *rr,unsigned char *LL,unsigned char *RR);
int s_box(unsigned char *aa,unsigned char *bb);

int UNION_DES(unsigned char *pData,unsigned char *pResult,int len,unsigned char *aKey,int flag);

#endif
