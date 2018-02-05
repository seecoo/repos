
#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/dir.h>

#include "UnionLog.h"
#include "UnionStr.h"
#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionTableDef.h"
#include "unionRealBaseDB.h"
#include "unionVersion.h"
#include "baseUIService.h"

/*********************************
�������:	0506
������:		�޸Ĳ˵�
��������:	�޸Ĳ˵�
**********************************/

int UnionDealServiceCode0506(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret;
	int	i = 0;
	int	newMenuName = 0;
	int	menuName = 0;
	char	tmpBuf[128+1];
	int	idValue[2] = {99999998,99999999};
	int	errFlag = 0;
	int	retErrNo = 0;
	char	sql[1024+1];

	// ��ȡ�²˵���
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRequestXMLPackageValue("body/newMenuName",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0506:: UnionReadRequestXMLPackageValue[body/newMenuName]!\n");
		return(ret);
	}
	else
		newMenuName = atoi(tmpBuf);
	
	// ��ȡ�˵���
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRequestXMLPackageValue("body/menuName",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0506:: UnionReadRequestXMLPackageValue[body/menuName]!\n");
		return(ret);
	}
	else
		menuName = atoi(tmpBuf);

	if (newMenuName == menuName)
		return 0;

	// �������Ƿ��Ѿ�����
	memset(sql,0,sizeof(sql));
	sprintf(sql,"select menuName from sysMenu where menuName = %d",newMenuName);
	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0506:: UnionSelectRealDBRecord[%s]!\n",sql);
		return(ret);
	}
	else if (ret > 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0506:: menuName[%d] is already exist!\n",newMenuName);
		UnionSetResponseRemark("�²˵����Ѿ�����");
		return(errCodeParameter);
	}

	// ����һ���˵�
	for (i = 0; i < 2; i++)
	{
		memset(sql,0,sizeof(sql));
		sprintf(sql,"insert into sysMenu(menuName,menuType,menuLevel,menuDisplayName,isVisible,seqNo,isLeaf) values(%d,1,1,'test',1,0,0)",idValue[i]);
		if ((ret = UnionExecRealDBSql(sql)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0506:: UnionExecRealDBSql[%s]!\n",sql);
			return(ret);
		}
	}

	// ����һ����ͼ
	memset(sql,0,sizeof(sql));		
	sprintf(sql,"insert into sysView(viewName) values(%d)",idValue[0]);
	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0506:: UnionExecRealDBSql[%s]!\n",sql);
		return(ret);
	}

	// �޸İ�ť
	memset(sql,0,sizeof(sql));
	sprintf(sql,"update sysButton set viewName = %d where viewName = %d",idValue[0],menuName);
	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0506:: UnionExecRealDBSql[%s]!\n",sql);
		errFlag = 1;
		UnionSetResponseRemark("���°�ťʧ��");
		retErrNo = ret;
		goto delete;
	}

	// �޸���ͼ
	memset(sql,0,sizeof(sql));
	sprintf(sql,"update sysView set viewName = %d where viewName = %d",idValue[1],menuName);
	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0506:: UnionExecRealDBSql[%s]!\n",sql);
		errFlag = 1;
		UnionSetResponseRemark("������ͼʧ��");
		retErrNo = ret;
		goto  delete;
	}

	// ���²˵�
	memset(sql,0,sizeof(sql));
	sprintf(sql,"update sysMenu set menuName = %d where menuName = %d",newMenuName,menuName);
	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0506:: UnionExecRealDBSql[%s]!\n",sql);
		errFlag = 1;
		UnionSetResponseRemark("���²˵�ʧ��");
		retErrNo = ret;
		goto  delete;
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0506:: UnionExecRealDBSql[%s]!\n",sql);
		errFlag = 1;
		UnionSetResponseRemark("���²˵�ʧ��");
		retErrNo = ret;
		goto  delete;
	}

	// �����Ӳ˵�
	memset(sql,0,sizeof(sql));
	sprintf(sql,"update sysMenu set menuParentName = %d where menuParentName = %d",newMenuName,menuName);
	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0506:: UnionExecRealDBSql[%s]!\n",sql);
		errFlag = 1;
		UnionSetResponseRemark("�����Ӳ˵�ʧ��");
		retErrNo = ret;
		goto  delete;
	}

	// �޸���ͼ
	memset(sql,0,sizeof(sql));
	sprintf(sql,"update sysView set viewName = %d where viewName = %d",newMenuName,idValue[1]);
	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0506:: UnionExecRealDBSql[%s]!\n",sql);
		errFlag = 1;
		UnionSetResponseRemark("������ͼʧ��");
		retErrNo = ret;
		goto  delete;
	}

	// �޸Ĳ�����ͼ
	memset(sql,0,sizeof(sql));
	sprintf(sql,"update sysButton set operViewName = %d where operViewName = %d",newMenuName,menuName);
	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0506:: UnionExecRealDBSql[%s]!\n",sql);
		errFlag = 1;
		UnionSetResponseRemark("���²�����ͼʧ��");
		retErrNo = ret;
		goto  delete;
	}

	// �޸İ�ť
	memset(sql,0,sizeof(sql));
	sprintf(sql,"update sysButton set viewName = %d where viewName = %d",newMenuName,idValue[0]);
	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0506:: UnionExecRealDBSql[%s]!\n",sql);
		errFlag = 1;
		UnionSetResponseRemark("���°�ťʧ��");
		retErrNo = ret;
		goto  delete;
	}
	
	// ������Ȩ
	memset(sql,0,sizeof(sql));
	sprintf(sql,"update privilege set privilegeAccessValue = %d where privilegeAccess = 'MENU' and privilegeAccessValue = %d",newMenuName,menuName); 
	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0506:: UnionExecRealDBSql[%s]!\n",sql);
		errFlag = 1;
		UnionSetResponseRemark("������Ȩʧ��");
		retErrNo = ret;
		goto  delete;
	}
delete:
	// ɾ�����ӵĲ˵�
	for (i = 0; i < 2; i++)
	{
		memset(sql,0,sizeof(sql));
		sprintf(sql,"delete from sysMenu where menuName = %d",idValue[i]); 
		if ((ret = UnionExecRealDBSql(sql)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0506:: UnionExecRealDBSql[%s]!\n",sql);
			return(ret);
		}
	}

	// ɾ�����ӵ���ͼ
	memset(sql,0,sizeof(sql));
	sprintf(sql,"delete from sysView where viewName = %d",idValue[0]); 
	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0506:: UnionExecRealDBSql[%s]!\n",sql);
		return(ret);
	}

	if (errFlag == 1)
		return(retErrNo);
	
	return 0;
}
