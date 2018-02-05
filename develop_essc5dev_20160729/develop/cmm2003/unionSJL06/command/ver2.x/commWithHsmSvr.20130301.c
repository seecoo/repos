//	Author:		张永定
//	Copyright:	Union Tech. Guangzhou
//	Date:		2012-05-25

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#ifndef _UnionLogMDL_3_x_
#define _UnionLogMDL_3_x_
#endif

#include "UnionLog.h"
#include "unionErrCode.h"
#include "unionMDLID.h"
#include "unionCommBetweenMDL.h"
#include "unionRealBaseDB.h"
#include "commWithHsmSvr.h"
#include "unionREC.h"
#include "UnionStr.h"
#include "unionMsgBufGroup.h"
#include "unionXMLPackage.h"
#include "unionTableData.h"
int UnionIsLastMsgIndexFree();

int		gunionLenOfHsmCmdHeader = 2;
int		gunionIsNotCheckHsmResCode = 0;
char		gunionDefaultHsmGroupID[64] = "102";
char		gunionSpecHsmGroupID[64];
int		gunionSpecHsm = 0;
char		gunionSpecHsmIPAddr[64];
unsigned long	gunionSpecHsmGrpMDLID = 0;

char		gunionCmdPrintType[32] = "";

// modified 2015-04-21
void UnionSetPrintTypeCharForHSMCmd(int value)
{
	unsigned char 	ch;

	memcpy(gunionCmdPrintType, "PRTT", 4);
	ch = gunionCmdPrintType[4];
	ch = (ch == 0)? 0 : (ch - '0');
	ch |= value;
	gunionCmdPrintType[4] = ch + '0';
	return;
}

// 使用BCD扩展打印HSM请求指令
void UnionSetBCDPrintTypeForHSMReqCmd()
{
	UnionSetPrintTypeCharForHSMCmd(0x01);
	return;
}
// 使用BCD扩展打印HSM响应指令
void UnionSetBCDPrintTypeForHSMResCmd()
{
	UnionSetPrintTypeCharForHSMCmd(0x02);
	return;
}
// 使用BCD扩展打印HSM指令
void UnionSetBCDPrintTypeForHSMCmd()
{
	UnionSetPrintTypeCharForHSMCmd(0x03);
	return;
}

// 使用***打印HSM请求指令报文体
void UnionSetMaskPrintTypeForHSMReqCmd()
{
	UnionSetPrintTypeCharForHSMCmd(0x04);
	return;
}
// 使用***打印HSM响应指令报文体
void UnionSetMaskPrintTypeForHSMResCmd()
{
	UnionSetPrintTypeCharForHSMCmd(0x08);
	return;
}
// 使用***打印HSM指令报文体
void UnionSetMaskPrintTypeForHSMCmd()
{
	UnionSetPrintTypeCharForHSMCmd(0x0C);
	return;
}
// end of modified 2015-04-21

void UnionResetPrintTypeForHSMCmd()
{
	memset(gunionCmdPrintType,0,6);
	return;
}
char *UnionGetHsmCmdPrintTypeStr()
{
	return(gunionCmdPrintType);
}

// 设置加密机指令头的长度
void UnionSetLenOfHsmCmdHeader(int len)
{
	gunionLenOfHsmCmdHeader = len;
}

// 设置不需要检查加密机响应码
void UnionSetIsNotCheckHsmResCode()
{
	gunionIsNotCheckHsmResCode = 1;
}

// 设置需要检查加密机响应码
void UnionSetIsCheckHsmResCode()
{
	gunionIsNotCheckHsmResCode = 0;
}

// 设置使用的加密机组ID
void UnionSetHsmGroupIDForHsmSvr(char *hsmGrpID)
{
	snprintf(gunionDefaultHsmGroupID,sizeof(gunionDefaultHsmGroupID),"%s",hsmGrpID);
}

// 读取使用的加密机组ID
char *UnionGetHsmGroupIDForHsmSvr()
{
	return(gunionDefaultHsmGroupID);
}

// 设置访问指定IP地址的加密机
// 使用这个函数，每个指令只能生效一次，下个指令恢复使用组内加密机
int UnionSetUseSpecHsmIPAddrForOneCmd(char *hsmIPAddr)
{
	int	len;

	if (hsmIPAddr == NULL)
		return(0);

	if (strlen(hsmIPAddr) == 0)
		return(0);

	gunionSpecHsm = 1;

	len = sprintf(gunionSpecHsmIPAddr,"HSMIP%02zu%s",strlen(hsmIPAddr),hsmIPAddr);
	gunionSpecHsmIPAddr[len] = 0;

	UnionLog("in UnionSetUseSpecHsmIPAddr:: is use spec hsm = [%s]\n",hsmIPAddr);
	return(0);
}

