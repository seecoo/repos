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
服务代码:	0105
服务名:		修改密码
功能描述:	修改密码
***************************************/
int UnionDealServiceCode0105(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret;
	int	len = 0;
	char	userID[40+1];
	char	tmpBuf[512+1];
	char	sql[512+1];
	char	transTime[16+1];
	char	curPassword[128+1];
	char	newPassword[128+1];
	char	userPassword[128+1];
	char	localPassword[128+1];
	char	identifyCode[128+1];
	char	systemTime[14+1];
	char	passwdUpdateTime[14+1];
	char	remark[128+1];
	
	// 读取用户ID
	memset(userID,0,sizeof(userID));
	if ((ret = UnionReadRequestXMLPackageValue("head/userID",userID,sizeof(userID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0105:: UnionReadRequestXMLPackageValue[%s]!\n","head/userID");
		return(ret);
	}

	//  传输时间
	memset(transTime,0,sizeof(transTime));
	if ((ret = UnionReadRequestXMLPackageValue("head/transTime",transTime,sizeof(transTime))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0105:: UnionReadRequestXMLPackageValue[%s]!\n","head/transTime");
		return(ret);
	}

	// 读取请求密码
	memset(curPassword,0,sizeof(curPassword));
	if ((ret = UnionReadRequestXMLPackageValue("body/curPassword",curPassword,sizeof(curPassword))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0105:: UnionReadRequestXMLPackageValue[%s]!\n","body/curPassword");
		return(ret);
	}

	// 读取新密码
	memset(newPassword,0,sizeof(newPassword));
	if ((ret = UnionReadRequestXMLPackageValue("body/newPassword",newPassword,sizeof(newPassword))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0105:: UnionReadRequestXMLPackageValue[%s]!\n","body/newPassword");
		return(ret);
	}

	// 读取用户表
	memset(sql,0,sizeof(sql));
	sprintf(sql,"select * from sysUser where userID = '%s'",userID);
	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0105:: UnionSelectRealDBRecord[%s]!\n",sql);
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0105:: userID[%s] not found!\n",userID);
		return(errCodeOperatorMDL_OperatorNotExists);
	}
	
	UnionLocateXMLPackage("detail", 1);
	
	// 读取认证码
	memset(identifyCode,0,sizeof(identifyCode));
	if ((ret = UnionReadXMLPackageValue("identifyCode", identifyCode, sizeof(identifyCode))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0105:: UnionReadXMLPackageValue[%s]!\n","identifyCode");
		return(ret);
	}
	
	// 检查认证码
	if (strlen(identifyCode) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0105:: 未获取认证码!\n");
		return(errCodeOperatorMDL_IdentifyCodeNotExists);
	}
	
	// 读取密码
	memset(userPassword,0,sizeof(userPassword));
	if ((ret = UnionReadXMLPackageValue("userPassword", userPassword, sizeof(userPassword))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0105:: UnionReadXMLPackageValue[%s]!\n","userPassword");
		return(ret);
	}

	// 读取密码更新时间
	memset(passwdUpdateTime,0,sizeof(passwdUpdateTime));
	if ((ret = UnionReadXMLPackageValue("passwdUpdateTime",passwdUpdateTime,sizeof(passwdUpdateTime))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0105:: UnionReadXMLPackageValue[%s]!\n","passwdUpdateTime");
		return(ret);
	}
	
	len = sprintf(tmpBuf,"%s%s%s",transTime,identifyCode,userPassword);
	tmpBuf[len] = 0;
	memset(localPassword,0,sizeof(localPassword));
	UnionMD5((unsigned char *)tmpBuf, strlen(tmpBuf), (unsigned char *)localPassword);

	// 检查密码是否过期
	memset(remark,0,sizeof(remark));
	if ((ret = UnionIsOverduePasswd(passwdUpdateTime,remark)) >= 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0105:: passwdUpdateTime[%s] 剩余天数[%d]!\n",passwdUpdateTime,ret);
		return(errCodeEsscMDL_InvalidOperation);
	}

	// 密码错误
	if (strcmp(localPassword,curPassword) != 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0105:: curPassword[%s] is wrong!\n",curPassword);
		return(errCodeOperatorMDL_WrongPassword);
	}

	// 检查新旧密码是否相同
	if (strcmp(userPassword,newPassword) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0105:: newPassword and oldPassword is same!\n");
		return(errCodeOperatorMDL_PasswordIsSame);
	}
		
	memset(systemTime,0,sizeof(systemTime));
	UnionGetFullSystemDateTime(systemTime);
	
	// 更新用户表
	memset(sql,0,sizeof(sql));
	sprintf(sql,"update sysUser set userPassword = '%s',passwdUpdateTime = '%s' where userID = '%s'",newPassword,systemTime,userID);
	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0105:: UnionExecRealDBSql[%s]!\n",sql);
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0105:: UnionExecRealDBSql[%s]!\n",sql);
		return(errCodeDatabaseMDL_RecordUpdateFailure);
	}
	
	return(0);
}

