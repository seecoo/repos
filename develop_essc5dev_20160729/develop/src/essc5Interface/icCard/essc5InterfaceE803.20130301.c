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

int UnionHsmCmdRY4(char *flag,char *CSCKByLMK,char *accNo,char *activeDate,char *svrCode,char *CSCOf5Digit,char *CSCOf4Digit,char *CSCOf3Digit,char *checkResultOf5CSC,char *checkResultOf4CSC,char *checkResultOf3CSC);

/***************************************
�������:       E803
������:         У����ͨ����ȫ�� 
��������:       У����ͨ����ȫ�룬�漰ָ��RY
***************************************/

int UnionDealServiceCodeE803(PUnionHsmGroupRec phsmGroupRec)
{
	int				ret = 0;
	char				mode[16];
	char				accNo[32];
	int				lenOfAccNo = 0;
	char				activeDate[16];
	char				serviceCode[16];
	char				safeCodeFive[16];
	char				safeCodeFour[16];
	char				safeCodeThree[16];
	char				keyName[136];
	char				checkResultOf5CSC[16];	
	char				checkResultOf4CSC[16];	
	char				checkResultOf3CSC[16];	
	char				checkResult[16];
	int				lenOfCheckResult = 0;
	char				tmpBuf[32];

        TUnionSymmetricKeyDB            csckKeyDB;
        PUnionSymmetricKeyValue         pcsckKeyValue = NULL;

        //��ȡģʽmode
        if ((ret = UnionReadRequestXMLPackageValue("body/mode",mode,sizeof(mode))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE803:: UnionReadRequestXMLPackageValue[%s]!\n","body/mode");
                return(ret);
        }

	if ((mode[0] != '0') && (mode[0] != '2') && (mode[0] != '3'))
	{
		UnionUserErrLog("in UnionDealServiceCodeE803:: mode[%s] is not invalid!\n",mode);
		return(errCodeParameter);
	}

	if ((ret = UnionReadRequestXMLPackageValue("body/keyName",keyName,sizeof(keyName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE803:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
		return(ret);
	}

	//��ȡ������serviceCode
        if((mode[0] == '2') || (mode[0] == '3'))
        {
                //��ȡ������
                if ((ret = UnionReadRequestXMLPackageValue("body/serviceCode",serviceCode,sizeof(serviceCode))) < 0)
                {
                        UnionUserErrLog("in UnionDealServiceCodeE803:: UnionReadRequestXMLPackageValue[%s]!\n","body/serviceCode");
                        return(ret);
                }
                else if (ret == 0)
                {
                        UnionUserErrLog("in UnionDealServiceCodeE803:: serviceCode can not be null!\n");
                        return(errCodeParameter);
                }
        }

        //��ȡ��Ч��
        if ((ret = UnionReadRequestXMLPackageValue("body/activeDate",activeDate,sizeof(activeDate))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE803:: UnionReadRequestXMLPackageValue[%s]!\n","body/activeDate");
                return(ret);
        }
        UnionFilterHeadAndTailBlank(activeDate);
	if (strlen(activeDate) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE803:: activeDate can not be null!\n");
		return(errCodeParameter);
	}
	if(strlen(activeDate) != 4)
	{
		UnionUserErrLog("in UnionDealServiceCodeE803:: activeDate[%s] format error!\n",activeDate);
		return(errCodeParameter);
	}

        //��ȡ�˺�
        if ((ret = UnionReadRequestXMLPackageValue("body/accNo",accNo,sizeof(accNo))) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE803:: UnionReadRequestXMLPackageValue[%s]!\n","body/accNo");
                return(ret);
        }
        UnionFilterHeadAndTailBlank(accNo);
        if((lenOfAccNo = strlen(accNo)) == 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE803:: accNo can not be null!\n");
                UnionSetResponseRemark("�˺Ų���Ϊ��!");
                return(errCodeParameter);
        }
        if (!UnionIsDigitString(accNo))
        {
                UnionUserErrLog("in UnionDealServiceCodeE803:: accNo[%s] is error!\n",accNo);
                UnionSetResponseRemark("�˺ŷǷ�[%s],����Ϊ����",accNo);
                return(errCodeHsmCmdMDL_InvalidAcc);
        }
        //���˺�����0��19λ
        if(lenOfAccNo < 19)
        {
                memset(tmpBuf,'0',19);
                memcpy(tmpBuf + (19 - lenOfAccNo),accNo,lenOfAccNo);
		memcpy(accNo,tmpBuf,19);
		accNo[19] = 0;
        }
        else if (lenOfAccNo > 19)
        {
                UnionUserErrLog("in UnionDealServiceCodeE803:: accNo[%s] error!\n",accNo);
                return(errCodeParameter);
        }

	if((mode[0] == '0') || (mode[0] == '2'))
	{
		//��ȡ5λ��CSCֵ
        	if ((ret = UnionReadRequestXMLPackageValue("body/safeCodeFive",safeCodeFive,sizeof(safeCodeFive))) < 0)
        	{
                	UnionUserErrLog("in UnionDealServiceCodeE803:: UnionReadRequestXMLPackageValue[%s]!\n","body/safeCodeFive");
                	return(ret);
        	}		
		//��ȡ4λ��CSCֵ
                if ((ret = UnionReadRequestXMLPackageValue("body/safeCodeFour",safeCodeFour,sizeof(safeCodeFour))) < 0)
                {
                        UnionUserErrLog("in UnionDealServiceCodeE803:: UnionReadRequestXMLPackageValue[%s]!\n","body/safeCodeFour");
                        return(ret);
                }
	}

	//��ȡ��λ��CSCֵ
	if ((ret = UnionReadRequestXMLPackageValue("body/safeCodeThree",safeCodeThree,sizeof(safeCodeThree))) < 0)
        {
                 UnionUserErrLog("in UnionDealServiceCodeE803:: UnionReadRequestXMLPackageValue[%s]!\n","body/safeCodeThree");
                 return(ret);
        }

        //��ȡ��Կ
	//memset(&csckKeyDB,0,sizeof(csckKeyDB));
        if ((ret = UnionReadSymmetricKeyDBRec(keyName,1,&csckKeyDB)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE803:: UnionReadSymmetricKeyDBRec[%s]!\n",keyName);
                return(ret);
        }

        //��ȡ��Կֵ
        if((pcsckKeyValue = UnionGetSymmetricKeyValue(&csckKeyDB,phsmGroupRec->lmkProtectMode)) == NULL)
        {
                UnionUserErrLog("in UnionDealServiceCodeE803:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
                UnionSetResponseRemark("��ȡ��Կֵʧ��");
                return(errCodeParameter);
        }

        //���ü��ܻ�ָ��RY4
	switch(csckKeyDB.algorithmID)
	{
		case	conSymmetricAlgorithmIDOfDES:
			switch(phsmGroupRec->hsmCmdVersionID)
			{
			       case conHsmCmdVerRacalStandardHsmCmd:
			       case conHsmCmdVerSJL06StandardHsmCmd:
					if((ret = UnionHsmCmdRY4(mode,pcsckKeyValue->keyValue,accNo,activeDate,serviceCode,safeCodeFive,safeCodeFour,safeCodeThree,checkResultOf5CSC,checkResultOf4CSC,checkResultOf3CSC)) < 0)
					{
						UnionUserErrLog("in UnionDealServiceCodeE803:: UnionHsmCmdRY4!\n");
						return(ret);
					}
					break;
			       default:
				       UnionUserErrLog("in UnionDealServiceCodeE803:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
				       UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
				       return(errCodeParameter);
			}
			break;
		case	conSymmetricAlgorithmIDOfSM4:	// SM4�㷨
		default:
			UnionUserErrLog("in UnionDealServiceCodeE803:: csckKeyDB.algorithmID[%d] is invalid!\n",csckKeyDB.algorithmID);
			return(errCodeEsscMDL_InvalidAlgorithmID);
	}
	
	//���÷���ģʽ��ʶmode
        if ((ret = UnionSetResponseXMLPackageValue("body/mode",mode)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE803:: UnionSetResponseXMLPackageValue mode[%s]!\n",mode);
                return(ret);
        }

	//���÷���У����
	checkResult[0] = 0;
	if((mode[0] == '0') || (mode[0] == '2'))
		lenOfCheckResult = snprintf(checkResult,sizeof(checkResult),"%c%c",checkResultOf5CSC[0],checkResultOf4CSC[0]);
	
	//���÷���У����
	snprintf(checkResult + lenOfCheckResult,sizeof(checkResult)-lenOfCheckResult,"%c",checkResultOf3CSC[0]);
	if ((ret = UnionSetResponseXMLPackageValue("body/checkResult",checkResult)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE803:: UnionSetResponseXMLPackageValue checkResult[%s]!\n",checkResult);
                return(ret);
        }

	return(0);
}
