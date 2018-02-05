//	Author:		张永定
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


/*************************************** 服务代码:	E203
服务名:  	E504 
***************************************/
int UnionDealServiceCodeE504(PUnionHsmGroupRec phsmGroupRec)
{
	char    certID[20];
        char    bankCertBuf[3000];
        char    data[5000];
        char    pkcs7Env[8192];
        int     sizeofPkcs7Env=8191;
        int     ret=0;
        char    sql[1024+1];
	char	vkIndexStr[5];
	int	vkIndex=0;

        memset(certID,0,sizeof(certID));
        memset(bankCertBuf,0,sizeof(bankCertBuf));
        memset(data,0,sizeof(data));
        memset(pkcs7Env,0,sizeof(pkcs7Env));
        memset(sql,0,sizeof(sql));
        memset(vkIndexStr,0,sizeof(vkIndexStr));
		

        if ((ret = UnionReadRequestXMLPackageValue("body/certID",certID,sizeof(certID))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE504:: UnionReadRequestXMLPackageValue[%s]!\n","body/certID");
                return(ret);
        }
        if ((ret = UnionReadRequestXMLPackageValue("body/pkcs7Env",pkcs7Env,sizeof(pkcs7Env))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE504:: UnionReadRequestXMLPackageValue[%s]!\n","body/pkcs7Env");
                return(ret);
        }
        sizeofPkcs7Env = strlen(pkcs7Env)+1;

        sprintf(sql,"select * from certOfMof  where certID = '%s' ",certID);
        if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
        {
                UnionUserErrLog("in UnionReadRemoteKeyPlatformRec:: UnionExecRealDBSql[%s]!\n",sql);
                return(ret);
        }
	
	else if (ret == 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE504:: certIDID[%s] not find!\n",certID);
                UnionSetResponseRemark("bankID[%s]不存在",certID);
                return(errCodeDatabaseMDL_RecordNotFound);
        }

        UnionLocateXMLPackage("detail", 1);

        if ((ret = UnionReadXMLPackageValue("bankCert", bankCertBuf, sizeof(bankCertBuf))) < 0)
        {
                UnionUserErrLog("in UnionReadRemoteKeyPlatformRec:: UnionReadXMLPackageValue[%s]!\n","bankCertBuf");
                return(ret);
        }
	
	if ((ret = UnionReadXMLPackageValue("vkIndex", vkIndexStr, sizeof(vkIndexStr))) < 0)
        {
                UnionUserErrLog("in UnionReadRemoteKeyPlatformRec:: UnionReadXMLPackageValue[%s]!\n","vkIndex");
                return(ret);
        }
	vkIndex = atoi(vkIndexStr);


        if((ret= UnionDecryptEnvelopWithoutSign(vkIndex, "", bankCertBuf, pkcs7Env,  sizeofPkcs7Env , data, sizeof(char)*(1024*1024*4+1)))<0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE504:: UnionEncryptEnvelopeWithoutSign!\n");
                return(ret);
        }
        UnionSetResponseXMLPackageValue("body/data",data);
        UnionLog("in UnionDealServiceCodeE504:: data[%d][%s]!\n",ret,data);

	return 0;
}


