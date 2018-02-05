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
�������:	0209
������:		��֤���Ա
��������:	��֤���Ա
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
	// ��ȡ�û�ID
	memset(userID,0,sizeof(userID));
	if ((ret = UnionReadRequestXMLPackageValue("body/userID",userID,sizeof(userID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0209:: UnionReadRequestXMLPackageValue[%s]!\n","head/userID");
		return(ret);
	}

	// ��ȡʱ��
	memset(transTime,0,sizeof(transTime));
	if ((ret = UnionReadRequestXMLPackageValue("head/transTime",transTime,sizeof(transTime))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0209:: UnionReadRequestXMLPackageValue[%s]!\n","head/transTime");
		return(ret);
	}
	

	// ��ȡ��������
	memset(reqPassword,0,sizeof(reqPassword));
	if ((ret = UnionReadRequestXMLPackageValue("body/password",reqPassword,sizeof(reqPassword))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0209:: UnionReadRequestXMLPackageValue[%s]!\n","body/password");
		return(ret);
	}

	// ��ȡ�û���
	memset(sql,0,sizeof(sql));
	sprintf(sql,"select * from sysUser where userID = '%s'",userID);
	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0209:: UnionSelectRealDBRecord[%s]!\n",sql);
		return(ret);
	}

	// ��ȡ������
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
	
	// ��ȡ�û�״̬
	memset(userStatus,0,sizeof(userStatus));
	if ((ret = UnionReadXMLPackageValue("userStatus", userStatus, sizeof(userStatus))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0209:: UnionReadXMLPackageValue[%s]!\n","userStatus");
		return(ret);
	}
	
	//����û�״̬
	if (!atoi(userStatus))
	{
		UnionUserErrLog("in UnionDealServiceCode0209:: userStatus[%s]!\n",userStatus);
		return(errCodeOperatorMDL_OperatorIsLocked);
	}

	// ��ȡ��֤��
	memset(identifyCode,0,sizeof(identifyCode));
	if ((ret = UnionReadXMLPackageValue("identifyCode", identifyCode, sizeof(identifyCode))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0209:: UnionReadXMLPackageValue[%s]!\n","identifyCode");
		return(ret);
	}
	
	// �����֤��
	if (strlen(identifyCode) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0209:: δ��ȡ��֤��!\n");
		return(errCodeOperatorMDL_IdentifyCodeNotExists);
	}
	
	// ��ȡ����
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
	
	// �������
	if (strcmp(localPassword,reqPassword) != 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0209:: reqPassword[%s]!\n",reqPassword);
		return(errCodeOperatorMDL_WrongPassword);
	}
		
	// ��ȡ��ɫ
	memset(userRoleList,0,sizeof(userRoleList));
	if ((ret = UnionReadXMLPackageValue("userRoleList", userRoleList, sizeof(userRoleList))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0209:: UnionReadXMLPackageValue[%s]!\n","userRoleList");
		return(ret);
	}

	// ����ɫ
	if (strcmp(userRoleList,"00") != 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0209:: userRoleList[%s] is error!\n",userRoleList);
		UnionSetResponseRemark("�û��������Ա");
		return(errCodeParameter);
	}

	// ��ȡ�������ʱ��
	if ((ret = UnionReadXMLPackageValue("passwdUpdateTime",passwdUpdateTime,sizeof(passwdUpdateTime))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0209:: UnionReadXMLPackageValue [%s]!\n","passwdUpdateTime");	
		return(ret);	
	}
	passwdUpdateTime[ret] = 0;

	// ��������Ƿ����
	if ((ret = UnionIsOverduePasswd(passwdUpdateTime,NULL)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0209:: UnionIsOverduePasswd [%s]!\n",passwdUpdateTime);	
		UnionSetResponseRemark("�����ѹ��ڣ�����������");
		return(errCodeOperatorMDL_OperatorPwdOverTime);
	}

	return(0);
}