// 设置访问指定组的加密机
// 使用这个函数，每个指令只能生效一次，下个指令恢复使用默认组内加密机
int UnionSetUseSpecHsmGroupForOneCmd(char *hsmGrpID)
{
	if (hsmGrpID == NULL)
		return(0);

	if (strlen(hsmGrpID) == 0)
		return(0);
	
	if ((gunionSpecHsmGrpMDLID = UnionGetFixedMDLIDOfTaskOfHsmGrp(hsmGrpID)) > 0)
	{
		snprintf(gunionSpecHsmGroupID,sizeof(gunionSpecHsmGroupID),"%s",hsmGrpID);
		UnionLog("in UnionSetUseSpecHsmIPAddr:: is use spec hsm group = [%s],hsmMDLID = [%ld]\n",hsmGrpID,gunionSpecHsmGrpMDLID);
		return(0);
	}
	else
	{
		UnionUserErrLog("in UnionSetUseSpecHsmIPAddr:: UnionGetFixedMDLIDOfTaskOfHsmGrp hsmGrpID = [%s]\n",hsmGrpID);
		return(gunionSpecHsmGrpMDLID);
	}
}

static void UnionHsmLog(int flag,char *hsmInfo, int lenOfStr, char *str)
{
	char		*ptr = NULL;
	char		*buf = NULL;
	int		printType;

	if ((hsmInfo == NULL) || (str == NULL))
	{
		UnionUserErrLog("in UnionHsmLog:: hsmInfo or str is null!\n");
		return;
	}
	
	printType = gunionCmdPrintType[4];

	ptr = str;
	buf = malloc(lenOfStr*2 + 1);

	// add by liwj 2014-12-23
	// E141服务，不屏蔽请求字符串
	if (flag && ((memcmp(str, "NG", 2) == 0) || (memcmp(str, "WWS1", 4) == 0)))
		goto plainPrint;
	// 不屏蔽173请求字符串,使用RSA：33 SM2：K6
	if (flag && ((memcmp(str, "33", 2) == 0) || (memcmp(str, "K6", 2) == 0)))
	{
		UnionSetBCDPrintTypeForHSMReqCmd();
		goto plainPrint;
	}
	// end
	
	if (flag)
	{
		if ((printType - '0') & 0x4)
		{
			memset(buf, '*', lenOfStr);
			buf[lenOfStr] = 0;
			//add by liwj 2014-12-18  保留指令字符
			memcpy(buf, str, 2);
			//end
			UnionLog("in UnionDirectHsmCmd:: req[%.2s][%s][%04d][%s]!\n",str,hsmInfo,lenOfStr,buf);
			free(buf);
			return;
		}
	}
	else
	{
		if ((printType - '0') & 0x8)
		{
			memset(buf, '*', lenOfStr);
			buf[lenOfStr] = 0;
			memcpy(buf, str, 2);
			UnionLog("in UnionDirectHsmCmd:: res[%.2s][%s][%04d][%s]!\n",str,hsmInfo,lenOfStr,buf);
			free(buf);
			return;
		}
	}

plainPrint: // add by liwj 2014-12-23
	if (flag)
	{ 
		// modified 2015-4-21 feng
		//if ((gunionCmdPrintType[4] == '1') || (gunionCmdPrintType[4] == '3'))
                if ((printType - '0') & 0x1)
		{
			bcdhex_to_aschex(str, lenOfStr, buf);
			buf[lenOfStr*2] = 0;
			ptr = buf;
		}
		UnionLog("in UnionDirectHsmCmd:: req[%.2s][%s][%04d][%s]!\n",str,hsmInfo,lenOfStr,ptr);
	}
	else
	{
		// modified 2015-4-21 feng
		//if ((gunionCmdPrintType[4] == '2') || (gunionCmdPrintType[4] == '3'))
		if ((printType - '0') & 0x2)
		{
			bcdhex_to_aschex(str, lenOfStr, buf);
			buf[lenOfStr*2] = 0;
			ptr = buf;
		}
		UnionLog("in UnionDirectHsmCmd:: res[%.2s][%s][%04d][%s]!\n",str,hsmInfo,lenOfStr,ptr);
	}
	// add by liwj 2014-12-23
	gunionCmdPrintType[4] = printType;
	// end
	free(buf);
	return;
}

