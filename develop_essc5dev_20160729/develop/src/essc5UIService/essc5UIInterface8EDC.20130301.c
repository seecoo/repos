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
  �������:	8EDC
  ������:	֤��ͬ���豸����(844)
  ��������:	֤��ͬ���豸����(844)
 ***************************************/
int UnionDealServiceCode8EDC(PUnionHsmGroupRec phsmGroupRec)
{
	int			ret;
	char			tmpBuf[128];
	char			ipAddr[16];
	int			port;
	char			hsmGrpID[128];

	TUnionHsmGroupRec	pSpecHsmGroup;

	// ��ȡIP��ַ
	if ((ret = UnionReadRequestXMLPackageValue("body/ipAddr",ipAddr,sizeof(ipAddr))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8EDC:: UnionReadRequestXMLPackageValue[%s]!\n","body/ipAddr");
		return(ret);
	}
	ipAddr[ret] = 0;

	// ��ȡ�˿ں�
	if ((ret = UnionReadRequestXMLPackageValue("body/port",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8EDC:: UnionReadRequestXMLPackageValue[%s]!\n","body/tmpBuf");
		return(ret);
	}
	tmpBuf[ret] = 0;
	port = atoi(tmpBuf);

	// ��ȡ���ܻ���ID
	if ((ret = UnionReadRequestXMLPackageValue("body/hsmGrpID",hsmGrpID,sizeof(hsmGrpID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8EDC:: UnionReadRequestXMLPackageValue[%s]!\n","body/hsmGrpID");
		return(ret);
	}
	hsmGrpID[ret] = 0;

	// ��ȡ������ʹ���������
	if ((ret = UnionGetHsmGroupRecByHsmGroupID(hsmGrpID,&pSpecHsmGroup)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8EDC:: UnionGetHsmGroupRecByHsmGroupID!\n");
		return(ret);
	}
	if (pSpecHsmGroup.hsmCmdVersionID != conHsmCmdVerSRJ1401StandardHsmCmd)
	{
		UnionUserErrLog("in UnionDealServiceCode8EDC:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
		UnionSetResponseRemark("���ܻ�ָ�����Ͳ�ƥ��");
		return(errCodeParameter);
	}
	UnionSetUseSpecHsmGroupForOneCmd(hsmGrpID);
	
	if ((ret = UnionSRJ1401CmdES844(ipAddr, port)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8EDC:: UnionSRJ1401CmdES844,ret = [%d]!\n",ret);
		return(ret);
	}

	if (ret == 0)
	{
		return(0);
	}
	else if (ret == 1)
	{
		UnionSetResponseRemark("�豸��Ϣ�Ѿ�����");
		return(ret);
	}

	return(ret);
}

