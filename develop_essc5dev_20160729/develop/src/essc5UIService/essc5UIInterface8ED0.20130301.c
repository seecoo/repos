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

/***************************************
  �������:	8ED0
  ������:	��ȡ��������ǰ֤������(623)
  ��������:	��ȡ��������ǰ֤������(623)
 ***************************************/
int UnionDealServiceCode8ED0(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret;
	int	totalNum= 0;
	char	tmpBuf[128];
	char	hsmGrpID[128];

	TUnionHsmGroupRec	pSpecHsmGroup;

	// ��ȡ�������ID
	if ((ret = UnionReadRequestXMLPackageValue("body/hsmGrpID",hsmGrpID,sizeof(hsmGrpID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8ED0:: UnionReadRequestXMLPackageValue[%s]!\n","body/hsmGrpID");
		return(ret);
	}
	hsmGrpID[ret] = 0;

	// ��ȡ������ʹ���������
	if ((ret = UnionGetHsmGroupRecByHsmGroupID(hsmGrpID,&pSpecHsmGroup)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8ED0:: UnionGetHsmGroupRecByHsmGroupID!\n");
		return(ret);
	}
	if (pSpecHsmGroup.hsmCmdVersionID != conHsmCmdVerSRJ1401StandardHsmCmd)
	{
		UnionUserErrLog("in UnionDealServiceCode8ED0:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
		UnionSetResponseRemark("���ܻ�ָ�����Ͳ�ƥ��");
		return(errCodeParameter);
	}
	UnionSetUseSpecHsmGroupForOneCmd(hsmGrpID);

	switch(pSpecHsmGroup.hsmCmdVersionID)
	{
	case conHsmCmdVerSRJ1401StandardHsmCmd:
		if ((ret = UnionSRJ1401CmdES623(&totalNum)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8ED0:: UnionSRJ1401CmdES623,ret = [%d]!\n",ret);
			return(ret);
		}
		break;
	default:
		UnionUserErrLog("in UnionDealServiceCode8ED0:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
		UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
		return(errCodeParameter);
	}

	// ����֤������
	snprintf(tmpBuf, sizeof(tmpBuf), "%d", totalNum);
	if ((ret = UnionSetResponseXMLPackageValue("body/totalNum",tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8ED0:: UnionSetResponseXMLPackageValue[%s]!\n","body/totalNum");
		return(ret);
	}

	return(0);
}

