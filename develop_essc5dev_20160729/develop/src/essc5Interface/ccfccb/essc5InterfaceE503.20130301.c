//	Author:		ÕÅÓÀ¶¨
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
#include "commWithHsmSvr.h"
#include "unionRealBaseDB.h"
#include "UnionLog.h"
#include "essc5Interface.h"
#include "UnionStr.h"
#include "symmetricKeyDB.h"
#include "asymmetricKeyDB.h"
#include "unionHsmCmd.h"


/***************************************
1þÎñ´úÂë:	E503
***************************************/
int UnionDealServiceCodeE503(PUnionHsmGroupRec phsmGroupRec)
{
        char    certID[20];
        char    bankCertBuf[3000];
        char    data[5000];
        int     dataLen=0;
        char    pkcs7Env[8192];
        int     sizeofPkcs7Env=8191;
        int     ret=0;
        char    sql[1024+1];


        memset(certID,0,sizeof(certID));
        memset(bankCertBuf,0,sizeof(bankCertBuf));
        memset(data,0,sizeof(data));
        memset(sql,0,sizeof(sql));

        if ((ret = UnionReadRequestXMLPackageValue("body/certID",certID,sizeof(certID))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE503:: UnionReadRequestXMLPackageValue[%s]!\n","body/certID");
                return(ret);
        }
        if ((ret = UnionReadRequestXMLPackageValue("body/data",data,sizeof(data))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE503:: UnionReadRequestXMLPackageValue[%s]!\n","body/data");
                return(ret);
        }
        dataLen=strlen(data);

        sprintf(sql,"select * from certOfMof  where certID = '%s' ",certID);
        if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
        {
                UnionUserErrLog("in UnionReadRemoteKeyPlatformRec:: UnionExecRealDBSql[%s]!\n",sql);
                return(ret);
        }

	else if (ret == 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE503:: certID[%s] not find!\n",certID);
                UnionSetResponseRemark("bankID[%s]²»´æÔÚ",certID);
                return(errCodeDatabaseMDL_RecordNotFound);
        }

        UnionLocateXMLPackage("detail", 1);

        if ((ret = UnionReadXMLPackageValue("bankCert", bankCertBuf, sizeof(bankCertBuf))) < 0)
        {
                UnionUserErrLog("in UnionReadRemoteKeyPlatformRec:: UnionReadXMLPackageValue[%s]!\n","bankCertBuf");
                return(ret);
        }

	if((ret= UnionEncryptEnvelopeWithoutSign(data, bankCertBuf, pkcs7Env , sizeofPkcs7Env))<0)
	{
                UnionUserErrLog("in UnionDealServiceCodeE503:: UnionEncryptEnvelopeWithoutSign!\n");
                return(ret);
	}
	UnionSetResponseXMLPackageValue("body/pkcs7Env",pkcs7Env);
        UnionUserErrLog("in UnionDealServiceCodeE501:: pkcs7Env[%d][%s]!\n",ret,pkcs7Env);
        return(0);

	return 0;
}


