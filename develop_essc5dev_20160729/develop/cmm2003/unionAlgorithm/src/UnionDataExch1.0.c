//	Author:		Wolfgang Wang
//	Date:		2002/6/20

#include <stdio.h>
#include <string.h>

#include "UnionDataExch.h"

int UnionCompress128BCDInto64Bits(char *p128BCDKey,unsigned char *p64BitsKey)
{
	int	i;
	
	for (i = 0; i < 8; i++)
	{
		if (p128BCDKey[2*i] >= 'A')
			p64BitsKey[i] = (char)((p128BCDKey[2*i] - 0x37) << 4);
		else
			p64BitsKey[i] = (char)((p128BCDKey[2*i] - '0') << 4);
		if (p128BCDKey[2*i+1] >= 'A')
			p64BitsKey[i] |= p128BCDKey[2*i+1] - 0x37;
		else
			p64BitsKey[i] |= p128BCDKey[2*i+1] - '0';
	}
	return(0);
}

int UnionUncompress64BitsInto128BCD(unsigned char *p64BitsKey,char *p128BCDKey)
{
	int	i;
	unsigned char xxc;
	
	for (i = 0; i < 8; i++)
	{
		xxc = p64BitsKey[i] / 16;
		if (xxc < 0x0a)
			p128BCDKey[2*i] = xxc + '0';
		else
			p128BCDKey[2*i] = xxc + 0x37;

		xxc = p64BitsKey[i] % 16;
		if (xxc < 0x0a)
			p128BCDKey[2*i+1] = xxc + '0';
		else
			p128BCDKey[2*i+1] = xxc + 0x37;
	}
	return(0);
}

int UnionCompressBCDIntoBits(char *pBCDData,int LenOfBCDData,unsigned char *pBitsData)
{
	int	i;
	
	for (i = 0; i < LenOfBCDData/2; i++)
	{
		if (pBCDData[2*i] >= 'A')
			pBitsData[i] = (char)((pBCDData[2*i] - 0x37) << 4);
		else
			pBitsData[i] = (char)((pBCDData[2*i] - '0') << 4);
		if (pBCDData[2*i+1] >= 'A')
			pBitsData[i] |= pBCDData[2*i+1] - 0x37;
		else
			pBitsData[i] |= pBCDData[2*i+1] - '0';
	}
	return(0);
}

int UnionUncompressBitsIntoBCD(unsigned char *pBitsData,int LenOfBitsData,char *pBCDData)
{
	int	i;
	unsigned char xxc;
	
	for (i = 0; i < LenOfBitsData; i++)
	{
		xxc = pBitsData[i] / 16;
		if (xxc < 0x0a)
			pBCDData[2*i] = xxc + '0';
		else
			pBCDData[2*i] = xxc + 0x37;

		xxc = pBitsData[i] % 16;
		if (xxc < 0x0a)
			pBCDData[2*i+1] = xxc + '0';
		else
			pBCDData[2*i+1] = xxc + 0x37;
	}
	return(0);
}