void UnionGetHsmResCmdCode(int codeLen, char *req, char *resCode)
{
	int		offset = 0;
	char		tmp[32];

	if(strncmp(req + offset, "PRTT", 4) == 0)
	{
		offset += 5; // PRTTn 五位打印格式
	}

	if(strncmp(req + offset, "HSMIP", 5) == 0)
	{
		offset += 5; // HSMIP
		memcpy(tmp, req + offset, 2);
		tmp[2] = 0;
		offset += 2; // ip长度
		offset += atoi(tmp); // ip内容
	}
	
	if (codeLen > 0)
	{
		memcpy(resCode, req + offset, codeLen);
		resCode[codeLen - 1] += 1;
		resCode[codeLen] = 0;
	}
	else
	{
		resCode[0] = 0;
	}
}

int UnionDirectHsmCmd(char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr)
{
	int	ret;
	int	len;
	int	lenOfHsmCmdHeader;
	int	timeoutOfHsm = 1;
	int	lenOfHsmReq = 0;
	char	resHsmCmdHeader[32];
	char	errCode[32];
	char	hsmReqStr[8192];
	char	useHsmInfo[512];
	unsigned long	hsmMDLID = 0;
	int	tryTimes = 0;

	// 读取服务超时时间
	if ((timeoutOfHsm = UnionReadIntTypeRECVar("timeoutOfHsm")) <= 0)
		timeoutOfHsm = 3;	
	else
		timeoutOfHsm += 1;
	
	// 指令头长度恢复成默认值2
	if ((lenOfHsmCmdHeader = gunionLenOfHsmCmdHeader) != 2)
		gunionLenOfHsmCmdHeader = 2;
	
	// 响应指令头
	// modified 2015-05-25 支持送PRTT和HSMIP的报文格式
	/*
	memcpy(resHsmCmdHeader,reqStr,lenOfHsmCmdHeader - 1);
	resHsmCmdHeader[lenOfHsmCmdHeader - 1] = reqStr[lenOfHsmCmdHeader - 1] + 1;
	resHsmCmdHeader[lenOfHsmCmdHeader] = 0;
	*/
	UnionGetHsmResCmdCode(lenOfHsmCmdHeader, reqStr, resHsmCmdHeader);
	// end of modification 2015-05-25 
	
	// 报文中加打印扩展字符标识
	if(strlen(UnionGetHsmCmdPrintTypeStr()) > 0)
		lenOfHsmReq = sprintf(hsmReqStr, "%s", UnionGetHsmCmdPrintTypeStr());

	if (gunionSpecHsm > 0)		// 使用指定的加密机
	{
		if ((hsmMDLID = UnionGetFixedMDLIDOfTaskOfHsmGrp(gunionDefaultHsmGroupID)) < 0)
		{		
			UnionUserErrLog("in UnionDirectHsmCmd:: UnionGetFixedMDLIDOfFunSvr! gunionDefaultHsmGroupID = [%s]\n",gunionDefaultHsmGroupID);
			return(hsmMDLID);
		}
		lenOfHsmReq += sprintf(hsmReqStr + lenOfHsmReq, "%s",gunionSpecHsmIPAddr);
		memcpy(hsmReqStr + lenOfHsmReq, reqStr, lenOfReqStr);
		lenOfHsmReq += lenOfReqStr;
		len = sprintf(useHsmInfo,"GROUP:%s HSMIP:%s",gunionDefaultHsmGroupID,gunionSpecHsmIPAddr);
	}
	else					// 使用组内的加密机
	{
		if (gunionSpecHsmGrpMDLID > 0)	// 使用指定组的加密机
		{
			hsmMDLID = gunionSpecHsmGrpMDLID;
			len = sprintf(useHsmInfo,"GROUP:%s",gunionSpecHsmGroupID);			
		}
		else				// 使用默认组的加密机
		{
			if ((hsmMDLID = UnionGetFixedMDLIDOfTaskOfHsmGrp(gunionDefaultHsmGroupID)) < 0)
			{		
				UnionUserErrLog("in UnionDirectHsmCmd:: UnionGetFixedMDLIDOfFunSvr! gunionDefaultHsmGroupID = [%s]\n",gunionDefaultHsmGroupID);
				return(hsmMDLID);
			}
			len = sprintf(useHsmInfo,"GROUP:%s",gunionDefaultHsmGroupID);			
		}
		memcpy(hsmReqStr + lenOfHsmReq, reqStr, lenOfReqStr);
		lenOfHsmReq += lenOfReqStr;
	}
	useHsmInfo[len] = 0;
	hsmMDLID += conMDLTypeOffsetOfHsmSvr;
	hsmReqStr[lenOfHsmReq] = 0;
	
	if (hsmMDLID <= 0)
	{
		UnionUserErrLog("in UnionDirectHsmCmd:: hsmMDLID = [%ld] error\n",hsmMDLID);
		return(errCodeParameter);
	}
	
	UnionLog("in UnionDirectHsmCmd:: hsmInfo[%s],timeout[%dS]\n",useHsmInfo,timeoutOfHsm);
	UnionHsmLog(1,useHsmInfo,lenOfReqStr,reqStr);

	// 执行服务
	tryTimes = 0;
	do
	{
		// added 2016-06-15
		if(tryTimes > 0 && ret == errCodeEsscMDL_WaitResponse) // 超时
		{
			if(!UnionIsLastMsgIndexFree()) // 发送消息未被处理则不重做
			{
				UnionUserErrLog("in UnionDirectHsmCmd:: UnionExchangeWithSpecModuleVerifyReqAndRes! timeout ret = [%d] hsmMDLID = [%ld]\n", ret, hsmMDLID);
				UnionSetResponseRemark("等待密码机响应超时.");
				return(errCodeEsscMDL_WaitResponse);
			}

		}
		// end of addition 2016-06-15
		UnionSetGroupIDOfMsgBuf(defMsgBufGroupOfHsmSvr);
		if ((ret = UnionExchangeWithSpecModuleVerifyReqAndRes(hsmMDLID,(unsigned char *)hsmReqStr,lenOfHsmReq,(unsigned char *)resStr,sizeOfResStr,timeoutOfHsm)) < 0)
		{
			UnionUserErrLog("in UnionDirectHsmCmd:: UnionExchangeWithSpecModuleVerifyReqAndRes! ret = [%d] hsmMDLID = [%ld] timeoutOfHsm = [%d]\n",ret,hsmMDLID,timeoutOfHsm);
		}
	}while(ret < 0 && tryTimes++ < 2);

	if(ret < 0)
	{
		UnionUserErrLog("in UnionDirectHsmCmd:: UnionExchangeWithSpecModuleVerifyReqAndRes! ret = [%d] hsmMDLID = [%ld] timeoutOfHsm = [%d] tryTimes = %d\n",ret,hsmMDLID,timeoutOfHsm, tryTimes);
		if (ret == errCodeEsscMDL_WaitResponse)
		{
			UnionSetResponseRemark("等待密码机响应超时");
		}
		gunionSpecHsm = 0;
		gunionSpecHsmGrpMDLID = 0;
		UnionResetPrintTypeForHSMCmd();
		return(ret);
	}

	gunionSpecHsm = 0;
	gunionSpecHsmGrpMDLID = 0;
	
	resStr[ret] = 0;
	//UnionLog("in UnionDirectHsmCmd:: res[%04d][%s]!\n",ret,resStr);
	UnionHsmLog(0,useHsmInfo,ret,resStr);

	UnionResetPrintTypeForHSMCmd();
	// 检查指令头
	if (lenOfHsmCmdHeader > 0)
	{
		if ((memcmp(resStr,resHsmCmdHeader,lenOfHsmCmdHeader) != 0) || (ret < lenOfHsmCmdHeader + 2))
		{
			UnionUserErrLog("in UnionDirectHsmCmd:: check head cmdError len[%d], exp[%s] != resStr = [%s]!\n", lenOfHsmCmdHeader, resHsmCmdHeader, resStr);
			return(errCodeHsmCmdMDL_ReturnLen);
		}
	}
	
	if (!gunionIsNotCheckHsmResCode)
	{	
		// 检查响应码
		if (memcmp(resStr + lenOfHsmCmdHeader,"00",2) != 0)
		{
			UnionUserErrLog("in UnionDirectHsmCmd:: resStr = [%s]!\n",resStr);
			memcpy(errCode,resStr + lenOfHsmCmdHeader,2);
			errCode[2] = 0;
			return(errCodeOffsetOfHsmReturnCodeMDL - atoi(errCode));
		}
	}
	
	return(ret);
}

