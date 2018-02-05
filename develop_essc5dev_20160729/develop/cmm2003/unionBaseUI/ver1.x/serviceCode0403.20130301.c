
#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "UnionLog.h"
#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionTableDef.h"
#include "unionRealBaseDB.h"
#include "baseUIService.h"

/*********************************
�������:	0403
������:		ɾ����
��������:	ɾ����
**********************************/

int UnionDealServiceCode0403(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret;
	char	tableName[40+1];
	char	fileName[1024+1];

	// ��ȡ����
	memset(tableName,0,sizeof(tableName));
	if ((ret = UnionReadRequestXMLPackageValue("body/tableName",tableName,sizeof(tableName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0403:: UnionReadRequestXMLPackageValue[%s]!\n","body/tableName");
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0403:: UnionReadRequestXMLPackageValue[%s] can not be null!\n","body/tableName");
		return(errCodeObjectMDL_FieldValueIsInvalid);
		
	}

	// �����Ƿ��Ѿ�����
	memset(fileName,0,sizeof(fileName));
	UnionGetFileNameOfTableDef(tableName,fileName);

	// �ļ��Ѿ�����
	if ((access(fileName,0) == 0))
	{
		// ɾ����
		if ((ret = unlink(fileName)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0403:: delete fileName[%s] is failed!\n",fileName);
			return(ret);
		}
	}
	else
	{
		UnionUserErrLog("in UnionDealServiceCode0403:: fileName[%s] not found!\n",fileName);
		return(errCodeDatabaseMDL_TableNotFound);
	}
	// ���ص������ڴ�
	if ((ret = UnionReloadTableDefTBL()) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0403:: UnionReloadTableDefTBL!\n");
		return(ret);
	}

	return 0;
}
