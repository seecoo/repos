/* Copyright:	Union Tech.
   Author:	Wolfgang Wang
   Date:	2006/8/8
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

#include "unionErrCode.h"
#include "centerREC.h"
#include "UnionSocket.h"
#include "UnionLog.h"

#include "unionServicePackage.h"
#include "errCodeTranslater.h"
#include "commWithTcpipSvr.h"

int UnionCommWithCmmPackSvr(int serviceID,PUnionCmmPackData ppackReq,PUnionCmmPackData ppackRes)
{
	int			lenOfReq,lenOfRes,ret;
	char			tmpBuf[8192+1];
	char			idOfAppOfRes[10+1];
	int			serviceIDOfRes;
        unsigned char           tmpConnTag[256+1];
	int			resCode;
	char			cliIPAddr[100];

        // 初始化运行环境
	if ((ret = UnionConnectCenterREC()) < 0)
	{
		UnionUserErrLog("in UnionCommWithHsmSvrForThread:: UnionConnectCenterREC!\n");
		return(ret);
	}

	// 打包包标识
	if ((ret = UnionPutCmmPackIdentifiedTag(ppackReq)) < 0)
	{
		UnionUserErrLog("in UnionCommWithCmmPackSvr:: UnionPutCmmPackIdentifiedTag [%d]!\n",conServiceFldProcID);
		return(ret);
	}
	strcpy(cliIPAddr,UnionGetIPAddrOfMyself());
	if ((ret = UnionPutCmmPackFldIntoFldDataList(conServiceFldPackCliIPAddr,strlen(cliIPAddr),cliIPAddr,ppackReq)) < 0)
	{
		UnionUserErrLog("in UnionCommWithCmmPackSvr:: UnionPutCmmPackFldIntoFldDataList [%d]!\n",conServiceFldPackCliIPAddr);
		return(ret);
	}
	if ((ret = UnionPutIntTypeCmmPackFldIntoFldDataList(conServiceFldPackCliPort,UnionGetPortOfCenterSecuSvr(0),ppackReq)) < 0)
	{
		UnionUserErrLog("in UnionCommWithCmmPackSvr:: UnionPutIntTypeCmmPackFldIntoFldDataList [%d]!\n",conServiceFldPackCliPort);
		return(ret);
	}

	// 打包
	if ((lenOfReq = UnionPackRequestPackage(UnionGetIDOfEsscAPI(),serviceID,ppackReq,tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionCommWithCmmPackSvr:: UnionPackRequestPackage!\n");
		return(lenOfReq);
	}
        memset(tmpConnTag,0,sizeof(tmpConnTag));
	if ((lenOfRes = UnionCommWithHsmSvrForThread(tmpBuf,lenOfReq,tmpBuf,sizeof(tmpBuf),tmpConnTag)) < 0)
	{
		UnionUserErrLog("in UnionCommWithCmmPackSvr:: UnionCommWithHsmSvrForThread!\n");
		return(lenOfRes);
	}
	// 解响应包
	memset(idOfAppOfRes,0,sizeof(idOfAppOfRes));
	if ((ret = UnionUnpackResponsePackage(tmpBuf,lenOfRes,idOfAppOfRes,&serviceIDOfRes,&resCode,ppackRes)) < 0)
	{
		UnionUserErrLog("in UnionCommWithCmmPackSvr:: UnionUnpackResponsePackage!\n");
		return(ret);
	}
	if ((ret = UnionVerifyCmmPackIdentifiedTag(ppackReq,ppackRes)) < 0)
	{
		UnionUserErrLog("in UnionCommWithCmmPackSvr:: UnionVerifyCmmPackIdentifiedTag!\n");
		UnionReleaseCommWithHsmSvr(tmpConnTag);
		return(ret);
	}
	if ((strcmp(idOfAppOfRes,UnionGetIDOfEsscAPI()) != 0) || (serviceIDOfRes != serviceID))
	{
		UnionUserErrLog("in UnionCommWithCmmPackSvr:: req and res not the same package! idOfAppOfRes [%s] != [%s],serviceIDOfRes[%d] !=  [%d]\n",
				idOfAppOfRes,UnionGetIDOfEsscAPI(),serviceIDOfRes,serviceID);
		UnionReleaseCommWithHsmSvr(tmpConnTag);
		return(errCodeRequestAndResponseNotSame);
	}
	return(resCode);
}
