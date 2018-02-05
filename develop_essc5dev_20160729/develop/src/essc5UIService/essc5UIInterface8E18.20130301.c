//	Author:		������
//	Copyright:	Union Tech. Guangzhou
//	Date:		2013-01-10

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionRealBaseDB.h"
#include "UnionLog.h"
#include "essc5UIService.h"
#include "UnionStr.h"
#include "unionHsmCmd.h"
#include "remoteKeyPlatform.h"
#include "commWithHsmSvr.h"
#include "unionCertFun.h"
#include "unionVKForm.h"
#include "asymmetricKeyDB.h"
#include "unionHsmCmdVersion.h"
#include "unionREC.h"
#include "unionRSA.h"
#include "base64.h"
#include "unionCertFunSM2.h"

/***************************************
�������:	8E18
������:		����֤��
��������:	����֤��
***************************************/

int switchAndImportVK(char *VK,char *keyByLmk,char *vkByLmk, int vkStoreLocation, char *vkIndex, char *hsmGroupID);
static int setVKKeyIndexUnused(PUnionAsymmetricKeyDB pUnionAsymmetricKeyDB);

int UnionDealServiceCode8E18(PUnionHsmGroupRec phsmGroupRec)
{
	int				ret=0;
	char				filePath[512];
	char				pfxFullName[256];
	char				x509FullName[256];
	char				pfxFileName[64];
	char				passwd[64];
	char				VK[4096];
	char				keyByLmk[64];	
	char				vkByLmk[4096];
	char				cerBuf[4096];
	char				pix[32];
	char				bankID[64];
	char				certVersion[32];
	char				evalue[32];
	char				dateTime[32];
	time_t				begin_t,end_t;
	char				sql[8192*3];
	char				*ptr = NULL;
	char				remark[128];
	char				certBuf[8192];
	char				tmpBuf[9024];
	char				certContent[3][4096];
	//char				issuerInfo[1024];	
	//char				userInfo[1024];	
	FILE				*fp;
	int				len = 0;
	//char				serialNumber[32];

	char				sysFullDate[16];
	char				caBcdDerPK[4096];
	char				caDerPK[4096];
	int				pkExponent = 0;
	char				vkIndex[16];
	
	TUnionX509CerSM2		x509Cer;
	TUnionAsymmetricKeyDB		asymmetricKeyDB;
	TUnionAsymmetricKeyDB		caAsymmetricKeyDB;

	memset(&asymmetricKeyDB,0,sizeof(asymmetricKeyDB));
	memset(&caAsymmetricKeyDB,0,sizeof(caAsymmetricKeyDB));
	memset(&x509Cer,0,sizeof(x509Cer));

	UnionGetFullSystemDate(sysFullDate);
	
	// ��ȡ��Կ�洢λ��
	if ((ret = UnionReadRequestXMLPackageValue("body/vkStoreLocation", tmpBuf, sizeof(tmpBuf))) <= 0)
		asymmetricKeyDB.vkStoreLocation = 0;
	else
	{
		tmpBuf[ret] = 0;
		asymmetricKeyDB.vkStoreLocation = atoi(tmpBuf);
	}

	if(asymmetricKeyDB.vkStoreLocation == 1 || asymmetricKeyDB.vkStoreLocation == 2)
	{
		// ��ȡ�������ID
		if ((ret = UnionReadRequestXMLPackageValue("body/hsmGroupID", asymmetricKeyDB.hsmGroupID, sizeof(asymmetricKeyDB.hsmGroupID))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8TE0:: UnionReadRequestXMLPackageValue[%s]!\n", "body/hsmGroupID");
			return(ret);
		}
		UnionFilterHeadAndTailBlank(asymmetricKeyDB.hsmGroupID);
		if (strlen(asymmetricKeyDB.hsmGroupID) == 0)
		{                                                                                                                 
			UnionUserErrLog("in UnionDealServiceCode8TE0:: hsmGroupID can not be null!\n");                           
			UnionSetResponseRemark("������鲻��Ϊ��");                                              
			return(errCodeParameter);                                                                                 
		}                                                                                                                 
		// ��ȡ˽Կ����                                                                                                   
		if ((ret = UnionReadRequestXMLPackageValue("body/vkIndex", vkIndex, sizeof(vkIndex))) < 0)                        
		{                                                                                                                 
			UnionUserErrLog("in UnionDealServiceCode8TE0:: UnionReadRequestXMLPackageValue[%s]!\n", "body/vkIndex");  
			return(ret);                                                                                              
		}                                                                                                                 
		snprintf(asymmetricKeyDB.vkIndex, sizeof(asymmetricKeyDB.vkIndex), "%02d", atoi(vkIndex));                        
		if(!UnionIsDigitStr(asymmetricKeyDB.vkIndex) || (atoi(asymmetricKeyDB.vkIndex) < 0 || atoi(asymmetricKeyDB.vkIndex) > 20))      
		{                                                                                                                 
			UnionUserErrLog("in UnionDealServiceCode8TE0:: vkIndex[%s] error!\n", asymmetricKeyDB.vkIndex);           
			UnionSetResponseRemark("�Ƿ���˽Կ����[%s],������0��20֮��", asymmetricKeyDB.vkIndex);   
			return(errCodeParameter);                                                                                 
		}                                                                                                                 
	}
	
	// ��ȡ�ļ�·��
	memset(filePath,0,sizeof(filePath));
	if ((ptr = UnionReadStringTypeRECVar("defaultDirOfFileReceiver")) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCode8E18:: UnionReadStringTypeRECVar[%s]!\n","defaultDirOfFileReceiver");
		return(errCodeRECMDL_VarNotExists);
	}

	UnionReadDirFromStr(ptr,-1,filePath);
	
	// ֤����
	memset(pfxFileName,0,sizeof(pfxFileName));
	if ((ret = UnionReadRequestXMLPackageValue("body/pfxFileName",pfxFileName,sizeof(pfxFileName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E18:: UnionReadRequestXMLPackageValue[%s]!\n","body/pfxFileName");
		UnionSetResponseRemark("��ȡ֤����ʧ��");
		return(ret);
	}

	UnionFilterHeadAndTailBlank(pfxFileName);
	if (strlen(pfxFileName) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E18:: pfxFileName is null!\n");
		UnionSetResponseRemark("֤��������Ϊ��!");
		return(errCodeParameter);
	}

	// Ӧ��ID
	memset(pix,0,sizeof(pix));
	if ((ret = UnionReadRequestXMLPackageValue("body/pix",pix,sizeof(pix))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E18:: UnionReadRequestXMLPackageValue[%s]!\n","body/pix");
		UnionSetResponseRemark("��ȡӦ��IDʧ��");
		return(ret);
	}
	else
	{
		UnionFilterHeadAndTailBlank(pix);
		if (strlen(pix) == 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E18:: pix can not be null!\n");
			UnionSetResponseRemark("Ӧ��ID����Ϊ��!");
			return(errCodeParameter);
		}
		if (!UnionIsBCDStr(pix))
		{
			UnionUserErrLog("in UnionDealServiceCode8E18:: pix[%s] is error!\n",pix);
			UnionSetResponseRemark("Ӧ��ID[%s]�Ƿ�,����Ϊʮ��������",pix);
			return(errCodeParameter);
		}
	}
	
	// �����л�����
	memset(bankID,0,sizeof(bankID));
	if ((ret = UnionReadRequestXMLPackageValue("body/bankID",bankID,sizeof(bankID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E18:: UnionReadRequestXMLPackageValue[%s]!\n","body/bankID");
		UnionSetResponseRemark("��ȡ�����л�����ʧ��");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(bankID);

	// ֤��汾��
	memset(certVersion,0,sizeof(certVersion));
	if ((ret = UnionReadRequestXMLPackageValue("body/certVersion",certVersion,sizeof(certVersion))) < 0)
	{
		//UnionUserErrLog("in UnionDealServiceCode8E18:: UnionReadRequestXMLPackageValue[%s]!\n","body/certVersion");
		//return(ret);
		strcpy(certVersion,"01");
	}
	UnionFilterHeadAndTailBlank(certVersion);

	// ֤������
	memset(passwd,0,sizeof(passwd));
	if ((ret = UnionReadRequestXMLPackageValue("body/password",passwd,sizeof(passwd))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E18:: UnionReadRequestXMLPackageValue[%s]!\n","body/bankVK");
		UnionSetResponseRemark("��ȡ֤������ʧ��");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(passwd);

	memset(remark,0,sizeof(remark));
	if ((ret = UnionReadRequestXMLPackageValue("body/remark",remark,sizeof(remark))) < 0)
	{
		//UnionUserErrLog("in UnionDealServiceCode8E18:: UnionReadRequestXMLPackageValue[%s]!\n","body/remark");
		//return(ret);
	}

	// ƴ����Կ����
	sprintf(asymmetricKeyDB.keyName,"CNAPS2.%s-%s-%s.cer",pix,bankID,certVersion);

	memset(pfxFullName,0,sizeof(pfxFullName));
	sprintf(pfxFullName,"%s/%s",filePath,pfxFileName);
	
	// ���֤���Ƿ����
	if ((access(pfxFullName,0) < 0))
	{
		UnionUserErrLog("in UnionDealServiceCode0306:: fileName[%s] not found!\n",pfxFullName);
		return(errCodeMngSvrFileNotExists);
	}

	memset(x509FullName,0,sizeof(x509FullName));
	sprintf(x509FullName,"%s/%s.x509",filePath,asymmetricKeyDB.keyName);

	// ��ȡ˽Կ�͹�Կ֤��
	memset(VK,0,sizeof(VK));
	memset(cerBuf,0,sizeof(cerBuf));
	if((ret=UnionGetVKAndCertFromPfx(pfxFullName,passwd,VK,x509FullName,NULL,cerBuf,NULL))<0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E18:: unionGetVKAndCerFromPfX!\n");
		UnionSetResponseRemark("֤�����ʧ��,����֤�������");
		return(ret);
	}
	
	// ����һ��zek��Կ
	memset(keyByLmk,0,sizeof(keyByLmk));
	switch(phsmGroupRec->hsmCmdVersionID)
	{
		case	conHsmCmdVerRacalStandardHsmCmd:
		case	conHsmCmdVerSJL06StandardHsmCmd:
			if((ret = UnionHsmCmdTA(0, 32, "00A", NULL, keyByLmk)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode8E18:: UnionHsmCmdTA!\n");
				return(ret);
			}
			break;
		default:
			UnionUserErrLog("in UnionDealServiceCode8E18:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
			UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
			return(errCodeParameter);
	}

	// ˽Կת��
	memset(vkByLmk,0,sizeof(vkByLmk));
	if ((ret = switchAndImportVK(VK, keyByLmk, vkByLmk, asymmetricKeyDB.vkStoreLocation, asymmetricKeyDB.vkIndex, asymmetricKeyDB.hsmGroupID)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E18:: witchAndImportVK error! ret = [%d] !\n", ret);
		return(ret);
	}

	// ˽Կ
	if(asymmetricKeyDB.vkStoreLocation != 1)
		strcpy(asymmetricKeyDB.vkValue,vkByLmk);

	// ��ȡ֤����Ϣ
	if ((ret = UnionGetCertificateInfoFromBufEx(cerBuf,strlen(cerBuf),&x509Cer)) < 0)
	{
		setVKKeyIndexUnused(&asymmetricKeyDB);
		UnionUserErrLog("in UnionDealServiceCode8E18:: UnionGetCertificateInfoFromBuf[%s]!\n",cerBuf);
		return(ret);
	}

	// �ж�֤����Ч��
	if(memcmp(sysFullDate, x509Cer.startDateTime, 8) < 0 || memcmp(sysFullDate, x509Cer.endDataTime, 8) > 0)
	{
		setVKKeyIndexUnused(&asymmetricKeyDB);
		UnionUserErrLog("in UnionDealServiceCode8E1F:: certificate expired!\n");
		return(errCodeOffsetOfCertMDL_OutOfDate);
	}
	
	// ��֤�鹫Կ����֤����Ч��
	len = snprintf(sql, sizeof(sql), "select keyName from certOfCnaps2 where hashID != 2 and cerFlag = 3 and userInfo = '%s'", x509Cer.issuserDN);
	if((ret = UnionSelectRealDBRecord(sql, 0, 0)) < 0)
	{
		setVKKeyIndexUnused(&asymmetricKeyDB);
		UnionUserErrLog("in UnionDealServiceCode8E18:: UnionSelectRealDBRecord[%s]!\n",sql);
		return(ret);
	}
	else if(ret == 0)
	{
		setVKKeyIndexUnused(&asymmetricKeyDB);
		UnionUserErrLog("in UnionDealServiceCode8E18:: rootCert[%s] record not found!\n", x509Cer.issuserDN);
		return(errCodeOffsetOfCertMDL_CaNotExist);
	}
	UnionLocateXMLPackage("detail", 1);
	if((ret = UnionReadXMLPackageValue("keyName", caAsymmetricKeyDB.keyName, sizeof(caAsymmetricKeyDB.keyName))) < 0)
	{
		setVKKeyIndexUnused(&asymmetricKeyDB);
		UnionUserErrLog("in UnionDealServiceCode8E18:: UnionReadRequestXMLPackageValue[%s]!\n", "keyName");
		return(ret);
	}
	
	// ��ȡ��֤�鹫Կ
	if((ret = UnionReadAsymmetricKeyDBRec(caAsymmetricKeyDB.keyName, 0, &caAsymmetricKeyDB)) < 0)
	{
		setVKKeyIndexUnused(&asymmetricKeyDB);
		UnionUserErrLog("in UnionDealServiceCode8E18:: UnionReadAsymmetricKeyDBRec[%s]!\n", caAsymmetricKeyDB.keyName);
		return(ret);
	}
	
	// ����֤����㹫Կ�����DER��ʽ
	// ��ȡ��Կָ��
	pkExponent = UnionGetCurrentRsaExponent();
	// ���ù�Կָ��
	if(pkExponent != caAsymmetricKeyDB.pkExponent);
		UnionSetCurrentRsaExponent(caAsymmetricKeyDB.pkExponent);
	if((ret = UnionFormANSIDERRSAPK(caAsymmetricKeyDB.pkValue, strlen(caAsymmetricKeyDB.pkValue), caBcdDerPK, sizeof(caBcdDerPK))) < 0)
	{
		setVKKeyIndexUnused(&asymmetricKeyDB);
		UnionUserErrLog("in UnionDealServiceCode8E18:: UnionFormANSIDERRSAPK!\n");
		UnionSetResponseRemark("�㹫Կת��DER��ʽʧ��");
		return(ret);
	}
	bcdhex_to_aschex(caBcdDerPK, ret, caDerPK);
	caDerPK[ret * 2] = 0;
	
	//��ԭ��Կָ��
	if(pkExponent != caAsymmetricKeyDB.pkExponent)
		UnionSetCurrentRsaExponent(pkExponent);
	
	// ��֤��Ĺ�Կ��֤֤��
	if((ret = UnionVerifyCertificateWithPKEx(caDerPK, (char*)cerBuf, strlen(cerBuf))) < 0)
	{
		setVKKeyIndexUnused(&asymmetricKeyDB);
		UnionUserErrLog("in UnionDealServiceCode8E18:: UnionVerifyCertificateWithPKEx caPK[%s], cert[%d][%s], ret = [%d]!\n", caDerPK, (int)strlen(cerBuf), cerBuf, ret);
		return(ret);
	}
	
	//��DER��ʽת�����㹫Կ
	if ((ret = UnionPKCSCerASN1DER_Decode(x509Cer.derPK,asymmetricKeyDB.pkValue,evalue)) < 0)
	{
		setVKKeyIndexUnused(&asymmetricKeyDB);
		UnionUserErrLog("in UnionDealServiceCode8E18:: UnionPKCSCerASN1DER_Decode[%s]!\n",x509Cer.derPK);
		return(ret);
	}
	// ��Կ��
	strcpy(asymmetricKeyDB.keyGroup,"default");

	// �㷨��ʶ
	asymmetricKeyDB.algorithmID = conAsymmetricAlgorithmIDOfRSA; 
	
	// ��Կ����
	asymmetricKeyDB.keyType = 0;

	// ����ʹ�þ���Կ
	asymmetricKeyDB.oldVersionKeyIsUsed = 1;

	// ����������
	asymmetricKeyDB.creatorType = conAsymmetricCreatorTypeOfUser;

	// ������
	if ((ret = UnionReadRequestXMLPackageValue("head/userID",asymmetricKeyDB.creator,sizeof(asymmetricKeyDB.creator))) <= 0)
	{
		setVKKeyIndexUnused(&asymmetricKeyDB);
		UnionUserErrLog("in UnionDealServiceCode8E18:: UnionReadRequestXMLPackageValue[%s]!\n","head/userID");
		return(ret);	
	}

	// ��Կ����
	asymmetricKeyDB.keyLen = strlen(asymmetricKeyDB.pkValue) * 4;

	// ��Կָ��
	if (strcmp(evalue,"010001") == 0)
		asymmetricKeyDB.pkExponent = 65537;

	if (atoi(evalue) == 3)
		asymmetricKeyDB.pkExponent = 3;

	// ��Կ״̬
	asymmetricKeyDB.status = 1;

	// ֤����ʼʱ��
	memcpy(asymmetricKeyDB.activeDate,x509Cer.startDateTime,8);

	memset(dateTime,0,sizeof(dateTime));
	sprintf(dateTime,"%s000000",asymmetricKeyDB.activeDate);

	if ((begin_t = UnionTranslateStringTimeToTime(dateTime)) <= 0)
	{
		setVKKeyIndexUnused(&asymmetricKeyDB);
		UnionUserErrLog("in UnionDealServiceCode8E18:: UnionTranslateStringTimeToTime dateTime[%s] length != 14!\n",dateTime);
		return(errCodeOffsetOfCertificate_InvalidDate);
	}

	// ֤�����ʱ��
	memset(dateTime,0,sizeof(dateTime));
	memcpy(dateTime,x509Cer.endDataTime,8);
	sprintf(dateTime,"%s000000",dateTime);

	if ((end_t = UnionTranslateStringTimeToTime(dateTime)) <= 0)
	{
		setVKKeyIndexUnused(&asymmetricKeyDB);
		UnionUserErrLog("in UnionDealServiceCode8E18:: UnionTranslateStringTimeToTime dateTime[%s] length != 14!\n",dateTime);
		return(errCodeOffsetOfCertificate_InvalidDate);
	}

	// ��Ч����
	asymmetricKeyDB.effectiveDays = (end_t - begin_t)/60/60/24;

	// ������Կ
	if ((ret = UnionGenerateAsymmetricKeyDBRec(&asymmetricKeyDB)) < 0)
	{
		setVKKeyIndexUnused(&asymmetricKeyDB);
		UnionUserErrLog("in UnionDealServiceCode8E18:: UnionGenerateAsymmetricKeyDBRec[%s]!\n",asymmetricKeyDB.keyName);
		return(ret);
	}

	if ((fp = fopen(pfxFullName,"rb")) == NULL)
	{
		setVKKeyIndexUnused(&asymmetricKeyDB);
		UnionDropAsymmetricKeyDB(&asymmetricKeyDB);
		UnionSystemErrLog("in UnionDealServiceCode8E18:: fopen [%s]\n", pfxFullName);
		return(errCodeUseOSErrCode);
	}

	if ((ret = fread(certBuf,1,sizeof(certBuf),fp)) < 0)
	{
		setVKKeyIndexUnused(&asymmetricKeyDB);
		UnionDropAsymmetricKeyDB(&asymmetricKeyDB);
		UnionSystemErrLog("in UnionDealServiceCode8E18:: fread error  ret = %d\n",ret);
		return(errCodeUseOSErrCode);
	}
	fclose(fp);

	certBuf[ret] = 0;
	to64frombits((unsigned char*)tmpBuf,(unsigned char *)certBuf,ret);

	len = strlen(tmpBuf);
	tmpBuf[len] = 0;

	if (len > 12000)
	{
		setVKKeyIndexUnused(&asymmetricKeyDB);
		UnionDropAsymmetricKeyDB(&asymmetricKeyDB);
		UnionSystemErrLog("in UnionDealServiceCode8E18:: after to 64 form bits fileSize[%d] > 8000",len);
		UnionSetResponseRemark("�ļ�[%s]̫��ת�����޷��浽���ݿ���",pfxFileName);
		return(errCodeSmallBuffer);
	}

	if (len <= 4000)
	{
		len = sprintf(certContent[0],"%s",tmpBuf);
		certContent[0][len] = 0;
		certContent[1][0] = 0;
		certContent[2][0] = 0;
	}
	else if (len <= 8000)
	{
		memcpy(certContent[0],tmpBuf,4000);
		certContent[0][4000] = 0;
		len = sprintf(certContent[1],"%s",&tmpBuf[4000]);
		certContent[1][len] = 0;
		certContent[2][0] = 0;
	}
	else
	{
		memcpy(certContent[0],tmpBuf,4000);
		memcpy(certContent[1],tmpBuf+4000,4000);
		certContent[0][4000] = 0;
		certContent[1][4000] = 0;
		len = sprintf(certContent[2],"%s",&tmpBuf[8000]);
		certContent[2][len] = 0;
	}
	
	/*
	len = sprintf(issuerInfo,"������[%s]  ʡ��[%s]  ������[%s]  ��λ��[%s]  ����[%s]  �û���[%s]  EMail��ַ[%s]  ����[%s]",x509Cer.issuerInfo.countryName,x509Cer.issuerInfo.stateOrProvinceName,x509Cer.issuerInfo.localityName,x509Cer.issuerInfo.organizationName,x509Cer.issuerInfo.organizationalUnitName,x509Cer.issuerInfo.commonName,x509Cer.issuerInfo.email,x509Cer.issuerInfo.alias);
	issuerInfo[len] = 0;

	len = sprintf(userInfo,"������[%s]  ʡ��[%s]  ������[%s]  ��λ��[%s]  ����[%s]  �û���[%s]  EMail��ַ[%s]  ����[%s]",x509Cer.userInfo.countryName,x509Cer.userInfo.stateOrProvinceName,x509Cer.userInfo.localityName,x509Cer.userInfo.organizationName,x509Cer.userInfo.organizationalUnitName,x509Cer.userInfo.commonName,x509Cer.userInfo.email,x509Cer.userInfo.alias);
	userInfo[len] = 0;
	
	len = sprintf(serialNumber,"%ld",x509Cer.serialNumber);
	serialNumber[len] = 0;
	*/
	
	// ����֤��
	len = sprintf(sql,"insert into certOfCnaps2 (keyName,cerFlag,serialNumber,version,hashID,startDateTime,endDataTime,issuerInfo,userInfo,bankCert,pfxFileName,certContent1,certContent2,certContent3,remark) "
			" values ('%s',0,'%s',%d,%d,'%s','%s','%s','%s','%s','%s','%s','%s','%s','%s')"
			,asymmetricKeyDB.keyName,x509Cer.serialNumberAscBuf,x509Cer.version,x509Cer.hashID,x509Cer.startDateTime,x509Cer.endDataTime,x509Cer.issuserDN,x509Cer.userDN,cerBuf,pfxFileName,certContent[0],certContent[1],certContent[2],remark);
	sql[len] = 0;
	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		setVKKeyIndexUnused(&asymmetricKeyDB);
		UnionDropAsymmetricKeyDB(&asymmetricKeyDB);
		UnionUserErrLog("in UnionDealServiceCode8E18:: UnionExecRealDBSql[%s]!\n",sql);
		return(ret);
	}

	// ɾ�����ɵ��ļ�
	if ((ret = unlink(x509FullName)) < 0)
	{
		setVKKeyIndexUnused(&asymmetricKeyDB);
		UnionDropAsymmetricKeyDB(&asymmetricKeyDB);
		UnionUserErrLog("in UnionDealServiceCode0403:: delete fileName[%s] is failed!\n",x509FullName);
		//return(ret);
	}

	return 0;
}

int switchAndImportVK(char *VK,char *keyByLmk,char *vkByLmk, int vkStoreLocation, char *vkIndex, char *hsmGroupID)
{
        char    kekLen;
        int     ret = 0;

        char	modulues[2048];
	char	publicExponent[48];
        char	privateExponent[2048];
	char	primeP[1024];
        char	primeQ[1024];
	char	dmp1[1024];
        char	dmq1[1024];
	char	coef[1024];

        char	enmodulues[2048];
	char	enpublicExponent[48];
        char	enprivateExponent[2048];
	char	enprimeP[1024];
        char	enprimeQ[1024];
	char	endmp1[1024];
        char	endmq1[1024];
	char	encoef[1024];

	int	vkByLmklen=0;
        int	mlen,pclen,ptlen,pPlen,pQlen,dpl,dql,cfl;
        int	n;
        char	IV[32] ="0000000000000000";
	char	keyType[32];
	char	vkByLmkBCD[2048];	
	char	mode[32];
	char	sql[512];
	int	hsmNum = 0;
	int	i = 0;
	char	ipAddr[32];

        //����Der��ʽ��VK����ֵ
        memset(modulues,0,sizeof(modulues));
        memset(publicExponent,0,sizeof(publicExponent));
        memset(privateExponent,0,sizeof(privateExponent));
        memset(primeP,0,sizeof(primeP));
        memset(primeQ,0,sizeof(primeQ));
        memset(dmp1,0,sizeof(dmp1));
        memset(dmq1,0,sizeof(dmq1));
        memset(coef,0,sizeof(coef));
        memset(enmodulues,0,sizeof(enmodulues));
        memset(enpublicExponent,0,sizeof(enpublicExponent));
        memset(enprivateExponent,0,sizeof(enprivateExponent));
        memset(enprimeP,0,sizeof(enprimeP));
        memset(enprimeQ,0,sizeof(enprimeQ));
        memset(endmp1,0,sizeof(endmp1));
	memset(endmq1,0,sizeof(endmq1));
	memset(encoef,0,sizeof(encoef));
	//memset(vkByLmkBCD,0,sizeof(vkByLmkBCD));

	if ((ret = UnionUnPackFromVK(VK, modulues, publicExponent, privateExponent, primeP, primeQ, dmp1, dmq1, coef)) < 0)
	{
		UnionUserErrLog("in switchAndImportVK:: UnionUnPackFromVK is error!\n");
		return (ret);
	}
	UnionLog("in switchAndImportVK:: publicExponent[%s][%s]\n",publicExponent,modulues);

	n = (strlen(modulues) * 8) / 2 ;

	//��������RSA����

	kekLen = '1';

	// ��Կ����
	memset(keyType,0,sizeof(keyType));
	strcpy(keyType,"00A");

	// �ӽ���ģʽ
	memset(mode,0,sizeof(mode));
	strcpy(mode,"01");

	// ���ܹ�Կģ
	if ((ret = UnionHsmCmdTG('2',mode,keyType,kekLen,keyByLmk,IV,strlen(modulues),modulues,&mlen,enmodulues)) < 0)
        {
                UnionUserErrLog("in switchAndImportVK:: RacalCmdTG modulues error !\n");
                return(ret);  
        }

        // ���ܹ�Կָ��
        if ((ret = UnionHsmCmdTG('2',mode,keyType,kekLen,keyByLmk,IV,strlen(publicExponent),publicExponent,&pclen,enpublicExponent)) < 0)
        {
                UnionUserErrLog("in switchAndImportVK:: RacalCmdTG publicExponent error !\n");
                return(ret);  
        }

        // ����˽Կ��ָ��
        if ((ret = UnionHsmCmdTG('2',mode,keyType,kekLen,keyByLmk,IV,strlen(privateExponent),privateExponent,&ptlen,enprivateExponent)) < 0)
        {
                UnionUserErrLog("in switchAndImportVK:: RacalCmdTG privateExponent error!\n");
                return(ret);  
        }

	// ����˽Կ��P
        if ((ret = UnionHsmCmdTG('2',mode,keyType,kekLen,keyByLmk,IV,strlen(primeP),primeP,&pPlen,enprimeP)) < 0)
        {
                UnionUserErrLog("in switchAndImportVK:: RacalCmdTG enprimeP error !\n");
                return(ret);  
        }

	// ����˽Կ��Q
        if ((ret = UnionHsmCmdTG('2',mode,keyType,kekLen,keyByLmk,IV,strlen(primeQ),primeQ,&pQlen,enprimeQ)) < 0)
        {
                UnionUserErrLog("in switchAndImportVK:: RacalCmdTG enprimeQ error !\n");
                return(ret);  
        }

	// ����˽Կ��dmp1
        if ((ret  = UnionHsmCmdTG('2',mode,keyType,kekLen,keyByLmk,IV,strlen(dmp1),dmp1,&dpl,endmp1)) < 0)
        {
                UnionUserErrLog("in switchAndImportVK:: RacalCmdTG endmp1 error !\n");
                return(ret);  
	} 

	// ����˽Կ��dmq1
        if ((ret = UnionHsmCmdTG('2',mode,keyType,kekLen,keyByLmk,IV,strlen(dmq1),dmq1,&dql,endmq1)) < 0)
        {
                UnionUserErrLog("in switchAndImportVK:: RacalCmdTG endmq1 error !\n");
                return(ret);  
        }

	// ����˽Կ��coef
        if ((ret = UnionHsmCmdTG('2',mode,keyType,kekLen,keyByLmk,IV,strlen(coef),coef,&cfl,encoef)) < 0)
        {
                UnionUserErrLog("in switchAndImportVK:: RacalCmdTG encoef error !\n");
                return(ret);  
        }

        if ((ret = UnionHsmCmdUI("00", IV, strlen(keyByLmk), keyByLmk, keyType, n, mlen, enmodulues, pclen, \
			enpublicExponent, ptlen, enprivateExponent, pPlen, enprimeP, pQlen, enprimeQ, dpl, endmp1, dql, \
			endmq1, cfl, encoef, (unsigned char*)vkByLmk, &vkByLmklen)) < 0)
        {
                UnionUserErrLog("in switchAndImportVK:: UnionHsmCmdUI is error!\n");
                return (ret);
        }

	// ������ܻ�
	if(vkStoreLocation)
	{
		snprintf(sql, sizeof(sql), "select * from hsm where hsmGroupID = '%s' and enabled = 1", hsmGroupID);
		if ((hsmNum = UnionSelectRealDBRecord(sql, 0, 0)) < 0)
		{
			UnionUserErrLog("in switchAndImportVK:: UnionSelectRealDBRecord[%s]!\n", sql);
			return(hsmNum);
		}
		else if (hsmNum == 0)
		{
			UnionUserErrLog("in switchAndImportVK:: hsmGroupID[%s]��û�п��������!\n", hsmGroupID);
			UnionSetResponseRemark("hsmGroupID[%s]��û�п��������", hsmGroupID);
			return(errCodeParameter);
		}
		ret = aschex_to_bcdhex(vkByLmk,vkByLmklen,vkByLmkBCD);	
		vkByLmkBCD[ret] = 0;
		
		for(i = 0; i < hsmNum; i++)
		{
			UnionLocateXMLPackage("detail", i+1);
			if((ret = UnionReadXMLPackageValue("ipAddr", ipAddr, sizeof(ipAddr))) < 0)
			{
				UnionUserErrLog("in switchAndImportVK:: UnionReadStringTypeRECVar[%s]!\n", "ipAddr");
				return(ret);
			}
			UnionSetUseSpecHsmIPAddrForOneCmd(ipAddr);
			if((ret = UnionHsmCmdEK(atoi(vkIndex),vkByLmklen/2,(unsigned char*)vkByLmkBCD)) < 0)
	        	{
	        	        UnionUserErrLog("in switchAndImportVK:: RacalCmdTY  vkindex = [%s] \n",vkIndex);
	        	        return(ret);
	        	}
		}
		
		// ����״̬��Ϊ������
		snprintf(sql, sizeof(sql), "update vkKeyIndex set status = 1 where hsmGroupID = '%s' and vkIndex = '%s' and algorithmID = 0", hsmGroupID, vkIndex);
		if((ret = UnionExecRealDBSql(sql)) <= 0)
		{
			UnionUserErrLog("in switchAndImportVK:: UnionExecRealDBSql[%s]!\n", sql);
			UnionSetResponseRemark("����˽Կ����״̬ʧ��");
			return(ret);
		}
	}	
	return (0);
}

static int setVKKeyIndexUnused(PUnionAsymmetricKeyDB pUnionAsymmetricKeyDB)
{
	int             ret = 0;
	char            updateVkStatusSql[512];

	if(!pUnionAsymmetricKeyDB->vkStoreLocation)
	        return 0;
	
	snprintf(updateVkStatusSql, sizeof(updateVkStatusSql), "update vkKeyIndex set status = 0 where hsmGroupID = '%s' and vkIndex = '%s' and algorithmID = 0", pUnionAsymmetricKeyDB->hsmGroupID, pUnionAsymmetricKeyDB->vkIndex);

	if ((ret = UnionExecRealDBSql(updateVkStatusSql)) <= 0)
	{
	        UnionUserErrLog("in updateVkKeyIndexStatus:: UnionExecRealDBSql[%s]!\n", updateVkStatusSql);
	        UnionSetResponseRemark("����˽Կ����״̬ʧ��");
	        return(errCodeParameter);
	}
	
	return 0;
}
