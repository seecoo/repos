//	Author:		������
//	Copyright:	Union Tech. Guangzhou
//	Date:		2012-05-25

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "UnionLog.h"
#include "unionErrCode.h"
#include "unionMDLID.h"
#include "unionCommBetweenMDL.h"
#include "commWithHsmSvr.h"
#include "unionREC.h"
#include "UnionStr.h"

int		gunionLenOfHsmCmdHeader = 2;
int		gunionIsNotCheckHsmResCode = 0;
char		gunionDefaultHsmGroupID[40+1] = "102";
char		gunionSpecHsmGroupID[40+1];
unsigned long	gunionSpecHsmMDLID = 0;
char		gunionSpecHsmIPAddr[64+1];
unsigned long	gunionSpecHsmGrpMDLID = 0;

char		gunionCmdPrintType[5+1] = "";

// ʹ��BCD��չ��ӡHSM����ָ��
void UnionSetBCDPrintTypeForHSMReqCmd()
{
	strcpy(gunionCmdPrintType, "PRTT1");
	return;
}
// ʹ��BCD��չ��ӡHSM��Ӧָ��
void UnionSetBCDPrintTypeForHSMResCmd()
{
	strcpy(gunionCmdPrintType, "PRTT2");
	return;
}
// ʹ��BCD��չ��ӡHSMָ��
void UnionSetBCDPrintTypeForHSMCmd()
{
	strcpy(gunionCmdPrintType, "PRTT3");
	return;
}

// ʹ��***��ӡHSM����ָ�����
void UnionSetMaskPrintTypeForHSMReqCmd()
{
	strcpy(gunionCmdPrintType, "PRTT4");
	return;
}
// ʹ��***��ӡHSM��Ӧָ�����
void UnionSetMaskPrintTypeForHSMResCmd()
{
	strcpy(gunionCmdPrintType, "PRTT8");
	return;
}
// ʹ��***��ӡHSMָ�����
void UnionSetMaskPrintTypeForHSMCmd()
{
	strcpy(gunionCmdPrintType, "PRTT<");
	return;
}

void UnionResetPrintTypeForHSMCmd()
{
	memset(gunionCmdPrintType,0,sizeof(gunionCmdPrintType));
	return;
}
char *UnionGetHsmCmdPrintTypeStr()
{
	return(gunionCmdPrintType);
}

// ���ü��ܻ�ָ��ͷ�ĳ���
void UnionSetLenOfHsmCmdHeader(int len)
{
	gunionLenOfHsmCmdHeader = len;
}

// ���ò���Ҫ�����ܻ���Ӧ��
void UnionSetIsNotCheckHsmResCode()
{
	gunionIsNotCheckHsmResCode = 1;
}

// ������Ҫ�����ܻ���Ӧ��
void UnionSetIsCheckHsmResCode()
{
	gunionIsNotCheckHsmResCode = 0;
}

// ����ʹ�õļ��ܻ���ID
void UnionSetHsmGroupIDForHsmSvr(char *hsmGrpID)
{
	memset(gunionDefaultHsmGroupID,0,sizeof(gunionDefaultHsmGroupID));
	strcpy(gunionDefaultHsmGroupID,hsmGrpID);
}

// ��ȡʹ�õļ��ܻ���ID
char *UnionGetHsmGroupIDForHsmSvr()
{
	return(gunionDefaultHsmGroupID);
}

// ���÷���ָ��IP��ַ�ļ��ܻ�
// ʹ�����������ÿ��ָ��ֻ����Чһ�Σ��¸�ָ��ָ�ʹ�����ڼ��ܻ�
int UnionSetUseSpecHsmIPAddrForOneCmd(char *hsmIPAddr)
{
	if (hsmIPAddr == NULL)
		return(0);

	if (strlen(hsmIPAddr) == 0)
		return(0);

	if ((gunionSpecHsmMDLID = UnionGetFixedMDLIDOfTaskOfHsm(hsmIPAddr)) > 0)
	{
		UnionLog("in UnionSetUseSpecHsmIPAddr:: is use spec hsm = [%s],hsmMDLID = [%ld]\n",hsmIPAddr,gunionSpecHsmMDLID);
		memset(gunionSpecHsmIPAddr,0,sizeof(gunionSpecHsmIPAddr));
		sprintf(gunionSpecHsmIPAddr,"HSMIP%02d%s",(int)strlen(hsmIPAddr),hsmIPAddr);
		return(0);
	}
	else
	{
		UnionUserErrLog("in UnionSetUseSpecHsmIPAddr:: UnionGetFixedMDLIDOfTaskOfHsm hsmIPAddr = [%s]\n",hsmIPAddr);
		return(gunionSpecHsmMDLID);
	}
}

