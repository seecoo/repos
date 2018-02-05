#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dlfcn.h>

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include "unionREC.h"
#include "UnionLog.h"
#include "UnionStr.h"
#include "unionRealBaseDB.h"
#include "unionXMLPackage.h"
#include "commWithHsmSvr.h"
#include "symmetricKeyDB.h"
#include "essc5Interface.h"

#define keyMngStrategyyNotPermitted 	0
#define keyMngStrategyyIsPermitted 	1

typedef struct
{
	char	strategyID[64];
	int	keyValueFlag;
	char	keyNameList[4096];
	int	status;
	int	maxUseTimes;
	char	remark[128];
}TUnionKeyMngStrategy;
typedef		TUnionKeyMngStrategy 	*PUnionKeyMngStrategy;

//modify by zhouxw, delete hsmGroupID
//int UnionStartDealStrategy(char *hsmGroupID,PUnionKeyMngStrategy pkeyMngStrategy)
int UnionStartDealStrategy(PUnionKeyMngStrategy pkeyMngStrategy)
{
	int	ret = 0;
	int	maxNum = 200;
	char	keyNameGrp[maxNum][128];
	int	keyNum = 0;
	int	i = 0;
	char	serviceCode[32];
	char	mode[32];
	char	dateTime[16];
	int	failNum = 0;

	TUnionHsmGroupRec       hsmGroupRec;
	TUnionSymmetricKeyDB	symmetricKeyDB;
	
	//modify by zhouxw
	//if (hsmGroupID == NULL || pkeyMngStrategy == NULL)
	if(pkeyMngStrategy == NULL)
	{
		UnionUserErrLog("in UnionStartDealStrategy:: error parameter!\n");
		return(errCodeParameter);
	}

	if ((keyNum = UnionSeprateVarStrIntoVarGrp(pkeyMngStrategy->keyNameList,strlen(pkeyMngStrategy->keyNameList),',',keyNameGrp,maxNum)) < 0)
	{
		UnionUserErrLog("in UnionStartDealStrategy:: UnionSeprateVarStrIntoVarGrp[%s]!\n",pkeyMngStrategy->keyNameList);
		return(keyNum);
	}

	//modify by zhouxw
	/*
	if (strlen(hsmGroupID) == 0)
		strcpy(hsmGroup,"default");
	else	
		strcpy(hsmGroup,hsmGroupID);

	UnionSetHsmGroupIDForHsmSvr(hsmGroup);
	if ((ret = UnionGetHsmGroupRecByHsmGroupID(hsmGroup,&hsmGroupRec)) < 0)
	{
		if (ret == errCodeEsscMDL_EsscHsmGrpIDNotDefined)
		{
			UnionUserErrLog("in UnionStartDealStrategy:: 密码机组[%s]不存在!\n",hsmGroup);
			return(errCodeEsscMDL_EsscHsmGrpIDNotDefined);
		}
		else
		{
			UnionUserErrLog("in UnionStartDealStrategy:: UnionGetHsmGroupRecByHsmGroupID[%s]!\n",hsmGroup);
			return(ret);
		}
	}
	*/
	if ((ret = UnionInitRequestXMLPackage(NULL,NULL,0)) < 0)
	{
		UnionUserErrLog("in UnionStartDealStrategy:: UnionInitRequestXMLPackage!\n");
		return(ret);
	}

	if ((ret = UnionSetRequestXMLPackageValue("head/sysID","keyStrategy")) < 0)
	{
		UnionUserErrLog("in UnionStartDealStrategy:: UnionSetRequestXMLPackageValue[head/sysID]!\n");
		return(ret);
	}

	if ((ret = UnionSetRequestXMLPackageValue("head/appID","keyStrategy")) < 0)
	{
		UnionUserErrLog("in UnionStartDealStrategy:: UnionSetRequestXMLPackageValue[head/appID]!\n");
		return(ret);
	}

	memset(dateTime,0,sizeof(dateTime));
	UnionGetFullSystemDateTime(dateTime);
	if ((ret = UnionSetRequestXMLPackageValue("head/transTime",dateTime)) < 0)
	{
		UnionUserErrLog("in UnionStartDealStrategy:: UnionSetRequestXMLPackageValue[head/transTime]!\n");
		return(ret);
	}

	if ((ret = UnionSetRequestXMLPackageValue("head/transFlag","1")) < 0)
	{
		UnionUserErrLog("in UnionStartDealStrategy:: UnionSetRequestXMLPackageValue[head/transFlag]!\n");
		return(ret);
	}

	if ((ret = UnionSetRequestXMLPackageValue("head/hash","")) < 0)
	{
		UnionUserErrLog("in UnionStartDealStrategy:: UnionSetRequestXMLPackageValue[head/hash]!\n");
		return(ret);
	}

	if ((ret = UnionSetRequestXMLPackageValue("head/userInfo","")) < 0)
	{
		UnionUserErrLog("in UnionStartDealStrategy:: UnionSetRequestXMLPackageValue[head/userInfo]!\n");
		return(ret);
	}

	for (i = 0; i < keyNum; i++)
	{
		failNum = 0;

		memset(&symmetricKeyDB,0,sizeof(symmetricKeyDB));
againThree:
		// 读取对称密钥
		if ((ret = UnionReadSymmetricKeyDBRec(keyNameGrp[i],0,&symmetricKeyDB)) < 0)
		{
			failNum ++;
			UnionUserErrLog("in UnionStartDealStrategy:: UnionReadSymmetricKeyDBRec[%s] failNum[%d]!\n",keyNameGrp[i],failNum);
			if (failNum < 3)
			{
				goto againThree;
			}
			else
				continue;
		}
		//add by zhouxw
		UnionSetHsmGroupIDForHsmSvr(symmetricKeyDB.keyGroup);
		if ((ret = UnionGetHsmGroupRecByHsmGroupID(symmetricKeyDB.keyGroup,&hsmGroupRec)) < 0)
		{
			if (ret == errCodeEsscMDL_EsscHsmGrpIDNotDefined)
			{
				UnionUserErrLog("in UnionStartDealStrategy:: 密码机组[%s]不存在!\n",symmetricKeyDB.keyGroup);
				return(errCodeEsscMDL_EsscHsmGrpIDNotDefined);
			}
			else
			{
				UnionUserErrLog("in UnionStartDealStrategy:: UnionGetHsmGroupRecByHsmGroupID[%s]!\n",symmetricKeyDB.keyGroup);
				return(ret);
			}
		}
		//add end
		memset(serviceCode,0,sizeof(serviceCode));
		if (strlen(symmetricKeyDB.keyApplyPlatform) > 0)
			strcpy(serviceCode,"E114");
		if (strlen(symmetricKeyDB.keyDistributePlatform) > 0)
			strcpy(serviceCode,"E115");
		if (strlen(serviceCode) == 0)
			strcpy(serviceCode,"E111");

		// 设置服务码
		if ((ret = UnionSetRequestXMLPackageValue("head/serviceCode",serviceCode)) < 0)
		{
			failNum ++;
			UnionUserErrLog("in UnionStartDealStrategy:: UnionReadSymmetricKeyDBRec[%s] failNum[%d]!\n",keyNameGrp[i],failNum);
			if (failNum < 3)
			{
				goto againThree;
			}
			else
				continue;
		}

		// 设置密钥名称
		if ((ret = UnionSetRequestXMLPackageValue("body/keyName",keyNameGrp[i])) < 0)
		{
			failNum ++;
			UnionUserErrLog("in UnionStartDealStrategy:: UnionReadSymmetricKeyDBRec[%s] failNum[%d]!\n",keyNameGrp[i],failNum);
			if (failNum < 3)
			{
				goto againThree;
			}
			else
				continue;
		}

		// 设置申请或分发模式
		memset(mode,0,sizeof(mode));
		if (strcmp(serviceCode,"E111") == 0)
			strcpy(mode,"1");
		else
			sprintf(mode,"%d",pkeyMngStrategy->keyValueFlag+1);
		if ((ret = UnionSetRequestXMLPackageValue("body/mode",mode)) < 0)
		{
			failNum ++;
			UnionUserErrLog("in UnionStartDealStrategy:: UnionSetRequestXMLPackageValue[body/mode] failNum[%d]!\n",failNum);
			if (failNum < 3)
			{
				goto againThree;
			}
			else
				continue;
		}

		if (strcmp(serviceCode,"E111") == 0)
		{
			if ((ret = UnionDealServiceCodeE111(&hsmGroupRec)) < 0)
			{
				failNum ++;
				UnionUserErrLog("in UnionStartDealStrategy:: UnionDealServiceCodeE111 failNum[%d]!\n",failNum);
				if (failNum < 3)
				{
					goto againThree;
				}
				else
					continue;
			}
		}
		else if (strcmp(serviceCode,"E114") == 0)
		{
			if ((ret = UnionDealServiceCodeE114(&hsmGroupRec)) < 0)
			{
				failNum ++;
				UnionUserErrLog("in UnionStartDealStrategy:: UnionDealServiceCodeE114 failNum[%d]!\n",failNum);
				if (failNum < 3)
				{
					goto againThree;
				}
				else
					continue;
			}
		}
		else if (strcmp(serviceCode,"E115") == 0)
		{
			if ((ret = UnionDealServiceCodeE115(&hsmGroupRec)) < 0)
			{
				failNum ++;
				UnionUserErrLog("in UnionStartDealStrategy:: UnionDealServiceCodeE115 failNum[%d]!\n",failNum);
				if (failNum < 3)
				{
					goto againThree;
				}
				else
					continue;
			}
		}
	}
	return 0;
}

