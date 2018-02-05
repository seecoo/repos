#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "essc5UIService.h"
#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionRealBaseDB.h"
#include "UnionLog.h"
#include "UnionStr.h"
#include "unionHsmCmd.h"
#include "unionHsmCmdVersion.h"

#include "unionSRJ1401Cmd.h"
#include "base64.h"

/***************************************
  �������:	8ED6
  ������:	֤��ɾ��(621)by userDn
  ��������:	֤��ɾ��(621)by userDn
 ***************************************/
int UnionDealServiceCode8ED6(PUnionHsmGroupRec phsmGroupRec)
{
	int			ret;
	char			userDN[128];
	char			hsmGrpID[128];

	TUnionHsmGroupRec	pSpecHsmGroup;

	// ֤��DN
	if ((ret = UnionReadRequestXMLPackageValue("body/userDN",userDN,sizeof(userDN))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8ED6:: UnionReadRequestXMLPackageValue[%s]!\n","body/userDN");
		return(ret);
	}
	userDN[ret] = 0;

	// ��ȡ�������ID
	if ((ret = UnionReadRequestXMLPackageValue("body/hsmGrpID",hsmGrpID,sizeof(hsmGrpID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8ED6:: UnionReadRequestXMLPackageValue[%s]!\n","body/hsmGrpID");
		return(ret);
	}
	hsmGrpID[ret] = 0;

	// ��ȡ������ʹ�õ��������
	if ((ret = UnionGetHsmGroupRecByHsmGroupID(hsmGrpID,&pSpecHsmGroup)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8ED6:: UnionGetHsmGroupRecByHsmGroupID!\n");
		return(ret);
	}
	if (pSpecHsmGroup.hsmCmdVersionID != conHsmCmdVerSRJ1401StandardHsmCmd)
	{
		UnionUserErrLog("in UnionDealServiceCode8ED6:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
		UnionSetResponseRemark("���ܻ�ָ�����Ͳ�ƥ��");
		return(errCodeParameter);
	}
	UnionSetUseSpecHsmGroupForOneCmd(hsmGrpID);

	switch(pSpecHsmGroup.hsmCmdVersionID)
	{
	case conHsmCmdVerSRJ1401StandardHsmCmd:
		if ((ret = UnionSRJ1401CmdES621(userDN)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8ED6:: UnionSRJ1401CmdES621,ret = [%d]!\n",ret);
			return(ret);
		}
		break;
	default:
		UnionUserErrLog("in UnionDealServiceCode8ED6:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
		UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
		return(errCodeParameter);
	}

	return(0);
}