int UnionGetHsmGroupRecByHsmGroupID(char *hsmGroupID,PUnionHsmGroupRec phsmGroupRec)
{
	int			ret;
	int			enabled = 0;
	char			sql[512+1];
	char			tmpBuf[128+1];
	char			*precStr = NULL;

	if ((precStr = UnionFindTableValue("hsmGroup", hsmGroupID)) == NULL)
	{		
		//len = sprintf(sql,"select lmkProtectMode,hsmCmdVersionID from hsmGroup,hsm where hsmGroup.hsmGroupID = hsm.hsmGroupID and hsmGroup.hsmGroupID = '%s' and hsm.enabled = 1 and hsmGroup.enabled = 1",hsmGroupID);
		snprintf(sql,sizeof(sql),"select lmkProtectMode,hsmCmdVersionID,enabled from hsmGroup where hsmGroupID = '%s'",hsmGroupID);
		if ((ret = UnionSelectRealDBRecord(sql,1,1)) < 0)
		{
			UnionUserErrLog("in UnionGetHsmGroupRecByHsmGroupID:: UnionSelectRealDBRecord[%s]!\n",sql);
			return(ret);
		}
		else if (ret == 0)
			return(errCodeEsscMDL_EsscHsmGrpIDNotDefined);
			
		UnionLocateXMLPackage("detail", 1);
		
		if ((ret = UnionReadXMLPackageValue("enabled", tmpBuf, sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionGetHsmGroupRecByHsmGroupID:: UnionReadXMLPackageValue[%s]!\n","enabled");
			return(ret);
		}
		tmpBuf[ret] = 0;
		enabled = atoi(tmpBuf);
		if ((ret = UnionReadXMLPackageValue("lmkProtectMode", phsmGroupRec->lmkProtectMode, sizeof(phsmGroupRec->lmkProtectMode))) < 0)
		{
			UnionUserErrLog("in UnionGetHsmGroupRecByHsmGroupID:: UnionReadXMLPackageValue[%s]!\n","lmkProtectMode");
			return(ret);
		}
		phsmGroupRec->lmkProtectMode[ret] = 0;
		if ((ret = UnionReadXMLPackageValue("hsmCmdVersionID", tmpBuf, sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionGetHsmGroupRecByHsmGroupID:: UnionReadXMLPackageValue[%s]!\n","hsmCmdVersionID");
			return(ret);
		}
		tmpBuf[ret] = 0;
		phsmGroupRec->hsmCmdVersionID = atoi(tmpBuf);
	}
	else
	{
		if (strlen(precStr) == 0)
		{
			UnionUserErrLog("in UnionGetHsmGroupRecByHsmGroupID:: hsmGroupID[%s] not found!\n",hsmGroupID);
			return(errCodeEsscMDL_EsscHsmGrpIDNotDefined);
		}
		
		/*
		if ((ret = UnionReadRecFldFromRecStr(precStr,strlen(precStr),"hsmGroupID",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionGetHsmGroupRecByHsmGroupID:: UnionReadRecFldFromRecStr[hsmGroupID][%s]!\n",precStr);
			return(ret);
		}
		tmpBuf[ret] = 0;
		if (strcmp(hsmGroupID,tmpBuf) != 0)
		{
			UnionUserErrLog("in UnionGetHsmGroupRecByHsmGroupID:: hsmGroupID[%s] not found!\n",hsmGroupID);
			return(errCodeEsscMDL_EsscHsmGrpIDNotDefined);
		}
		*/
		if ((ret = UnionReadRecFldFromRecStr(precStr,strlen(precStr),"enabled",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionGetHsmGroupRecByHsmGroupID:: UnionReadRecFldFromRecStr[enabled][%s]!\n",precStr);
			return(ret);
		}
		tmpBuf[ret] = 0;
		enabled = atoi(tmpBuf);
		if ((ret = UnionReadRecFldFromRecStr(precStr,strlen(precStr),"lmkProtectMode",phsmGroupRec->lmkProtectMode,sizeof(phsmGroupRec->lmkProtectMode))) < 0)
		{
			UnionUserErrLog("in UnionGetHsmGroupRecByHsmGroupID:: UnionReadRecFldFromRecStr[lmkProtectMode][%s]!\n",precStr);
			return(ret);
		}
		phsmGroupRec->lmkProtectMode[ret] = 0;
		if ((ret = UnionReadRecFldFromRecStr(precStr,strlen(precStr),"hsmCmdVersionID",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionGetHsmGroupRecByHsmGroupID:: UnionReadRecFldFromRecStr[hsmCmdVersionID][%s]!\n",precStr);
			return(ret);
		}
		tmpBuf[ret] = 0;
		phsmGroupRec->hsmCmdVersionID = atoi(tmpBuf);
	}

	if (enabled != 1)
	{
		UnionUserErrLog("in UnionGetHsmGroupRecByHsmGroupID:: hsmGroupID[%s]未激活!\n",hsmGroupID);
		return(errCodeEsscMDL_EsscHsmGrpIDNotDefined);
	}
	
	strcpy(phsmGroupRec->hsmGroupID,hsmGroupID);

	return(0);
}
