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
#include "unionRSA.h"
#include "unionHsmCmdVersion.h"

/***************************************
服务代码:	E502
***************************************/

int UnionVerifyRsaPkcsWith38(char *hashID,int PKLen,char *PK,int signDataLen,char *signData,int signLen,char *sign);

int UnionDealServiceCodeE502(PUnionHsmGroupRec phsmGroupRec)
{
	char    pk[2000];
        char    certID[20];
        char    bankCertBuf[3000];
        char    caCertBuf[3000];
        char    data[5000];
        int     dataLen=0;
        char    pkcs7Sign[8192];
        int     sizeofPkcs7Sign=8191;
        int     ret=0;
        char    sql[1024+1];
	char	sign[1024];
	int	hashID=0;
	char	hashIDBuf[4];
	char    shapadStr[] =  "3021300906052B0E03021A05000414";
	int	padLen = 0;
	char	tmpb[1000];
	char	hashdata[100];
	char	pkValue[2048+1];
        char	evalue[2048+1];
	char	signBCD[512+1];
	char	data1[1024];

        memset(pk,0,sizeof(pk));
        memset(certID,0,sizeof(certID));
        memset(bankCertBuf,0,sizeof(bankCertBuf));
        memset(caCertBuf,0,sizeof(caCertBuf));
        memset(data,0,sizeof(data));
        memset(data1,0,sizeof(data1));
        memset(sql,0,sizeof(sql));
	memset(sign,0,sizeof(sign));
	memset(tmpb,0,sizeof(tmpb));
	

        if ((ret = UnionReadRequestXMLPackageValue("body/certID",certID,sizeof(certID))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE502:: UnionReadRequestXMLPackageValue[%s]!\n","body/certID");
                return(ret);
        }
        if ((ret = UnionReadRequestXMLPackageValue("body/data",data,sizeof(data))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE502:: UnionReadRequestXMLPackageValue[%s]!\n","body/data");
                return(ret);
        }
        dataLen = strlen(data);

	if ((ret = UnionReadRequestXMLPackageValue("body/sign",pkcs7Sign,sizeof(pkcs7Sign))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE502:: UnionReadRequestXMLPackageValue[%s]!\n","body/sign");
                return(ret);
        }
	if(strlen(pkcs7Sign)>sizeofPkcs7Sign)
	{
		UnionUserErrLog("in UnionDealServiceCodeE502:: sign to long[%s]!\n","body/sign");
                return(-1);
	}
        sizeofPkcs7Sign = strlen(pkcs7Sign);
	
	if((ret=UnionParaseJHSignWithPkcs7((unsigned char*)pkcs7Sign,sizeofPkcs7Sign,bankCertBuf,sign,(unsigned char*)data1,&dataLen,&hashID))<0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE502:: UnionParaseJHSignWithPkcs7 failed!\n");
                return(ret);
	}

	memset(hashIDBuf, 0, sizeof(hashIDBuf));
        sprintf(hashIDBuf, "%02d", hashID);
	if((ret = UnionGetCertificatePKFromBuf(bankCertBuf,strlen(bankCertBuf),pk,sizeof(pk)))<0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE502:: UnionGetCertificatePKFromBuf failed!\n");
                return(ret);
	}
	UnionDebugLog("in UnionDealServiceCodeE502=[%s], sign=[%s], data=[%s], datalen=[%d], hashID=[%d],pk=[%s]!\n", bankCertBuf,sign,data,dataLen,hashID,pk);
	memset(hashdata,0,sizeof(hashdata));
        memset(tmpb,0,sizeof(tmpb));
        padLen = strlen(shapadStr);
        UnionSHA1(data,dataLen,tmpb);
        aschex_to_bcdhex(shapadStr,padLen,hashdata);
        memcpy(hashdata+padLen/2,tmpb,20);
	char tmptmp[200];
	memset(tmptmp,0,200);
	bcdhex_to_aschex(tmpb,20,tmptmp);
        UnionLog("in UnionDealServiceCodeE502::hashedData=[%s][%s][%s]!\n",data,tmptmp ,hashdata);

        memset(pkValue, 0, sizeof(pkValue));
        memset(evalue,0,sizeof(evalue));
        UnionPKCSCerASN1DER_Decode(pk, pkValue, evalue);

        UnionDebugLog("in UnionDealServiceCodeE502::sign=[%s], hashID=[%d],pk=[%s]!\n", sign,hashID,pkValue);
        memset(signBCD, 0, sizeof(signBCD));;
        aschex_to_bcdhex(sign, strlen(sign), signBCD);
	
	switch(phsmGroupRec->hsmCmdVersionID)
        {
                case conHsmCmdVerRacalStandardHsmCmd:
                case conHsmCmdVerSJL06StandardHsmCmd:
			ret = UnionVerifyRsaPkcsWith38(hashIDBuf, strlen(pkValue), pkValue, 35, hashdata, strlen(sign)/2, signBCD);
                	break;
                default:
                        UnionUserErrLog("in UnionDealServiceCodeE502:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
                        UnionSetResponseRemark("非法的加密机指令类型");
                        return(errCodeParameter);
        }

	if ( ret < 0 )
        {
                UnionUserErrLog("in UnionDealServiceCodeE502:: UnionSignWithHsm!\n");
		UnionSetResponseXMLPackageValue("body/verify","wrong");
                return(ret);
        }
	UnionLog("in UnionDealServiceCodeE502:: UnionSignWithHsm!\n");
	UnionSetResponseXMLPackageValue("body/verify","ok");
	return 0;
}

int UnionVerifyRsaPkcsWith38(char *hashID,int PKLen,char *PK,int signDataLen,char *signData,int signLen,char *sign)
{
        int     ret;
        char    signature[1024+1];

        char    signBuf[1024];
        char    signDataBuf[1024];

        memset(signBuf, 0, sizeof(signBuf));
        memset(signDataBuf, 0, sizeof(signDataBuf));

        bcdhex_to_aschex(sign, signLen, signBuf);
        bcdhex_to_aschex(signData, signDataLen, signDataBuf);

        UnionAuditLog("in UnionVerifyRsaPkcsWith38::sign=[%s]!\n", signBuf);
        UnionAuditLog("in UnionVerifyRsaPkcsWith38::signData=[%s]!\n", signDataBuf);
        UnionAuditLog("in UnionVerifyRsaPkcsWith38::PK=[%s]!\n", PK);

        ret = UnionHsmCmd38(hashID[1],NULL,signLen,sign, signDataLen,signData,PK);
	if(ret<0)
	{
		UnionUserErrLog("UnionVerifyRsaPkcsWith38 failed ret=[%d]!\n",ret);
		return ret;
	}
	
        memcpy(sign,signature,ret);

        return ret;
}


