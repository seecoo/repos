//	Author:		������
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
�������:	0105
������:		�޸�����
��������:	�޸�����
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
	
	// ��ȡ�û�ID
	memset(userID,0,sizeof(userID));
	if ((ret = UnionReadRequestXMLPackageValue("head/userID",userID,sizeof(userID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0105:: UnionReadRequestXMLPackageValue[%s]!\n","head/userID");
		return(ret);
	}

	//  ����ʱ��
	memset(transTime,0,sizeof(transTime));
	if ((ret = UnionReadRequestXMLPackageValue("head/transTime",transTime,sizeof(transTime))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0105:: UnionReadRequestXMLPackageValue[%s]!\n","head/transTime");
		return(ret);
	}

	// ��ȡ��������
	memset(curPassword,0,sizeof(curPassword));
	if ((ret = UnionReadRequestXMLPackageValue("body/curPassword",curPassword,sizeof(curPassword))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0105:: UnionReadRequestXMLPackageValue[%s]!\n","body/curPassword");
		return(ret);
	}

	// ��ȡ������
	memset(newPassword,0,sizeof(newPassword));
	if ((ret = UnionReadRequestXMLPackageValue("body/newPassword",newPassword,sizeof(newPassword))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0105:: UnionReadRequestXMLPackageValue[%s]!\n","body/newPassword");
		return(ret);
	}

	// ��ȡ�û���
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
	
	// ��ȡ��֤��
	memset(identifyCode,0,sizeof(identifyCode));
	if ((ret = UnionReadXMLPackageValue("identifyCode", identifyCode, sizeof(identifyCode))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0105:: UnionReadXMLPackageValue[%s]!\n","identifyCode");
		return(ret);
	}
	
	// �����֤��
	if (strlen(identifyCode) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0105:: δ��ȡ��֤��!\n");
		return(errCodeOperatorMDL_IdentifyCodeNotExists);
	}
	
	// ��ȡ����
	memset(userPassword,0,sizeof(userPassword));
	if ((ret = UnionReadXMLPackageValue("userPassword", userPassword, sizeof(userPassword))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0105:: UnionReadXMLPackageValue[%s]!\n","userPassword");
		return(ret);
	}

	// ��ȡ�������ʱ��
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

	// ��������Ƿ����
	memset(remark,0,sizeof(remark));
	if ((ret = UnionIsOverduePasswd(passwdUpdateTime,remark)) >= 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0105:: passwdUpdateTime[%s] ʣ������[%d]!\n",passwdUpdateTime,ret);
		return(errCodeEsscMDL_InvalidOperation);
	}

	// �������
	if (strcmp(localPassword,curPassword) != 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0105:: curPassword[%s] is wrong!\n",curPassword);
		return(errCodeOperatorMDL_WrongPassword);
	}

	// ����¾������Ƿ���ͬ
	if (strcmp(userPassword,newPassword) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0105:: newPassword and oldPassword is same!\n");
		return(errCodeOperatorMDL_PasswordIsSame);
	}
		
	memset(systemTime,0,sizeof(systemTime));
	UnionGetFullSystemDateTime(systemTime);
	
	// �����û���
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

