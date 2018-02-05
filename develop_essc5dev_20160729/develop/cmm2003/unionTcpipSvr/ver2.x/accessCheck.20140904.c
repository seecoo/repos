//	Author:		������
//	Copyright:	Union Tech. Guangzhou
//	Date:		2012-05-25

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#ifndef _UnionLogMDL_3_x_
#define _UnionLogMDL_3_x_
#endif

#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_
#endif

#include "unionVersion.h"
#include "unionMDLID.h"
#include "unionCommBetweenMDL.h"
#include "UnionLog.h"
#include "UnionStr.h"
#include "unionErrCode.h"
#include "unionREC.h"
#include "UnionSHA1.h"
#include "UnionTask.h"

#include "unionMsgBufGroup.h"
#include "unionXMLPackage.h"
#include "unionFunSvrName.h"
#include "errCodeTranslater.h"
#include "unionRealBaseDB.h"
#include "commWithHsmSvr.h"
#include "registerRunningTime.h"
#include "accessCheck.h"
#include "unionTableData.h"

char	gunionTableDataFlag[32];

int UnionSendMaxConnMonInfoToTransMonMDL(unsigned char *msg,int lenOfMsg)
{
        int     ret = 0;
        TUnionMessageHeader     reqMsgHeader;

        if (UnionExistsTaskOfName("transMonSvr") <= 0 && UnionExistsTaskOfName("resSpier -SYSMAXCONN") <= 0)
        {
                UnionLog("in UnionSendMaxConnMonInfoToTransMonMDL:: task transMonSvr or resSpierForTaskTbl not started!\n");
                return(0);
        }

	UnionSetGroupIDOfMsgBuf(defMsgBufGroupOfMon);
        memset(&reqMsgHeader,0,sizeof(reqMsgHeader));
        if ((ret = UnionSendRequestToSpecifiedModuleWithNewMsgHeaderWithTime(conMDLTypeOffsetOfMonSvr + conMDLTypeUnionMon_MaxConn,msg,lenOfMsg,&reqMsgHeader,1)) < 0)
        {
                UnionLog("in UnionSendMaxConnMonInfoToTransMonMDL:: UnionSendRequestToSpecifiedModuleWithNewMsgHeader!\n");
                return(0);
        }

	UnionDebugLog("in UnionSendRequestToSpecifiedModuleWithNewMsgHeaderWithTime:: send\n");
	
        return(ret);
}

