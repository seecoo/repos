// Copyright: Union Tech.
// Author:	Wolfgang Wang
// Date:	2008/5/11

#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "UnionStr.h"
#include "unionErrCode.h"
#include "UnionLog.h"

int UnionExchangeWithSpecModuleVerifyReqAndRes(long mdlID __attribute__((unused)),unsigned char *reqStr,int lenOfReqStr,unsigned char *resStr,int sizeOfResStr,int timeoutOfFunSvr __attribute__((unused)))
{
	char	tmpBuf[8192+1];
	int	len;
	char	tag[2+1];
	char	fileName[256+1];
	FILE	*fp;
	int	useBits = 0;

	memset(tag,0,sizeof(tag));
	len = reqStr[0] * 256 + reqStr[1];
	if (len + 2 == lenOfReqStr)	// 前面两个字节是长度
		memcpy(tag,reqStr+2,2);
	else
	{
		bcdhex_to_aschex((char *)reqStr,1,tag);
		useBits = 1;
	}
	if (!useBits)
	{
		reqStr[lenOfReqStr] = 0;
		UnionRealNullLog("nonBits::reqStr=[%04d][%s]\n",lenOfReqStr-2,reqStr+2);
	}
	else
	{
		if (lenOfReqStr*2 >= (int)sizeof(tmpBuf)) 
		{
			UnionUserErrLog("in UnionExchangeWithSpecModuleVerifyReqAndRes:: lenOfReqStr [%d] too large!\n",lenOfReqStr);
			return(errCodeSmallBuffer);
		}
		bcdhex_to_aschex((char *)reqStr,lenOfReqStr,tmpBuf);
		tmpBuf[lenOfReqStr*2] = 0;
		UnionRealNullLog("useBits::reqStr=[%04d][%s]\n",lenOfReqStr,tmpBuf);
	}
	
	// 获得响应	
	sprintf(fileName,"%s/%s",getenv("UNIONTEMP"),tag);
	if ((fp = fopen(fileName,"r")) != NULL)
	{
		len = UnionReadOneDataLineFromTxtFile(fp,tmpBuf,sizeof(tmpBuf));
		fclose(fp);
	}
	else
		len = -1;
	// 没有定义响应，把请求直接复制给响应
	if (len < 0)
	{
		memcpy(resStr,reqStr,lenOfReqStr);
		len = lenOfReqStr;
	}
	else
	{
		if (useBits)
		{
			if (len / 2 >= sizeOfResStr)
			{
				UnionUserErrLog("in UnionExchangeWithSpecModuleVerifyReqAndRes:: sizeOfResStr [%d] too small!\n",sizeOfResStr);
				return(errCodeSmallBuffer);
			}
			aschex_to_bcdhex(tmpBuf,len,(char *)resStr);
			len = len / 2;
		}
		else
		{
			if (len >= sizeOfResStr-2)
			{
				UnionUserErrLog("in UnionExchangeWithSpecModuleVerifyReqAndRes:: sizeOfResStr [%d] too small!\n",sizeOfResStr);
				return(errCodeSmallBuffer);
			}
			memcpy(resStr+2,tmpBuf,len);
			resStr[0] = len / 256;
			resStr[1] = len % 256;
			len = len + 2;
		}
	}
	resStr[len] = 0;
	if (!useBits)
		UnionRealNullLog("nonBits::resStr=[%04d][%s]\n",len-2,resStr+2);
	else
	{
		bcdhex_to_aschex((char *)resStr,len,tmpBuf);
		tmpBuf[len*2] = 0;
		UnionRealNullLog("useBits::resStr=[%04d][%s]\n",len,tmpBuf);
	}
	return(len);	
}
