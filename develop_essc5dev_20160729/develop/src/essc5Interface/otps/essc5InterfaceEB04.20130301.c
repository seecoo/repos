/*
 * Copyright (c) 2011 Union Tech.
 * All rights reserved.
 *
 * Created on: 2016-01-27
 *   @Author: chenwd
 *   @Version 1.0
 */

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "essc5Interface.h"
#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionRealBaseDB.h"
#include "UnionLog.h"
#include "UnionStr.h"
#include "unionHsmCmd.h"
#include "symmetricKeyDB.h"
#include "asymmetricKeyDB.h"
#include "UnionXOR.h"
#include "unionHsmCmdVersion.h"
#include "hsmDefaultFunction.h"
/*
���ܣ�ָ��WJ����SM4��Կ����ӡ
���������
	keyType		3A	��Կ����
	fldNum		2A	�����ֶθ���
	fld1		128A	�����ֶ�1
	fld2		128A	�����ֶ�2
	fld3		128A	�����ֶ�n

���������
	component	32H	����

*/
int UnionDealServiceCodeEB04(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret = 0,keyType,fldNum,len = 0,j = 0;
	char	tmpBuf[64];
	char	fld[20][80];
	char	component[64];
	char	sql[128];
	char	ipAddr[32];

	//��Կ����
	if ((ret = UnionReadRequestXMLPackageValue("body/keyType",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB04:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyType");
		return(ret);
	}
	tmpBuf[ret] = 0;
	keyType = UnionConvertSymmetricKeyKeyType(tmpBuf);

        // ��ȡ��������ڵĴ�ӡ�����    
        ret = sprintf(sql,"select ipAddr from hsm where status=1 and enabled=1 and uses=1 and hsmGroupID='%s' order by ipAddr",phsmGroupRec->hsmGroupID);
	sql[ret] = 0;
	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB02:: UnionExecRealDBSql[%s]!\n",sql);
		UnionSetResponseRemark("��ȡ�������ʧ��");
		return(ret);
        }
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB02:: UnionExecRealDBSql[%s]!\n",sql);
		UnionSetResponseRemark("�������[%s]û�п��õĴ�ӡ��;�������",phsmGroupRec->hsmGroupID);
		return(errCodeDatabaseMDL_RecordNotFound);
	}

	UnionLocateXMLPackage("detail", 1);
	memset(ipAddr,0,sizeof(ipAddr));
	if ((ret = UnionReadXMLPackageValue("ipAddr", ipAddr, sizeof(ipAddr))) < 0)
	{
			UnionUserErrLog("in UnionDealServiceCodeEB02:: UnionReadXMLPackageValue[%s]!\n","ipAddr");
			return(ret);
	}
        
        // ʹ��ָ���ļ��ܻ�
        UnionSetUseSpecHsmIPAddrForOneCmd(ipAddr);
        	
	//�����ֶθ���
	if ((ret = UnionReadRequestXMLPackageValue("body/fldNum",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB04:: UnionReadRequestXMLPackageValue[%s]!\n","body/partKeyNum");
		return(ret);
	}
	tmpBuf[ret] = 0;
	fldNum = atoi(tmpBuf);

	for(j=0; j<fldNum; j++)
	{
		memset(fld[j], 0, sizeof(fld[j]));
		
		len = sprintf(tmpBuf,"body/fld%d",j+1);
		tmpBuf[len] = 0;
		if ((ret = UnionReadRequestXMLPackageValue(tmpBuf,fld[j],sizeof(fld[j]))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeEB04:: UnionReadRequestXMLPackageValue[%s]!\n",tmpBuf);
			return(ret);
		}
	}

	UnionSetIsUseNormalZmkType();
	memset(component,0,sizeof(component));
	if ((ret = UnionHsmCmdWJ(keyType, fldNum, fld, component)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB04:: UnionHsmCmdWJ err!\n");
		return(ret);
	}

	// ���ط���
	if ((ret = UnionSetResponseXMLPackageValue("body/component",component)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB04:: UnionSetResponseXMLPackageValue component[%s]!\n",component);
		return(ret);
	}

	return(0);
}
