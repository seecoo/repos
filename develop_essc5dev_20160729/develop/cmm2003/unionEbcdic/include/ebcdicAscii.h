// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2004/12/05
// Version:	1.0

#ifndef _EbcdicAscii_
#define _EbcdicAscii_

int ibm2cnGBK34 (unsigned char *s, unsigned char *t);

int ibm2cnGBKU (unsigned char *s, unsigned char *t);

int ibm2cnGBK4ext(unsigned char *s, unsigned char *t);

//�������2������
//�������ַ��������GBK/3/3ת����IBM�����ַ��������:
int cn2ibmGBK34 (unsigned char *s, unsigned char *t);

int cn2ibmGBK4ext(unsigned char *s, unsigned char *t);

/* GBK/U  Range: CN: A140 - A7A0, IBM: 7CA0 - 804E = */
/* GBK/U  Range: CN: AAA1 - AFFE, IBM: 7641 - 78FD = */
/* GBK/U  Range: CN: F8A1 - FEFE, IBM: 7941 - 7C9F = */
//�������ַ��������GBK/Uת����IBM�����ַ��������:
int cn2ibmGBKU (unsigned char *s, unsigned char *t);

void UnionAsciiToEbcdic (unsigned char *AsciiBuffer,unsigned char *EbcdicBuffer,int TranLen);
void UnionEbcdicToAscii (unsigned char *EbcdicBuffer,unsigned char *AsciiBuffer,int TranLen);

#endif
