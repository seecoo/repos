//	Author:		张永定
//	Copyright:	Union Tech. Guangzhou
//	Date:		2013-01-10

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "UnionLog.h"
#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionTableDef.h"
#include "unionRealBaseDB.h"
#include "baseUIService.h"

/***************************************
服务代码:	0201
服务名:		增加用户
功能描述:	增加用户
***************************************/
int UnionDealServiceCode0201(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret;
	int	loginMode = 0;
	char	tmpBuf[16+1];
	char	userID[60+1];
	char	userName[128+1];
	char	userPassword[128+1];
	char	userRoleList[128+1];
	char	organization[40+1];
	char	sql[8192+1];
	char	createTime[14+1];
	char	operTermList[2048+1];
	char	remark[128+1];
	char	passwdUpdateTime[14+1];
	char	userPK[512+1];
	char	cardPKXY[512+1];
	
	memset(createTime,0,sizeof(createTime));
	UnionGetFullSystemDateTime(createTime);

	// 读取用户ID
	memset(userID,0,sizeof(userID));
	if ((ret = UnionReadRequestXMLPackageValue("body/userID",userID,sizeof(userID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0201:: UnionReadRequestXMLPackageValue[%s]!\n","body/userID");
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0201:: userID can not be null!\n");
		return(errCodeParameter);
	}
	// 读取姓名
	memset(userName,0,sizeof(userName));
	if ((ret = UnionReadRequestXMLPackageValue("body/userName",userName,sizeof(userName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0201:: UnionReadRequestXMLPackageValue[%s]!\n","body/userName");
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0201:: userName can not be null!\n");
		return(errCodeParameter);
	}
	
	// 读取登录方式
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRequestXMLPackageValue("body/loginMode",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		loginMode = 0;
		//UnionUserErrLog("in UnionDealServiceCode0201:: UnionReadRequestXMLPackageValue[%s]!\n","body/loginMode");
		//return(ret);
	}
	else 
		loginMode = atoi(tmpBuf);
	
	// 读取密码密文
	memset(userPassword,0,sizeof(userPassword));
	memset(passwdUpdateTime,0,sizeof(passwdUpdateTime));
	if (loginMode != 1)
	{
		if ((ret = UnionReadRequestXMLPackageValue("body/userPassword",userPassword,sizeof(userPassword))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0201:: UnionReadRequestXMLPackageValue[%s]!\n","body/userPassword");
			return(ret);
		}
		else if (ret == 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0201:: userPassword can not be null!\n");
			return(errCodeParameter);
		}
		memset(passwdUpdateTime,0,sizeof(passwdUpdateTime));
		strcpy(passwdUpdateTime,createTime);
	}
	
	// 读取用户PK
	memset(userPK,0,sizeof(userPK));
	if (loginMode == 2)	// USBKey
	{
		if ((ret = UnionReadRequestXMLPackageValue("body/userPK",userPK,sizeof(userPK))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0201:: UnionReadRequestXMLPackageValue[%s]!\n","body/userPK");
			return(ret);
		}
		else if (ret == 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0201:: userPK can not be null!\n");
			return(errCodeParameter);
		}
	}

	// 读取卡公钥
	if (loginMode == 3)
	{
		if ((ret = UnionReadRequestXMLPackageValue("body/cardPKXY",cardPKXY,sizeof(cardPKXY))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0201:: UnionReadRequestXMLPackageValue[%s]!\n","body/cardPKXY");
			return(ret);
		}
		else if (ret == 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0201:: cardPKXY can not be null!\n");
			return(errCodeParameter);
		}
		cardPKXY[ret] = 0;
	}
	
	// 读取角色
	memset(userRoleList,0,sizeof(userRoleList));
	if ((ret = UnionReadRequestXMLPackageValue("body/userRoleList",userRoleList,sizeof(userRoleList))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0201:: UnionReadRequestXMLPackageValue[%s]!\n","body/userRoleList");
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0201:: userRoleList can not be null!\n");
		return(errCodeParameter);
	}

	// 读取所属机构
	memset(organization,0,sizeof(organization));
	if ((ret = UnionReadRequestXMLPackageValue("body/organization",organization,sizeof(organization))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0201:: UnionReadRequestXMLPackageValue[%s]!\n","body/organization");
		return(ret);
	}

	// 读取操作终端列表
	memset(operTermList,0,sizeof(operTermList));
	if ((ret = UnionReadRequestXMLPackageValue("body/operTermList",operTermList,sizeof(operTermList))) <= 0)
	{
		strcpy(operTermList,"");
	}
	
	// 读取备注
	memset(remark,0,sizeof(remark));
	if ((ret = UnionReadRequestXMLPackageValue("body/remark",remark,sizeof(remark))) <= 0)
	{
		strcpy(remark,"");
	}
	
	memset(sql,0,sizeof(sql));
	if (loginMode == 3)
	{
		sprintf(sql,"insert into sysUser(userID,userName,loginMode,userPassword,userPK,cardPKXY,userRoleList,organization,operTermList,loginFlag,loginTimes,wrongPasswordTimes,userStatus,createTime,remark) values ("
		"'%s','%s',%d,'%s','%s','%s','%s','%s','%s',0,0,0,1,'%s','%s')",
		userID,userName,loginMode,userPassword,userPK,cardPKXY,userRoleList,organization,operTermList,createTime,remark);
	}
	else
	{
		sprintf(sql,"insert into sysUser(userID,userName,loginMode,userPassword,userPK,userRoleList,organization,operTermList,loginFlag,loginTimes,wrongPasswordTimes,userStatus,createTime,remark) values ("
		"'%s','%s',%d,'%s','%s','%s','%s','%s',0,0,0,1,'%s','%s')",
		userID,userName,loginMode,userPassword,userPK,userRoleList,organization,operTermList,createTime,remark);
	}	

	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0201:: UnionExecRealDBSql[%s]!\n",sql);
		return(ret);
	}

	return(0);
}

