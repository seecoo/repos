// Copyright: Union Tech.
// Author:	Wolfgang Wang
// Date:	2008/5/11

#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_
#endif

#define _realBaseDB_2_x_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if ( defined __linux__ )
#include <zlib.h>
#endif

#ifndef _WIN32
#include <signal.h>
#include <setjmp.h>
#include <time.h>
#include "unionREC.h"
#include "UnionTask.h"
#include "unionCommConf.h"
#include "unionREC.h"
#else
#include "unionRECVar.h"
#include <winsock.h>
#endif

#include "UnionLog.h"
#include "UnionStr.h"
#include "errCodeTranslater.h"
#include "unionFunSvrName.h"
#include "unionMsgBufGroup.h"

#include "unionVersion.h"
#include "UnionSocket.h"
#include "synchTCPIPSvr.h"
#include "unionCommBetweenMDL.h"
#include "commWithTransSpier.h"
#include "unionXMLPackage.h"
#include "unionRealBaseDB.h"
#include "packageConvert.h"
#include "registerRunningTime.h"
#include "unionHighCachedAPI.h"
#include "unionTableData.h"

#ifndef _WIN32
int				gsynchTCPIPSvrSckHDL = -1;
extern PUnionTaskInstance 	ptaskInstance;

#ifndef _noRegisterClient_
PUnionCommConf			pgunionCommConf = NULL;
#endif

jmp_buf gunionTCPSvrJmpEnvForEsscClient;
jmp_buf gunionTCPSvrJmpEnvForMaxIdleTime;

void UnionDealTCPSvrJmpEnvForEsscClient();
void UnionDealTCPSvrJmpEnvForMaxIdleTime();

int UnionSetXMLResponsePackageForError(int resCode);
void UnionSetResponseRemarkForError(char *fmt,...);

int UnionGetActiveTCIPSvrSckHDL()
{
	return(gsynchTCPIPSvrSckHDL);
}
#endif

char	gunionTableDataFlag[3+1];

int UnionSendMonInfoToTransMonMDL(unsigned char *msg,int lenOfMsg)
{
	int	ret;
	TUnionMessageHeader	reqMsgHeader;

	if (UnionExistsTaskOfName("transMonSvr") <= 0)
	{
		//UnionLog("in UnionSendYLPackageToTransMonSvrMDL:: task ylqzTransMon not started!\n");
		return(0);
	}

	memset(&reqMsgHeader,0,sizeof(reqMsgHeader));
	if ((ret = UnionSendRequestToSpecifiedModuleWithNewMsgHeaderWithTime(conMDLTypeOffsetOfMonSvr + conMDLTypeUnionMonSvr,msg,lenOfMsg,&reqMsgHeader,1)) < 0)
	{
		UnionLog("in UnionSendMonInfoToTransMonMDL:: UnionSendRequestToSpecifiedModuleWithNewMsgHeader!\n");
		return(0);
	}

	return(0);
}

TUnionModuleID UnionReadMDLIDFromHighCached(char *sysID,char *appID,char *serviceCode,char *cliIPAddr,char *key)
{
	int	ret;
	char	tmpBuf[16+1];
	
	// ʹ���˹����ڴ�
	if (memcmp(gunionTableDataFlag,"111",3) == 0)
		return(0);

	sprintf(key,"externalSystem:%s|app:%s|serviceCode:%s|serviceType:%s",sysID,appID,serviceCode,cliIPAddr);
	if ((ret = UnionGetHighCachedValue(key,tmpBuf,sizeof(tmpBuf))) <= 0)
		return(0);
	
	tmpBuf[ret] = 0;	
	return(atol(tmpBuf));
}

int UnionSetMDLIDToHighCached(char *key,TUnionModuleID mdlID)
{
	int	len;
	char	tmpBuf[16+1];

	// ʹ���˹����ڴ�
	if (memcmp(gunionTableDataFlag,"111",3) == 0)
		return(0);

	len = sprintf(tmpBuf,"%ld",mdlID);
	tmpBuf[len] = 0;
	return(UnionSetHighCachedValue(key,tmpBuf,len,0));
}

