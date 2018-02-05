//	Author:		������
//	Copyright:	Union Tech. Guangzhou
//	Date:		2013-01-10

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionTableDef.h"
#include "unionRealBaseDB.h"
#include "UnionLog.h"
#include "baseUIService.h"

/***************************************
�������:	0001
������:		��ȡ��֤��
��������:	��ȡ��֤��
***************************************/
int UnionDealServiceCode0000(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret;
	char	tmpBuf[1024+1];
	char	lenBuf[16+1];
	
	// ��ȡ��������
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRequestXMLPackageValue("body/test",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0000:: UnionReadRequestXMLPackageValue[%s]!\n","body/test");
		return(ret);
	}

	UnionLog("in UnionDealServiceCode0000:: tmpBuf[%s]\n",tmpBuf);
	// ������Ӧ����
	memset(lenBuf,0,sizeof(lenBuf));
	sprintf(lenBuf,"%d",(int)strlen(tmpBuf));
	if ((ret = UnionSetResponseXMLPackageValue("body/test",lenBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0000:: UnionSetResponseXMLPackageValue[%s]!\n","body/test");
		return(ret);
	}
	
	return(0);
}

