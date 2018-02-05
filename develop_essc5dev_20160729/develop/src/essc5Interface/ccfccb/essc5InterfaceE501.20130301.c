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
#include "unionCertFun.h"

/***************************************
服务代码:	E501
***************************************/
int UnionDealServiceCodeE501(PUnionHsmGroupRec phsmGroupRec)
{
	char	vk[2000];
	char	certID[20];
	char	bankCertBuf[3000];
	char	caCertBuf[3000];
	char	data[5000];
	int	dataLen=0;
	char	pkcs7Sign[8192];
	int	sizeofPkcs7Sign=8191;
	int	ret=0;
	char    sql[1024+1];
	char	vkIndexStr[5];
	int	vkIndex;

	memset(vk,0,sizeof(vk));
	memset(certID,0,sizeof(certID));
	memset(bankCertBuf,0,sizeof(bankCertBuf));
	memset(caCertBuf,0,sizeof(caCertBuf));
	memset(data,0,sizeof(data));
	memset(sql,0,sizeof(sql));

	if ((ret = UnionReadRequestXMLPackageValue("body/certID",certID,sizeof(certID))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE501:: UnionReadRequestXMLPackageValue[%s]!\n","body/certID");
                return(ret);
        }
        if ((ret = UnionReadRequestXMLPackageValue("body/data",data,sizeof(data))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE501:: UnionReadRequestXMLPackageValue[%s]!\n","body/data");
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
                UnionUserErrLog("in UnionDealServiceCodeE501:: certIDID[%s] not find!\n",certID);
                UnionSetResponseRemark("bankID[%s]不存在",certID);
                return(errCodeDatabaseMDL_RecordNotFound);
        }

        UnionLocateXMLPackage("detail", 1);

        if ((ret = UnionReadXMLPackageValue("bankVK", vk, sizeof(vk))) < 0)
        {
                UnionUserErrLog("in UnionReadRemoteKeyPlatformRec:: UnionReadXMLPackageValue[%s]!\n","vk");
                return(ret);
        }

        if ((ret = UnionReadXMLPackageValue("bankCert", bankCertBuf, sizeof(bankCertBuf))) < 0)
        {
                UnionUserErrLog("in UnionReadRemoteKeyPlatformRec:: UnionReadXMLPackageValue[%s]!\n","bankCertBuf");
                return(ret);
        }

        if ((ret = UnionReadXMLPackageValue("caCert",caCertBuf , sizeof(caCertBuf))) < 0)
        {
                UnionUserErrLog("in UnionReadRemoteKeyPlatformRec:: UnionReadXMLPackageValue[%s]!\n","caCertBuf");
                return(ret);
        }

	if ((ret = UnionReadXMLPackageValue("vkIndex",vkIndexStr , sizeof(vkIndexStr))) < 0)
        {
                UnionUserErrLog("in UnionReadRemoteKeyPlatformRec:: UnionReadXMLPackageValue[%s]!\n","vkIndex");
                return(ret);
        }
	vkIndex=atoi(vkIndexStr);	

	memset(pkcs7Sign,0,sizeof(pkcs7Sign));

        ret=UnionJHSignWithPkcs7Hsm(vkIndex,vk,(unsigned char*)data,dataLen,0,bankCertBuf,caCertBuf,pkcs7Sign,sizeofPkcs7Sign);
	if(ret<0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE501: UnionJHSignWithPkcs7Hsm[%d]!\n",ret);
		return ret;
	}
        UnionSetResponseXMLPackageValue("body/sign",pkcs7Sign);
	UnionLog("in UnionDealServiceCodeE501:: pkcs7sign[%d][%s]!\n",ret,pkcs7Sign);
        return(0);
}

