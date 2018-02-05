// Wolfgang Wang
// 2004/11/17

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <malloc.h>

#include "UnionAlgorithm.h"
#include "UnionStr.h"

#include "unionPBOCKeyGenerate.h"

int UnionGeneratePBOCDPKLeftPart(char *MPK,char *inputData,char *leftDPK)
{
	char	tmpBuf[32+1];
	
	Union1DesEncrypt64BitsText(MPK,inputData,tmpBuf);
	Union1DesDecrypt64BitsText(MPK+16,tmpBuf,tmpBuf);
	Union1DesEncrypt64BitsText(MPK,tmpBuf,tmpBuf);

	memcpy(leftDPK,tmpBuf,16);
	return(0);
}

int UnionGeneratePBOCDPKRightPart(char *MPK,char *inputData,char *rightDPK)
{
	char		tmpBuf1[16+1];
	char		tmpBuf2[16+1];
	int	i;
	
	aschex_to_bcdhex(inputData,16,(char *)tmpBuf1);
	for (i = 0; i < 8; i++)
		tmpBuf1[i] = ~tmpBuf1[i];
	bcdhex_to_aschex((char *)tmpBuf1,8,(char *)tmpBuf2);
	Union1DesEncrypt64BitsText(MPK,tmpBuf2,tmpBuf1);
	Union1DesDecrypt64BitsText(MPK+16,tmpBuf1,tmpBuf1);
	Union1DesEncrypt64BitsText(MPK,tmpBuf1,tmpBuf1);
	
	memcpy(rightDPK,tmpBuf1,16);
	return(0);
}

int UnionGeneratePBOCDPK(char *MPK,char *inputData,char *DPK)
{
	UnionGeneratePBOCDPKLeftPart(MPK,inputData,DPK);
	UnionGeneratePBOCDPKRightPart(MPK,inputData,DPK+16);
	return(0);
}

int UnionGeneratePBOCSessionKey(char *DPK,char *inputData,char *sessionKey)
{
	Union1DesEncrypt64BitsText(DPK,inputData,sessionKey);
	Union1DesDecrypt64BitsText(DPK+16,sessionKey,sessionKey);
	Union1DesEncrypt64BitsText(DPK,sessionKey,sessionKey);
	return(0);
}
