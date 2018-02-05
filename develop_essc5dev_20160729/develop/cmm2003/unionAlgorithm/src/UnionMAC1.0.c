
#include <stdio.h>
#include <string.h>

#include "unionErrCode.h"
#include "UnionDataExch.h"
#include "UnionXOR.h"
#include "UnionDes.h"
#include "UnionMAC.h"
#include "UnionLog.h"


/* MAC arithmetic: MAC_CODE_1 */
int UnionGenerateANSIX99MAC(char *pPlainKey,char *pMacData,int nLenOfMacData,char *pMAC)
{
	int	i,nReturn;
	char	aLeftDataBlock[17],aDataBlock[17],aMac[17];

	if (nLenOfMacData<=0)
	{
		UnionUserErrLog("in UnionGenerateANSIX99MAC:: nLenOfMacData [%d] Error!\n",nLenOfMacData);
		return (errCodeParameter);
	}

	if ((pPlainKey == NULL) || (pMacData == NULL) || (pMAC == NULL))
	{
		UnionUserErrLog("in UnionGenerateANSIX99MAC:: Null Pointer!\n");
		return(errCodeParameter);
	}

	memset(aMac,'0',sizeof(aMac));
	for (i = 0; i < nLenOfMacData / 16; i++)
	{
		if ((nReturn = UnionXOR(aMac,pMacData+i*16,16,aDataBlock)) < 0)
		{
			UnionUserErrLog("in UnionGenerateANSIX99MAC:: UnionXOR Error!\n");
			return(nReturn);
		}
		if ((nReturn = Union1DesEncrypt64BitsText(pPlainKey,aDataBlock,aMac)) < 0)
		{
			UnionUserErrLog("in UnionGenerateANSIX99MAC:: Union1DesEncrypt64BitsText Error!\n");
			return(nReturn);
		}
	}
	if (nLenOfMacData % 16 != 0)
	{
		memset(aLeftDataBlock,'0',16);
		memcpy(aLeftDataBlock,pMacData+i*16,nLenOfMacData % 16);
		if ((nReturn = UnionXOR(aMac,aLeftDataBlock,16,aDataBlock)) < 0)
		{
			UnionUserErrLog("in UnionGenerateANSIX99MAC:: UnionXOR Error!\n");
			return(nReturn);
		}
		if ((nReturn = Union1DesEncrypt64BitsText(pPlainKey,aDataBlock,aMac)) < 0)
		{
			UnionUserErrLog("in UnionGenerateANSIX99MAC:: Union1DesEncrypt64BitsText Error!\n");
			return(nReturn);
		}
	}		
	memcpy(pMAC,aMac,16);		
	return (0);
}

