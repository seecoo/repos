//      Author:         ������
//      Copyright:      Union Tech. Guangzhou
//      Date:           2013-01-10

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "essc5UIService.h"
#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionRealBaseDB.h"
#include "UnionLog.h"
#include "UnionStr.h"
#include "symmetricKeyDB.h"
#include "unionHsmCmd.h"
#include "commWithHsmSvr.h"
#include "unionHsmCmdVersion.h"

/***************************************
�������:       8E04
������:         ��ӡ�Գ���Կ
��������:       ��ӡ�Գ���Կ
***************************************/
int UnionDealServiceCode8E04(PUnionHsmGroupRec phsmGroupRec)
{
        int                             ret;
        char                            keyValuePrintFormat[2048];
        int                             numOfComponent = 0;
        char                            tmpNum[32];
	int				len = 0;
        char                            hsmIP[64];
	char				sql[128];
	char				isCheckAlonePrint[32];

        TUnionSymmetricKeyDB            symmetricKeyDB;

        memset(&symmetricKeyDB,0,sizeof(symmetricKeyDB));

        // ��Կ����
        if ((ret = UnionReadRequestXMLPackageValue("body/keyName",symmetricKeyDB.keyName,sizeof(symmetricKeyDB.keyName))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E04:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
                return(ret);
        }
        UnionFilterHeadAndTailBlank(symmetricKeyDB.keyName);
	
        // ��ʽ���� 
        if ((ret = UnionReadRequestXMLPackageValue("body/keyValuePrintFormat",keyValuePrintFormat,sizeof(keyValuePrintFormat))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E04:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyValuePrintFormat");
                return(ret);
        }
	keyValuePrintFormat[ret] = 0;
        UnionFilterHeadAndTailBlank(keyValuePrintFormat);
        if (strlen(keyValuePrintFormat) == 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E04:: UnionReadRequestXMLPackageValue keyValuePrintFormat can not be null!\n");
                return(errCodeParameter);
        }

	// ��ѯ��ӡ��ʽ
	len = sprintf(sql,"select * from keyPrintFormat where formatName = '%s'",keyValuePrintFormat);
	sql[len] = 0;
	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
                UnionUserErrLog("in UnionDealServiceCode8E04:: UnionSelectRealDBRecord sql[%s]!\n",sql);
                return(ret);
	}

	if ((ret = UnionLocateXMLPackage("detail",1)) < 0)	
	{
                UnionUserErrLog("in UnionDealServiceCode8E04:: UnionLocateXMLPackage[%s]!\n","detail");
                return(ret);
	}

	// ��ȡ�Ƿ񵥶���ӡУ��ֵ
	memset(isCheckAlonePrint,0,sizeof(isCheckAlonePrint));
	if ((ret = UnionReadXMLPackageValue("isCheckAlonePrint",isCheckAlonePrint,sizeof(isCheckAlonePrint))) < 0)
	{
                UnionUserErrLog("in UnionDealServiceCode8E04:: UnionReadXMLPackageValue[%s]!\n","isCheckAlonePrint");
                return(ret);
	}
        UnionFilterHeadAndTailBlank(isCheckAlonePrint);

	// ��ȡ��ӡ��ʽ
	if ((ret = UnionReadXMLPackageValue("format",keyValuePrintFormat,sizeof(keyValuePrintFormat))) < 0)
	{
                UnionUserErrLog("in UnionDealServiceCode8E04:: UnionReadXMLPackageValue[%s]!\n","format");
                return(ret);
	}
	keyValuePrintFormat[ret] = 0;


        // ��������
        memset(tmpNum,0,sizeof(tmpNum));
        if ((ret = UnionReadRequestXMLPackageValue("body/numOfComponent",tmpNum,sizeof(tmpNum))) <= 0)
                numOfComponent = 2;
        else if (atoi(tmpNum) <= 1 || atoi(tmpNum) > 9)
        {
                UnionUserErrLog("in UnionDealServiceCode8E04:: numOfComponent[%d] is invalid!\n",atoi(tmpNum));
                return(errCodeParameter);
        }
        else
                numOfComponent = atoi(tmpNum);

        // �����IP
        memset(hsmIP,0,sizeof(hsmIP));
        if ((ret = UnionReadRequestXMLPackageValue("body/ipAddr",hsmIP,sizeof(hsmIP))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E04:: UnionReadRequestXMLPackageValue[%s]!\n","body/hsmIP");
                return(ret);
        }
        UnionFilterHeadAndTailBlank(hsmIP);
        if (!UnionIsValidIPAddrStr(hsmIP))
        {
                UnionUserErrLog("in UnionDealServiceCode8E04:: ipAddr[%s] is valid!\n",hsmIP);
                return(errCodeInvalidIPAddr);
        }

        // ��ȡ�Գ���Կ
        if ((ret = UnionReadSymmetricKeyDBRec(symmetricKeyDB.keyName,0,&symmetricKeyDB)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E04:: UnionReadSymmetricKeyDBRec[%s]!\n",symmetricKeyDB.keyName);
                return(ret);
        }
	
	//add by zhouxw 20150907
	//UnionSetHsmGroupIDForHsmSvr(symmetricKeyDB.keyGroup);
	//add end
	//add by zhouxw 20151013
	if ((ret = UnionGetHsmGroupRecByHsmGroupID(symmetricKeyDB.keyGroup,phsmGroupRec)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E00:: UnionGetHsmGroupRecByHsmGroupID �������[%s]������!\n",symmetricKeyDB.keyGroup);
		return(ret);	
        }
	//add end

	if ((ret = UnionGenerateAndPrintSymmetricKey(phsmGroupRec,&symmetricKeyDB,keyValuePrintFormat,atoi(isCheckAlonePrint),numOfComponent,hsmIP,0,NULL)) < 0)
	{
                UnionUserErrLog("in UnionDealServiceCode8E04:: UnionGenerateAndPrintSymmetricKey!\n");
                return(ret);
	}

        if ((ret = UnionUpdateSymmetricKeyDBKeyValue(&symmetricKeyDB)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E04:: UnionUpdateSymmetricKeyDBKeyValue keyName[%s]!\n",symmetricKeyDB.keyName);
                return(ret);
        }

        return(0);
}


