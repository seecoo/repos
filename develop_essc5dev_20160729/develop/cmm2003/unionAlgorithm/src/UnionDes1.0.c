//	Author:		Wolfgang Wang
//	Date:		2002/6/20

#include <stdio.h>
#include <string.h>

#include "UnionDes.h"
#include "UnionLog.h"

// The pPlainKey must be 64bits, i.e. BCD 16chars
// pPlainText must be 64bits,i.e. BCD 16chars
int Union1DesEncrypt64BitsText(char *pPlainKey,char *pPlainText,char *pCryptogram)
{
	unsigned char	aKey[9];
	unsigned char	pData[9];
	unsigned char	pResult[9];
	int		ret;
	
	/* compress the 16 bytes key into 8 bytes */
	if ((ret = UnionCompress128BCDInto64Bits(pPlainKey,aKey)) != 0)
	{
		UnionUserErrLog("in Union1DesEncrypt64BitsText:: UnionCompress128BCDInto64Bits!\n");
		return(ret);
	}

	// Compress PlainText
	if ((ret = UnionCompress128BCDInto64Bits(pPlainText,pData)) != 0)
	{
		UnionUserErrLog("in Union1DesEncrypt64BitsText:: UnionCompress128BCDInto64Bits!\n");
		return(ret);
	}

	UNION_DES(pData,pResult,8,aKey,1);
	
	// Uncompress Result
	if ((ret = UnionUncompress64BitsInto128BCD(pResult,pCryptogram)) != 0)
	{
		UnionUserErrLog("in Union1DesEncrypt64BitsText:: UnionUncompress64BitsInto128BCD!\n");
		return(ret);
	}
	
	return(0);
}

// The pPlainKey must be 64bits, i.e. BCD 16chars
// The pCryptogram must be 64bits, i.e. BCD 16chars.
int Union1DesDecrypt64BitsText(char *pPlainKey,char *pCryptogram,char *pPlainText)
{
	unsigned char	aKey[9];
	unsigned char	pData[9];
	unsigned char	pResult[9];
	int		ret;
	
	/* compress the 16 bytes key into 8 bytes */
	if ((ret = UnionCompress128BCDInto64Bits(pPlainKey,aKey)) != 0)
	{
		UnionUserErrLog("in Union1DesDecrypt64BitsText:: UnionCompress128BCDInto64Bits!\n");
		return(ret);
	}

	// Compress Cryptogram
	if ((ret = UnionCompress128BCDInto64Bits(pCryptogram,pData)) != 0)
	{
		UnionUserErrLog("in Union1DesDecrypt64BitsText:: UnionCompress128BCDInto64Bits!\n");
		return(ret);
	}

	UNION_DES(pData,pResult,8,aKey,0);
	
	// Uncompress Result
	if ((ret = UnionUncompress64BitsInto128BCD(pResult,pPlainText)) != 0)
	{
		UnionUserErrLog("in Union1DesDecrypt64BitsText:: UnionUncompress64BitsInto128BCD!\n");
		return(ret);
	}
	
	return(0);
}

// The pPlainKey must be 128bits, i.e. BCD 32chars
// The plaintext must be 64bits, i.e. BCD 16 chars
int Union3DesEncrypt64BitsText(char *pPlainKey,char *pPlainText,char *pCryptogram)
{
	unsigned char	aKey1[9],aKey2[9];
	unsigned char	pData[9];
	unsigned char	pResult1[9],pResult2[9],pResult3[9];
	int		ret;
	
	/* compress the 16 bytes left key into 8 bytes */
	if ((ret = UnionCompress128BCDInto64Bits(pPlainKey,aKey1)) != 0)
	{
		UnionUserErrLog("in Union3DesEncrypt64BitsText:: UnionCompress128BCDInto64Bits!\n");
		return(ret);
	}
	/* compress the 16 bytes right key into 8 bytes */
	if ((ret = UnionCompress128BCDInto64Bits(&pPlainKey[16],aKey2)) != 0)
	{
		UnionUserErrLog("in Union3DesEncrypt64BitsText:: UnionCompress128BCDInto64Bits!\n");
		return(ret);
	}

	// Compress PlainText
	if ((ret = UnionCompress128BCDInto64Bits(pPlainText,pData)) != 0)
	{
		UnionUserErrLog("in Union3DesEncrypt64BitsText:: UnionCompress128BCDInto64Bits!\n");
		return(ret);
	}

	UNION_DES(pData,pResult1,8,aKey1,1);
	UNION_DES(pResult1,pResult2,8,aKey2,0);
	UNION_DES(pResult2,pResult3,8,aKey1,1);
	
	// Uncompress Result
	if ((ret = UnionUncompress64BitsInto128BCD(pResult3,pCryptogram)) != 0)
	{
		UnionUserErrLog("in Union3DesEncrypt64BitsText:: UnionUncompress64BitsInto128BCD!\n");
		return(ret);
	}
	
	return(0);
}

// The pPlainKey must be 128bits, i.e. BCD 32chars
// the cryptogram must be 64bits, i.e. BCD 16chars
int Union3DesDecrypt64BitsText(char *pPlainKey,char *pCryptogram,char *pPlainText)
{
	unsigned char	aKey1[9],aKey2[9];
	unsigned char	pData[9];
	unsigned char	pResult1[9],pResult2[9],pResult3[9];
	int		ret;
	
	/* compress the 16 bytes left key into 8 bytes */
	if ((ret = UnionCompress128BCDInto64Bits(pPlainKey,aKey1)) != 0)
	{
		UnionUserErrLog("in Union3DesDecrypt64BitsText:: UnionCompress128BCDInto64Bits!\n");
		return(ret);
	}
	/* compress the 16 bytes right key into 8 bytes */
	if ((ret = UnionCompress128BCDInto64Bits(&pPlainKey[16],aKey2)) != 0)
	{
		UnionUserErrLog("in Union3DesDecrypt64BitsText:: UnionCompress128BCDInto64Bits!\n");
		return(ret);
	}

	// Compress PlainText
	if ((ret = UnionCompress128BCDInto64Bits(pCryptogram,pData)) != 0)
	{
		UnionUserErrLog("in Union3DesEncrypt64BitsText:: UnionCompress128BCDInto64Bits!\n");
		return(ret);
	}

	UNION_DES(pData,pResult1,8,aKey1,0);
	UNION_DES(pResult1,pResult2,8,aKey2,1);
	UNION_DES(pResult2,pResult3,8,aKey1,0);
	
	// Uncompress Result
	if ((ret = UnionUncompress64BitsInto128BCD(pResult3,pPlainText)) != 0)
	{
		UnionUserErrLog("in Union3DesEncrypt64BitsText:: UnionUncompress64BitsInto128BCD!\n");
		return(ret);
	}
	
	return(0);
}

