
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
�������:	0508
������:		�ƶ��˵�
��������:	�ƶ��˵�
**********************************/

int UnionDealServiceCode0508(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret = 0,i = 0;
	int	menuName = 0;
	char	tmpBuf[128];
	int	seqNo = 0;
	int	menuLevel = 0;
	int	menuParentName = -1;
	int	oriMenuLevel = 0;
	int	oriMenuName = 0;
	int	totalNum = 0;
	int	moveNum = 0;
	char	sql[8192];
	char	menuList[4096];

	// ��ȡ�˵���
	if ((ret = UnionReadRequestXMLPackageValue("body/menuName",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0508:: UnionReadRequestXMLPackageValue[body/newMenuName]!\n");
		return(ret);
	}
	else
		menuName = atoi(tmpBuf);
	
	// ��ȡ���˵���
	if ((ret = UnionReadRequestXMLPackageValue("body/menuParentName",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0508:: UnionReadRequestXMLPackageValue[body/menuParentName]!\n");
		return(ret);
	}
	if (strlen(tmpBuf) > 0)
		menuParentName = atoi(tmpBuf);

	// ��ȡ�˵�����
	if ((ret = UnionReadRequestXMLPackageValue("body/menuLevel",tmpBuf,sizeof(tmpBuf))) <= 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0508:: UnionReadRequestXMLPackageValue[body/seqNo]!\n");
		return(ret);
	}
	else
		menuLevel = atoi(tmpBuf);

	// ��ȡ�˵�˳���
	if ((ret = UnionReadRequestXMLPackageValue("body/seqNo",tmpBuf,sizeof(tmpBuf))) <= 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0508:: UnionReadRequestXMLPackageValue[body/seqNo]!\n");
		return(ret);
	}
	else
		seqNo = atoi(tmpBuf);

	
	// ��ȡ�˵��ƶ�ǰ��Ϣ
	snprintf(sql,sizeof(sql),"select menuParentName,menuLevel,seqNo from sysMenu where menuName = %d",menuName);
	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0508:: UnionSelectRealDBRecord[%s]!\n",sql);
		return(ret);
	}

	if ((ret = UnionLocateXMLPackage("detail",1)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0508:: UnionLocateXMLPackage[%s]\n","detail");
		return(ret);
	}

	// ��ȡ�˵�����
	if ((ret = UnionReadXMLPackageValue("menuLevel",tmpBuf,sizeof(tmpBuf))) < 0) 
	{
		UnionUserErrLog("in UnionDealServiceCode0508:: UnionReadXMLPackageValue[%s]\n","menuLevel");
		return(ret);
	}
	else
		oriMenuLevel = atoi(tmpBuf);

	if (menuParentName == -1)
		snprintf(sql,sizeof(sql),"update sysMenu set menuParentName = NULL, menuLevel = %d, seqNo = %d  where menuName = %d",menuLevel,seqNo,menuName);
	else
		snprintf(sql,sizeof(sql),"update sysMenu set menuParentName = %d, menuLevel = %d, seqNo = %d  where menuName = %d",menuParentName,menuLevel,seqNo,menuName);
	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0508:: UnionExecRealDBSql[%s]!\n",sql);
		return(ret);
	}

	// �޸��Ӳ˵�����
	oriMenuName = menuName;
	moveNum = menuLevel - oriMenuLevel;
	
	snprintf(tmpBuf,sizeof(tmpBuf),"%d",menuName);
	if ((ret = UnionSelectRealDBTree(0,"sysMenu","menuName","menuParentName",2,tmpBuf,menuList,sizeof(menuList))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0508:: UnionSelectRealDBTree!\n");
		return(ret);
	}
	if (ret == 0)
		return(0);

	snprintf(sql,sizeof(sql),"select menuName,menuLevel from sysMenu where menuName in (%s)",menuList);
	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0508:: UnionSelectRealDBRecord[%s]!\n",sql);
		return(ret);
	}
	if (ret == 0)
		return(0);

	// ��ȡ����
	if ((ret = UnionReadXMLPackageValue("totalNum",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0508:: UnionReadXMLPackageValue[%s]\n","totalNum");
		return(ret);
	}
	else
		totalNum = atoi(tmpBuf);	

	// ��ȡҪ�ƶ��Ĳ˵�
	for (i = 1; i <= totalNum; i++)
	{
		if ((ret = UnionLocateXMLPackage("detail",i)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0508:: UnionLocateXMLPackage[%s][%d]\n","detail",i);
			return(ret);
		}

		// ��ȡ�˵�����
		if ((ret = UnionReadXMLPackageValue("menuLevel",tmpBuf,sizeof(tmpBuf))) < 0) 
		{
			UnionUserErrLog("in UnionDealServiceCode0508:: UnionReadXMLPackageValue[%s][%d]\n","seqNo",i);
			return(ret);
		}
		else
			menuLevel = atoi(tmpBuf);
		

		// ��ȡ�˵���
		if ((ret = UnionReadXMLPackageValue("menuName",tmpBuf,sizeof(tmpBuf))) < 0) 
		{
			UnionUserErrLog("in UnionDealServiceCode0508:: UnionReadXMLPackageValue[%s][%d]\n","menuName",i);
			return(ret);
		}
		else
			menuName = atoi(tmpBuf);
		if (menuName == oriMenuName)
			continue;

		snprintf(sql,sizeof(sql),"update sysMenu set menuLevel = %d  where menuName = %d",menuLevel + moveNum,menuName);
		if ((ret = UnionExecRealDBSql(sql)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0508:: UnionExecRealDBSql[%s]!\n",sql);
			return(ret);
		}
	}

	return 0;
}
