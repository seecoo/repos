//	Author:		zhouxw
//	Copyright:	Union Tech. Guangzhou
//	Date:		20150713

#ifndef	_realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<time.h>

#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionRealBaseDB.h"
#include "UnionLog.h"
#include "essc5UIService.h"
#include "UnionStr.h"
#include "symmetricKeyDB.h"
#include "unionHsmCmdVersion.h"
#include "unionREC.h"

/***************************************
�������:       8E1C
������:         ��ӡ�ն���Կ
��������:       ��ӡ�ն���Կ
***************************************/

int UnionDealServiceCode8E1C(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret = 0;
	int	algorithmID = 0;
	
	int	len;
	char    sql[512];
        char    isCheckAlonePrint[16];
        char    keyValuePrintFormat[2048];
	int	i = 0;
	char	tmpBuf[128];
	char	*ptr;
	char	appID[32];
	int	numOfComponent;
	char	hsmIP[16];
	char	userID[64];
	char	keyName[3][128];
	char	keyType[3][16]={"zak","zpk","zmk"};
	char	appendPara[3][80];
	
	TUnionSymmetricKeyDB		symmetricKeyDB;
	
	//�ն˺�
	if((ret = UnionReadRequestXMLPackageValue("body/terminalID", appendPara[0], sizeof(appendPara[0]))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1C:: UnionReadRequestXMLPackageValue[%s]\n", "body/terminalID");
		return(ret);
	}
	
	if(ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1C:: UnionReadRequestXMLPackageValue[%s]\n", "body/terminalID");
		UnionSetResponseRemark("�ն˺�Ϊ��");
		return(errCodeParameter);
	}
	appendPara[0][ret] = 0;
	UnionFilterHeadAndTailBlank(appendPara[0]);
	
	len = snprintf(sql, sizeof(sql), "select * from keyPrintMsg where terminalID='%s'", appendPara[0]);
	sql[len] = 0;
	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E1C:: UnionSelectRealDBRecord sql[%s]!\n",sql);
                return(ret);
        }       
        if ((ret = UnionLocateXMLPackage("detail",1)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E1C:: UnionLocateXMLPackage[%s]!\n","detail");
                return(ret);
        }
	//������
	if((ret = UnionReadXMLPackageValue("branchID", appendPara[1], sizeof(appendPara[1]))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1C:: UnionReadXMLPackageValue[%s]\n", "branchID");
		return(ret);
	}
	appendPara[1][ret] = 0;
	UnionFilterHeadAndTailBlank(appendPara[1]);
	//��װ��ַ
	if((ret = UnionReadXMLPackageValue("address", appendPara[2], sizeof(appendPara[2]))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1C:: UnionReadXMLPackageValue[%s]\n", "address");
		return(ret);
	}
	appendPara[2][ret] = 0;
	UnionFilterHeadAndTailBlank(appendPara[2]);
		
	if((ptr = strchr(appendPara[2], '|')) == NULL)
		strcpy(appID, "atmp");
	else
	{
		if((ptr + 1) == NULL)
			strcpy(appID, "atmp");
		else
			strcpy(appID, ptr+1);
		*ptr = 0;
	}
	//��Կ��
	snprintf(keyName[0], sizeof(keyName[0]), "%s.%s.%s", appID, appendPara[0], "zak");
	snprintf(keyName[1], sizeof(keyName[1]), "%s.%s.%s", appID, appendPara[0], "zpk");
	snprintf(keyName[2], sizeof(keyName[2]), "%s.%s.%s", appID, appendPara[0], "zmk");
	
	//�㷨��ʶ
	if((ret = UnionReadXMLPackageValue("algorithmID", tmpBuf, sizeof(tmpBuf))) <= 0)
		algorithmID = 0;
	else
	{
		tmpBuf[ret] = 0;
		UnionFilterHeadAndTailBlank(tmpBuf);
		algorithmID = atoi(tmpBuf);
	}
	
	//��������
	if((ret = UnionReadRequestXMLPackageValue("body/numOfComponent", tmpBuf, sizeof(tmpBuf))) <= 0)
		numOfComponent = 2;
	else
	{
		tmpBuf[ret] = 0;
		numOfComponent =atoi(tmpBuf);
	}
	if(numOfComponent <= 1 || numOfComponent > 9)
        {
                UnionUserErrLog("in UnionDealServiceCode8E1C:: numOfComponent[%d] is invalid\n", numOfComponent);
                return(errCodeParameter);
        }
	
	// �����IP
        if ((ret = UnionReadRequestXMLPackageValue("body/ipAddr",hsmIP,sizeof(hsmIP))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E1C:: UnionReadRequestXMLPackageValue[%s]!\n","body/hsmIP");
                return(ret);
        }
	hsmIP[ret] = 0;
        UnionFilterHeadAndTailBlank(hsmIP);
        if (!UnionIsValidIPAddrStr(hsmIP))
        {
                UnionUserErrLog("in UnionDealServiceCode8E1C:: ipAddr[%s] is valid!\n",hsmIP);
                return(errCodeInvalidIPAddr);
        }	

	if((ret = UnionReadRequestXMLPackageValue("head/userID", userID, sizeof(userID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1C:: UnionReadRequestXMLPackageValue[%s]\n", "head/userID");
		return(ret);
	}
	
	if(ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1C:: UnionReadRequestXMLPackageValue[%s]\n", "head/userID");
		UnionSetResponseRemark("�û�IDΪ��");
		return(errCodeParameter);
	}
	userID[ret] = 0;
	UnionFilterHeadAndTailBlank(userID);

	// ��ѯ��ӡ��ʽ
        len = snprintf(sql, sizeof(sql), "select * from keyPrintFormat where formatName = 'batchKeyPrintFormat'");
        sql[len] = 0;
        if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E1C:: UnionSelectRealDBRecord sql[%s]!\n",sql);
                return(ret);
        }
	else if(ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode8E1C:: record can't found[%s]!\n", sql);
		return(errCodeRecordNotExists);
	}

        if ((ret = UnionLocateXMLPackage("detail",1)) < 0)
        {
        	UnionUserErrLog("in UnionDealServiceCode8E1C:: UnionLocateXMLPackage[%s]!\n","detail");
                return(ret);
        }
        // ��ȡ�Ƿ񵥶���ӡУ��ֵ
        if ((ret = UnionReadXMLPackageValue("isCheckAlonePrint",isCheckAlonePrint,sizeof(isCheckAlonePrint))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E1C:: UnionReadXMLPackageValue[%s]!\n","isCheckAlonePrint");
                return(ret);
        }
        isCheckAlonePrint[ret] = 0;
        UnionFilterHeadAndTailBlank(isCheckAlonePrint);

        // ��ȡ��ӡ��ʽ
        if ((ret = UnionReadXMLPackageValue("format",keyValuePrintFormat,sizeof(keyValuePrintFormat))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E1C:: UnionReadXMLPackageValue[%s]!\n","format");
		return(ret);
        }
        keyValuePrintFormat[ret] = 0;
	
	for(i = 0; i < 3; i++)
	{
		memset(&symmetricKeyDB, 0, sizeof(symmetricKeyDB));
		memcpy(symmetricKeyDB.keyName, keyName[i], strlen(keyName[i]) + 1);
		if((ret = UnionReadSymmetricKeyDBRec(symmetricKeyDB.keyName,0,&symmetricKeyDB)) >= 0)
		{
			if(symmetricKeyDB.algorithmID == algorithmID)
                                continue;
                        else
                        {
                                UnionUserErrLog("in UnionDealServiceCode8E1C:: [%s][%s] already exist\n", symmetricKeyDB.keyName, symmetricKeyDB.algorithmID == 0 ? "DES�㷨":"SM4�㷨");
				UnionSetResponseRemark("��ͬ�㷨����Կ�Ѵ���");
				return(errCodeParameter);
                        }
		}
	}		

	for(i = 0; i < 3; i++)
        {               
                memset(&symmetricKeyDB, 0, sizeof(symmetricKeyDB));
                memcpy(symmetricKeyDB.keyName, keyName[i], strlen(keyName[i]) + 1);
                if((ret = UnionReadSymmetricKeyDBRec(symmetricKeyDB.keyName,0,&symmetricKeyDB)) >= 0)
			continue;
	
		symmetricKeyDB.keyType = UnionConvertSymmetricKeyKeyType(keyType[i]);
		symmetricKeyDB.algorithmID = algorithmID;
		symmetricKeyDB.keyLen = con128BitsSymmetricKey;
		
		//��ԿĬ������
		strcpy(symmetricKeyDB.keyGroup,"default");
        	symmetricKeyDB.inputFlag = 1;
        	symmetricKeyDB.outputFlag = 1;
        	symmetricKeyDB.status = conSymmetricKeyStatusOfInitial;
        	symmetricKeyDB.oldVersionKeyIsUsed = 1;
        	symmetricKeyDB.creatorType = conSymmetricCreatorTypeOfUser;
        	strcpy(symmetricKeyDB.creator,userID);
		
		if((ret = UnionCreateSymmetricKeyDB(&symmetricKeyDB)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode8E1C:: UnionCreateSymmetricKeyDB[%s]\n", symmetricKeyDB.keyName);
			return(ret);
		}
	}
	
	// ��ӡ��Կ
        if ((ret = UnionGenerateAndPrintSymmetricKey(phsmGroupRec,&symmetricKeyDB,keyValuePrintFormat,atoi(isCheckAlonePrint),numOfComponent,hsmIP,3,appendPara)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E1C:: UnionGenerateAndPrintSymmetricKey!\n");
		return(ret);
        }

        //������Կ
        if ((ret = UnionUpdateSymmetricKeyDBKeyValue(&symmetricKeyDB)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode8E1C:: UnionUpdateSymmetricKeyDBKeyValue keyName[%s]!\n",symmetricKeyDB.keyName);
		return(ret);
        }
	
	return(0);
}

