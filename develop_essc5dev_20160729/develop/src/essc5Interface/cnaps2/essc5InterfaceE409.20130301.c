#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif  
       
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <libgen.h>


#include "essc5Interface.h"
#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionRealBaseDB.h"
#include "UnionLog.h"
#include "essc5Interface.h"
#include "UnionStr.h"
#include "unionHsmCmd.h"
#include "asymmetricKeyDB.h"
#include "UnionMD5.h"
#include "UnionSHA1.h"
#include "unionHsmCmdVersion.h"


#include "unionCertErrCode.h"
#include "unionCertFun.h"
#include "sjl06.h"
#include "unionCertFunSM2.h"



/***************************************
  服务代码:	E409
  服务名:  	E409 验签
 ***************************************/

const char defaultUserID[] = "1234567812345678"; 


int UnionGetOtherBankSM2CertFileName(char *bankID, char *version,char *fileName);


int UnionDealServiceCodeE409(PUnionHsmGroupRec phsmGroupRec)
{
	int ret;
	char bankID[16+1];
	char version[8];
 	char userID[32];
	int userIDlen;
	char signatureData[1024];
//	int lenSignatureData;
	char base64Sign[4096+1];

	char fileName[256];
	FILE *fp = NULL;
	TUnionX509CerSM2 tCertInfo;
	char	dataBuf[8192*4] = {0};
//	char pkxy[64] = {0};
	char signatureDER[256] = {0};
	int  signatureDERLen = 0;
	char signature[128] = {0};
	int len = 0;

	int lengRCaPK;
	char rCaPK[2048];
	int lenCaCertDataSign;
	char caCertDataSign[1024];
	int lenHashVal;
	char hashVal[2048];



	
	//获取银行ID号
	memset(bankID, 0, sizeof(bankID));
	if ((ret = UnionReadRequestXMLPackageValue("body/BankID", bankID, sizeof(bankID))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE409:: UnionReadRequestXMLPackageValue[%s]!\n","body/BankID");
			return(ret);
		}
	bankID[ret] = 0;
	

	//获取版本号
	memset(version, 0, sizeof(version));
	if ((ret = UnionReadRequestXMLPackageValue("body/Version", version, sizeof(version))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE409:: UnionReadRequestXMLPackageValue[%s]!\n","body/Version");
			return(ret);
		}
	version[ret] = 0;

	//获取用户ID号
	memset(userID, 0, sizeof( userID));
	if ((ret = UnionReadRequestXMLPackageValue("body/UserID",userID, sizeof(userID))) < 0)
		userIDlen = 0;
	else
	{
		userIDlen=ret;
		userID[ret] = 0;
	}

	//获取原始签名数据
	memset(signatureData, 0, sizeof(signatureData));
	if ((ret = UnionReadRequestXMLPackageValue("body/SignatureData",signatureData, sizeof(signatureData))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE409:: UnionReadRequestXMLPackageValue[%s]!\n","body/SignatureData");
			return(ret);
		}
	 signatureData[ret] = 0;


		
	//获取签名(base64)
	memset(base64Sign, 0, sizeof(base64Sign));
	if ((ret = UnionReadRequestXMLPackageValue("body/Base64Sign",base64Sign, sizeof(base64Sign))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE409:: UnionReadRequestXMLPackageValue[%s]!\n","body/Base64Sign");
			return(ret);
		}
	base64Sign[ret] = 0;

#if 1

	if(strlen(base64Sign) > (64+2)/3*4 + 20)
	{
		UnionUserErrLog("in UnionVerifySM2Signature::parameter base64Sign [%s] err!\n",base64Sign);
		return(errCodeParameter);
	}
	if((signatureDERLen = from64tobits(signatureDER,base64Sign)) <= 0)
	{
		UnionUserErrLog("in UnionVerifySM2Signature::parameter base64Sign [%s] err1!\n",base64Sign);
		return(errCodeParameter);
	}
	ret = UnionGetRSFromSM2SigDer((unsigned char*)signatureDER,signatureDERLen,(unsigned char*)signature);
	if(ret != 64)
	{
		UnionUserErrLog("in UnionVerifySM2Signature::parameter base64Sign [%s] err2!\n",base64Sign);
		return(errCodeParameter);
	}
	if((ret = UnionGetOtherBankSM2CertFileName(bankID,version,fileName)) < 0)
	{
		UnionUserErrLog("in UnionVerifySM2Signature::UnionGetOtherBankSM2CertFileName bankID[%s],version[%s] failed!\n",bankID,version);
		return(ret);
	}
	
	if ((fp = fopen(fileName,"rb")) == NULL)
	{
		UnionUserErrLog("in UnionVerifySM2Signature::fopen fileName=[%s]!\n", fileName);
		return(-10000);
	}

	len=fread(dataBuf,1,8192*2,fp); 
	fclose(fp);
	
	memset(&tCertInfo,0,sizeof(tCertInfo));
	if((ret = UnionGetCertificateInfoFromBufEx(dataBuf,len,&tCertInfo)) < 0)
	{
		UnionUserErrLog("in UnionVerifySM2Signature::UnionGetCertificateInfoFromBufEx failed!\n");
		return ret;
	}
	UnionLog("tCertInfo.derPK=[%s].\n",tCertInfo.derPK);
	if(strncmp(tCertInfo.derPK,"04",2) != 0)
	{
		UnionUserErrLog("in UnionVerifySM2Signature::tx509Cert.derPK[%s] err!\n",tCertInfo.derPK);
		return ret;
	}
	
		strcpy(rCaPK,tCertInfo.derPK+2);
		lengRCaPK=strlen(rCaPK);
		UnionLog("rCaPK=[%s][%d]\n",rCaPK,lengRCaPK);


	memset(caCertDataSign,0,sizeof(caCertDataSign));
//	UnionLog("signature=[%s]\n",signature);
	bcdhex_to_aschex(signature,strlen(signature),caCertDataSign);
	lenCaCertDataSign=strlen(caCertDataSign);
	UnionLog("signature=[%s][%d]\n",caCertDataSign,lenCaCertDataSign);

	memset(hashVal,0,sizeof(hashVal));
//	ret=strlen(signatureData);
//	UnionLog("signatureData=[%s][%d]\n",signatureData,ret);
	bcdhex_to_aschex(signatureData,strlen(signatureData),hashVal);
	lenHashVal=strlen(hashVal);
	UnionLog("data=[%s][%d]\n",hashVal,lenHashVal);


#endif		
	//验签的指令 userID, data都十六进制数据，不是为原数据
	if(userIDlen == 0)
	{
		ret=UnionHsmCmdK4(-1,"02",strlen(defaultUserID),defaultUserID,lengRCaPK,rCaPK,lenCaCertDataSign,caCertDataSign,lenHashVal,hashVal);
		if(ret < 0)
		{
			 UnionUserErrLog("in UnionDealServiceCodeE409:: UnionHsmCmdK4 error!\n");
			 if ((len= UnionSetResponseXMLPackageValue("body/Verify_result", "failed")) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE409: UnionSetResponseXMLPackageValue [%s]!\n","VerifySM2Signature_result");
				return(len);
			}
			 return(ret);
		}
		else
		{
			if ((len= UnionSetResponseXMLPackageValue("body/Verify_result", "Pass")) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE409: UnionSetResponseXMLPackageValue [%s]!\n","VerifySM2Signature_result");
				return(len);
			}
		}
	}
	else
	{
		ret = UnionHsmCmdK4(-1,"02",userIDlen,userID,lengRCaPK,rCaPK,lenCaCertDataSign,caCertDataSign,lenHashVal,hashVal);
		if(ret <0)
		{
	 		 UnionUserErrLog("in UnionDealServiceCodeE409:: UnionHsmCmdK4 error!\n");
			if ((len = UnionSetResponseXMLPackageValue("body/Verify_result", "failed")) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE409: UnionSetResponseXMLPackageValue [%s]!\n","VerifySM2Signature_result");
				return(len);
			}
			return(ret);
		}
		else
		{
			if ((ret = UnionSetResponseXMLPackageValue("body/Verify_result", "Pass")) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE409: UnionSetResponseXMLPackageValue [%s]!\n","VerifySM2Signature_result");
				return(ret);
			}
		}
	}

	return 0;
}

int UnionGetOtherBankSM2CertFileName(char *bankID, char *version,char *fileName)
{
	char    *p = NULL;
	char    path[256];
	if ((p = getenv("UNIONREC")) == NULL)
  {
      UnionUserErrLog("in UnionGetOtherBankSM2CertFileName::getenv UNIONREC null!\n");
      return (errCodeNullPointer);
  }
  memset(path, 0, sizeof(path));
  strcpy(path, p);
  sprintf(fileName,"%s/fileDir/cipscert/SM.%s%s.cer",path,bankID,version);
  return 0;
}

