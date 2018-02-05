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
  服务代码:	E408
  服务名:  	E408  签名
 ***************************************/

const char defaultUserID[] = "1234567812345678"; 


int UnionDealServiceCodeE408(PUnionHsmGroupRec phsmGroupRec)
{  
	int ret;
	char char_vkIndex[2+1];
	int vkIndex;
	char userID[32];
	int userIDlen;
	char signatureData[2048];
	int lenSignatureData;
	char base64Signature[4096+1];
	int lenOfData;
	char data[1024];
//	char tempdata[4096];
	char	signature[512] = {0};
	char	tempofsignature[512] = {0};
	char	signatureDer[256] = {0};
	int signatureDerLen = 0;
//	int	len = 0;
//	char	dataBuf[8192*2+32];

	
	//获取私钥索引号
	memset(char_vkIndex, 0, sizeof(char_vkIndex));
	if ((ret = UnionReadRequestXMLPackageValue("body/VkIndex", char_vkIndex, sizeof(vkIndex))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE408:: UnionReadRequestXMLPackageValue[%s]!\n","body/VkIndex");
			return(ret);
		}
	char_vkIndex[ret] = 0;
	vkIndex=atoi(char_vkIndex);

	//获取用户ID号
	memset(userID, 0, sizeof(userID));
	if ((ret = UnionReadRequestXMLPackageValue("body/UserID",userID, sizeof(userID))) < 0)
		userIDlen = 0;
	else
	{
		userIDlen=ret;
		userID[ret] = 0;
	}


	//获取原始签名数据
	memset(signatureData, 0, sizeof(signatureData));
	if ((ret = UnionReadRequestXMLPackageValue("body/SignatureData", signatureData, sizeof(signatureData))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE408:: UnionReadRequestXMLPackageValue[%s]!\n","body/SignatureData");
			return(ret);
		}
	lenSignatureData=ret;
	signatureData[ret] = 0;
//	UnionLog("in UnionDealServiceCodeE408 :: signatureData[%s][%d]\n",signatureData,lenSignatureData);

 		memset(data,0,sizeof(data));
		bcdhex_to_aschex(signatureData,strlen(signatureData),data);
		lenOfData=strlen(data);
 

		if(userIDlen == 0)
		{ 
			if((ret=UnionHsmCmdK3("02",strlen(defaultUserID),defaultUserID,lenOfData,data,vkIndex,0,NULL,tempofsignature,sizeof(tempofsignature)))<0)
			{
			 	UnionUserErrLog("in UnionDealServiceCodeE408:: UnionHsmCmdK3 error!\n");
             	return(ret);
			}
		}
		else
		{ 
			if((ret=UnionHsmCmdK3("02",userIDlen, userID,lenOfData,data,vkIndex,0,NULL,tempofsignature,sizeof(tempofsignature)))<0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE408:: UnionHsmCmdK3 error!\n");
             	return(ret);
			}
		}
 		
		memset(signature,0,sizeof(signature));
		aschex_to_bcdhex(tempofsignature,strlen(tempofsignature),signature);
 
 		if((ret = UnionRSToSM2SigDer((unsigned char *)signature,(unsigned char*)signatureDer,&signatureDerLen)) < 0)
		{
			UnionUserErrLog("in UnionGenerateSM2Signature::UnionRSToSM2SigDer failed! ret = [%d]!\n", ret);
			return(ret);
		}
		
		to64frombits(base64Signature,signatureDer,signatureDerLen);
 
		//设置签名(base64格式):	
	if ((ret = UnionSetResponseXMLPackageValue("body/Base64Signature", base64Signature)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE408: UnionSetResponseXMLPackageValue [%s]!\n", base64Signature);
		return(ret);
	}

	return 0;
}

