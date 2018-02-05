#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "essc5Interface.h"
#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionRealBaseDB.h"
#include "UnionLog.h"
#include "UnionStr.h"
#include "symmetricKeyDB.h"
#include "unionHsmCmd.h"
#include "remoteKeyPlatform.h"
#include "unionHsmCmdVersion.h"

int UnionHsmCmdRY3(char *flag,char *CSCKByLMK,char *accNo,char *activeDate,char *svrCode,char  *CSCOf5Digit,char *CSCOf4Digit,char *CSCOf3Digit);

/***************************************
�������:       E802
������:         ������ͨ����ȫ��
��������:       ������ͨ����ȫ�룬�漰ָ��RY
***************************************/

int UnionDealServiceCodeE802(PUnionHsmGroupRec phsmGroupRec)
{
	int				ret = 0;
	char				tmpBuf[32];
	char				mode[16];
	char				accNo[32];
	int				lenOfAccNo = 0;
	char				activeDate[16];
	char				serviceCode[16];		
	char				keyName[136];
	char				safeCodeFive[16];
	char				safeCodeFour[16];
	char				safeCodeThree[16];

        TUnionSymmetricKeyDB            csckKeyDB;
        PUnionSymmetricKeyValue         pcsckKeyValue = NULL;

	//��ȡģʽmode
	if ((ret = UnionReadRequestXMLPackageValue("body/mode",mode,sizeof(mode))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE802:: UnionReadRequestXMLPackageValue[%s]!\n","body/mode");
                return(ret);
        }
	
	if ((mode[0] != '0') && (mode[0] != '2') && (mode[0] != '3'))
	{
		UnionUserErrLog("in UnionDealServiceCodeE802:: mode[%s] is not invalid!\n",mode);
		return(errCodeParameter);
	}
	
	// ��ȡ��Կ����
        if ((ret = UnionReadRequestXMLPackageValue("body/keyName",keyName,sizeof(keyName))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE802:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
                return(ret);
        }
	
	if ((mode[0] == '2') || (mode[0] == '3'))
	{
		//��ȡ������
        	if ((ret = UnionReadRequestXMLPackageValue("body/serviceCode",serviceCode,sizeof(serviceCode))) < 0)
        	{
                	UnionUserErrLog("in UnionDealServiceCodeE802:: UnionReadRequestXMLPackageValue[%s]!\n","body/serviceCode");
                	return(ret);
        	}
        	else if (ret == 0)
        	{
                	UnionUserErrLog("in UnionDealServiceCodeE802:: serviceCode can not be null!\n");
                	return(errCodeParameter);
        	}
		UnionFilterHeadAndTailBlank(serviceCode);
	}

	//��ȡ�˺�
        if ((ret = UnionReadRequestXMLPackageValue("body/accNo",accNo,sizeof(accNo))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE802:: UnionReadRequestXMLPackageValue[%s]!\n","body/accNo");
                return(ret);
        }
        UnionFilterHeadAndTailBlank(accNo);
        if((lenOfAccNo = strlen(accNo)) == 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE802:: accNo can not be null!\n");
                UnionSetResponseRemark("�˺Ų���Ϊ��!");
                return(errCodeParameter);
        }
	if (!UnionIsDigitString(accNo))
	{
		UnionUserErrLog("in UnionDealServiceCodeE802:: accNo[%s] is error!\n",accNo);
		UnionSetResponseRemark("�˺ŷǷ�[%s],����Ϊ����",accNo);
		return(errCodeHsmCmdMDL_InvalidAcc);
	}

	//���˺�����0��19λ
	if(lenOfAccNo < 19)
	{
		memset(tmpBuf,'0',19);
		memcpy(tmpBuf + (19 - lenOfAccNo),accNo,strlen(accNo));
		memcpy(accNo,tmpBuf,19);
		accNo[19] = 0;
	}
	else if (lenOfAccNo > 19)
	{
		UnionUserErrLog("in UnionDealServiceCodeE802:: accNo[%s] error!\n",accNo);
		return(errCodeParameter);
	}
	
	//��ȡ��Ч��
        if ((ret = UnionReadRequestXMLPackageValue("body/activeDate",activeDate,sizeof(activeDate))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE802:: UnionReadRequestXMLPackageValue[%s]!\n","body/activeDate");
                return(ret);
        }
        UnionFilterHeadAndTailBlank(activeDate);
        if (strlen(activeDate) == 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE802:: activeDate can not be null!\n");
                return(errCodeParameter);
        }
	
	//��ȡCSCK��Կ
	//memset(&csckKeyDB,0,sizeof(csckKeyDB));
        if ((ret = UnionReadSymmetricKeyDBRec(keyName,1,&csckKeyDB)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE802:: UnionReadSymmetricKeyDBRec[%s]!\n",keyName);
                return(ret);
        }

	//��ȡ��Կֵ
	if((pcsckKeyValue = UnionGetSymmetricKeyValue(&csckKeyDB,phsmGroupRec->lmkProtectMode)) == NULL)
	{
                UnionUserErrLog("in UnionDealServiceCodeE802:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
                UnionSetResponseRemark("��ȡ��Կֵʧ��");
                return(errCodeParameter);	
	}

	//���ü��ܻ�ָ��RY3
	switch(csckKeyDB.algorithmID)
	{
		case	conSymmetricAlgorithmIDOfDES:
			switch(phsmGroupRec->hsmCmdVersionID)
			{
			       case conHsmCmdVerRacalStandardHsmCmd:
			       case conHsmCmdVerSJL06StandardHsmCmd:
					if((ret = UnionHsmCmdRY3(mode,pcsckKeyValue->keyValue,accNo,activeDate,serviceCode,safeCodeFive,safeCodeFour,safeCodeThree)) < 0)
					{
						UnionUserErrLog("in UnionDealServiceCodeE802:: UnionHsmCmdRY3!\n");
						return(ret);
					}
					break;
				default:
					UnionUserErrLog("in UnionDealServiceCodeE802:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
					UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
					return(errCodeParameter);
			}
			break;
		case	conSymmetricAlgorithmIDOfSM4:	// SM4�㷨
		default:
			UnionUserErrLog("in UnionDealServiceCodeE802:: csckKeyDB.algorithmID[%d] is invalid!\n",csckKeyDB.algorithmID);
			return(errCodeEsscMDL_InvalidAlgorithmID);
	}
	
	//���÷���mode
        if ((ret = UnionSetResponseXMLPackageValue("body/mode",mode)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE802:: UnionSetResponseXMLPackageValue mode[%s]!\n",mode);
                return(ret);
        }

	if((mode[0] == '0') || (mode[0] == '2'))
	{	
		//���÷���safeCodeFive
        	if ((ret = UnionSetResponseXMLPackageValue("body/safeCodeFive",safeCodeFive)) < 0)
        	{
                	UnionUserErrLog("in UnionDealServiceCodeE802:: UnionSetResponseXMLPackageValue safeCodeFive[%s]!\n",safeCodeFive);
                	return(ret);
        	}
		//���÷���safeCodeFour
        	if ((ret = UnionSetResponseXMLPackageValue("body/safeCodeFour",safeCodeFour)) < 0)
        	{
                	UnionUserErrLog("in UnionDealServiceCodeE802:: UnionSetResponseXMLPackageValue safeCodeFour[%s]!\n",safeCodeFour);
                	return(ret);
        	}
	}
	//���÷���safeCodeThree
	if ((ret = UnionSetResponseXMLPackageValue("body/safeCodeThree",safeCodeThree)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE802:: UnionSetResponseXMLPackageValue safeCodeThree[%s]!\n",safeCodeThree);
		return(ret);
	}

	return(0);
}
