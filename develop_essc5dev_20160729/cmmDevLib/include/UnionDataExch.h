//	Date:	2002/6/20

#ifndef _UnionDataExch

int UnionCompress128BCDInto64Bits(char *p128BCDKey,unsigned char *p64BitsKey);
int UnionUncompress64BitsInto128BCD(unsigned char *p64BitsKey,char *p128BCDKey);

int UnionCompressBCDIntoBits(char *pBCDData,int LenOfBCDData,unsigned char *pBitsData);
int UnionUncompressBitsIntoBCD(unsigned char *pBitsData,int LenOfBitsData,char *pBCDData);

#endif