// ���÷���ָ����ļ��ܻ�
// ʹ�����������ÿ��ָ��ֻ����Чһ�Σ��¸�ָ��ָ�ʹ��Ĭ�����ڼ��ܻ�
int UnionSetUseSpecHsmGroupForOneCmd(char *hsmGrpID)
{
	if (hsmGrpID == NULL)
		return(0);

	if (strlen(hsmGrpID) == 0)
		return(0);
	
	if ((gunionSpecHsmGrpMDLID = UnionGetFixedMDLIDOfTaskOfHsmGrp(hsmGrpID)) > 0)
	{
		memset(gunionSpecHsmGroupID,0,sizeof(gunionSpecHsmGroupID));
		sprintf(gunionSpecHsmGroupID,hsmGrpID);
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
	char		buf[8192+1];
	int		printType;
	
	printType = gunionCmdPrintType[4];

	ptr = str;

	// add by liwj 2014-12-23 ������173�����ַ���,ʹ��RSA��33 SM2��K6
	if (flag && ((memcmp(str, "33", 2) == 0) || (memcmp(str, "K6", 2) == 0)))
	{
		UnionSetBCDPrintTypeForHSMReqCmd();
		goto plainPrint;
	}
	// end

	if((printType - '0') & 0x4)
	{
		memset(buf, '*', lenOfStr);
		buf[lenOfStr] = 0;
		//add by liwj 2014-12-18  ����ָ���ַ�
                memcpy(buf, str, 2);
                //end
		ptr = buf;
		if (flag)
			UnionLog("in UnionDirectHsmCmd:: req[%.2s][%24s][%04d][%s]!\n",str,hsmInfo,lenOfStr,ptr);
		else
			UnionLog("in UnionDirectHsmCmd:: res[%.2s][%24s][%04d][%s]!\n",str,hsmInfo,lenOfStr,ptr);
			
		return;
	}
	/*
	if((printType - '0') & 0x1)
	{
		bcdhex_to_aschex(str, lenOfStr, buf);
		buf[lenOfStr*2] = 0;
		ptr = buf;
	}
	*/
plainPrint: // add by liwj 2014-12-23
	if (flag)
	{
		if ((gunionCmdPrintType[4] = '1') || (gunionCmdPrintType[4] = '3'))
		{
			bcdhex_to_aschex(str, lenOfStr, buf);
			buf[lenOfStr*2] = 0;
			ptr = buf;
		}
		UnionLog("in UnionDirectHsmCmd:: req[%.2s][%24s][%04d][%s]!\n",str,hsmInfo,lenOfStr,ptr);
	}
	else
	{
		if ((gunionCmdPrintType[4] = '2') || (gunionCmdPrintType[4] = '3'))
		{
			bcdhex_to_aschex(str, lenOfStr, buf);
			buf[lenOfStr*2] = 0;
			ptr = buf;
		}
		UnionLog("in UnionDirectHsmCmd:: res[%.2s][%24s][%04d][%s]!\n",str,hsmInfo,lenOfStr,ptr);
	}
	// add by liwj 2014-12-23
	gunionCmdPrintType[4] = printType;
	// end

	return;
}

int UnionDirectHsmCmd(char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr)
{
	int	ret;
	int	lenOfHsmCmdHeader;
	int	timeoutOfHsm = 1;
	int	lenOfHsmReq;
	char	resHsmCmdHeader[4+1];
	char	errCode[2+1];
	char	hsmReqStr[8192*2+1];
	char	useHsmInfo[128+1];
	unsigned long	hsmMDLID = 0;
	char	tmpBuf[8192*2+1];

	// ��ȡ����ʱʱ��
	if ((timeoutOfHsm = UnionReadIntTypeRECVar("timeoutOfHsm")) <= 0)
		timeoutOfHsm = 3;	
	else
		timeoutOfHsm += 1;
	
	// ָ��ͷ���Ȼָ���Ĭ��ֵ2
	if ((lenOfHsmCmdHeader = gunionLenOfHsmCmdHeader) != 2)
		gunionLenOfHsmCmdHeader = 2;
		
	// ��Ӧָ��ͷ
	memset(resHsmCmdHeader,0,sizeof(resHsmCmdHeader));
	memcpy(resHsmCmdHeader,reqStr,lenOfHsmCmdHeader - 1);
	resHsmCmdHeader[lenOfHsmCmdHeader - 1] = reqStr[lenOfHsmCmdHeader - 1] + 1;
	
	memset(hsmReqStr,0,sizeof(hsmReqStr));
	lenOfHsmReq = 0;

	// �����мӴ�ӡ��չ�ַ���ʶ
	if(strlen(UnionGetHsmCmdPrintTypeStr()) > 0)
	{
		strcpy(hsmReqStr, UnionGetHsmCmdPrintTypeStr());
		lenOfHsmReq = strlen(UnionGetHsmCmdPrintTypeStr());
	}

	memset(useHsmInfo,0,sizeof(useHsmInfo));
	
	if (gunionSpecHsmMDLID > 0)		// ʹ��ָ���ļ��ܻ�
	{
		//hsmMDLID = gunionSpecHsmMDLID;
		if ((hsmMDLID = UnionGetFixedMDLIDOfTaskOfHsmGrp(gunionDefaultHsmGroupID)) < 0)
		{		
			UnionUserErrLog("in UnionDirectHsmCmd:: UnionGetFixedMDLIDOfFunSvr! gunionDefaultHsmGroupID = [%s]\n",gunionDefaultHsmGroupID);
			return(hsmMDLID);
		}
		strcpy(hsmReqStr + lenOfHsmReq, gunionSpecHsmIPAddr);
		lenOfHsmReq += strlen(gunionSpecHsmIPAddr);
		memcpy(hsmReqStr + lenOfHsmReq, reqStr, lenOfReqStr);
		lenOfHsmReq += lenOfReqStr;
		sprintf(useHsmInfo,"GROUP:%s HSMIP:%s",gunionDefaultHsmGroupID,gunionSpecHsmIPAddr);
	}
	else					// ʹ�����ڵļ��ܻ�
	{
		if (gunionSpecHsmGrpMDLID > 0)	// ʹ��ָ����ļ��ܻ�
		{
			hsmMDLID = gunionSpecHsmGrpMDLID;
			sprintf(useHsmInfo,"GROUP:%s",gunionSpecHsmGroupID);			
		}
		else				// ʹ��Ĭ����ļ��ܻ�
		{
			if ((hsmMDLID = UnionGetFixedMDLIDOfTaskOfHsmGrp(gunionDefaultHsmGroupID)) < 0)
			{		
				UnionUserErrLog("in UnionDirectHsmCmd:: UnionGetFixedMDLIDOfFunSvr! gunionDefaultHsmGroupID = [%s]\n",gunionDefaultHsmGroupID);
				return(hsmMDLID);
			}
			sprintf(useHsmInfo,"GROUP:%s",gunionDefaultHsmGroupID);			
		}
		memcpy(hsmReqStr + lenOfHsmReq, reqStr, lenOfReqStr);
		lenOfHsmReq += lenOfReqStr;
	}

	if (hsmMDLID <= 0)
	{
		UnionUserErrLog("in UnionDirectHsmCmd:: hsmMDLID = [%ld] error\n",hsmMDLID);
		return(errCodeParameter);
	}
	
	UnionLog("in UnionDirectHsmCmd:: HSM��Ϣ[%s],��ʱʱ��=[%dS]\n",useHsmInfo,timeoutOfHsm);

	//UnionLog("in UnionDirectHsmCmd:: req[%04d][%s]!\n",lenOfReqStr,reqStr);
	UnionHsmLog(1,useHsmInfo,lenOfReqStr,reqStr);
	memset(tmpBuf, 0, sizeof(tmpBuf));
	bcdhex_to_aschex(reqStr, lenOfReqStr, tmpBuf);
	UnionAuditLog("===== reqStr [%s] ==\n", tmpBuf);

	// ִ�з���
	if ((ret = UnionExchangeWithSpecModuleVerifyReqAndRes(hsmMDLID,(unsigned char *)hsmReqStr,lenOfHsmReq,(unsigned char *)resStr,sizeOfResStr,timeoutOfHsm)) < 0)
	{
		UnionUserErrLog("in UnionDirectHsmCmd:: UnionExchangeWithSpecModuleVerifyReqAndRes! ret = [%d] hsmMDLID = [%ld] timeoutOfHsm = [%d]\n",ret,hsmMDLID,timeoutOfHsm);
		gunionSpecHsmMDLID = 0;
		gunionSpecHsmGrpMDLID = 0;
		UnionResetPrintTypeForHSMCmd();
		return(ret);
	}

	gunionSpecHsmMDLID = 0;
	gunionSpecHsmGrpMDLID = 0;
	UnionResetPrintTypeForHSMCmd();
	
	resStr[ret] = 0;
	//UnionLog("in UnionDirectHsmCmd:: res[%04d][%s]!\n",ret,resStr);
	UnionHsmLog(0,useHsmInfo,ret,resStr);
	memset(tmpBuf, 0, sizeof(tmpBuf));
	bcdhex_to_aschex(resStr, ret, tmpBuf);
	UnionAuditLog("==== resStr [%s] ==\n", tmpBuf);

	// ���ָ��ͷ
	if ((memcmp(resStr,resHsmCmdHeader,lenOfHsmCmdHeader) != 0) || (ret < lenOfHsmCmdHeader + 2))
	{
		UnionUserErrLog("in UnionDirectHsmCmd:: check head cmdError resStr = [%s]!\n",resStr);
		return(errCodeHsmCmdMDL_ReturnLen);
	}
	
	if (!gunionIsNotCheckHsmResCode)
	{	
		// �����Ӧ��
		memset(errCode,0,sizeof(errCode));
		memcpy(errCode,resStr + lenOfHsmCmdHeader,2);
		if (memcmp(errCode,"00",2) != 0)
		{
			UnionUserErrLog("in UnionDirectHsmCmd:: resStr = [%s]!\n",resStr);
			return(errCodeHsmCmdMDL_ErrCodeNotSuccess);
		}
	}
	
	return(ret);
}
