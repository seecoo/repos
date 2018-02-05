//	Author:		张永定
//	Copyright:	Union Tech. Guangzhou
//	Date:		2013-01-10

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unionREC.h"
#include "UnionMD5.h"
#include "UnionLog.h"
#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionTableDef.h"
#include "unionRealBaseDB.h"
#include "baseUIService.h"
#include "UnionStr.h"

/***************************************
服务代码:	0209
服务名:		验证审核员
功能描述:	验证审核员
***************************************/
int UnionDealServiceCode0209(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret;
	int	totalNum = 0;
	char	userID[40+1];
	char	userRoleList[512+1];
	char	transTime[14+1];
	char	tmpBuf[2048+1];
	char	sql[512+1];
	char	reqPassword[128+1];
	char	userPassword[128+1];
	char	localPassword[128+1];
	char	identifyCode[128+1];
	char	userStatus[1+1];
	char	passwdUpdateTime[14+1];
	// 读取用户ID
	memset(userID,0,sizeof(userID));
	if ((ret = UnionReadRequestXMLPackageValue("body/userID",userID,sizeof(userID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0209:: UnionReadRequestXMLPackageValue[%s]!\n","head/userID");
		return(ret);
	}

	// 读取时间
	memset(transTime,0,sizeof(transTime));
	if ((ret = UnionReadRequestXMLPackageValue("head/transTime",transTime,sizeof(transTime))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0209:: UnionReadRequestXMLPackageValue[%s]!\n","head/transTime");
		return(ret);
	}
	

	// 读取请求密码
	memset(reqPassword,0,sizeof(reqPassword));
	if ((ret = UnionReadRequestXMLPackageValue("body/password",reqPassword,sizeof(reqPassword))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0209:: UnionReadRequestXMLPackageValue[%s]!\n","body/password");
		return(ret);
	}

	// 读取用户表
	memset(sql,0,sizeof(sql));
	sprintf(sql,"select * from sysUser where userID = '%s'",userID);
	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0209:: UnionSelectRealDBRecord[%s]!\n",sql);
		return(ret);
	}

	// 读取总数量
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadXMLPackageValue("totalNum", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0209:: UnionReadXMLPackageValue[%s]!\n","totalNum");
		return(ret);
	}
	totalNum = atoi(tmpBuf);
	if (totalNum == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0209:: userID[%s] not found!\n",userID);
		return(errCodeOperatorMDL_OperatorNotExists);
	}
	
	UnionLocateXMLPackage("detail", 1);
	
	// 读取用户状态
	memset(userStatus,0,sizeof(userStatus));
	if ((ret = UnionReadXMLPackageValue("userStatus", userStatus, sizeof(userStatus))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0209:: UnionReadXMLPackageValue[%s]!\n","userStatus");
		return(ret);
	}
	
	//检查用户状态
	if (!atoi(userStatus))
	{
		UnionUserErrLog("in UnionDealServiceCode0209:: userStatus[%s]!\n",userStatus);
		return(errCodeOperatorMDL_OperatorIsLocked);
	}

	// 读取认证码
	memset(identifyCode,0,sizeof(identifyCode));
	if ((ret = UnionReadXMLPackageValue("identifyCode", identifyCode, sizeof(identifyCode))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0209:: UnionReadXMLPackageValue[%s]!\n","identifyCode");
		return(ret);
	}
	
	// 检查认证码
	if (strlen(identifyCode) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0209:: 未获取认证码!\n");
		return(errCodeOperatorMDL_IdentifyCodeNotExists);
	}
	
	// 读取密码
	memset(userPassword,0,sizeof(userPassword));
	if ((ret = UnionReadXMLPackageValue("userPassword", userPassword, sizeof(userPassword))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0209:: UnionReadXMLPackageValue[%s]!\n","userPassword");
		return(ret);
	}
	
	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"%s%s%s",transTime,identifyCode,userPassword);
	memset(localPassword,0,sizeof(localPassword));
	UnionMD5((unsigned char *)tmpBuf, strlen(tmpBuf), (unsigned char *)localPassword);
	
	// 密码错误
	if (strcmp(localPassword,reqPassword) != 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0209:: reqPassword[%s]!\n",reqPassword);
		return(errCodeOperatorMDL_WrongPassword);
	}
		
	// 读取角色
	memset(userRoleList,0,sizeof(userRoleList));
	if ((ret = UnionReadXMLPackageValue("userRoleList", userRoleList, sizeof(userRoleList))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0209:: UnionReadXMLPackageValue[%s]!\n","userRoleList");
		return(ret);
	}

	// 检查角色
	if (strcmp(userRoleList,"00") != 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0209:: userRoleList[%s] is error!\n",userRoleList);
		UnionSetResponseRemark("用户不是审核员");
		return(errCodeParameter);
	}

	// 读取密码更新时间
	if ((ret = UnionReadXMLPackageValue("passwdUpdateTime",passwdUpdateTime,sizeof(passwdUpdateTime))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0209:: UnionReadXMLPackageValue [%s]!\n","passwdUpdateTime");	
		return(ret);	
	}
	passwdUpdateTime[ret] = 0;

	// 检查密码是否过期
	if ((ret = UnionIsOverduePasswd(passwdUpdateTime,NULL)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0209:: UnionIsOverduePasswd [%s]!\n",passwdUpdateTime);	
		UnionSetResponseRemark("密码已过期，请重置密码");
		return(errCodeOperatorMDL_OperatorPwdOverTime);
	}

	return(0);
}