/* MAC arithmetic: MAC_CODE_1 */
int UnionGenerateMACWithoutPredifinedHeaderAndWithA0x80Tail(char *key,int lenOfKey,char *macData,int lenOfMacData,char *mac)
{
	int	i,ret,num;
	char	tmpBuf[2048+1];
	char	tmpMac[16+1];
	char	xorResult[16+1];
	char	tmp[16+1];
		
	if ((lenOfMacData < 0) || (key == NULL) || (macData == NULL) || (mac == NULL) || ((lenOfKey != 16) && (lenOfKey !=32) && (lenOfKey != 48)))
	{
		UnionUserErrLog("in UnionGenerateMACWithoutPredifinedHeaderAndWithA0x80Tail:: Null Pointer or lenOfKey [%d] or lenOfMacData [%d] error!\n",
		lenOfKey,lenOfMacData);
		return(errCodeParameter);
	}
	
	if ((lenOfMacData + 2) % 16 == 0)
	{
		if (lenOfMacData + 2 > sizeof(tmpBuf))
		{
			UnionUserErrLog("in UnionGenerateMACWithoutPredifinedHeaderAndWithA0x80Tail:: lenOfMacData = [%d] too long!\n",lenOfMacData);
			return(errCodeParameter);
		}
	}
	else
	{
		if ((lenOfMacData + 2) % 16 * 16 + 16 > sizeof(tmpBuf))
		{
			UnionUserErrLog("in UnionGenerateMACWithoutPredifinedHeaderAndWithA0x80Tail:: lenOfMacData = [%d] too long!\n",lenOfMacData);
			return(errCodeParameter);
		}
	}
	
	//memset(tmpBuf,0,sizeof(tmpBuf));
	//memcpy(tmpBuf,key,lenOfKey);
	//UnionLog("in UnionGenerateMACWithoutPredifinedHeaderAndWithA0x80Tail:: key = [%s]\n",key);
	
	memset(tmpBuf,0,sizeof(tmpBuf));
	memcpy(tmpBuf,macData,lenOfMacData);
	memcpy(tmpBuf+lenOfMacData,"80",2);
	lenOfMacData += 2;
	if (lenOfMacData % 16 != 0)
	{
		memset(tmpBuf+lenOfMacData,'0',16-lenOfMacData%16);
		lenOfMacData = lenOfMacData / 16 * 16 + 16;
	}
	//UnionLog("in UnionGenerateMACWithoutPredifinedHeaderAndWithA0x80Tail:: \nlenOfMacData = [%d]\nmacData=[%s]\n",lenOfMacData,tmpBuf);
	memset(tmpMac,0,sizeof(tmpMac));
	memcpy(tmpMac,tmpBuf,16);
	if (lenOfMacData == 16)
	{
		if ((ret = Union1DesEncrypt64BitsText(key,tmpMac,tmpMac)) < 0)
		{
			UnionUserErrLog("in UnionGenerateMACWithoutPredifinedHeaderAndWithA0x80Tail:: Union1DesEncrypt64BitsText Error!\n");
			return(ret);
		}
	}
	else
	{
		//UnionLog("in UnionGenerateMACWithoutPredifinedHeaderAndWithA0x80Tail:: tmpMac before XOR = [%s]\n",tmpMac);
		for (i = 1,num = lenOfMacData / 16; i < num; i++)
		{
			memset(xorResult,0,sizeof(xorResult));
			memset(tmp,0,sizeof(tmp));
			memcpy(tmp,tmpBuf+i*16,16);
			//UnionLog("in UnionGenerateMACWithoutPredifinedHeaderAndWithA0x80Tail:: xor data [%s] [%s]\n",tmpMac,tmp);
			if ((ret = UnionXOR(tmpMac,tmpBuf+i*16,16,xorResult)) < 0)
			{
				UnionUserErrLog("in UnionGenerateMACWithoutPredifinedHeaderAndWithA0x80Tail:: UnionXOR Error!\n");
				return(ret);
			}
			//UnionLog("in UnionGenerateMACWithoutPredifinedHeaderAndWithA0x80Tail:: xorResult after XOR = [%s]\n",
			//	xorResult);
			if ((ret = Union1DesEncrypt64BitsText(key,xorResult,tmpMac)) < 0)
			{
				UnionUserErrLog("in UnionGenerateMACWithoutPredifinedHeaderAndWithA0x80Tail:: Union1DesEncrypt64BitsText Error!\n");
				return(ret);
			}
			//UnionLog("in UnionGenerateMACWithoutPredifinedHeaderAndWithA0x80Tail:: tmpMac after encrypt = [%s]\n",tmpMac);
		}
	}
	if (lenOfKey == 32)
	{
		Union1DesDecrypt64BitsText(key+16,tmpMac,tmpMac);
		//UnionLog("in UnionGenerateMACWithoutPredifinedHeaderAndWithA0x80Tail:: tmpMac after decrypt = [%s]\n",tmpMac);
		Union1DesEncrypt64BitsText(key,tmpMac,tmpMac);
		//UnionLog("in UnionGenerateMACWithoutPredifinedHeaderAndWithA0x80Tail:: tmpMac after encrypt = [%s]\n",tmpMac);
	}
	if (lenOfKey == 48)
	{
		Union1DesDecrypt64BitsText(key+16,tmpMac,tmpMac);
		Union1DesEncrypt64BitsText(key+32,tmpMac,tmpMac);
	}
	memcpy(mac,tmpMac,16);		
	return (0);
}
