
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
#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionTableDef.h"
#include "unionRealBaseDB.h"
#include "baseUIService.h"

/*********************************
�������:	0409
������:		�����ݿ����ؽ���
��������:	�����ݿ����ؽ���
**********************************/

int UnionDealServiceCode0409(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret;
	
	// ɾ����
	if ((ret = UnionDealServiceCode0408(phsmGroupRec)) < 0)
	{
		UnionLog("in UnionDealServiceCode0409:: UnionDealServiceCode0408!\n");
		return(ret);
	}
	
	// �½���
	if ((ret = UnionDealServiceCode0407(phsmGroupRec)) < 0)
	{
		UnionLog("in UnionDealServiceCode0409:: UnionDealServiceCode0407!\n");
		return(ret);
	}
	return(0);		
}
