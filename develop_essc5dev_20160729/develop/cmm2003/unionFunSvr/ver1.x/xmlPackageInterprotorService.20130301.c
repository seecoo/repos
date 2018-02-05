//	Author:		张永定
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

#include "UnionLog.h"
#include "UnionStr.h"
#include "unionErrCode.h"
#include "unionREC.h"
#include "UnionSHA1.h"

#include "unionXMLPackage.h"
#include "unionFunSvrName.h"
#include "errCodeTranslater.h"
#include "unionRealBaseDB.h"
#include "commWithHsmSvr.h"
#include "registerRunningTime.h"
#include "unionTableData.h"

int UnionDealServiceTask(char *serviceCode,PUnionHsmGroupRec phsmGroupRec);

int UnionUserSpecFunForFunSvr()
{
	return(0);
}

int UnionReadHsmGroupIDListByAppID(char *appID,char *hsmGroupIDList,int sizeofBuf)
{
	int			ret;
	int			len;
	char			sql[128];
	char			*precStr = NULL;
	
	if ((precStr = UnionFindTableValue("app", appID)) == NULL)
	{	
		// 查找应用表	
		len = sprintf(sql,"select hsmGroupIDList from app where appID = '%s'",appID);
		sql[len] = 0;
		if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
		{
			UnionUserErrLog("in UnionReadHsmGroupIDListByAppID:: UnionSelectRealDBRecord[%s]!\n",sql);
			return(ret);
		}
		else if (ret == 0)
		{
			UnionUserErrLog("in UnionReadHsmGroupIDByAppID:: 应用ID[%s]不存在!\n",appID);
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
	
	// 拼分域定义串
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
			ret = sprintf(hsmGroupID,"%s",hsmGroupIDGrp[seq]);
		else if (i == seq)
			continue;
		else
			ret = sprintf(hsmGroupID,"%s",hsmGroupIDGrp[i]);
	
		hsmGroupID[ret] = 0;
		
		// 查找密码机组表	
		if ((ret = UnionGetHsmGroupRecByHsmGroupID(hsmGroupID,phsmGroupRec)) < 0)
		{
			if (ret == errCodeEsscMDL_EsscHsmGrpIDNotDefined)
				continue;
			else
			{
				UnionUserErrLog("in UnionReadHsmGroupIDByAppID:: 密码机组[%s]不存在!\n",hsmGroupID);
				return(ret);
			}
		}
		else
			break;
	}
		
	if (i == hsmGroupNum)
	{
		UnionUserErrLog("in UnionReadHsmGroupIDByAppID:: hsmGroupIDList[%s]!\n",hsmGroupIDList);
		UnionSetResponseRemark("无可用的密码机组或密码机");
		return(errCodeEsscMDL_EsscHsmGrpIDNotDefined);
	}
	return(0);
}

int UnionCheckLoginFlagOfUser(char *sysID,char *userID,char *serviceCode)
{
	int	ret;
	int	len;
	int	loginFlag;
	int	maxFreeTimes;
	long	freeTimes;
	char	loginSysID[32];
	char	systemDateTime[32];
	char	passwdUpdateTime[32];
	char	clientIPAddr[64];
	char	lastLoginLocation[64];
	char	lastOperationTime[32];
	char	sql[512];
	char	tmpBuf[128];
	
	// 检测用户登录状态
	if (UnionIsUITrans(sysID) && 				// 是UI
		(memcmp(serviceCode,"00",2) != 0) &&		// 基本类
		(memcmp(serviceCode,"01",2) != 0))		// 登录类
	{
		// 查找用户表
		len = sprintf(sql,"select * from sysUser where userID = '%s'",userID);	
		sql[len] = 0;
		if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
		{
			UnionUserErrLog("in UnionFunSvrInterprotor:: UnionSelectRealDBRecord[%s]!\n",sql);
			return(ret);
		}
		else if (ret == 0)
		{
			UnionUserErrLog("in UnionFunSvrInterprotor:: 用户[%s]不存在!\n",userID);
			return(errCodeOperatorMDL_OperatorNotExists);
		}
		
		UnionLocateXMLPackage("detail", 1);
	
		if ((ret = UnionReadXMLPackageValue("loginFlag", tmpBuf, sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionFunSvrInterprotor:: UnionReadXMLPackageValue[%s]!\n","loginFlag");
			return(ret);
		}
		tmpBuf[ret] = 0;
		loginFlag = atoi(tmpBuf);
		if (!loginFlag)
		{
			UnionUserErrLog("in UnionFunSvrInterprotor:: 用户[%s]未登录!\n",userID);
			return(errCodeOperatorMDL_NotLogon);
		}
		
		// 检查登录地点
		if ((ret = UnionReadRequestXMLPackageValue("head/clientIPAddr",clientIPAddr,sizeof(clientIPAddr))) < 0)
		{
			UnionUserErrLog("in UnionFunSvrInterprotor:: UnionReadRequestXMLPackageValue[%s]!\n","head/clientIPAddr");
			return(ret);
		}
		clientIPAddr[ret] = 0;

		if ((ret = UnionReadXMLPackageValue("lastLoginLocation", lastLoginLocation, sizeof(lastLoginLocation))) < 0)
		{
			UnionUserErrLog("in UnionFunSvrInterprotor:: UnionReadXMLPackageValue[%s]!\n","lastLoginLocation");
			return(ret);
		}
		lastLoginLocation[ret] = 0;

		if ((ret = UnionReadXMLPackageValue("lastOperationTime", lastOperationTime, sizeof(lastOperationTime))) < 0)
		{
			UnionUserErrLog("in UnionFunSvrInterprotor:: UnionReadXMLPackageValue[%s]!\n","lastOperationTime");
			return(ret);
		}
		lastOperationTime[ret] = 0;
		
		if (strcmp(clientIPAddr,lastLoginLocation) != 0)
		{
			UnionUserErrLog("in UnionFunSvrInterprotor:: 客户端地址[%s]和登录地址[%s]不一致!\n",clientIPAddr,lastLoginLocation);
			return(errCodeOperatorMDL_InvalidLogon);
		}
		
		// 检查登录系统编号
		if ((ret = UnionReadXMLPackageValue("loginSysID", loginSysID, sizeof(loginSysID))) < 0)
		{
			UnionUserErrLog("in UnionFunSvrInterprotor:: UnionReadXMLPackageValue[%s]!\n","loginSysID");
			return(ret);
		}
		loginSysID[ret] = 0;
		if (strcmp(sysID,loginSysID) != 0)
		{
			UnionUserErrLog("in UnionFunSvrInterprotor:: 系统编号[%s]和登录系统编号[%s]不一致!\n",sysID,loginSysID);
			return(errCodeOperatorMDL_InvalidLogon);
		}
			
		// 检查空闲时间
		// 取上次操作到目前的秒数
		if ((maxFreeTimes = UnionReadIntTypeRECVar("freeTimesOfUser")) <= 0)
			maxFreeTimes = 300;  // 默认300秒

		UnionCalcuSecondsPassedAfterSpecTime(lastOperationTime, &freeTimes);
		if ((freeTimes - maxFreeTimes) > 0)
		{
			UnionUserErrLog("in UnionFunSvrInterprotor:: 最后操作时间[%s] 空闲时间[%ld] > [%d],请重新登录!\n",lastOperationTime,freeTimes,maxFreeTimes);
			len = sprintf(sql,"update sysUser set loginFlag = 0 where userID = '%s'",userID);
			sql[len] = 0;
			UnionExecRealDBSql(sql);
			return(errCodeOperatorMDL_OperatorOverMaxFreeTime);
		}
		
		// 检查密码是否过期
		if ((ret = UnionReadXMLPackageValue("passwdUpdateTime", passwdUpdateTime, sizeof(passwdUpdateTime))) < 0)
		{
			UnionUserErrLog("in UnionFunSvrInterprotor:: UnionReadXMLPackageValue[%s]!\n","passwdUpdateTime");
			return(ret);
		}
		passwdUpdateTime[ret] = 0;
		if ((ret = UnionIsOverduePasswd(passwdUpdateTime,NULL)) < 0)
		{
			UnionUserErrLog("in UnionFunSvrInterprotor:: UnionReadXMLPackageValue[%s]!\n","passwdUpdateTime");
			UnionSetResponseRemark("密码已过期，请重置密码");
			return(errCodeOperatorMDL_OperatorPwdOverTime);
		}
	}
	
	if (UnionIsUITrans(sysID) && (strcmp(userID,"NONUSER") != 0) && (memcmp(serviceCode,"0103",4) != 0) && (memcmp(serviceCode,"0104",4) != 0) && (memcmp(serviceCode,"0105",4) != 0))
	{
		UnionGetFullSystemDateTime(systemDateTime);
		systemDateTime[14] = 0;

		// 更新用户表
		len = sprintf(sql,"update sysUser set lastOperationTime = '%s' where userID = '%s'",systemDateTime,userID);
		sql[len] = 0;
		if ((ret = UnionExecRealDBSql(sql)) < 0)
		{
			UnionUserErrLog("in UnionFunSvrInterprotor::UnionExecRealDBSql!\n");
			return(ret);
		}
	}
	return(0);
}

int UnionFunSvrInterprotor(unsigned char *reqStr,int lenOfReqStr,unsigned char *resStr,int sizeOfResStr)
{
	int     ret;
	int	len;
	char    serviceCode[32];
	char    userID[128];
	char	sysID[32];
	char	appID[32];
	char	transFlag[32];
	char    errRemark[128];
	char    tmpBuf[512];
	char    tmpBuf1[512];
	char    tmpBuf2[512];
	unsigned char	hash[64];
	TUnionHsmGroupRec	hsmGroupRec;
	
	if ((reqStr == NULL) || (resStr == NULL))
	{
		UnionUserErrLog("in UnionFunSvrInterprotor:: Null Pointer or lenOfReqStr = [%d] sizeOfBuf = [%d]!\n",lenOfReqStr,sizeOfResStr);
		return(errCodeParameter);
	}
	
	// 解XML请求包
	if ((ret = UnionInitRequestXMLPackage(NULL, (char *)reqStr, lenOfReqStr)) < 0)
	{
		UnionUserErrLog("in UnionFunSvrInterprotor:: UnionInitRequestXMLPackage!\n");
		goto packNow;
	}
	
	// 初始化XML响应包
	if ((ret = UnionInitHeadOfResponseXMLPackage()) < 0)
	{
		UnionUserErrLog("in UnionFunSvrInterprotor:: UnionInitHeadOfResponseXMLPackage!\n");
		goto packNow;
	}
	
	UnionSetTimeByRequestPackage();
	UnionRegisterRunningTime(NULL);

	// 获取系统ID
	if ((ret = UnionReadRequestXMLPackageValue("head/sysID",sysID,sizeof(sysID))) < 0)
	{
		UnionUserErrLog("in UnionFunSvrInterprotor:: UnionReadRequestXMLPackageValue[%s]!\n","head/sysID");
		goto packNow;
	}
	sysID[ret] = 0;

	if (UnionIsUITrans(sysID))
	{
		if ((ret = UnionReadRequestXMLPackageValue("head/userID",userID,sizeof(userID))) < 0)
		{
			UnionUserErrLog("in UnionFunSvrInterprotor:: UnionReadRequestXMLPackageValue[%s]!\n","head/userID");
			goto packNow;
		}
		userID[ret] = 0;
	}
	else
	{
		// 获取应用ID
		if ((ret = UnionReadRequestXMLPackageValue("head/appID",appID,sizeof(appID))) < 0)
		{
			UnionUserErrLog("in UnionFunSvrInterprotor:: UnionReadRequestXMLPackageValue[%s]!\n","head/appID");
			goto packNow;
		}
		appID[ret] = 0;
	}	
	
	// 获取服务代码
	if ((ret = UnionReadRequestXMLPackageValue("head/serviceCode",serviceCode,sizeof(serviceCode))) < 0)
	{
		UnionUserErrLog("in UnionFunSvrInterprotor:: UnionReadRequestXMLPackageValue[%s]!\n","head/serviceCode");
		goto packNow;
	}
	serviceCode[ret] = 0;

	if (UnionIsUITrans(sysID))
		ret = sprintf(tmpBuf1,"************ New of Transcation[%s %s] ************",userID,serviceCode);
	else
		ret = sprintf(tmpBuf1,"************ New of Transcation[%s:%s %s] ************",sysID,appID,serviceCode);
	tmpBuf1[ret] = 0;
		
	memset(tmpBuf2,'*',ret);
	tmpBuf2[ret] = 0;
	UnionLog("\n\n%s\n%s\n\n\n",tmpBuf1,tmpBuf2);
	
	UnionRegisterRunningTime(NULL);
	// 记录日志
	UnionLogRequestXMLPackage();

	UnionRegisterRunningTime(NULL);
	if ((UnionIsUITrans(sysID)) || ((UnionReadIntTypeRECVar("isCheckHashOfPackage") > 0) && (strcmp(appID,"RKM") != 0)))
	{
		if ((len = UnionSelectRequestXMLPackageValue("body",(char *)resStr,sizeOfResStr)) > 0)
		{
			// 计算HASH
			resStr[len] = 0;
			memset(tmpBuf,0,sizeof(tmpBuf));
			UnionSHA1(resStr,len,(unsigned char *)tmpBuf);
			
			ret = bcdhex_to_aschex(tmpBuf,20,(char *)hash);
			hash[ret] = 0;
	
			if ((ret = UnionReadRequestXMLPackageValue("head/hash",tmpBuf,sizeof(tmpBuf))) < 0)
			{
				UnionUserErrLog("in UnionFunSvrInterprotor:: UnionReadRequestXMLPackageValue[%s]!\n","head/hash");
				goto packNow;
			}
			tmpBuf[ret] = 0;
			
			if (strcmp((char *)hash,tmpBuf) != 0)
			{
				UnionUserErrLog("in UnionFunSvrInterprotor:: local hash[%s] != remote hash[%s],len[%d] hashData[%s]\n",hash,tmpBuf,len,resStr);
				ret = errCodeHashValueErr;
				goto packNow;
			}
		}
	}
	UnionRegisterRunningTime(NULL);

	if (UnionIsUITrans(sysID))
	{
		UnionSetIsNotUseHighCached();
		if ((ret = UnionGetHsmGroupRecByHsmGroupID("default",&hsmGroupRec)) < 0)
		{
			if (ret != errCodeEsscMDL_EsscHsmGrpIDNotDefined)
			{
				UnionUserErrLog("in UnionFunSvrInterprotor:: 密码机组[%s]不存在!\n","default");
				goto packNow;
			}
		}
	}
	else
	{
		UnionSetIsUseHighCached();
		// 查找应用表	
		if ((ret = UnionReadHsmGroupIDByAppID(appID,&hsmGroupRec)) < 0)
		{
			UnionUserErrLog("in UnionFunSvrInterprotor:: UnionReadHsmGroupIDByAppID[%s]!\n",appID);
			goto packNow;
		}
	}
	UnionSetHsmGroupIDForHsmSvr(hsmGroupRec.hsmGroupID);
	
	// 获取交易标识
	if ((ret = UnionReadRequestXMLPackageValue("head/transFlag",transFlag,sizeof(transFlag))) < 0)
	{
		UnionUserErrLog("in UnionFunSvrInterprotor:: UnionReadRequestXMLPackageValue[%s]!\n","head/transFlag");
		goto packNow;
	}
	transFlag[ret] = 0;
	
	if (atoi(transFlag) != 1)		// 非请求
	{
		UnionUserErrLog("in UnionFunSvrInterprotor:: transFlag[%s] != 1!\n",transFlag);
		ret = errCodeAPIPackageNotRequest;
		goto packNow;
	}
		
	// 检测用户登录状态
	if ((ret = UnionCheckLoginFlagOfUser(sysID,userID,serviceCode)) < 0)
	{
		UnionUserErrLog("in UnionFunSvrInterprotor:: UnionCheckLoginFlagOfUser[%d]!\n",ret);
		goto packNow;
	}
	
	UnionRegisterRunningTime(NULL);
	// 任务处理
	if ((ret = UnionDealServiceTask(serviceCode,&hsmGroupRec)) < 0)
	{
		UnionUserErrLog("in UnionFunSvrInterprotor:: UnionDealServiceTask[%d]!\n",ret);
		goto packNow;
	}
packNow:
	
	// 重置XML定位
	UnionLocateResponseXMLPackage("",0);
	UnionRegisterRunningTime(NULL);

	if (ret >= 0)
	{
		if (UnionIsUITrans(sysID) || (UnionReadIntTypeRECVar("isCheckHashOfPackage") > 0))
		{
			if ((len = UnionSelectResponseXMLPackageValue("body",(char *)resStr,sizeOfResStr)) > 0)
			{
				// 设置HASH
				resStr[len] = 0;
				memset(tmpBuf,0,sizeof(tmpBuf));
				UnionSHA1(resStr,len,(unsigned char *)tmpBuf);
				ret = bcdhex_to_aschex(tmpBuf,20,(char *)hash);
				hash[ret] = 0;
				
				UnionSetResponseXMLPackageValue("head/hash",(char *)hash);
			}
			else if (len <= 0) // 如果body不存在,则设置hash为空
				UnionSetResponseXMLPackageValue("head/hash","");
		}
	}
	
	// 设置响应描述
	if (ret < 0)    // 执行错误
	{

		// 设置响应码
		len = sprintf(tmpBuf,"%06d",abs(ret));
		tmpBuf[len] = 0;
		UnionSetResponseXMLPackageValue("head/responseCode",tmpBuf);

		//if (UnionReadResponseXMLPackageValue("head/responseRemark",errRemark,sizeof(errRemark)) <= 0)
		if ((len = UnionGetResponseRemark(errRemark,sizeof(errRemark))) <= 0)
		{
			if ((len = UnionTranslateErrCodeIntoRemark(ret,errRemark,sizeof(errRemark))) > 0)       // 转换错误码
			{
				errRemark[len] = 0;
				UnionSetResponseXMLPackageValue("head/responseRemark",errRemark);
			}
			else
				UnionUserErrLog("in UnionFunSvrInterprotor:: UnionTranslateErrCodeIntoRemark ret = [%d]!\n",len);
		}
		else
		{
			errRemark[len] = 0;
			UnionSetResponseXMLPackageValue("head/responseRemark",errRemark);
		}
	}
	else
	{
		UnionSetResponseXMLPackageValue("head/responseCode","000000");
		UnionSetResponseXMLPackageValue("head/responseRemark","成功");	
	}	

	// 记录日志
	UnionLogResponseXMLPackage();

	if (UnionIsUITrans(sysID))
		ret = sprintf(tmpBuf1,"************ End of Transcation[%s %s] ************",userID,serviceCode);
	else
		ret = sprintf(tmpBuf1,"************ End of Transcation[%s:%s %s] ************",sysID,appID,serviceCode);		
	tmpBuf1[ret] = 0;
	memset(tmpBuf2,'*',ret);
	tmpBuf2[ret] = 0;
	UnionLog("\n\n%s\n%s\n\n\n",tmpBuf1,tmpBuf2);
	
	UnionRegisterRunningTime(NULL);

	len = UnionResponseXMLPackageToBuf((char *)resStr, sizeOfResStr);		
	resStr[len] = 0;

	return(len);
}