int UnionCheckExternalSystemDef(char *sysID,char *appID,char *cliIPAddr,char *clientIPAddr)
{
	int			ret;
	int			len;
	int			enabled = 0;
	int			isCheckIPAddrOfPackage = 0;
	char			sql[512+1];
	char			tmpBuf[128+1];
	char			ipAddrList[1024+1];
	char			appIDList[1024+1];
	char			sysName[128+1];
	char			*precStr = NULL;
	
	if ((precStr = UnionFindTableValue("externalSystem", sysID)) == NULL)
	{
		gunionTableDataFlag[0] = '0';
		// ��ȡ�ⲿϵͳ��
		len = sprintf(sql,"select sysName,ipAddrList,appIDList,enabled from externalSystem where sysID = '%s'",sysID);
		sql[len] = 0;
		
		if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
		{
			UnionUserErrLog("in UnionCheckExternalSystemDef:: UnionSelectRealDBRecord[%s]! ret = [%d]\n",sql,ret);
			return(errCodeEssc_AppNotDefined);
		}
		if (ret == 0)
		{
			UnionUserErrLog("in UnionCheckExternalSystemDef:: sysID[%s] not found! sql = [%s]\n",sysID,sql);
			return(errCodeEssc_AppNotDefined);
		}
	
		UnionLocateXMLPackage("detail", 1);
		
		if ((ret = UnionReadXMLPackageValue("sysName",sysName,sizeof(sysName))) < 0)
		{
			UnionUserErrLog("in UnionCheckExternalSystemDef:: UnionReadXMLPackageValue[sysName]! ret = [%d]\n",ret);
			return(errCodeEssc_AppNotDefined);
		}
		sysName[ret] = 0;
		
		if ((ret = UnionReadXMLPackageValue("enabled",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionCheckExternalSystemDef:: UnionReadXMLPackageValue[enabled]! ret = [%d]\n",ret);
			return(errCodeEssc_AppNotDefined);
		}
		tmpBuf[ret] = 0;
		enabled = atoi(tmpBuf);		
		if ((ret = UnionReadXMLPackageValue("ipAddrList",ipAddrList,sizeof(ipAddrList))) < 0)
		{
			UnionUserErrLog("in UnionCheckExternalSystemDef:: UnionReadXMLPackageValue[ipAddrList]! ret = [%d]\n",ret);
			return(errCodeEssc_AppNotDefined);
		}
		ipAddrList[ret] = 0;
		if ((ret = UnionReadXMLPackageValue("appIDList",appIDList,sizeof(appIDList))) < 0)
		{
			UnionUserErrLog("in UnionCheckExternalSystemDef:: UnionReadXMLPackageValue[appIDList]! ret = [%d]\n",ret);
			return(errCodeEssc_AppNotDefined);
		}
		appIDList[ret] = 0;
	}
	else
	{
		gunionTableDataFlag[0] = '1';
		if (strlen(precStr) == 0)
		{
			UnionUserErrLog("in UnionCheckExternalSystemDef:: sysID[%s] not found!\n",sysID);
			return(errCodeEssc_AppNotDefined);
		}
		if ((ret = UnionReadRecFldFromRecStr(precStr,strlen(precStr),"sysName",sysName,sizeof(sysName))) < 0)
		{
			UnionUserErrLog("in UnionCheckExternalSystemDef:: UnionReadRecFldFromRecStr[sysName][%s]!\n",precStr);
			return(ret);
		}
		sysName[ret] = 0;
		if ((ret = UnionReadRecFldFromRecStr(precStr,strlen(precStr),"enabled",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionCheckExternalSystemDef:: UnionReadRecFldFromRecStr[enabled][%s]!\n",precStr);
			return(ret);
		}
		tmpBuf[ret] = 0;
		enabled = atoi(tmpBuf);
		if ((ret = UnionReadRecFldFromRecStr(precStr,strlen(precStr),"ipAddrList",ipAddrList,sizeof(ipAddrList))) < 0)
		{
			UnionUserErrLog("in UnionCheckExternalSystemDef:: UnionReadRecFldFromRecStr[ipAddrList][%s]!\n",precStr);
			return(ret);
		}
		ipAddrList[ret] = 0;
		if ((ret = UnionReadRecFldFromRecStr(precStr,strlen(precStr),"appIDList",appIDList,sizeof(appIDList))) < 0)
		{
			UnionUserErrLog("in UnionCheckExternalSystemDef:: UnionReadRecFldFromRecStr[appIDList][%s]!\n",precStr);
			return(ret);
		}
		appIDList[ret] = 0;			
	}
	// ����ⲿϵͳ�Ƿ񼤻�
	if (enabled != 1)
	{
		UnionUserErrLog("in UnionCheckExternalSystemDef:: �ⲿϵͳ[%s:%s]δ����!\n",sysID,sysName);
		UnionSetResponseRemarkForError("�ⲿϵͳ[%s:%s]δ����",sysID,sysName);
		return(errCodeEssc_AppNotDefined);
	}
	// ���IP��ַ
	if (strlen(ipAddrList) > 0)
	{
		isCheckIPAddrOfPackage = UnionReadIntTypeRECVar("isCheckIPAddrOfPackage");
		if (isCheckIPAddrOfPackage <= 0)
		{
			if ((ret = UnionIsFldStrInUnionFldListStr(ipAddrList,strlen(ipAddrList),',',cliIPAddr)) <= 0)
			{
				UnionUserErrLog("in UnionCheckExternalSystemDef:: �ⲿϵͳ[%s:%s],cliIPAddr[%s]����ipAddrList[%s]��!\n",sysID,sysName,cliIPAddr,ipAddrList);
				UnionSetResponseRemarkForError("IP��ַ[%s]�����ⲿϵͳ[%s]��Ȩ�����",cliIPAddr,sysID);
				return(errCodeInvalidIPAddr);
			}
		}
		else if (isCheckIPAddrOfPackage == 1)
		{
			if ((ret = UnionIsFldStrInUnionFldListStr(ipAddrList,strlen(ipAddrList),',',clientIPAddr)) <= 0)
			{
				UnionUserErrLog("in UnionCheckExternalSystemDef:: �ⲿϵͳ[%s:%s],clientIPAddr[%s]����ipAddrList[%s]��!\n",sysID,sysName,clientIPAddr,ipAddrList);
				UnionSetResponseRemarkForError("IP��ַ[%s]�����ⲿϵͳ[%s]��Ȩ�����",clientIPAddr,sysID);
				return(errCodeInvalidIPAddr);
			}
		}
	}
	// ���Ӧ�ñ��
	if ((ret = UnionIsFldStrInUnionFldListStr(appIDList,strlen(appIDList),',',appID)) <= 0)
	{
		UnionUserErrLog("in UnionCheckExternalSystemDef:: �ⲿϵͳ[%s:%s],appID[%s]����appIDList[%s]��!\n",sysID,sysName,appID,appIDList);
		UnionSetResponseRemarkForError("Ӧ�ñ��[%s]�����ⲿϵͳ[%s]��Ȩ�����",appID,sysID);
		return(errCodeEssc_AppNotDefined);
	}
	return(1);
}

int UnionCheckAppDef(char *appID,char *serviceCode,int *timeout)
{
	int			ret;
	int			len;
	int			enabled = 0;;
	char			sql[512+1];
	char			tmpBuf[128+1];
	char			appName[128+1];
	char			serviceCodeList[1024+1];
	char			*precStr = NULL;
	
	if ((precStr = UnionFindTableValue("app", appID)) == NULL)
	{
		gunionTableDataFlag[1] = '0';
		// ��ȡӦ�ñ�
		len = sprintf(sql,"select appName,serviceCodeList,timeout,enabled from app where appID = '%s'",appID);
		sql[len] = 0;
		if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
		{
			UnionUserErrLog("in UnionCheckAppDef:: UnionSelectRealDBRecord[%s]! ret = [%d]\n",sql,ret);
			return(errCodeEssc_AppNotDefined);
		}
		if (ret == 0)
		{
			UnionUserErrLog("in UnionCheckAppDef:: appID[%s] not found! sql = [%s]\n",appID,sql);
			return(errCodeEssc_AppNotDefined);
		}
		UnionLocateXMLPackage("detail", 1);

		// ��ȡӦ������
		if ((ret = UnionReadXMLPackageValue("appName",appName,sizeof(appName))) < 0)
		{
			UnionUserErrLog("in UnionCheckAppDef:: UnionReadXMLPackageValue[appName]! ret = [%d]\n",ret);
			return(errCodeEssc_AppNotDefined);
		}
		appName[ret] = 0;
		
		if ((ret = UnionReadXMLPackageValue("enabled",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionCheckAppDef:: UnionReadXMLPackageValue[enabled]! ret = [%d]\n",ret);
			return(errCodeEssc_AppNotDefined);
		}
		tmpBuf[ret] = 0;
		enabled = atoi(tmpBuf);
		if ((ret = UnionReadXMLPackageValue("serviceCodeList",serviceCodeList,sizeof(serviceCodeList))) < 0)
		{
			UnionUserErrLog("in UnionCheckAppDef:: UnionReadXMLPackageValue[serviceCodeList]! ret = [%d]\n",ret);
			return(errCodeEssc_AppNotDefined);
		}
		serviceCodeList[ret] = 0;
		// ��ȡ��ʱʱ��
		if ((ret = UnionReadXMLPackageValue("timeout",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionCheckAppDef:: UnionReadXMLPackageValue[timeout]! ret = [%d]\n",ret);
			return(errCodeEssc_AppNotDefined);
		}
		tmpBuf[ret] = 0;
		*timeout = atoi(tmpBuf);
	}
	else
	{
		gunionTableDataFlag[1] = '1';

		if (strlen(precStr) == 0)
		{
			UnionUserErrLog("in UnionCheckExternalSystemDef:: appID[%s] not found!\n",appID);
			return(errCodeEssc_AppNotDefined);
		}
		if ((ret = UnionReadRecFldFromRecStr(precStr,strlen(precStr),"appName",appName,sizeof(appName))) < 0)
		{
			UnionUserErrLog("in UnionCheckAppDef:: UnionReadRecFldFromRecStr[appName][%s]!\n",precStr);
			return(ret);
		}
		appName[ret] = 0;
		if ((ret = UnionReadRecFldFromRecStr(precStr,strlen(precStr),"enabled",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionCheckAppDef:: UnionReadRecFldFromRecStr[enabled][%s]!\n",precStr);
			return(ret);
		}
		tmpBuf[ret] = 0;
		enabled = atoi(tmpBuf);
		if ((ret = UnionReadRecFldFromRecStr(precStr,strlen(precStr),"serviceCodeList",serviceCodeList,sizeof(serviceCodeList))) < 0)
		{
			UnionUserErrLog("in UnionCheckAppDef:: UnionReadRecFldFromRecStr[serviceCodeList][%s]!\n",precStr);
			return(ret);
		}
		serviceCodeList[ret] = 0;
		if ((ret = UnionReadRecFldFromRecStr(precStr,strlen(precStr),"timeout",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionCheckAppDef:: UnionReadRecFldFromRecStr[timeout][%s]!\n",precStr);
			return(ret);
		}
		tmpBuf[ret] = 0;
		*timeout = atoi(tmpBuf);		
	}

	// ���Ӧ���Ƿ񼤻�
	if (enabled != 1)
	{
		
		UnionUserErrLog("in UnionCheckAppDef:: Ӧ��[%s:%s]δ����!\n",appID,appName);
		UnionSetResponseRemarkForError("Ӧ��[%s:%s]δ����",appID,appName);
		return(errCodeEssc_AppNotDefined);
	}
	// �������б�
	if ((ret = UnionIsFldStrInUnionFldListStr(serviceCodeList,strlen(serviceCodeList),',',serviceCode)) <= 0)
	{
		UnionUserErrLog("in UnionCheckAppDef:: Ӧ��[%s:%s] ������[%s]���ڿ��ŷ����б�[%s]��!\n",appID,appName,serviceCode,serviceCodeList);
		UnionSetResponseRemarkForError("������[%s]����Ӧ��[%s]����Ȩ�����",serviceCode,appID);
		return(errCodeInvalidService);
	}
	return(1);
}

int UnionCheckServiceCodeAndServiceTypeDef(char *serviceCode,TUnionModuleID *mdlID)
{
	int			ret;
	int			len;
	int			typeEnabled = 0;
	int			codeEnabled = 0;
	char			sql[512+1];
	char			tmpBuf[128+1];
	char			serviceType[128+1];
	char			*precStr = NULL;
	
	if ((precStr = UnionFindTableValue("serviceCode", serviceCode)) == NULL)
	{
		gunionTableDataFlag[2] = '0';
		// ��ȡ��������
		len = sprintf(sql,"select serviceCode.serviceType,serviceCode.codeEnabled, serviceType.mdlID, serviceType.typeEnabled from serviceCode,serviceType where serviceCode.serviceCode = '%s' and serviceCode.serviceType = serviceType.serviceType",serviceCode);
		sql[len] = 0;
		if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
		{
			UnionUserErrLog("in UnionCheckServiceCodeAndServiceTypeDef:: UnionSelectRealDBRecord[%s]! ret = [%d]\n",sql,ret);
			return(errCodeInvalidService);
		}
		if (ret == 0)
		{
			UnionUserErrLog("in UnionCheckServiceCodeAndServiceTypeDef:: serviceCode[%s] not found! sql = [%s]\n",serviceCode,sql);
			UnionSetResponseRemarkForError("������[%s]δ����",serviceCode);
			return(errCodeInvalidService);
		}
	
		UnionLocateXMLPackage("detail", 1);
		
		if ((ret = UnionReadXMLPackageValue("serviceType",serviceType,sizeof(serviceType))) < 0)
		{
			UnionUserErrLog("in UnionCheckServiceCodeAndServiceTypeDef:: UnionReadXMLPackageValue[serviceType]! ret = [%d]\n",ret);
			return(errCodeInvalidService);
		}
		serviceType[ret] = 0;
		if ((ret = UnionReadXMLPackageValue("typeEnabled",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionCheckServiceCodeAndServiceTypeDef:: UnionReadXMLPackageValue[typeEnabled]! ret = [%d]\n",ret);
			return(errCodeInvalidService);
		}
		tmpBuf[ret] = 0;		
		typeEnabled = atoi(tmpBuf);	
				
		if ((ret = UnionReadXMLPackageValue("codeEnabled",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionCheckServiceCodeAndServiceTypeDef:: UnionReadXMLPackageValue[codeEnabled]! ret = [%d]\n",ret);
			return(errCodeInvalidService);
		}
		tmpBuf[ret] = 0;
		
		codeEnabled = atoi(tmpBuf);
		
		if ((ret = UnionReadXMLPackageValue("mdlID",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionCheckServiceCodeAndServiceTypeDef:: UnionReadXMLPackageValue[mdlID]! ret = [%d]\n",ret);
			return(ret);
		}
		tmpBuf[ret] = 0;
		*mdlID = atol(tmpBuf);
	}
	else
	{
		gunionTableDataFlag[2] = '1';
		if (strlen(precStr) == 0)
		{
			UnionUserErrLog("in UnionCheckServiceCodeAndServiceTypeDef:: serviceCode[%s] δ����\n",serviceCode);
			UnionSetResponseRemark("������[%s]δ����",serviceCode);
			return(errCodeInvalidService);
		}
		if ((ret = UnionReadRecFldFromRecStr(precStr,strlen(precStr),"codeEnabled",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionCheckServiceCodeAndServiceTypeDef:: UnionReadRecFldFromRecStr[codeEnabled][%s]!\n",precStr);
			return(ret);
		}
		tmpBuf[ret] = 0;
		codeEnabled = atoi(tmpBuf);
		if ((ret = UnionReadRecFldFromRecStr(precStr,strlen(precStr),"serviceType",serviceType,sizeof(serviceType))) < 0)
		{
			UnionUserErrLog("in UnionCheckServiceCodeAndServiceTypeDef:: UnionReadRecFldFromRecStr[serviceType][%s]!\n",precStr);
			return(ret);
		}
		serviceType[ret] = 0;

		// ���ҷ������ͱ�
		precStr = UnionFindTableValue("serviceType", serviceType);
		if ((precStr == NULL) || strlen(precStr) == 0)
		{
			UnionUserErrLog("in UnionCheckServiceCodeAndServiceTypeDef:: serviceType[%s]δ����\n",serviceType);
			UnionSetResponseRemark("��������[%s]δ����",serviceType);
			return(errCodeInvalidService);
		}
		if ((ret = UnionReadRecFldFromRecStr(precStr,strlen(precStr),"typeEnabled",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionCheckServiceCodeAndServiceTypeDef:: UnionReadRecFldFromRecStr[typeEnabled][%s]!\n",precStr);
			return(ret);
		}
		tmpBuf[ret] = 0;
		typeEnabled = atoi(tmpBuf);
		if ((ret = UnionReadRecFldFromRecStr(precStr,strlen(precStr),"mdlID",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionCheckServiceCodeAndServiceTypeDef:: UnionReadRecFldFromRecStr[mdlID][%s]!\n",precStr);
			return(ret);
		}
		tmpBuf[ret] = 0;
		*mdlID = atoi(tmpBuf);
	}
	
	// �������͵ķ����Ƿ�����
	if (typeEnabled != 1)
	{
		UnionUserErrLog("in UnionCheckServiceCodeAndServiceTypeDef:: serviceType[%s]δ����\n",serviceType);
		UnionSetResponseRemarkForError("��������[%s]δ����",serviceType);
		return(errCodeInvalidService);
	}
	// ���˷����Ƿ�����
	if (codeEnabled != 1)
	{
		UnionUserErrLog("in UnionCheckServiceCodeAndServiceTypeDef:: serviceCode[%s] δ����\n",serviceCode);
		UnionSetResponseRemarkForError("������[%s]δ����",serviceCode);
		return(errCodeInvalidService);
	}

	if (*mdlID < 0)
		return(*mdlID);
	
	if (*mdlID < conMDLTypeOffsetOfTcpAndTask)
		*mdlID += conMDLTypeOffsetOfTcpAndTask;
		
	return(1);
}
// ���ݷ�������ȡ�������
TUnionModuleID UnionGetMDLIDByServiceCode(char *cliIPAddr,int *timeout,int *isUI)
{
	int	ret;
	int	len;
	char	sql[2048+1];
	char	serviceCode[10+1];
	char	tmpBuf[128+1];
	char	sysID[16+1];
	char	appID[16+1];
	// char	sysList[1024+1];
	char	clientIPAddr[32+1];
	char	key[256+1];
	char	*ptr;
	TUnionModuleID	mdlID = -1;
	
	// ��ȡsysID
	if ((ret = UnionReadRequestXMLPackageValue("head/sysID",sysID,sizeof(sysID))) < 0)
	{
		UnionUserErrLog("in UnionGetMDLIDByServiceCode:: UnionReadRequestXMLPackageValue[head/sysID]! ret = [%d]\n",ret);
		return(ret);
	}
	sysID[ret] = 0;

	// ��ȡ�������
	if ((ret = UnionReadRequestXMLPackageValue("head/serviceCode",serviceCode,sizeof(serviceCode))) < 0)
	{
		UnionUserErrLog("in UnionGetMDLIDByServiceCode:: UnionReadRequestXMLPackageValue! ret = [%d]\n",ret);
		return(ret);
	}
	serviceCode[ret] = 0;
	
	// ��ȡ�ͻ���IP��ַ
	if ((ret = UnionReadRequestXMLPackageValue("head/clientIPAddr",clientIPAddr,sizeof(clientIPAddr))) < 0)
	{
		UnionUserErrLog("in UnionGetMDLIDByServiceCode:: UnionReadRequestXMLPackageValue! ret = [%d]\n",ret);
		return(ret);
	}
	clientIPAddr[ret] = 0;

	// ���泬ʱ
	if (UnionIsUITrans(sysID))
	{
		*isUI = 1;
		if (strcmp(sysID,"CONFUI") == 0)
			len = sprintf(tmpBuf,"timeoutOf%s","CUI");
		else
			len = sprintf(tmpBuf,"timeoutOf%s",sysID);

		tmpBuf[len] = 0;
		if ((*timeout = UnionReadIntTypeRECVar(tmpBuf)) <= 0)
			*timeout = 10;
	}
	// APP��ʱ
	else
	{
		*isUI = 0;
		// ��ȡappID
		if ((ret = UnionReadRequestXMLPackageValue("head/appID",appID,sizeof(appID))) < 0)
		{
			UnionUserErrLog("in UnionGetMDLIDByServiceCode:: UnionReadRequestXMLPackageValue[head/appID]! ret = [%d]\n",ret);
			return(errCodeEssc_AppNotDefined);
		}
		appID[ret] = 0;

		if (strcasecmp(UnionGetIDOfCustomization(),"aliyun") == 0)	
		{
			if ((ptr = strchr(sysID,':')) == NULL)
			{
				UnionUserErrLog("in UnionCheckUnitID:: sysID[%s] error\n",sysID);
				UnionSetResponseRemark("����ͷ����,ϵͳID[%s]��ʽ����",sysID);
				return(errCodeParameter);
			}
			ptr += 1;
			// ��ȡ��֯��
			snprintf(sql,sizeof(sql),"select organizationName from organization where organizationID = '%s'",ptr);
			if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
			{
				UnionUserErrLog("in UnionGetMDLIDByServiceCode:: UnionSelectRealDBRecord[%s]! ret = [%d]\n",sql,ret);
				return(ret);
			}
			if (ret == 0)
			{
				UnionUserErrLog("in UnionGetMDLIDByServiceCode:: unitID[%s] not found! sql = [%s]\n",ptr,sql);
				UnionSetResponseRemarkForError("����ͷ����,��λID[%s]������",ptr);
				return(errCodeParameter);
			}
		}
		
		if ((mdlID = UnionReadMDLIDFromHighCached(sysID,appID,serviceCode,cliIPAddr,key)) > 0)
		{
			//UnionLog("in UnionGetMDLIDByServiceCode:: mdlID[%ld]\n",mdlID);
			return(mdlID);
		}
		
		// ����ⲿϵͳ
		if ((ret = UnionCheckExternalSystemDef(sysID,appID,cliIPAddr,clientIPAddr)) < 0)
		{
			UnionUserErrLog("in UnionGetMDLIDByServiceCode:: UnionCheckExternalSystemDef!\n");
			return(ret);
		}

		// ��ȡӦ�ñ�
		if ((ret = UnionCheckAppDef(appID,serviceCode,timeout)) < 0)
		{
			UnionUserErrLog("in UnionGetMDLIDByServiceCode:: UnionCheckAppDef!\n");
			return(ret);
		}
	}

	// �������붨��
	if ((ret = UnionCheckServiceCodeAndServiceTypeDef(serviceCode,&mdlID)) < 0)
	{
		UnionUserErrLog("in UnionGetMDLIDByServiceCode:: UnionCheckServiceCodeAndServiceTypeDef!\n");
		return(ret);
	}

	if (!*isUI)
		UnionSetMDLIDToHighCached(key,mdlID);
	
	return(mdlID);
}

int UnionSynchTCPIPTaskServer(int handle,struct sockaddr_in *cli_addr,int port,int (*UnionTaskActionBeforeExit)())
{
	unsigned char	reqStr[conMaxSizeOfClientMsg*10+1];
	unsigned char	resStr[conMaxSizeOfClientMsg*10+1];
	unsigned char	tmpBuf[conMaxSizeOfClientMsg*10+1];
	unsigned char	transMonBuf[1024+1];
	unsigned int	lenOfReq = 0;
	unsigned int	lenOfRes = 0;
	int		ret;
	unsigned long	len = 0;
	unsigned long	useTime = 0;
	int		timeoutOfClient = 0;
	int		maxIdleTimeOfSckConn = 0;
	char		cliName[100];
	char		taskName[100];
	char		cliIPAddr[40+1];
	int		finishThisTask = 0;
	int		recvFromClientOK = 0;
	char		strUseTime[128+1];
	int		timeout = 0;
	int		isUI = 0;
	int		lenOfLengthField = 2;
	int		commType;
	TUnionModuleID	taskMDLID = -1;

	memset(cliIPAddr,0,sizeof(cliIPAddr));
	inet_ntop(AF_INET, (void *)&cli_addr->sin_addr, cliIPAddr, sizeof(cliIPAddr));
	len = sprintf(cliName,"%s %d",cliIPAddr,port);
	cliName[len] = 0;
	
	len = sprintf(taskName,"%s client %d",UnionGetApplicationName(),port);
	taskName[len] = 0;

#ifndef _WIN32		
	gsynchTCPIPSvrSckHDL = handle;
	// ��������ʵ��
	if (UnionExistsTaskClassOfName(taskName))
	{
		if ((ptaskInstance = UnionCreateTaskInstanceOfName(UnionTaskActionBeforeExit,cliName)) == NULL)
		{
			ret = errCodeCreateTaskInstance;
			UnionUserErrLog("in %s:: UnionCreateTaskInstanceOfName! ret = [%d]\n",cliName,ret);
			goto errorExit;
		}
		commType = conCommShortConn;
	}
	else
	{
		if ((ptaskInstance = UnionCreateTaskInstanceOfName(UnionTaskActionBeforeExit,"%s %d",UnionGetApplicationName(),port)) == NULL)
		{
			ret = errCodeCreateTaskInstance;
			UnionUserErrLog("in %s:: UnionCreateTaskInstanceOfName! ret = [%d]\n",cliName,ret);
			goto errorExit;
		}
		commType = conCommShortConn;
	}
#ifndef _noRegisterClient_
	if ((pgunionCommConf = UnionAddServerCommConf(cliIPAddr,port,commType,"ͨѶ����")) == NULL)
	{
#ifdef _limitClient_
		ret = errCodeYLCommConfMDL_AddCommConf;
		UnionUserErrLog("in %s:: UnionAddServerCommConf [%s] [%d]! ret = [%d]\n",cliName,cliIPAddr,port,ret);
		goto errorExit;
#endif
	}
	UnionSetCommConfOK(pgunionCommConf);
#endif
#endif
	
loop:	// ѭ������ͻ�������
	recvFromClientOK = 0;

	// ��ȡ��ʱֵ
	if ((timeoutOfClient = UnionReadIntTypeRECVar("timeoutOfRecvClientData")) < 0)
		timeoutOfClient = 3;

	// �������ӿ���ʱ�����
	if ((maxIdleTimeOfSckConn = UnionReadIntTypeRECVar("maxIdleTimeOfSckConn")) < 0)
		maxIdleTimeOfSckConn = 1800;
#ifdef _WIN32
	// add codes here!
#else
#if ( defined __linux__ ) || ( defined __hpux )
	if (sigsetjmp(gunionTCPSvrJmpEnvForMaxIdleTime,1) != 0) // ��ʱ�˳�
#elif ( defined _AIX )
	if (setjmp(gunionTCPSvrJmpEnvForMaxIdleTime) != 0)	// ��ʱ�˳�
#endif
	{
		ret = errCodeEsscMDL_TCPIPTaskTooIdle;
		UnionUserErrLog("in %s:: this connection too idle! close it now! ret = [%d]\n",cliName,ret);
		goto errorExit;
	}
	alarm(maxIdleTimeOfSckConn);
	signal(SIGALRM,UnionDealTCPSvrJmpEnvForMaxIdleTime);
#endif
		
	// ��ʼ�ӿͻ��˽�������
	// �������ݳ���
	if ((ret = UnionReceiveFromSocketUntilLen(handle,reqStr,2)) < 0)
	{
		UnionUserErrLog("in %s:: UnionReceiveFromSocketUntilLen %d! ret = [%d]\n",cliName,2,ret);
		goto errorExit;
	}
	if (memcmp(reqStr,"00",2) == 0)		// ��������8���ֽ�
	{
		if ((ret = UnionReceiveFromSocketUntilLen(handle,reqStr,6)) < 0)
		{
			UnionUserErrLog("in %s:: UnionReceiveFromSocketUntilLen %d! ret = [%d]\n",cliName,6,ret);
			goto errorExit;
		}
		lenOfLengthField = 8;
	}
	else
		lenOfLengthField = 2;

#ifndef _WIN32
	alarm(0);
#endif
	if (ret == 0)
	{
		UnionProgramerLog("in %s:: connection closed by peer!\n",cliName);
		return(0);
	}
	if (lenOfLengthField == 2)
		lenOfReq = reqStr[0] * 256 + reqStr[1];
	else
		lenOfReq = atol((char *)reqStr);
	
	if ((lenOfReq  < 0) || (lenOfReq >= (int)sizeof(reqStr)))
	{
		ret = errCodeSocketMDL_DataLen;
		UnionUserErrLog("in %s:: lenOfReq = [%d] error! ret = [%d]\n",cliName,lenOfReq,ret);
		goto errorExit;
	}
	if (lenOfReq == 0)	// ���Ա���
	{
		UnionProgramerLog("in %s:: testing pack received!\n",cliName);
		lenOfRes = 0;
		goto sendToClient;
	}
	// ���ý��տͻ������ݵĳ�ʱ����
#ifdef _WIN32
	// addcodes here
#else
#if ( defined __linux__ ) || ( defined __hpux )
	if (sigsetjmp(gunionTCPSvrJmpEnvForEsscClient,1) != 0)	// ��ʱ�˳�
#elif ( defined _AIX )
	if (setjmp(gunionTCPSvrJmpEnvForEsscClient) != 0) // ��ʱ�˳�
	{
		ret = errCodeSocketMDL_RecvDataTimeout;
		UnionUserErrLog("in %s:: recv from client time out! ret = [%d]\n",cliName,ret);
		goto errorExit;
	}
#endif
	alarm(timeoutOfClient);
	signal(SIGALRM,UnionDealTCPSvrJmpEnvForEsscClient);
#endif
	// ���տͻ�������
	if ((ret = UnionReceiveFromSocketUntilLen(handle,reqStr,lenOfReq)) != lenOfReq)
	{
		UnionUserErrLog("in %s:: UnionReceiveFromSocketUntilLen expected [%d] real = [%d]!\n",cliName,lenOfReq,ret);
		ret = errCodeSocketMDL_DataLen;
		goto errorExit;
	}
	reqStr[ret] = 0;
	recvFromClientOK = 1;
#ifndef _WIN32
	alarm(0);
#ifndef _noRegisterClient_
	UnionIncreaseCommConfWorkingTimes(pgunionCommConf);
#endif
#endif

	// ���õ�ǰʱ��
	UnionReInitUserSetTime();
	UnionSetStageOfTransProcessing(defStageOfTransBefore);

	// ��������ת��ΪXML����
	if ((ret = UnionConvertOtherPackageToXMLPackage(reqStr,lenOfReq,cliIPAddr)) < 0)
	{
		UnionUserErrLog("in %s:: UnionConvertOtherPackageToXMLPackage! ret = [%d]\n",cliName,ret);
		goto errorExit;
	}
	else if (ret == 0)
	{
		// ��ʼ�������
		if ((ret = UnionInitRequestXMLPackage(NULL,(char *)reqStr,lenOfReq)) < 0)
		{
			UnionUserErrLog("in %s:: UnionInitRequestXMLPackage! reqStr = [%s]\n",cliName,reqStr);
			goto errorExit;
		}
	}
	
	UnionSetTimeToRequestPackage();
	UnionRegisterRunningTime(NULL);

	// ���ݷ�������ȡ�������
	if ((taskMDLID = UnionGetMDLIDByServiceCode(cliIPAddr,&timeout,&isUI)) < 0)
	{
		UnionUserErrLog("in %s:: UnionGetMDLIDByServiceCode! ret = [%d]\n",cliName,ret);
		ret = taskMDLID;
		goto errorExit;
	}
	UnionRegisterRunningTime(NULL);

	// ��Ϣ����
	// # ��0��:UIͨѶ
	// # ��1��:APPͨѶ
	// # ��2��:dbSvr
	// # ��3��:hsmSvr
	// # ��4��:�����Ϣ
	if (isUI)
		UnionSetGroupIDOfMsgBuf(defMsgBufGroupOfUI);
	else
		UnionSetGroupIDOfMsgBuf(defMsgBufGroupOfAPP);

	lenOfReq = UnionRequestXMLPackageToBuf((char *)reqStr,sizeof(reqStr));
		
	if ((lenOfRes = UnionExchangeWithSpecModuleVerifyReqAndRes(taskMDLID,reqStr,lenOfReq,
			tmpBuf,sizeof(tmpBuf)-1,timeout)) < 0)
	{
		ret = lenOfRes;
		UnionUserErrLog("in %s:: UnionExchangeWithSpecModuleVerifyReqAndRes! ret = [%d]\n",cliName,ret);
		goto errorExit;
	}
	else
	{
		ret = lenOfRes;
		finishThisTask = 0;
	}
	tmpBuf[lenOfRes] = 0;
	
	UnionSetStageOfTransProcessing(defStageOfTransAfter);
	UnionInitRegisterNodeID();
	
	// ��ȡ��ʹ��ʱ��	
	if ((ret =  UnionInitResponseXMLPackage(NULL,(char *)tmpBuf,lenOfRes)) < 0)
	{
		UnionUserErrLog("in %s:: UnionInitResponseXMLPackage! ret = [%d]\n",cliName,ret);
		goto errorExit;		
	}
	UnionRegisterRunningTime(NULL);
	useTime = UnionGetTotalRunningTimeInMacroSeconds();
	len = sprintf(strUseTime,"%ld",useTime);
	strUseTime[len] = 0;
	if ((ret = UnionSetResponseXMLPackageValue("head/useTime",strUseTime)) < 0)
	{
		UnionUserErrLog("in %s:: UnionSetResponseXMLPackageValue! ret = [%d]\n",cliName,ret);
		goto errorExit;		
	}
	
sendToClient: // ��ͻ��˷�������
	
#ifndef _WIN32
	alarm(0);
#endif

	if ((lenOfRes = UnionResponseXMLPackageToBuf((char *)tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in %s:: UnionResponseXMLPackageToBuf! ret = [%d]\n",cliName,lenOfRes);
		ret = lenOfRes;
		goto errorExit;
	}
	tmpBuf[lenOfRes] = 0;
		
	// XML����ת��Ϊ��������
	if ((ret = UnionConvertXMLPackageToOtherPackage(tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in %s:: UnionConvertXMLPackageToOtherPackage! ret = [%d]\n",cliName,ret);
		return(ret);
	}
	else if (ret > 0)
		lenOfRes = ret;

	// ���ͼ����Ϣ
	if (!isUI)
	{
		// ɾ��body�ڵ�
		UnionDeleteResponseXMLPackageNode("body",0);
		if ((ret = UnionResponseXMLPackageToBuf((char *)transMonBuf,sizeof(transMonBuf))) > 0)
		{
			transMonBuf[ret] = 0;
			
			// ������Ϣ����
			UnionSetGroupIDOfMsgBuf(defMsgBufGroupOfMon);
			// ���ͼ����Ϣ
			UnionSendMonInfoToTransMonMDL(transMonBuf,ret);
			// �ָ���Ϣ����
			UnionSetGroupIDOfMsgBuf(defMsgBufGroupOfAPP);
		}
	}

	// 2013-04-26 add
	if ((lenOfLengthField == 2) && (lenOfRes > 65535))
	{
		// ѹ��
		len = sizeof(resStr);
		if ((ret = compress(resStr, &len, tmpBuf, lenOfRes)) != 0)
		{
			UnionUserErrLog("in %s:: compress ret = [%d]!\n",cliName,ret);
			return(0 - abs(ret));
			goto errorExit;
		}
		
		// ���ѹ����־
		tmpBuf[0] = '1'; 
		tmpBuf[1] = lenOfRes / (256 * 256);
		tmpBuf[2] = (lenOfRes % (256 * 256)) / 256;
		tmpBuf[3] = (lenOfRes % (256 * 256)) % 256;
		lenOfRes = len + 4;
		if (lenOfRes > 65535)
		{
			UnionUserErrLog("in %s:: lenOfRes[%d] > 65535!\n",cliName,lenOfRes);
			ret = errCodeEsscMDL_ClientDataLenTooLong;
			goto errorExit;
		}

		memcpy(tmpBuf + 4,resStr,len);
		tmpBuf[lenOfRes] = 0;
	}

	if (lenOfLengthField == 2)
	{
		resStr[0] = lenOfRes / 256;
		resStr[1] = lenOfRes % 256;
		memcpy(resStr+2,tmpBuf,lenOfRes);
	}
	else
	{
		sprintf((char *)resStr,"%0*d",lenOfLengthField,lenOfRes);
		memcpy(resStr + lenOfLengthField,tmpBuf,lenOfRes);
	}
	resStr[lenOfRes+lenOfLengthField] = 0;

	if ((ret = UnionSendToSocket(handle,resStr,lenOfRes+lenOfLengthField)) < 0)
	{
		UnionUserErrLog("in %s:: UnionSendToSocket ret = [%d]!\n",cliName,ret);
		return(ret);
	}
	// ѭ������
	if (finishThisTask)
	{
		sleep(1);
		return(ret);
	}
	else
	{
		// 2013-4-2 ������ DEL
		//UnionCloseDatabase();
		goto loop;
	}
errorExit:
	if (ret >= 0)
		ret = errCodeSocketMDL_ErrCodeNotSet;

	if (!recvFromClientOK)
	{
		return(ret);
	}

	UnionSetXMLResponsePackageForError(ret);
	finishThisTask = 1;
	goto sendToClient;
}

#ifndef _WIN32
void UnionDealTCPSvrJmpEnvForEsscClient()
{
	UnionUserErrLog("gunionTCPSvrJmpEnvForEsscClient:: timeout!\n");
#if ( defined __linux__ ) || ( defined __hpux )
	siglongjmp(gunionTCPSvrJmpEnvForEsscClient,10);
#elif ( defined _AIX )
	longjmp(gunionTCPSvrJmpEnvForEsscClient,10);
#endif
}

void UnionDealTCPSvrJmpEnvForMaxIdleTime()
{
	UnionUserErrLog("gunionTCPSvrJmpEnvForEsscClient:: timeout!\n");
#if ( defined __linux__ ) || ( defined __hpux )
	siglongjmp(gunionTCPSvrJmpEnvForMaxIdleTime,10);
#elif ( defined _AIX )
	longjmp(gunionTCPSvrJmpEnvForMaxIdleTime,10);
#endif
}
#endif

