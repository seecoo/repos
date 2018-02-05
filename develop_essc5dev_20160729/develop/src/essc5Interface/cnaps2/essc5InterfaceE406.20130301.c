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


const char defaultUserID[] = "1234567812345678"; 


/***************************************
  服务代码:	E406
  服务名:  	E406 导入他行证书
 ***************************************/
 
int Set_sql(char *sql,PUnionX509CerSM2 ptCertInfo,char *banknum,char *fileName,unsigned char 	*certBuf);

int VerifySM2Signature(int lenCaCertDataSign,char *caCertDataSign,char *signatureData,PUnionX509CerSM2 pCertInfo,char *userID,int userIDlen);

int UnionDealServiceCodeE406(PUnionHsmGroupRec phsmGroupRec)
{
		int 			ret;
		char 			bankID[48];
		char			version[3+1];
		char 			signdata[1024];
		char         	sql[2048];
	    char			Pkcs7Str[4096];
		char			x509FileName[1024+1];

		unsigned char 			cert[8192] = {0};
		unsigned char 	certBuf[8192] = {0};
		unsigned char 	buffer[8192] = {0};
		int 			certLen = 0;
		unsigned char	sign[128] = {0};
		int 			signlen = 0;
		unsigned char 	data[8192*2] = {0};
		int 			datalen = 0;
		int 			signType = 0;
		int 			algID = 0;
		int 			hashID = 0;
		TUnionX509CerSM2 tCertInfo, caCertInfo;
		char			path[256]={0};
		char			*p = NULL;
		FILE			*fp = NULL;
		char			fileName[256] = {0};
		char			dataBuf[8192] = {0};
		int 			len = 0;
		int  			lenPkcs7Str;
		int 			lenSignData;
		char			pk[256];
		char			certActiveData[32];
		char			certEndData[32]; 
		char			certID[64];
		char			certFileName[128];
		char 			tempbuffer[1024]={0};
		int 			lenoftempbuffer;
		char 			userID[32];
		int 			userIDlen;

	
	//获取银行行号
	memset(bankID, 0, sizeof(bankID));
	if ((ret = UnionReadRequestXMLPackageValue("body/BankID", bankID, sizeof(bankID))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE406:: UnionReadRequestXMLPackageValue[%s]!\n","body/BankID");
			return(ret);
		}
	bankID[ret] = 0;

	//获取用户ID号
	memset(userID, 0, sizeof( userID));
	if ((ret = UnionReadRequestXMLPackageValue("body/UserID",userID, sizeof(userID))) < 0)
		userIDlen = 0;
	else
	{
		userIDlen=ret;
		userID[ret] = 0;
	}


	//获取版本号
	memset(version, 0, sizeof(version));
	if ((ret = UnionReadRequestXMLPackageValue("body/Version", version, sizeof(version))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE406:: UnionReadRequestXMLPackageValue[%s]!\n","body/Version");
			return(ret);
		}
	version[ret] = 0;

	//获取原始签名数据
	memset(signdata, 0, sizeof(signdata));
	if ((ret = UnionReadRequestXMLPackageValue("body/SignatureData", signdata, sizeof(signdata))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE406:: UnionReadRequestXMLPackageValue[%s]!\n","body/SignatureData");
			return(ret);
		}
	signdata[ret] = 0;
	lenSignData=strlen(signdata);


	//获取 PKCS7(base64格式)
	memset(Pkcs7Str, 0, sizeof(Pkcs7Str));
	if ((ret = UnionReadRequestXMLPackageValue("body/Pkcs7Str", Pkcs7Str, sizeof(Pkcs7Str))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE406:: UnionReadRequestXMLPackageValue[%s]!\n","body/Pkcs7Str");
			return(ret);
		}
	Pkcs7Str[ret] = 0;
	lenPkcs7Str=strlen(Pkcs7Str);
	

	memset(cert,0,sizeof(cert));
	memset(sign,0,sizeof(sign));
	memset(data,0,sizeof(data));

	if ((ret = UnionParasePKcs7SignEx((unsigned char *)Pkcs7Str,lenPkcs7Str, cert, &certLen,sign,&signlen,data,&datalen,&signType,&algID,&hashID)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE406::UnionParasePKcs7SignEx ret = [%d]!\n", ret);
		return(ret);
	}

//	UnionLog("in UnionDealServiceCodeE917::cert[%d][%s]\n ",certLen,cert);
//	UnionLog("in UnionDealServiceCodeE917::sign[%d][%s]\n ",signlen,sign);
//	UnionLog("in UnionDealServiceCodeE917::data[%d][%s]\n ",datalen,data);
//	UnionLog("in UnionDealServiceCodeE917::signType[%d] algID[%d] hashID[%d]\n ",signType,algID,hashID);

		//扩张签名的到可见的签名便于下面的数据验签
		memset(tempbuffer,0,sizeof(tempbuffer));
		bcdhex_to_aschex(sign,strlen(sign),tempbuffer);
		lenoftempbuffer=strlen(tempbuffer);
//		UnionLog("signature=[%s][%d]\n",tempbuffer,lenoftempbuffer);


	if ((p = getenv("UNIONREC")) == NULL)
        {
                UnionUserErrLog("in UnionLoadOtherCer::getenv LocalCer null!\n");
                return (errCodeNullPointer);
        }
        memset(path, 0, sizeof(path));
        strcpy(path, p);
        
	memset(fileName, 0, sizeof(fileName));
	sprintf(fileName, "%s/fileDir/cipscaroot/%s", path, "oca1sm2.cer");


	if ((fp = fopen(fileName,"rb")) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCodeE406::fopen fileName=[%s]!\n", fileName);
		return(-10000);
	}
	
	len=fread(dataBuf,1,8192*2,fp); 
	fclose(fp);

	memset(&tCertInfo,0,sizeof(tCertInfo));
	memset(x509FileName,0,sizeof(x509FileName));
	memset(&caCertInfo,0,sizeof(caCertInfo));

	if ((ret = UnionGetCertificateInfoFromBufEx(dataBuf,len,&caCertInfo)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE406::UnionGetCertificateInfoFromBufEx cert[%d][%s], ret = [%d]!\n", len, dataBuf, ret);
		return(ret);
	}

//	UnionLog("caCertInfo.derPK[%s]\n",caCertInfo.derPK);

#if 1
	if ((ret = UnionVerifyCertificateWithPKEx(caCertInfo.derPK,(char *)cert,certLen)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE406::UnionVerifyCertificateWithPKEx caPK[%s], cert[%d][%s], ret = [%d]!\n",caCertInfo.derPK, len, dataBuf, ret);
		return(ret);
	}
#endif

	if ((ret = UnionGetCertificateInfoFromBufEx((char *)cert,certLen,&tCertInfo)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE406::cert[%d][%s], ret = [%d]!\n",certLen,cert,ret);
		return(ret);
	}

//	UnionLog("tCertInfo.derPK[%s]\n",tCertInfo.derPK);
	//签名数据的验签，通过则可以把证书录入数据库，否则返回退出
	if((ret=VerifySM2Signature(lenoftempbuffer,tempbuffer,signdata,&tCertInfo,userID,userIDlen))<0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE406::VerifySM2Signature:: error!\n");
		return(ret);
	}

	sprintf(x509FileName,"%s/fileDir/cipscert/SM.%s%s.cer",path,bankID,version);

	to64frombits(certBuf, cert, certLen);
	UnionPemStrPreHandle((char *)certBuf,(char *)buffer,"CERTIFICATE",8192);

	if ((fp = fopen(x509FileName, "wb+")) == NULL)
	{
                UnionUserErrLog("in UnionDealServiceCodeE406::fopen[%s] null!\n", x509FileName);
                return (errCodeNullPointer);
	}

	fprintf(fp, "%s", buffer);
	fclose(fp);
	strcpy(pk, tCertInfo.derPK);
	strcpy(certActiveData, tCertInfo.startDateTime);
	strcpy(certEndData, tCertInfo.endDataTime);
	strcpy(certID, tCertInfo.serialNumberAscBuf);
	sprintf(certFileName, "SM.%s%s.cer", bankID,version);
//	UnionLog("pk[%s], certActiveData[%s],certEndData[%s], certID[%s], certFileName[%s]!\n", tCertInfo.derPK, tCertInfo.startDateTime, tCertInfo.endDataTime, tCertInfo.serialNumberAscBuf, certFileName);


	//往数据表中插入相关证书资料
	memset(sql,0,sizeof(sql));	
	len=Set_sql(sql,&tCertInfo,bankID,certFileName,certBuf);

	sql[len] = 0;
	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
			 UnionUserErrLog("in UnionDealServiceCodeE406:: UnionExecRealDBSql[%s]!\n",sql);
			 return(ret);
 	}

	//设置返回报文,返回信息有
	//设置 pk公钥，“04|x|y”
	if ((ret = UnionSetResponseXMLPackageValue("body/PK", pk)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE406: UnionSetResponseXMLPackageValue [%s]!\n", pk);
		return(ret);
	}
	
	//设置 certActiveData:						证书生效日期
	if ((ret = UnionSetResponseXMLPackageValue("body/CertActiveData", certActiveData)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE406: UnionSetResponseXMLPackageValue [%s]!\n", certActiveData);
		return(ret);
	}

	//设置 certEndData：						 证书失效日期
	if ((ret = UnionSetResponseXMLPackageValue("body/CertEndData", certEndData)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE406: UnionSetResponseXMLPackageValue [%s]!\n", certEndData);
		return(ret);
	}

	//设置 certID：									 证书序列号	
	if ((ret = UnionSetResponseXMLPackageValue("body/CertID", certID)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE406: UnionSetResponseXMLPackageValue [%s]!\n", certID);
		return(ret);
	}

	//设置 certFileName: 						证书文件名	
	if ((ret = UnionSetResponseXMLPackageValue("body/CertFileName", certFileName)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE406: UnionSetResponseXMLPackageValue [%s]!\n", certFileName);
		return(ret);
	}


	return 0;
}

int Set_sql(char *sql,PUnionX509CerSM2 ptCertInfo,char *banknum,char *fileName,unsigned char 	*certBuf)
{	
	int len;
	char bankID[32]={0};
	char version[4]={0};
	int certFlag=0;
	int hashID;
	int algID;
	char issuserDN[1024]={0};
	char userDN[1024]={0};	
	char derPK[2048]={0};
	char startDateTime[16]={0};
	char endDataTime[16]={0};
	char serialNumberAscBuf[128]={0};

	

	sprintf(bankID,"%s",banknum);
	sprintf(version,"%03d",ptCertInfo->version);
	sprintf(issuserDN,"%s",ptCertInfo->issuserDN);
	sprintf(userDN,"%s",ptCertInfo->userDN);
	hashID=ptCertInfo->hashID;
	algID=ptCertInfo->algID;
	sprintf(derPK,"%s",ptCertInfo->derPK);
	sprintf(startDateTime,"%s",ptCertInfo->startDateTime);
	sprintf(endDataTime,"%s",ptCertInfo->endDataTime);
	sprintf(serialNumberAscBuf,"%s",ptCertInfo->serialNumberAscBuf);
	
	len=sprintf(sql,"insert into certOfCIPS (bankID,version,certFlag,hashID,algID,issuserDN,userDN,derPK,startDateTime,endDateTime,serialNumberAscBuf,fileName,content) values ('%s','%s',%d,%d,%d,'%s','%s','%s','%s','%s','%s','%s','%s')",bankID,version,certFlag,hashID,algID,issuserDN,userDN,derPK,startDateTime,endDataTime,serialNumberAscBuf,fileName,certBuf);

	return len;
}



int VerifySM2Signature(int lenCaCertDataSign,char *caCertDataSign,char *signatureData,PUnionX509CerSM2 pCertInfo,char *userID,int userIDlen)
{
	int ret;
	int lengRCaPK;
	char rCaPK[2048];
	int lenHashVal;
	char hashVal[2048];


	if (caCertDataSign == NULL || signatureData == NULL || pCertInfo==NULL || userID==NULL) 
	{
		UnionUserErrLog("in VerifySM2Signature::paramters err!\n");
		return(-1);
	}

	memset(rCaPK,0,sizeof(rCaPK));
	strcpy(rCaPK,pCertInfo->derPK+2);
	lengRCaPK=strlen(rCaPK);
//	UnionLog("rCaPK=[%s][%d]\n",rCaPK,lengRCaPK);

	memset(hashVal,0,sizeof(hashVal));
	bcdhex_to_aschex(signatureData,strlen(signatureData),hashVal);
	lenHashVal=strlen(hashVal);
//	UnionLog("data=[%s][%d]\n",hashVal,lenHashVal);


	//验签的指令 userID, data都十六进制数据，不是为原数据
		if(userIDlen == 0)
		{
			ret=UnionHsmCmdK4(-1,"02",strlen(defaultUserID),defaultUserID,lengRCaPK,rCaPK,lenCaCertDataSign,caCertDataSign,lenHashVal,hashVal);
			if(ret < 0)
			{
				 UnionUserErrLog("in UnionDealServiceCodeE406::VerifySM2Signature::UnionHsmCmdK4 error!\n");
				 return(ret);
			}
			else
			{
				UnionLog("in UnionDealServiceCodeE406::VerifySM2Signature::UnionHsmCmdK4 PASS!\n");
				return(ret);
			}
		}
		else
		{
			ret = UnionHsmCmdK4(-1,"02",userIDlen,userID,lengRCaPK,rCaPK,lenCaCertDataSign,caCertDataSign,lenHashVal,hashVal);
			if(ret <0)
			{
				UnionUserErrLog("in UnionDealServiceCodeE406::VerifySM2Signature::UnionHsmCmdK4 error!\n");
				return(ret);
			}
			else
			{
				UnionLog("in UnionDealServiceCodeE406::VerifySM2Signature::UnionHsmCmdK4 PASS!\n");
				return(ret);
			}		
		}

}