int UnionDealDaemonTask(char *parameter)
{
	int	ret = 0;
	int	len = 0;
	char	sql[256];
	char	tmpBuf[32];
	int	i = 0;
	int	totalNum = 0;

	PUnionKeyMngStrategy	pkeyMngStrategy = NULL;
	PUnionKeyMngStrategy	p = NULL;
	
	len = sprintf(sql,"select * from keyMngStrategy where status=1");
	sql[len] = 0;

	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in UnionDealDaemonTask:: UnionSelectRealDBRecord ret[%d], sql[%s]!\n",ret,sql);	
		return(ret);
	}else if (ret == 0)
	{
		UnionLog("in UnionDealDaemonTask:: UnionSelectRealDBRecord count[0], sql[%s]!\n",sql);
		return (ret);
	}
	
	if ((ret = UnionReadXMLPackageValue("totalNum",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionLog("in UnionDealDaemonTask:: UnionSelectRealDBRecord count[0], sql[%s]!\n",sql);
		return (ret);
	}
	tmpBuf[ret] = 0;
	totalNum = atoi(tmpBuf);	

	// 申请空间，保存 sql 结果
	pkeyMngStrategy = (PUnionKeyMngStrategy)malloc(totalNum * sizeof(TUnionKeyMngStrategy));
	if (pkeyMngStrategy == NULL)
	{
		UnionUserErrLog("in UnionDealDaemonTask:: malloc!\n");
		return errCodeUseOSErrCode;
	}
	p = pkeyMngStrategy;
	memset((void *)p, 0, totalNum * sizeof(TUnionKeyMngStrategy));

	for (i = 0; i < totalNum; i++, p++)
	{
		if ((ret = UnionLocateXMLPackage("detail",i + 1)) < 0)
		{
			UnionUserErrLog("in UnionDealDaemonTask:: UnionLocateXMLPackage[%s][%d]!\n","detail",i + 1);
			goto exitException;
		}

		// 读取策略标识
		if ((ret = UnionReadXMLPackageValue("strategyID",p->strategyID,sizeof(p->strategyID))) < 0)
		{
			UnionUserErrLog("in UnionDealDaemonTask:: UnionLocateXMLPackage[%s][%d]!\n","strategyID",i + 1);	
			goto exitException;
		}

		// 密钥更新标识
		if ((ret = UnionReadXMLPackageValue("keyValueFlag",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealDaemonTask:: UnionLocateXMLPackage[%s][%d]!\n","keyValueFlag",i + 1);	
			goto exitException;
		}
		tmpBuf[ret] = 0;
		p->keyValueFlag = atoi(tmpBuf);

		// 密钥名称列表
		if ((ret = UnionReadXMLPackageValue("keyNameList",p->keyNameList,sizeof(p->keyNameList))) < 0)
		{
			UnionUserErrLog("in UnionDealDaemonTask:: UnionLocateXMLPackage[%s][%d]!\n","keyNameList",i + 1);
			goto exitException;
		}

		// 激活状态
		if ((ret = UnionReadXMLPackageValue("status",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealDaemonTask:: UnionLocateXMLPackage[%s][%d]!\n","status",i + 1);	
			goto exitException;
		}
		tmpBuf[ret] = 0;
		p->status = atoi(tmpBuf);
	}

	p = pkeyMngStrategy;
	// 开始处理
	for (i = 0; i < totalNum; i++, p++)
	{
		if (keyMngStrategyyNotPermitted == p->status)
			continue;

		if ((ret = UnionStartDealStrategy(p)) < 0)
		{
			UnionUserErrLog("in UnionDealDaemonTask:: UnionStartDealStrategy!\n");	
			goto exitException;
		}
	}

	free(pkeyMngStrategy);
	return 0;

exitException:
	free(pkeyMngStrategy);
	return (ret);
}
