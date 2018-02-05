
#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "UnionMD5.h"
#include "UnionLog.h"
#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionTableDef.h"
#include "unionRealBaseDB.h"
#include "baseUIService.h"

/***************************************
服务代码:	0202
服务名:		修改密码
功能描述:	修改密码
***************************************/
int UnionDealServiceCode0202(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret;
	char	userID[60+1];
	char	curPassword[128+1];
	char	newPassword[128+1];
	char	sql[8192+1];
	char	transTime[14+1];
	char	userStatus[1+1];
	char	identifyCode[20+1];
	char	userPassword[128+1];
	char	localPassword[128+1];
	char	passwdUpdateTime[14+1];
	char	tmpBuf[1024+1];

	// 读取用户ID
	memset(userID,0,sizeof(userID));
	if ((ret = UnionReadRequestXMLPackageValue("head/userID",userID,sizeof(userID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0202:: UnionReadRequestXMLPackageValue[%s]!\n","head/userID");
		return(ret);
	}

	// 读取时间
	memset(transTime,0,sizeof(transTime));
	if ((ret = UnionReadRequestXMLPackageValue("head/transTime",transTime,sizeof(transTime))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0202:: UnionReadRequestXMLPackageValue[%s]!\n","head/transTime");
		return(ret);
	}

	// 读取当前密码密文
	memset(curPassword,0,sizeof(curPassword));
	if ((ret = UnionReadRequestXMLPackageValue("body/curPassword",curPassword,sizeof(curPassword))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0202:: UnionReadRequestXMLPackageValue[%s]!\n","body/curPassword");
		return(ret);
	}

	// 读取新密码密文
	memset(newPassword,0,sizeof(newPassword));
	if ((ret = UnionReadRequestXMLPackageValue("body/newPassword",newPassword,sizeof(newPassword))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0202:: UnionReadRequestXMLPackageValue[%s]!\n","body/newPassword");
		return(ret);
	}

	// 读取用户表
	memset(sql,0,sizeof(sql));
	sprintf(sql,"select * from sysUser where userID = '%s'",userID);
	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0202: UnionSelectRealDBRecord[%s]!\n",sql);
		return(ret);
	}
	
	if ((ret =  UnionLocateXMLPackage("detail", 1)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0202:: UnionLocateXMLPackage[%s]!\n","detail");
		return(ret);
	}
	
	// 读取用户状态
	memset(userStatus,0,sizeof(userStatus));
	if ((ret = UnionReadXMLPackageValue("userStatus",userStatus,sizeof(userStatus))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0202:: UnionReadXMLPackageValue[%s]!\n","userStatus");
		return(ret);
	}
	
	// 验证是否登录
	if (!atoi(userStatus))
	{
		UnionUserErrLog("in UnionDealServiceCode0202:: userStatus[%s]!\n",userStatus);
		return(errCodeOperatorMDL_PasswordLocked);
	}

	// 读取用户密码
	memset(userPassword,0,sizeof(userPassword));
	if ((ret = UnionReadXMLPackageValue("userPassword", userPassword, sizeof(userPassword))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0202:: UnionReadXMLPackageValue[%s]!\n","userPassword");
		return(ret);
	}

	// 读取认证码
	memset(identifyCode,0,sizeof(identifyCode));
	if ((ret = UnionReadXMLPackageValue("identifyCode", identifyCode, sizeof(identifyCode))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0202:: UnionReadXMLPackageValue[%s]!\n","identifyCode");
		return(ret);
	}

	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"%s%s%s",transTime,identifyCode,userPassword);
	memset(localPassword,0,sizeof(localPassword));
	UnionMD5((unsigned char *)tmpBuf, strlen(tmpBuf), (unsigned char *)localPassword);

	if (strcmp(localPassword,curPassword) != 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0202:: tmpBuf = [%s] localPassword[%s] != curPassword[%s]!\n",tmpBuf,localPassword,curPassword);
		return(errCodeOperatorMDL_WrongPassword);
	}

	// 检查新旧密码是否相同
	if (strcmp(userPassword,newPassword) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0202:: oldPassword[%s] == newPassword[%s]!\n",userPassword,newPassword);
		return(errCodeOperatorMDL_PasswordIsSame);
	}

	memset(passwdUpdateTime,0,sizeof(passwdUpdateTime));
	UnionGetFullSystemDateTime(passwdUpdateTime);
	// 更新密码
	memset(sql,0,sizeof(sql));
	sprintf(sql,"update sysUser set userPassword = '%s',passwdUpdateTime = '%s' where userID = '%s'",newPassword,passwdUpdateTime,userID);

	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0202:: UnionExecRealDBSql[%s]!\n",sql);
		return(ret);
	}else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0202:: UnionExecRealDBSql[%s]!\n",sql);
		return(errCodeDatabaseMDL_RecordUpdateFailure);
	}

	return(0);
}