int UnionCheckExternalSystemDef(int isHsmCmd,char *sysID,char *appID,char *cliIPAddr,int port,char *clientIPAddr)
{
	//int			i;
	int			ret;
	int			enabled = 0;
	int			maxConn = 0;
	int			isCheckIPAddrOfPackage = 0;
	char			sql[512];
	char			tmpBuf[128];
	char			ipAddrList[4096];
	char			appIDList[2048];
	char			sysName[128];
	char			*precStr = NULL;
	char			connTaskName[256];//add by lusj 20151116
	//PUnionTableDataTBL	ptableDataTBL = NULL;
	
	//if ((ptableDataTBL = UnionGetTableDataTBL("externalSystem")) == NULL)
	if ((precStr = UnionFindTableValue("externalSystem", sysID)) == NULL)
	{
		if (isHsmCmd)
			return(0);

		gunionTableDataFlag[0] = '0';
		// ��ȡ�ⲿϵͳ��
		snprintf(sql,sizeof(sql),"select * from externalSystem where sysID = '%s'",sysID);
		
		if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
		{
			UnionUserErrLog("in UnionCheckExternalSystemDef:: UnionSelectRealDBRecord[%s]! ret = [%d]\n",sql,ret);
			return(errCodeEssc_AppNotDefined);
		}
		if (ret == 0)
		{
			if (isHsmCmd)
				return(0);
			UnionUserErrLog("in UnionCheckExternalSystemDef:: sysID[%s] not found! sql = [%s]\n",sysID,sql);
			return(errCodeEssc_AppNotDefined);
		}
	
		UnionLocateXMLPackage("detail", 1);
		
		if ((ret = UnionReadXMLPackageValue("sysName",sysName,sizeof(sysName))) < 0)
		{
			UnionUserErrLog("in UnionCheckExternalSystemDef:: UnionReadXMLPackageValue[sysName]! ret = [%d]\n",ret);
			return(errCodeEssc_AppNotDefined);
		}
		
		if ((ret = UnionReadXMLPackageValue("enabled",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionCheckExternalSystemDef:: UnionReadXMLPackageValue[enabled]! ret = [%d]\n",ret);
			return(errCodeEssc_AppNotDefined);
		}
		enabled = atoi(tmpBuf);		

		if ((ret = UnionReadXMLPackageValue("ipAddrList",ipAddrList,sizeof(ipAddrList))) < 0)
		{
			UnionUserErrLog("in UnionCheckExternalSystemDef:: UnionReadXMLPackageValue[ipAddrList]! ret = [%d]\n",ret);
			return(errCodeEssc_AppNotDefined);
		}
		if ((ret = UnionReadXMLPackageValue("appIDList",appIDList,sizeof(appIDList))) < 0)
		{
			UnionUserErrLog("in UnionCheckExternalSystemDef:: UnionReadXMLPackageValue[appIDList]! ret = [%d]\n",ret);
			return(errCodeEssc_AppNotDefined);
		}
		if ((ret = UnionReadXMLPackageValue("maxConn",tmpBuf,sizeof(tmpBuf))) < 0)
			maxConn = 0;
		else
			maxConn = atoi(tmpBuf);
	}
	else
	{
		gunionTableDataFlag[0] = '1';

		if (strlen(precStr) == 0)
		{
			UnionUserErrLog("in UnionCheckExternalSystemDef:: sysID[%s] not found!\n",sysID);
			return(errCodeEssc_AppNotDefined);
		}
		/*
		if ((ret = UnionReadRecFldFromRecStr(precStr,strlen(precStr),"sysID",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionCheckExternalSystemDef:: UnionReadRecFldFromRecStr[sysID][%s]!\n",precStr);
			return(ret);
		}
		tmpBuf[ret] = 0;
		if (strcmp(sysID,tmpBuf) != 0)
		{
			UnionUserErrLog("in UnionCheckExternalSystemDef:: sysID[%s] not found!\n",sysID);
			return(errCodeEssc_AppNotDefined);
		}
		*/
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
		if ((ret = UnionReadRecFldFromRecStr(precStr,strlen(precStr),"maxConn",tmpBuf,sizeof(tmpBuf))) < 0)
			maxConn = 0;
		else
			maxConn = atoi(tmpBuf);
	}
	
	// ����ⲿϵͳ�Ƿ񼤻�
	if (enabled != 1)
	{
		UnionUserErrLog("in UnionCheckExternalSystemDef:: �ⲿϵͳ[%s:%s]δ����!\n",sysID,sysName);
		UnionSetResponseRemark("�ⲿϵͳ[%s:%s]δ����",sysID,sysName);
		return(errCodeEssc_AppNotDefined);
	}
	// ���IP��ַ
	if (strlen(ipAddrList) > 0)
	{
		isCheckIPAddrOfPackage = UnionReadIntTypeRECVar("isCheckIPAddrOfPackage");
		if (isHsmCmd || (isCheckIPAddrOfPackage <= 0))
		{
			if ((ret = UnionIsFldStrInUnionFldListStr(ipAddrList,strlen(ipAddrList),',',cliIPAddr)) <= 0)
			{
				UnionUserErrLog("in UnionCheckExternalSystemDef:: �ⲿϵͳ[%s:%s],cliIPAddr[%s]����ipAddrList[%s]��!\n",sysID,sysName,cliIPAddr,ipAddrList);
				UnionSetResponseRemark("IP��ַ[%s]�����ⲿϵͳ[%s]��Ȩ�����",cliIPAddr,sysID);
				return(errCodeInvalidIPAddr);
			}
		}
		else if (isCheckIPAddrOfPackage == 1)
		{
			if ((ret = UnionIsFldStrInUnionFldListStr(ipAddrList,strlen(ipAddrList),',',clientIPAddr)) <= 0)
			{
				UnionUserErrLog("in UnionCheckExternalSystemDef:: �ⲿϵͳ[%s:%s],clientIPAddr[%s]����ipAddrList[%s]��!\n",sysID,sysName,clientIPAddr,ipAddrList);
				UnionSetResponseRemark("IP��ַ[%s]�����ⲿϵͳ[%s]��Ȩ�����",clientIPAddr,sysID);
				return(errCodeInvalidIPAddr);
			}
		}
	}

	// ������������
	if (maxConn > 0)
	{
		//add begin by lusj 20151116
		snprintf(connTaskName,sizeof(connTaskName),"%s busy %d %s",UnionGetApplicationName(),port,cliIPAddr);
		if (maxConn < UnionExistsTaskOfAlikeName(connTaskName))
		{
			UnionUserErrLog("in UnionCheckExternalSystemDef::maxConn[%d] < ExistsTaskOfAlikeName[%d]!\n",maxConn,UnionExistsTaskOfAlikeName(connTaskName));
			// ���ͼ����Ϣ
			ret = snprintf(tmpBuf,sizeof(tmpBuf),"sysID=%s|currConnNum=%d|maxConnNum=%d|cliIPAddr=%s",sysID,UnionExistsTaskOfAlikeName(connTaskName),maxConn,cliIPAddr);
			UnionSendMaxConnMonInfoToTransMonMDL((unsigned char *)tmpBuf,ret);
			return(errCodeEsscMDL_maxConnIsFull);
		}

		//add end by lusj 20151116
	}
	// ���Ӧ�ñ��
	if (!isHsmCmd)
	{
		if ((ret = UnionIsFldStrInUnionFldListStr(appIDList,strlen(appIDList),',',appID)) <= 0)
		{
			UnionUserErrLog("in UnionCheckExternalSystemDef:: �ⲿϵͳ[%s:%s],appID[%s]����appIDList[%s]��!\n",sysID,sysName,appID,appIDList);
			UnionSetResponseRemark("Ӧ�ñ��[%s]�����ⲿϵͳ[%s]��Ȩ�����",appID,sysID);
			return(errCodeEssc_AppNotDefined);
		}
	}
	return(1);
}

int UnionCheckAppDef(char *appID,char *serviceCode,int *timeout)
{
	//int			i;
	int			ret;
	int			enabled = 0;
	char			sql[512];
	char			tmpBuf[128];
	char			appName[128];
	char			serviceCodeList[1024];
	char			*precStr = NULL;
	//PUnionTableDataTBL	ptableDataTBL = NULL;
	
	//if ((ptableDataTBL = UnionGetTableDataTBL("app")) == NULL)
	if ((precStr = UnionFindTableValue("app", appID)) == NULL)
	{
		gunionTableDataFlag[1] = '0';
		// ��ȡӦ�ñ�
		snprintf(sql,sizeof(sql),"select appName,serviceCodeList,timeout,enabled from app where appID = '%s'",appID);

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
			UnionUserErrLog("in UnionCheckAppDef:: appID[%s] not found!\n",appID);
			UnionSetResponseRemark("Ӧ��[%s:%s]δ����",appID,appName);
			return(errCodeEssc_AppNotDefined);
		}
		/*
		if ((ret = UnionReadRecFldFromRecStr(precStr,strlen(precStr),"appID",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionCheckAppDef:: UnionReadRecFldFromRecStr[appID][%s]!\n",precStr);
			return(ret);
		}
		tmpBuf[ret] = 0;
		if (strcmp(appID,tmpBuf) != 0)
		{
			UnionUserErrLog("in UnionCheckAppDef:: appID[%s] not found!\n",appID);
			UnionSetResponseRemark("Ӧ��[%s:%s]δ����",appID,appName);
			return(errCodeEssc_AppNotDefined);
		}
		*/
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
		UnionSetResponseRemark("Ӧ��[%s:%s]δ����",appID,appName);
		return(errCodeEssc_AppNotDefined);
	}
	// �������б�
	if ((ret = UnionIsFldStrInUnionFldListStr(serviceCodeList,strlen(serviceCodeList),',',serviceCode)) <= 0)
	{
		UnionUserErrLog("in UnionCheckAppDef:: Ӧ��[%s:%s] ������[%s]���ڿ��ŷ����б�[%s]��!\n",appID,appName,serviceCode,serviceCodeList);
		UnionSetResponseRemark("������[%s]����Ӧ��[%s]����Ȩ�����",serviceCode,appID);
		return(errCodeInvalidService);
	}
	return(1);
}

int UnionCheckServiceCodeAndServiceTypeDef(char *serviceCode)
{
	//int			i;
	int			ret;
	int			typeEnabled = 0;
	int			codeEnabled = 0;
	char			sql[512];
	char			tmpBuf[128];
	char			serviceType[128];
	//char			*serviceCodeStr = NULL;
	//char			*serviceTypeStr = NULL;
	char			*precStr = NULL;
	//PUnionTableDataTBL	pserviceCodeTBL = NULL;
	//PUnionTableDataTBL	pserviceTypeTBL = NULL;
	
	//if (((pserviceCodeTBL = UnionGetTableDataTBL("serviceCode")) == NULL) ||
		//((pserviceTypeTBL = UnionGetTableDataTBL("serviceType")) == NULL))
	if ((precStr = UnionFindTableValue("serviceCode", serviceCode)) == NULL)
	{
		gunionTableDataFlag[2] = '0';
		// ��ȡ��������
		snprintf(sql,sizeof(sql),"select serviceCode.serviceType,serviceCode.codeEnabled, serviceType.typeEnabled from serviceCode,serviceType where serviceCode.serviceCode = '%s' and serviceCode.serviceType = serviceType.serviceType",serviceCode);

		if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
		{
			UnionUserErrLog("in UnionCheckServiceCodeAndServiceTypeDef:: UnionSelectRealDBRecord[%s]! ret = [%d]\n",sql,ret);
			return(errCodeInvalidService);
		}
		if (ret == 0)
		{
			UnionUserErrLog("in UnionCheckServiceCodeAndServiceTypeDef:: serviceCode[%s] not found! sql = [%s]\n",serviceCode,sql);
			UnionSetResponseRemark("������[%s]δ����",serviceCode);
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
		/*
		if ((ret = UnionReadRecFldFromRecStr(precStr,strlen(precStr),"serviceCode",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionCheckServiceCodeAndServiceTypeDef:: UnionReadRecFldFromRecStr[serviceCode][%s]!\n",precStr);
			return(ret);
		}
		tmpBuf[ret] = 0;
		if (strcmp(serviceCode,tmpBuf) != 0)
		{
			UnionUserErrLog("in UnionCheckServiceCodeAndServiceTypeDef:: serviceCode[%s] δ����\n",serviceCode);
			UnionSetResponseRemark("������[%s]δ����",serviceCode);
			return(errCodeInvalidService);
		}
		*/
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

		precStr = UnionFindTableValue("serviceType", serviceType);	
		if ((precStr == NULL) || strlen(precStr) == 0)
		{
			UnionUserErrLog("in UnionCheckServiceCodeAndServiceTypeDef:: serviceType[%s]δ����\n",serviceType);
			UnionSetResponseRemark("��������[%s]δ����",serviceType);
			return(errCodeInvalidService);
		}

		// ���ҷ������ͱ�
		/*
		if ((ret = UnionReadRecFldFromRecStr(precStr,strlen(precStr),"serviceType",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionCheckServiceCodeAndServiceTypeDef:: UnionReadRecFldFromRecStr[serviceType][%s]!\n",precStr);
			return(ret);
		}
		tmpBuf[ret] = 0;
		if (strcmp(serviceType,tmpBuf) != 0)
		{
			UnionUserErrLog("in UnionCheckServiceCodeAndServiceTypeDef:: serviceType[%s]δ����\n",serviceType);
			UnionSetResponseRemark("��������[%s]δ����",serviceType);
			return(errCodeInvalidService);
		}
		*/

		if ((ret = UnionReadRecFldFromRecStr(precStr,strlen(precStr),"typeEnabled",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionCheckServiceCodeAndServiceTypeDef:: UnionReadRecFldFromRecStr[typeEnabled][%s]!\n",precStr);
			return(ret);
		}
		tmpBuf[ret] = 0;
		typeEnabled = atoi(tmpBuf);
	}
	
	// �������͵ķ����Ƿ�����
	if (!typeEnabled)
	{
		UnionUserErrLog("in UnionCheckServiceCodeAndServiceTypeDef:: serviceType[%s]δ����\n",serviceType);
		UnionSetResponseRemark("��������[%s]δ����",serviceType);
		return(errCodeInvalidService);
	}
	// ���˷����Ƿ�����
	if (!codeEnabled)
	{
		UnionUserErrLog("in UnionCheckServiceCodeAndServiceTypeDef:: serviceCode[%s] δ����\n",serviceCode);
		UnionSetResponseRemark("������[%s]δ����",serviceCode);
		return(errCodeInvalidService);
	}

	return(1);
}

int UnionCheckUnitID(char *sysID)
{
	int	ret;
	char	sql[256];
	char	*ptr;

	if (strcasecmp(UnionGetIDOfCustomization(),"aliyun") != 0)			// ������
		return(0);
		
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
		UnionUserErrLog("in UnionCheckUnitID:: UnionSelectRealDBRecord[%s]! ret = [%d]\n",sql,ret);
		return(ret);
	}
	if (ret == 0)
	{
		UnionUserErrLog("in UnionCheckUnitID:: unitID[%s] not found! sql = [%s]\n",ptr,sql);
		UnionSetResponseRemark("����ͷ����,��λID[%s]������",ptr);
		return(errCodeParameter);
	}

	return(0);
}

// ������
int UnionAccessCheck(char *cliIPAddr,int port,int *timeout,int *isUI)
{
	int	ret;
	char	tmpBuf[128];
	PUnionXMLPackageHead	pxmlPackageHead;

	pxmlPackageHead = UnionGetXMLPackageHead();

	*isUI = pxmlPackageHead->isUI;
	
	// ���泬ʱ
	if (pxmlPackageHead->isUI)
	{
		if (strcmp(pxmlPackageHead->sysID,"CONFUI") == 0)
			snprintf(tmpBuf,sizeof(tmpBuf),"timeoutOf%s","CUI");
		else
			snprintf(tmpBuf,sizeof(tmpBuf),"timeoutOf%s",pxmlPackageHead->sysID);

		if ((*timeout = UnionReadIntTypeRECVar(tmpBuf)) <= 0)
			*timeout = 10;
	}
	// APP��ʱ
	else
	{
		if ((ret = UnionCheckUnitID(pxmlPackageHead->sysID)) < 0)
		{
			UnionUserErrLog("in UnionAccessCheck:: UnionCheckUnitID!\n");
			return(ret);
		}

		// ����ⲿϵͳ
		if ((ret = UnionCheckExternalSystemDef(0,pxmlPackageHead->sysID,pxmlPackageHead->appID,cliIPAddr,port,pxmlPackageHead->clientIPAddr)) < 0)
		{
			UnionUserErrLog("in UnionAccessCheck:: UnionCheckExternalSystemDef!\n");
			return(ret);
		}

		// ��ȡӦ�ñ�
		if ((ret = UnionCheckAppDef(pxmlPackageHead->appID,pxmlPackageHead->serviceCode,timeout)) < 0)
		{
			UnionUserErrLog("in UnionAccessCheck:: UnionCheckAppDef!\n");
			return(ret);
		}
	}

	// �������붨��
	if ((ret = UnionCheckServiceCodeAndServiceTypeDef(pxmlPackageHead->serviceCode)) < 0)
	{
		UnionUserErrLog("in UnionAccessCheck:: UnionCheckServiceCodeAndServiceTypeDef!\n");
		return(ret);
	}

	return(0);
}

int UnionCheckLoginFlagOfUser(char *sysID,char *userID,char *serviceCode)
{
	int	ret;
	int	loginFlag;
	int	maxFreeTimes;
	long	freeTimes;
	char	loginSysID[32];
	char	systemDateTime[32];
	char	passwdUpdateTime[32];
	char	lastLoginLocation[64];
	char	lastOperationTime[32];
	char	sql[512];
	char	tmpBuf[128];
	PUnionXMLPackageHead	pxmlPackageHead;

	pxmlPackageHead = UnionGetXMLPackageHead();
	
	// ����û���¼״̬
	if (UnionIsUITrans(sysID) && 				// ��UI
		(memcmp(serviceCode,"00",2) != 0) &&		// ������
		(memcmp(serviceCode,"0309",4) != 0) &&		// �޸���Ȩ��
		(memcmp(serviceCode,"01",2) != 0))		// ��¼��
	{
		// �����û���
		snprintf(sql,sizeof(sql),"select * from sysUser where userID = '%s'",userID);	

		if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
		{
			UnionUserErrLog("in UnionCheckLoginFlagOfUser:: UnionSelectRealDBRecord[%s]!\n",sql);
			return(ret);
		}
		else if (ret == 0)
		{
			UnionUserErrLog("in UnionCheckLoginFlagOfUser:: �û�[%s]������!\n",userID);
			return(errCodeOperatorMDL_OperatorNotExists);
		}
		
		UnionLocateXMLPackage("detail", 1);
	
		if ((ret = UnionReadXMLPackageValue("loginFlag", tmpBuf, sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionCheckLoginFlagOfUser:: UnionReadXMLPackageValue[%s]!\n","loginFlag");
			return(ret);
		}
		tmpBuf[ret] = 0;
		loginFlag = atoi(tmpBuf);
		if (!loginFlag)
		{
			UnionUserErrLog("in UnionCheckLoginFlagOfUser:: �û�[%s]δ��¼!\n",userID);
			return(errCodeOperatorMDL_NotLogon);
		}
		
		if ((ret = UnionReadXMLPackageValue("lastLoginLocation", lastLoginLocation, sizeof(lastLoginLocation))) < 0)
		{
			UnionUserErrLog("in UnionCheckLoginFlagOfUser:: UnionReadXMLPackageValue[%s]!\n","lastLoginLocation");
			return(ret);
		}
		lastLoginLocation[ret] = 0;

		if ((ret = UnionReadXMLPackageValue("lastOperationTime", lastOperationTime, sizeof(lastOperationTime))) < 0)
		{
			UnionUserErrLog("in UnionCheckLoginFlagOfUser:: UnionReadXMLPackageValue[%s]!\n","lastOperationTime");
			return(ret);
		}
		lastOperationTime[ret] = 0;
		
		if (strcmp(pxmlPackageHead->clientIPAddr,lastLoginLocation) != 0)
		{
			UnionUserErrLog("in UnionCheckLoginFlagOfUser:: �ͻ��˵�ַ[%s]�͵�¼��ַ[%s]��һ��!\n",pxmlPackageHead->clientIPAddr,lastLoginLocation);
			return(errCodeOperatorMDL_InvalidLogon);
		}
		
		// ����¼ϵͳ���
		if ((ret = UnionReadXMLPackageValue("loginSysID", loginSysID, sizeof(loginSysID))) < 0)
		{
			UnionUserErrLog("in UnionCheckLoginFlagOfUser:: UnionReadXMLPackageValue[%s]!\n","loginSysID");
			return(ret);
		}
		loginSysID[ret] = 0;
		if (strcmp(sysID,loginSysID) != 0)
		{
			UnionUserErrLog("in UnionCheckLoginFlagOfUser:: ϵͳ���[%s]�͵�¼ϵͳ���[%s]��һ��!\n",sysID,loginSysID);
			return(errCodeOperatorMDL_InvalidLogon);
		}
			
		// ������ʱ��
		// ȡ�ϴβ�����Ŀǰ������
		if ((maxFreeTimes = UnionReadIntTypeRECVar("freeTimesOfUser")) <= 0)
			maxFreeTimes = 300;  // Ĭ��300��

		UnionCalcuSecondsPassedAfterSpecTime(lastOperationTime, &freeTimes);
		if ((freeTimes - maxFreeTimes) > 0)
		{
			UnionUserErrLog("in UnionCheckLoginFlagOfUser:: ������ʱ��[%s] ����ʱ��[%ld] > [%d],�����µ�¼!\n",lastOperationTime,freeTimes,maxFreeTimes);
			snprintf(sql,sizeof(sql),"update sysUser set loginFlag = 0 where userID = '%s'",userID);
			UnionExecRealDBSql(sql);
			return(errCodeOperatorMDL_OperatorOverMaxFreeTime);
		}
		
		// ��������Ƿ����
		if ((ret = UnionReadXMLPackageValue("passwdUpdateTime", passwdUpdateTime, sizeof(passwdUpdateTime))) < 0)
		{
			UnionUserErrLog("in UnionCheckLoginFlagOfUser:: UnionReadXMLPackageValue[%s]!\n","passwdUpdateTime");
			return(ret);
		}
		passwdUpdateTime[ret] = 0;
		if ((ret = UnionIsOverduePasswd(passwdUpdateTime,NULL)) < 0)
		{
			UnionUserErrLog("in UnionCheckLoginFlagOfUser:: UnionReadXMLPackageValue[%s]!\n","passwdUpdateTime");
			UnionSetResponseRemark("�����ѹ��ڣ�����������");
			return(errCodeOperatorMDL_OperatorPwdOverTime);
		}
	}
	
	if (pxmlPackageHead->isUI && (strcmp(userID,"NONUSER") != 0) && (memcmp(serviceCode,"0103",4) != 0) && (memcmp(serviceCode,"0104",4) != 0) && (memcmp(serviceCode,"0105",4) != 0))
	{
		UnionGetFullSystemDateTime(systemDateTime);
		systemDateTime[14] = 0;

		// �����û���
		snprintf(sql,sizeof(sql),"update sysUser set lastOperationTime = '%s' where userID = '%s'",systemDateTime,userID);
		if ((ret = UnionExecRealDBSql(sql)) < 0)
		{
			UnionUserErrLog("in UnionCheckLoginFlagOfUser::UnionExecRealDBSql!\n");
			return(ret);
		}
	}
	return(0);
}

int UnionReadHsmGroupIDListByAppID(char *appID,char *hsmGroupIDList,int sizeofBuf)
{
	//int			i;
	int			ret;
	int			len;
	char			sql[128];
	//char			tmpBuf[128];
	char			*precStr = NULL;
	//PUnionTableDataTBL	ptableDataTBL = NULL;
	
	//if ((ptableDataTBL = UnionGetTableDataTBL("app")) == NULL)
	if ((precStr = UnionFindTableValue("app", appID)) == NULL)
	{	
		// ����Ӧ�ñ�	
		len = snprintf(sql,sizeof(sql),"select hsmGroupIDList from app where appID = '%s'",appID);
		sql[len] = 0;
		if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
		{
			UnionUserErrLog("in UnionReadHsmGroupIDListByAppID:: UnionSelectRealDBRecord[%s]!\n",sql);
			return(ret);
		}
		else if (ret == 0)
		{
			UnionUserErrLog("in UnionReadHsmGroupIDByAppID:: Ӧ��ID[%s]������!\n",appID);
			return(errCodeEssc_AppNotDefined);
		}

		UnionLocateXMLPackage("detail", 1);
		
		if ((ret = UnionReadXMLPackageValue("hsmGroupIDList",hsmGroupIDList, sizeofBuf)) < 0)
		{
			UnionUserErrLog("in UnionReadHsmGroupIDListByAppID:: UnionReadXMLPackageValue[%s]!\n","hsmGroupIDList");
			return(ret);
		}
		hsmGroupIDList[ret] = 0;
		
	}
	else
	{
		if (strlen(precStr) == 0)
		{
			UnionUserErrLog("in UnionReadHsmGroupIDListByAppID:: appID[%s] not found!\n",appID);
			return(errCodeEssc_AppNotDefined);
		}
		/*
		if ((ret = UnionReadRecFldFromRecStr(precStr,strlen(precStr),"appID",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionReadHsmGroupIDListByAppID:: UnionReadRecFldFromRecStr[appID][%s]!\n",precStr);
			return(ret);
		}
		tmpBuf[ret] = 0;
		if (strcmp(appID,tmpBuf) != 0)
		{
			UnionUserErrLog("in UnionReadHsmGroupIDListByAppID:: appID[%s] not found!\n",appID);
			return(errCodeEssc_AppNotDefined);
		}
		*/
		if ((ret = UnionReadRecFldFromRecStr(precStr,strlen(precStr),"hsmGroupIDList",hsmGroupIDList,sizeofBuf)) < 0)
		{
			UnionUserErrLog("in UnionReadHsmGroupIDListByAppID:: UnionReadRecFldFromRecStr[hsmGroupIDList][%s]!\n",precStr);
			return(ret);
		}
		hsmGroupIDList[ret] = 0;
	}
	
	return(strlen(hsmGroupIDList));
}

int UnionReadHsmGroupIDByAppID(char *appID,PUnionHsmGroupRec phsmGroupRec)
{
	int			i;
	int			ret;
	int			seq;
	int			hsmGroupNum = 0;
	char			hsmGroupID[32];
	char			hsmGroupIDList[128];
	char			hsmGroupIDGrp[8][128];
	
	if ((ret = UnionReadHsmGroupIDListByAppID(appID,hsmGroupIDList,sizeof(hsmGroupIDList))) < 0)
	{
		UnionUserErrLog("in UnionReadHsmGroupIDByAppID:: UnionReadHsmGroupIDListByAppID[%s]!\n",appID);
		return(ret);
	}
	hsmGroupIDList[ret] = 0;
	
	// ƴ�����崮
	if ((hsmGroupNum = UnionSeprateVarStrIntoVarGrp(hsmGroupIDList,strlen(hsmGroupIDList),',',hsmGroupIDGrp,16)) < 0)
	{
		UnionUserErrLog("in UnionReadHsmGroupIDByAppID:: UnionSeprateVarStrIntoVarGrp[%s]!\n",hsmGroupIDList);
		return(hsmGroupNum);
	}
	else if (hsmGroupNum == 0)
	{
		UnionUserErrLog("in UnionReadHsmGroupIDByAppID:: hsmGroupIDList[%s]!\n",hsmGroupIDList);
		return(errCodeEsscMDL_EsscHsmGrpIDNotDefined);
	}
		
	if (hsmGroupNum > 1)
	{
		srand((unsigned)time(NULL));
		seq = rand() % hsmGroupNum;
	}
	else
		seq = 0;
		
	for (i = -1; i < hsmGroupNum; i++)
	{
		if (i == -1)
			snprintf(hsmGroupID,sizeof(hsmGroupID),"%s",hsmGroupIDGrp[seq]);
		else if (i == seq)
			continue;
		else
			snprintf(hsmGroupID,sizeof(hsmGroupID),"%s",hsmGroupIDGrp[i]);
	
		// ������������	
		if ((ret = UnionGetHsmGroupRecByHsmGroupID(hsmGroupID,phsmGroupRec)) < 0)
		{
			if (ret == errCodeEsscMDL_EsscHsmGrpIDNotDefined)
				continue;
			else
			{
				UnionUserErrLog("in UnionReadHsmGroupIDByAppID:: �������[%s]������!\n",hsmGroupID);
				return(ret);
			}
		}
		else
			break;
	}
		
	if (i == hsmGroupNum)
	{
		UnionUserErrLog("in UnionReadHsmGroupIDByAppID:: hsmGroupIDList[%s]!\n",hsmGroupIDList);
		UnionSetResponseRemark("�޿��õ��������������");
		return(errCodeEsscMDL_EsscHsmGrpIDNotDefined);
	}
	return(0);
}
