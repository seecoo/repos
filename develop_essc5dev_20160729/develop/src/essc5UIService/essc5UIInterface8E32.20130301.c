//      Author:         zhouxw
//      Copyright:      Union Tech. Guangzhou
//      Date:           20150713

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include "essc5UIService.h"
#include "UnionLog.h"
#include "unionXMLPackage.h"
#include "unionErrCode.h"
#include "unionRealBaseDB.h"
#include "UnionStr.h"
#include "unionHsmCmd.h"
#include "unionHsmCmdVersion.h"
#include "asymmetricKeyDB.h"
#include "unionCertFunSM2.h"

/***************************************
  �������:     8E32
  ������:       ����SM2֤�������ļ�
  ��������:     ����SM2֤�������ļ�
 ***************************************/
int UnionDealServiceCode8E32(PUnionHsmGroupRec phsmGroupRec)
{
	int		ret;
	char		fileName[128];
	
	//��Կ��
	if((ret = UnionReadRequestXMLPackageValue("body/fileName", fileName, sizeof(fileName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E32:: UnionReadRequestXMLPackageValue[%s]\n", "body/fileName");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(fileName);

	if((ret = UnionSetResponseXMLPackageValue("file/filename", fileName)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E32:: UnionSetResponseXMLPackageValue[%s]\n", "file/filename");
		return(ret);
	}

	return(0);
}
