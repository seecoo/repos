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

/***************************************
  �������:     8E30
  ������:       ����SM2֤�������ļ�
  ��������:     ����SM2֤�������ļ�
 ***************************************/
int UnionDealServiceCode8E30(PUnionHsmGroupRec phsmGroupRec)
{
	int		ret;
	char		version[16];
	char		userInfo[1024];
	int		hashID;
	char		pk[512];
	int		format;
	char		remark[128];
	char		fileName[128];
	char		fileFullName[512];
	char		path[512];
	char		tmpBuf[128];
	int		len;
	char		sql[512];
	char		fileStoreDir[512];
	char		sign[2048];
	char		certDN[1024];
	char		fieldSeperate;
	char		tmpVarStr[32][128];
	int		varNum;
	int		i;
	int		isFstOU = 0;
	int		isFstO = 0;
	char		*ptr;
	int		vkIndex;
	char		vkValue[1024];	
	int		vkLen = 0;
	
	TUnionX509UserInfoSM2		reqUserInfo;
	TUnionAsymmetricKeyDB		asymmetricKeyDB;
	
	memset(&reqUserInfo, 0, sizeof(reqUserInfo));
	memset(&asymmetricKeyDB, 0, sizeof(asymmetricKeyDB));
	
	//��Կ��
	if((ret = UnionReadRequestXMLPackageValue("body/keyName", asymmetricKeyDB.keyName, sizeof(asymmetricKeyDB.keyName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E30:: UnionReadRequestXMLPackageValue[%s]\n", "body/keyName");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(asymmetricKeyDB.keyName);
	
	len = snprintf(sql, sizeof(sql), "select * from reqCertOfCIPS where keyName='%s'", asymmetricKeyDB.keyName);
	sql[len] = 0;
	if((ret = UnionSelectRealDBRecord(sql, 0, 0)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E30:: UnionSelectRealDBRecord[%s] ret[%d]\n", sql, ret);
		return(ret);
	}
	if(ret > 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E30:: key[%s] has been used for one reqCert\n", asymmetricKeyDB.keyName);
		UnionSetResponseRemark("��Կ�ѱ�����������һ������֤��");
		return(errCodeParameter);
	}
	
	if((ret = UnionReadAsymmetricKeyDBRec(asymmetricKeyDB.keyName, 1, &asymmetricKeyDB)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E30:: UnionReadAsymmetricKeyDBRec keyName[%s]\n", asymmetricKeyDB.keyName);
		return(ret);
	}
	
	//add by zhouxw 20150908
	UnionSetHsmGroupIDForHsmSvr(asymmetricKeyDB.keyGroup);	
	//add end
	//add by zhouxw 20151013
        if ((ret = UnionGetHsmGroupRecByHsmGroupID(asymmetricKeyDB.keyGroup,phsmGroupRec)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E30:: UnionGetHsmGroupRecByHsmGroupID �������[%s]������!\n",asymmetricKeyDB.keyGroup);
                return(ret);
        }
        //add end
	
	if(asymmetricKeyDB.algorithmID == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E30:: keyName[%s] algorithmID[%s]\n", asymmetricKeyDB.keyName, asymmetricKeyDB.algorithmID == 0 ? "RSA":"SM2");
		UnionSetResponseRemark("��Կ��ΪSM2��Կ");
		return(errCodeParameter);
	}
	
	if(asymmetricKeyDB.vkStoreLocation == 0)
	{
		vkIndex = -1;
		vkLen = aschex_to_bcdhex(asymmetricKeyDB.vkValue, strlen(asymmetricKeyDB.vkValue), vkValue);
		vkValue[vkLen] = 0;
	}
	else
		vkIndex = atoi(asymmetricKeyDB.vkIndex);

	if(asymmetricKeyDB.keyType != 0 && asymmetricKeyDB.keyType != 2)
	{
		UnionUserErrLog("in UnionDealServiceCode8E30:: keyName[%s] is not used for sign\n", asymmetricKeyDB.keyName);
		UnionSetResponseRemark("��Կ���Ͳ�����ǩ��");
		return(errCodeParameter);
	}
	
	snprintf(pk, sizeof(pk), "04%s", asymmetricKeyDB.pkValue);
	
	//�汾��
	if((ret = UnionReadRequestXMLPackageValue("body/version", version, sizeof(version))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E30:: UnionReadRequestXMLPackageValue[%s]\n", "body/version");
		return(ret);
	}
	if(ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E30:: version[%s] is NULL\n", "body/version");
		UnionSetResponseRemark("�汾��Ϊ��");
		return(errCodeParameter);
	}
	version[ret] = 0;
	snprintf(fileName, sizeof(fileName), "SM.%s.req", version);
	
	len = snprintf(sql, sizeof(sql), "select * from fileType where fileTypeID = 'CIP1'");
	sql[len] = 0;
	if((ret = UnionSelectRealDBRecord(sql, 0, 0)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E30:: UnionSelectRealDBRecord[%s]\n", sql);
                return(ret);
        }
        else if(ret == 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E30:: fileTypeID[CIP1]\n");
                return(errCodeDatabaseMDL_RecordNotFound);
        }
	
	if((ret = UnionLocateXMLPackage("detail", 1)) < 0)	
	{
		UnionUserErrLog("in UnionDealServiceCode8E30:: UnionLocateXMLPackage[%s]\n", "detail");
		return(ret);
	}
	//��ȡĿ¼
	if((ret = UnionReadXMLPackageValue("fileStoreDir", fileStoreDir, sizeof(fileStoreDir))) < 0)
	{	
		UnionUserErrLog("in UnionDealServiceCode8E30:: UnionReadRequestXMLPackageValue[%s]\n", "fileStoreDir");
		return(ret);
	}
	fileStoreDir[ret] = 0;
	UnionReadDirFromStr(fileStoreDir, -1, path);

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
	
	UnionLog("fieldSeperate %c\n", fieldSeperate);

	//֤��DN
	if((ret = UnionReadRequestXMLPackageValue("body/certDN", certDN, sizeof(certDN))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E30:: UnionReadRequestXMLPackageValue[%s]\n", "body/certDN");
		return(ret);
	}
	certDN[ret] = 0;
	
	if((varNum = UnionSeprateVarStrIntoVarGrp(certDN, ret, fieldSeperate, tmpVarStr, 32)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E30:: UnionSeprateVarStrIntoVarGrp[%s]\n", certDN);
		return(varNum);
	}
	if(varNum == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E30:: no var in certDN\n");
		UnionSetResponseRemark("֤��DN�д�");
		return(errCodeParameter);
	}
	
	for(i = 0; i < varNum; i++)
		UnionUserErrLog("%s\n", tmpVarStr[i]);
	
	for(i = 0; i < varNum; i++)
	{
		if((ptr = strchr(tmpVarStr[i], '=')) != NULL)
		{
			*ptr = 0;
			UnionUserErrLog("%s\n", tmpVarStr[i]);
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
	
	//ƴװ�û���Ϣ
	snprintf(userInfo, sizeof(userInfo), "������[%s]|ʡ��[%s]|������[%s]|��λ1[%s]|��λ2[%s]|����1[%s]|����2[%s]|�û���[%s]|Email[%s]|����[%s]", reqUserInfo.countryName, reqUserInfo.stateOrProvinceName, reqUserInfo.localityName, reqUserInfo.organizationName, reqUserInfo.organizationName2, reqUserInfo.organizationalUnitName, reqUserInfo.organizationalUnitName2, reqUserInfo.commonName, reqUserInfo.email, reqUserInfo.alias);

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
	
	//����֤�������ļ�
	if ((ret = UnionGenerateSM2PKCS10Ex(vkIndex, vkValue, vkLen, pk, &reqUserInfo, format, fileFullName, sign)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E30:: UnionGenerateSM2PKCS10Ex vkIndex=[%d] vkLen[%d]!\n", vkIndex, vkLen);
                return(ret);
        }

	//�����ݲ������ݿ�
	len = snprintf(sql, sizeof(sql), "insert into reqCertOfCIPS values('%s', '%s', '%s', %d, %d, '%s', '%s', %d, '%s', '%s')", version, asymmetricKeyDB.keyName, userInfo, hashID, asymmetricKeyDB.algorithmID, pk, sign, format, fileName, remark);
	sql[len] = 0;
	if((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E30:: UnionExecRealDBSql[%s]\n", sql);
		return(ret);
	}
		
	if((ret = UnionSetResponseXMLPackageValue("file/filename", fileName)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E30:: UnionSetResponseXMLPackageValue[%s]\n", "file/filename");
		return(ret);
	}
	return(0);
}

