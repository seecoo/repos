//      Author:         zhouxw
//      Copyright:      Union Tech. Guangzhou
//      Date:           20150713

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include "essc5UIService.h"
#include "UnionLog.h"
#include "unionXMLPackage.h"
#include "unionErrCode.h"
#include "unionRealBaseDB.h"
#include "UnionStr.h"
#include "unionHsmCmd.h"
#include "unionHsmCmdVersion.h"
#include "asymmetricKeyDB.h"
#include "unionCertFunSM2.h"
#include "unionREC.h"

/***************************************
  �������:     8E30
  ������:       ����SM2֤�������ļ�
  ��������:     ����SM2֤�������ļ�
 ***************************************/
static int setVKKeyIndexUnused(PUnionAsymmetricKeyDB pUnionAsymmetricKeyDB);
int UnionGenerateSM2Key(PUnionAsymmetricKeyDB pUnionAsymmetricKeyDB);

int UnionDealServiceCode8E1E(PUnionHsmGroupRec phsmGroupRec)
{
	int		ret;
	//char		userInfo[1024];
	int		hashID;
	char		pk[512];
	int		format;
	char		remark[128];
	char		fileName[128];
	char		fileFullName[512];
	char		path[512];
	char		tmpBuf[128];
	int		len;
	char		sql[2048];
	char		sign[2048];
	char		certDN[1024];
	char		fieldSeperate;
	char		tmpVarStr[32][128];
	int		varNum;
	int		i;
	int		isFstOU = 0;
	int		isFstO = 0;
	//int		vkIndex = 0;
	char		vkValue[1024];	
	int		vkLen = 0;
	
	char		bankID[64];
	char		pix[32];
	char		certVersion[32];
	char		*ptr = NULL;
	char		reqContent[8192*2];
	FILE		*fp = NULL;
	char		filePath[128];

	char		vkIndex[16];
	int		intVkIndex = 0;
	
	TUnionX509UserInfoSM2		reqUserInfo;
	TUnionAsymmetricKeyDB		asymmetricKeyDB;
	
	memset(&reqUserInfo, 0, sizeof(reqUserInfo));
	memset(&asymmetricKeyDB, 0, sizeof(asymmetricKeyDB));

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
	// ��ȡ������
	if((ret = UnionReadRequestXMLPackageValue("body/bankID", bankID, sizeof(bankID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1E:: UnionReadRequestXMLPackageValue[%s]!\n", "body/bankID");
		return(ret);
	}
	else if(ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1E:: UnionReadRequestXMLPackageValue[%s]!\n", "body/bankID");
		UnionSetResponseRemark("�����Ų���Ϊ��");
		return(errCodeParameter);
	}
	else
		bankID[ret] = 0;
	
	// ��ȡӦ��ID
	if((ret = UnionReadRequestXMLPackageValue("body/pix", pix, sizeof(pix))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1E:: UnionReadRequestXMLPackageValue[%s]!\n", "body/pix");
		return(ret);
	}
	else if(ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1E:: UnionReadRequestXMLPackageValue[%s]!\n", "body/pix");
		UnionSetResponseRemark("Ӧ��ID����Ϊ��");
		return(errCodeParameter);
	}
	else
		pix[ret] = 0;
	
	// ��ȡ�汾��
	if((ret = UnionReadRequestXMLPackageValue("body/certVersion", certVersion, sizeof(certVersion))) <= 0)
	{
		memcpy(certVersion, "01", 2);
	}
	else
		certVersion[ret] = 0;
	
	snprintf(asymmetricKeyDB.keyName, sizeof(asymmetricKeyDB.keyName), "CNAPS2.%s-%s-%s.cer_sm2", pix, bankID, certVersion);
	
	// �������һ��SM2��Կ
	//if((ret = UnionGenerateSM2Key(&asymmetricKeyDB, startDate, endDate)) < 0)
	if((ret = UnionGenerateSM2Key(&asymmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1E:: UnionGenerateSM2Key[%s]!\n", asymmetricKeyDB.keyName);
		return(ret);
	}
	
	//add by zhouxw 20150908
	UnionSetHsmGroupIDForHsmSvr(asymmetricKeyDB.keyGroup);	
	//add end

	//add by zhouxw 20151013
        if ((ret = UnionGetHsmGroupRecByHsmGroupID(asymmetricKeyDB.keyGroup,phsmGroupRec)) < 0)
        {
		setVKKeyIndexUnused(&asymmetricKeyDB);
		UnionDropAsymmetricKeyDB(&asymmetricKeyDB);
                UnionUserErrLog("in UnionDealServiceCode8E1E:: UnionGetHsmGroupRecByHsmGroupID �������[%s]������!\n",asymmetricKeyDB.keyGroup);
                return(ret);
        }
        //add end
	
	snprintf(pk, sizeof(pk), "04%s", asymmetricKeyDB.pkValue);
	//vkLen = snprintf(vkValue, sizeof(vkValue), "%s", asymmetricKeyDB.vkValue);
	vkLen = aschex_to_bcdhex(asymmetricKeyDB.vkValue, strlen(asymmetricKeyDB.vkValue), vkValue);
	
	snprintf(fileName, sizeof(fileName), "CNAPS2.%s-%s-%s-SM2.csr", bankID, pix, certVersion);
	
	//��ȡĿ¼
	if ((ptr = UnionReadStringTypeRECVar("defaultDirOfFileReceiver")) == NULL)
	{
		setVKKeyIndexUnused(&asymmetricKeyDB);
		UnionDropAsymmetricKeyDB(&asymmetricKeyDB);
		UnionUserErrLog("in UnionDealServiceCode8E18:: UnionReadStringTypeRECVar[%s]!\n","defaultDirOfFileReceiver");
		return(errCodeRECMDL_VarNotExists);
	}
	UnionReadDirFromStr(ptr,-1,path);

	//֤�������ļ���(ȫ·��)
	snprintf(fileFullName, sizeof(fileFullName), "%s/%s", path, fileName);

	//�ָ���
	if((ret = UnionReadRequestXMLPackageValue("body/fieldSeperate", tmpBuf, sizeof(tmpBuf))) <= 0)
		fieldSeperate = ',';
	else 
	{
		tmpBuf[ret] = 0;
		fieldSeperate = tmpBuf[0];
	}
	
	//֤��DN
	if((ret = UnionReadRequestXMLPackageValue("body/certDN", certDN, sizeof(certDN))) < 0)
	{
		setVKKeyIndexUnused(&asymmetricKeyDB);
		UnionDropAsymmetricKeyDB(&asymmetricKeyDB);
		UnionUserErrLog("in UnionDealServiceCode8E1E:: UnionReadRequestXMLPackageValue[%s]\n", "body/certDN");
		return(ret);
	}
	certDN[ret] = 0;
	
	if((varNum = UnionSeprateVarStrIntoVarGrp(certDN, ret, fieldSeperate, tmpVarStr, 32)) < 0)
	{
		setVKKeyIndexUnused(&asymmetricKeyDB);
		UnionDropAsymmetricKeyDB(&asymmetricKeyDB);
		UnionUserErrLog("in UnionDealServiceCode8E1E:: UnionSeprateVarStrIntoVarGrp[%s]\n", certDN);
		return(varNum);
	}
	if(varNum == 0)
	{
		setVKKeyIndexUnused(&asymmetricKeyDB);
		UnionDropAsymmetricKeyDB(&asymmetricKeyDB);
		UnionUserErrLog("in UnionDealServiceCode8E1E:: no var in certDN\n");
		UnionSetResponseRemark("֤��DN�д�");
		return(errCodeParameter);
	}
	
	for(i = 0; i < varNum; i++)
	{
		if((ptr = strchr(tmpVarStr[i], '=')) != NULL)
		{
			*ptr = 0;
			if(strcasecmp(tmpVarStr[i], "CN") == 0)
			{
				strcpy(reqUserInfo.commonName, ptr+1);
				continue;
			}
	
			if(strcasecmp(tmpVarStr[i], "E") == 0)
			{
				strcpy(reqUserInfo.email, ptr+1);
				continue;
			}
	
			if(strcasecmp(tmpVarStr[i], "OU") == 0)
			{
				if(isFstOU)
					strcpy(reqUserInfo.organizationalUnitName, ptr+1);
				else
				{
					strcpy(reqUserInfo.organizationalUnitName2, ptr+1);
					isFstOU = 1;
				}
				continue;
			}
	
			if(strcasecmp(tmpVarStr[i], "O") == 0)
			{
				if(isFstO)
					strcpy(reqUserInfo.organizationName, ptr+1);
				else
				{
					strcpy(reqUserInfo.organizationName2, ptr+1);
					isFstO = 1;
				}
				continue;
			}
	
			if(strcasecmp(tmpVarStr[i], "L") == 0)
			{
				strcpy(reqUserInfo.localityName, ptr+1);
				continue;
			}
		
			if(strcasecmp(tmpVarStr[i], "S") == 0)
			{
				strcpy(reqUserInfo.stateOrProvinceName, ptr+1);
				continue;
			}	

			if(strcasecmp(tmpVarStr[i], "C") == 0)
			{
				strcpy(reqUserInfo.countryName, ptr+1);
				continue;
			}
		}
	}
	
	/*
	//ƴװ�û���Ϣ
	snprintf(userInfo, sizeof(userInfo), "������[%s]|ʡ��[%s]|������[%s]|��λ1[%s]|��λ2[%s]|����1[%s]|����2[%s]|�û���[%s]|Email[%s]|����[%s]", reqUserInfo.countryName, reqUserInfo.stateOrProvinceName, reqUserInfo.localityName, reqUserInfo.organizationName, reqUserInfo.organizationName2, reqUserInfo.organizationalUnitName, reqUserInfo.organizationalUnitName2, reqUserInfo.commonName, reqUserInfo.email, reqUserInfo.alias);
	*/

	//ժҪ�㷨��ʶ
	if((ret = UnionReadRequestXMLPackageValue("body/hashID", tmpBuf, sizeof(tmpBuf))) <= 0)
		hashID = 2;
	else
	{
		tmpBuf[ret] = 0;
		hashID = atoi(tmpBuf);
	}
	
	//�ļ���ʽ(0:PEM, 1:DER)
	if((ret = UnionReadRequestXMLPackageValue("body/format", tmpBuf, sizeof(tmpBuf))) <= 0)
		format = 0;
	else
	{
		tmpBuf[ret] = 0;
		format = atoi(tmpBuf);
	}
	
	//��ע
	if((ret = UnionReadRequestXMLPackageValue("body/remark", remark, sizeof(remark))) <= 0)
		memset(remark, 0, sizeof(remark));
	else
		remark[ret] = 0;
	
	if(asymmetricKeyDB.vkStoreLocation != 0)
	{
		intVkIndex = atoi(asymmetricKeyDB.vkIndex);
	}
	else
	{
		intVkIndex = -1;
	}
	
	//����֤�������ļ�
	if ((ret = UnionGenerateSM2PKCS10Ex(intVkIndex, vkValue, vkLen, pk, &reqUserInfo, format, fileFullName, sign)) < 0)
        {
		setVKKeyIndexUnused(&asymmetricKeyDB);
		UnionDropAsymmetricKeyDB(&asymmetricKeyDB);
                UnionUserErrLog("in UnionDealServiceCode8E1E:: UnionGenerateSM2PKCS10Ex vkValue=[%s] vkLen[%d]!\n", vkValue, vkLen);
                return(ret);
        }
	if((fp = fopen(fileFullName, "rb")) == NULL)
	{
		setVKKeyIndexUnused(&asymmetricKeyDB);
		UnionDropAsymmetricKeyDB(&asymmetricKeyDB);
		UnionUserErrLog("in UnionDealServiceCode8E1E:: fopen fileName[%s]!\n", fileFullName);
		return(errCodeParameter);
	}
	len = fread(reqContent, 1, 8192*2, fp);
	fclose(fp);
	reqContent[len] = 0;
	
	//�����ݲ������ݿ�
	len = snprintf(sql, sizeof(sql), "insert into reqCertOfCNAPS2 values('%s', '%s', %d, %d, %d, '%s', '%s', '%s')", asymmetricKeyDB.keyName, certDN, hashID, asymmetricKeyDB.algorithmID, format, fileName, reqContent, remark);
	sql[len] = 0;
	if((ret = UnionExecRealDBSql(sql)) < 0)
	{
		setVKKeyIndexUnused(&asymmetricKeyDB);
		UnionDropAsymmetricKeyDB(&asymmetricKeyDB);
		UnionUserErrLog("in UnionDealServiceCode8E1E:: UnionExecRealDBSql[%s]\n", sql);
		return(ret);
	}
	if((ret = UnionSetResponseXMLPackageValue("file/filename", fileName)) < 0)
	{
		setVKKeyIndexUnused(&asymmetricKeyDB);
		UnionDropAsymmetricKeyDB(&asymmetricKeyDB);
		UnionUserErrLog("in UnionDealServiceCode8E1E:: UnionSetResponseXMLPackageValue[%s]\n", "file/filename");
		return(ret);
	}
	return(0);
}

int UnionGenerateSM2Key(PUnionAsymmetricKeyDB pUnionAsymmetricKeyDB)
{
	int		ret = 0;
	//time_t		begin_t, end_t;
	//char		dateTime[32];
	int		lenOfVK = 0;
	char		vkValue[1024];
	
	char		sql[512];
	int		hsmNum = 0;
	int		i = 0;
	char		ipAddr[32];

	if(pUnionAsymmetricKeyDB == NULL || pUnionAsymmetricKeyDB->keyName == NULL || strlen(pUnionAsymmetricKeyDB->keyName) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1E:: UnionGenerateSM2Key parameter error!\n");
		return(errCodeParameter);
	}
	
	// ��Կ��
	strcpy(pUnionAsymmetricKeyDB->keyGroup, "default");
	// �㷨��ʶ
	pUnionAsymmetricKeyDB->algorithmID = conAsymmetricAlgorithmIDOfSM2;
	// ��Կ����
	pUnionAsymmetricKeyDB->keyType = 0;
	// ����ʹ�þ���Կ
	pUnionAsymmetricKeyDB->oldVersionKeyIsUsed = 1;
	// ����������
	pUnionAsymmetricKeyDB->creatorType = conAsymmetricCreatorTypeOfUser;
	// ������
	if((ret = UnionReadRequestXMLPackageValue("head/userID", pUnionAsymmetricKeyDB->creator, sizeof(pUnionAsymmetricKeyDB->creator))) < 0)
	{
		UnionUserErrLog("in UnionGenerateSM2Key:: UnionReadRequestXMLPackageValue[%s]!\n", "head/userID");
		return(ret);
	}
	pUnionAsymmetricKeyDB->keyLen = 256;
	pUnionAsymmetricKeyDB->pkExponent = 0;
	pUnionAsymmetricKeyDB->status = conAsymmetricKeyStatusOfInitial;
	
	if((ret = UnionHsmCmdK1('0', pUnionAsymmetricKeyDB->keyLen, pUnionAsymmetricKeyDB->pkValue, sizeof(pUnionAsymmetricKeyDB->pkValue), &lenOfVK, (unsigned char*)vkValue, sizeof(vkValue))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1E:: UnionHsmCmdK1!\n");
		return(ret);
	}
	if(pUnionAsymmetricKeyDB->vkStoreLocation != 1)
		bcdhex_to_aschex(vkValue, lenOfVK, pUnionAsymmetricKeyDB->vkValue);
		
	// ������Կ
	if((ret = UnionGenerateAsymmetricKeyDBRec(pUnionAsymmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionGenerateSM2Key:: UnionGenerateAsymmetricKeyDBRec[%s]!\n", "pUnionAsymmetricKeyDB->keyName");
		return(ret);
	}
	
	if(pUnionAsymmetricKeyDB->vkStoreLocation)
	{
		sprintf(sql,"select * from hsm where hsmGroupID = '%s' and enabled = 1",pUnionAsymmetricKeyDB->hsmGroupID);
		if ((hsmNum = UnionSelectRealDBRecord(sql,0,0)) < 0)
		{
			UnionDropAsymmetricKeyDB(pUnionAsymmetricKeyDB);
			UnionUserErrLog("in UnionGenerateSM2Key:: UnionExecRealDBSql[%s]!\n",sql);
			return(hsmNum);
		}
		else if (hsmNum == 0)
		{
			UnionDropAsymmetricKeyDB(pUnionAsymmetricKeyDB);
			UnionUserErrLog("in UnionGenerateSM2Key:: hsmGroupID[%s]��û�п��������!\n",pUnionAsymmetricKeyDB->hsmGroupID);
			UnionSetResponseRemark("hsmGroupID[%s]��û�п��������", pUnionAsymmetricKeyDB->hsmGroupID);
			return(errCodeParameter);
		}
		for(i = 0; i< hsmNum; i++)
		{
			UnionLocateXMLPackage("detail", i+1);
			if((ret = UnionReadXMLPackageValue("ipAddr", ipAddr, sizeof(ipAddr))) < 0)
			{
				UnionDropAsymmetricKeyDB(pUnionAsymmetricKeyDB);
				UnionUserErrLog("in UnionGenerateSM2Key:: UnionReadXMLPackageValue[%s]!\n", "ipAddr");
				return(ret);
			}
			UnionSetUseSpecHsmIPAddrForOneCmd(ipAddr);
			if ((ret = UnionHsmCmdK2(atoi(pUnionAsymmetricKeyDB->vkIndex),lenOfVK,(unsigned char *)vkValue)) < 0)
			{
				UnionDropAsymmetricKeyDB(pUnionAsymmetricKeyDB);
				UnionUserErrLog("in UnionGenerateSM2Key:: UnionHsmCmdK2 hsm[%s] vkIndex[%s]!\n",ipAddr,pUnionAsymmetricKeyDB->vkIndex);
			}
		}
		snprintf(sql,sizeof(sql),"update vkKeyIndex set status = 1 where hsmGroupID = '%s' and vkIndex = '%s' and algorithmID= 1",pUnionAsymmetricKeyDB->hsmGroupID,pUnionAsymmetricKeyDB->vkIndex);
		if ((ret = UnionExecRealDBSql(sql)) <= 0)
		{
			UnionDropAsymmetricKeyDB(pUnionAsymmetricKeyDB);
			UnionUserErrLog("in UnionGenerateSM2Key:: UnionExecRealDBSql[%s]!\n",sql);
			UnionSetResponseRemark("����˽Կ״̬ʧ��");
			return(ret);
		}
	}
		
	return 0;
}

static int setVKKeyIndexUnused(PUnionAsymmetricKeyDB pUnionAsymmetricKeyDB)
{
	int             ret = 0;
	char            updateVkStatusSql[512];

	if(!pUnionAsymmetricKeyDB->vkStoreLocation)
	        return 0;
	
	snprintf(updateVkStatusSql, sizeof(updateVkStatusSql), "update vkKeyIndex set status = 0 where hsmGroupID = '%s' and vkIndex = '%s' and algorithmID = 1", pUnionAsymmetricKeyDB->hsmGroupID, pUnionAsymmetricKeyDB->vkIndex);

	if ((ret = UnionExecRealDBSql(updateVkStatusSql)) <= 0)
	{
	        UnionUserErrLog("in updateVkKeyIndexStatus:: UnionExecRealDBSql[%s]!\n", updateVkStatusSql);
	        UnionSetResponseRemark("����˽Կ����״̬ʧ��");
	        return(errCodeParameter);
	}
	
	return 0;
}
