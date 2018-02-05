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
  服务代码:	E407
  服务名:  	E407 导出本行证书
 ***************************************/

int UnionGetMyBankSM2CertFileName(char *fileName,char *bankID,char *version);

int UnionSM2Sign_Cert(int vkIndex, unsigned char *vkByLmk,int vklen, unsigned char *userID,int userIDlen,unsigned char *data,int datalen,unsigned char rout[32],unsigned char sout[32]);


int UnionDealServiceCodeE407(PUnionHsmGroupRec phsmGroupRec)
{
	int ret;
	char char_vkIndex[2+1];
	char 			bankID[48];
	char			version[3+1];
	int vkIndex;
	char userID[32];
	int userIDlen;
	int lenSignatureData;
 	char signatureData[1024] ;
	char pkcs7Str[4096];
	int sizeOfPkcs7;

	unsigned char rout[64] = {0};
	unsigned char sout[64] = {0};
	   
	char	fileName[256];
	char	  dataBuf[8192*4] = {0};
	unsigned char	  signature[128] = {0};
	FILE *fp = NULL;
	int len = 0;
	char 			tempbuffer[1024]={0};
	int 			lenoftempbuffer;
	
	//获取私钥索引号
	memset(char_vkIndex, 0, sizeof(char_vkIndex));
	if ((ret = UnionReadRequestXMLPackageValue("body/VkIndex", char_vkIndex, sizeof(vkIndex))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE407:: UnionReadRequestXMLPackageValue[%s]!\n","body/VkIndex");
			return(ret);
		}
	char_vkIndex[ret] = 0;

	vkIndex=atoi(char_vkIndex);
//	UnionLog("in UnionDealServiceCodeEES2::vkIndex = [%d]\n",vkIndex);

	//获取用户ID号
	memset(userID, 0, sizeof(userID));
	if ((ret = UnionReadRequestXMLPackageValue("body/UserID", userID, sizeof(userID))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE407:: UnionReadRequestXMLPackageValue[%s]!\n","body/Version");
			return(ret);
		}
	userIDlen=ret;
	userID[ret] = 0;

	//获取原始签名数据
	memset(signatureData, 0, sizeof(signatureData));
	if ((ret = UnionReadRequestXMLPackageValue("body/SignatureData", signatureData, sizeof(signatureData))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE407:: UnionReadRequestXMLPackageValue[%s]!\n","body/SignatureData");
			return(ret);
		}
	lenSignatureData=ret;
	signatureData[ret] = 0;

	sizeOfPkcs7=sizeof(pkcs7Str);

	
	//获取银行行号
	memset(bankID, 0, sizeof(bankID));
	if ((ret = UnionReadRequestXMLPackageValue("body/BankID", bankID, sizeof(bankID))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE406:: UnionReadRequestXMLPackageValue[%s]!\n","body/BankID");
			return(ret);
		}
	bankID[ret] = 0;


	//获取版本号
	memset(version, 0, sizeof(version));
	if ((ret = UnionReadRequestXMLPackageValue("body/Version", version, sizeof(version))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE406:: UnionReadRequestXMLPackageValue[%s]!\n","body/Version");
			return(ret);
		}
	version[ret] = 0;
	
#if 1
	UnionLoadLoadOpenSSLAlg();

//	UnionLog("in UnionDealServiceCodeEES2::vkIndex = [%d]\n",vkIndex);

	if ((ret = UnionSM2Sign_Cert(vkIndex, NULL,0, (unsigned char *)userID, userIDlen,(unsigned char *)signatureData,lenSignatureData, rout, sout)) < 0)
	{
		UnionUserErrLog("in UnionGetMyBankSM2Cert::UnionSM2Sign vkIndex=[%d], userID=[%s], signatureData=[%s], lenSignatureData=[%d]!\n", vkIndex, userID, signatureData, lenSignatureData);
		return(ret);
	}
	memcpy(signature, rout, 32);
	memcpy(signature+32, sout, 32);

	UnionLog("in UnionDealServiceCodeE407 ::rout[%s],sout[%s] \n!",rout,sout);

	//扩张签名的到可见的签名便于下面的数据验签
	memset(tempbuffer,0,sizeof(tempbuffer));
	bcdhex_to_aschex(signature,strlen(signature),tempbuffer);
	lenoftempbuffer=strlen(tempbuffer);
	UnionLog("signature=[%s][%d]\n",tempbuffer,lenoftempbuffer);

	  if(UnionGetMyBankSM2CertFileName(fileName,bankID,version) < 0)  
	{
		UnionUserErrLog("in UnionGetMyBankSM2Cert::UnionGetMyBankSM2CertFileName null!\n");
		return (errCodeNullPointer);
	}
		   
	  if ((fp = fopen(fileName,"rb")) == NULL)
	  {
		  UnionUserErrLog("in UnionGetMyBankSM2Cert::fopen fileName=[%s]!\n", fileName);
		  return(-10000);
	  }
	
	  len=fread(dataBuf,1,8192*2,fp); 
	  fclose(fp);
	  
	  if ((ret = UnionFormSM2Pkcs7SignDetach(signature, dataBuf, len, NULL, 0, pkcs7Str, sizeOfPkcs7, 0)) < 0)
	  {
		  UnionUserErrLog("in UnionGetMyBankSM2Cert::UnionFormSM2Pkcs7SignDetach dataBuf=[%s]!",dataBuf);
		  return(ret);
	  }

#endif
	
		//设置绑定报文，base64格式:	
	if ((ret = UnionSetResponseXMLPackageValue("body/Pkcs7Str", pkcs7Str)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE407: UnionSetResponseXMLPackageValue [%s]!\n", pkcs7Str);
		return(ret);
	}
	return 0;
}

int UnionGetMyBankSM2CertFileName(char *fileName,char *bankID,char *version)
{
	char    *p = NULL;
	char    path[256];
	if ((p = getenv("UNIONREC")) == NULL)
  {
      UnionUserErrLog("in UnionGetMyBankCertFileName::getenv LocalCer null!\n");
      return (errCodeNullPointer);
  }
  memset(path, 0, sizeof(path));
  strcpy(path, p);
  sprintf(fileName, "%s/fileDir/cipscert/SM.%s%s.cer",path,bankID,version);
  return 0;
}


//SM2私钥签名 
/*
输入:
	d:私钥
	x: 公钥x
	y: 公钥y
	userID:用户ID
	userIDlen:用户ID长度
	data:待签名数据
	datalen:数据长度
输出:
	rout:签名r
	sout:签名s
*/
int UnionSM2Sign_Cert(int vkIndex, unsigned char *vkByLmk,int vklen, unsigned char *userID,int userIDlen,unsigned char *data,int datalen,unsigned char rout[32],unsigned char sout[32])
{ 
	unsigned char signature[512];
	int ret = 0;
	int lenOftempData;
	char tempdata[1024];
	char chartemp[512];
	
	memset(tempdata,0,sizeof(tempdata));
	bcdhex_to_aschex((char *)data,strlen((char *)data),tempdata);
	lenOftempData=strlen(tempdata);
	memset(signature,0,sizeof(signature));

	UnionLog("vkIndex = [%d]\n",vkIndex);
	if((ret=UnionHsmCmdK3("02",userIDlen,(char *)userID,lenOftempData,tempdata,vkIndex,0,NULL,(char *)signature,sizeof(signature)))<0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE407:: UnionSM2Sign_Cert:: UnionHsmCmdK3 error!\n");
		return(ret);
	}


	aschex_to_bcdhex((char *)signature,strlen((char *)signature),chartemp);

	UnionLog("signature = [%s]\n",signature);
	
	memcpy(rout,chartemp,32);
	memcpy(sout,chartemp+32,32);
	 
	return 0;
}


