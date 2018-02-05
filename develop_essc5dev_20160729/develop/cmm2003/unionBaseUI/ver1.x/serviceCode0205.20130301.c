
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
�������:	0205
������:		ɾ����ɫ
��������:	ɾ����ɫ
***************************************/
int UnionDealServiceCode0205(PUnionHsmGroupRec phsmGroupRec)
{
	int	i;
	int	ret;
	char	roleID[2+1];
	char	sql[2][512+1];
	char	userID[60+1];
	char	userRoleList[128+1];
	char	tmpBuf[128+1];
	int	totalNum = 0;
	char	*tmp = NULL;

	// ��ȡɾ����ɫID
	memset(roleID,0,sizeof(roleID));
	if ((ret = UnionReadRequestXMLPackageValue("body/roleID",roleID,sizeof(roleID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0205:: UnionReadRequestXMLPackageValue[%s]!\n","body/roleID");
		return(ret);
	}
	
	if (memcmp(roleID,"01",2) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0205:: ������ɫ[%s],������ɾ��!\n",roleID);
		return(errCodeOperatorMDL_CannotDeleteSelf);
	}

	// ɾ����ɫ���е���Ϣ
	memset(sql[0],0,sizeof(sql[1]));
	sprintf(sql[0],"delete from sysRole where roleID = '%s'",roleID);

	// ɾ����Ȩ���н�ɫ��Ϣ
	memset(sql[1],0,sizeof(sql[1]));
	sprintf(sql[1],"delete from  privilege where privilegeMaster = 'ROLE' and  privilegeMasterValue = '%s'",roleID);

	for (i = 0; i < 2; i++)
	{
		if ((ret = UnionExecRealDBSql(sql[i])) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0205:: UnionExecRealDBSql[%s]!\n",sql[i]);
			return(ret);
		}
	}

	// �������ݿ��е��û���Ľ�ɫ
	// ��ȡ�û��ͽ�ɫ
	memset(sql[0],0,sizeof(sql[0]));
	sprintf(sql[0],"select userID,userRoleList from sysUser where userRoleList like '%%%s%%'",roleID);

	if ((ret = UnionSelectRealDBRecord(sql[0],0,0)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0205:: UnionSelectRealDBRecord[%s]!\n",sql[0]);
		return(ret);
	}
	else if (ret == 0)
	{
		return(ret);
	}

	// ��ȡ������
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadXMLPackageValue("totalNum", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0205:: UnionReadXMLPackageValue[%s]!\n","totalNum");
		return(ret);
	}
	totalNum = atoi(tmpBuf);

	for (i = 1; i <= totalNum; i++)
	{
		if ((ret = UnionLocateXMLPackage("detail", i)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0205:: UnionLocateXMLPackage[%d]!\n",i);
			return(ret);
		}
		// �û�ID
		memset(userID,0,sizeof(userID));
		if ((ret = UnionReadXMLPackageValue("userID", userID, sizeof(userID))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0205:: UnionReadXMLPackageValue[%s]!\n","userID");
			return(ret);
		}

		// ��ɫ
		memset(userRoleList,0,sizeof(userRoleList));
		if ((ret = UnionReadXMLPackageValue("userRoleList", userRoleList, sizeof(userRoleList))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0205:: UnionReadXMLPackageValue[%s]!\n","userRoleList");
			return(ret);
		}

		// ȥ��ָ����ɫ
		if (strcmp(userRoleList,roleID) == 0)
		{
			// ��ֻ�иý�ɫʱ,ɾ���û�
			memset(sql[1],0,strlen(sql[1]));
			sprintf(sql[1],"delete from  sysUser where userID = '%s'",userID);
			if ((ret = UnionExecRealDBSql(sql[1])) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0205:: UnionExecRealDBSql[%s]!\n",sql[1]);
				return(ret);
			}
			continue;
		}
		else 
		{
			tmp = strstr(userRoleList,roleID);
			if (tmp == NULL)
				continue;
			if (strlen(userRoleList) == (strlen(userRoleList)-strlen(tmp) + strlen(roleID)))
				userRoleList[strlen(userRoleList) - strlen(tmp) -1] = 0;
			else
				strcpy(&userRoleList[strlen(userRoleList) - strlen(tmp)],tmp + strlen(roleID) + 1);
		}
		
		// �����û���ɫ��
		memset(sql[1],0,strlen(sql[1]));
		sprintf(sql[1],"update sysUser set userRoleList = '%s' where userID = '%s'",userRoleList,userID);
		if ((ret = UnionExecRealDBSql(sql[1])) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0205:: UnionExecRealDBSql[%s]!\n",sql[1]);
			return(ret);
		}
	}

	return(0);
}

