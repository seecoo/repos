//	Author:		������
//	Copyright:	Union Tech. Guangzhou
//	Date:		2013-01-10

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
#include "essc5Interface.h"
#include "UnionStr.h"
#include "unionHsmCmd.h"
#include "defaultIBMPinOffsetDef.h"
#include "symmetricKeyDB.h"
#include "unionHsmCmdVersion.h"
//add by zhouxw 20151110
#include "3DesRacalSyntaxRules.h"
//add end

/***************************************
�������:	E140
������:		����PIN
��������:	����PIN
***************************************/
int UnionDealServiceCodeE140(PUnionHsmGroupRec phsmGroupRec)
{
	int				ret;
	int				lenOfPin;
	int				mode = 1;
	char				plainPin[32];
	char				tmpBuf[32];
	char				accNo[64];
	char				pinBlock[64];
	char				keyName[160];
	char				format[32];
	TUnionSymmetricKeyDB		zpkKeyDB;
	PUnionSymmetricKeyValue		pzpkKeyValue = NULL;
	
	//add by zhouxw	20151029
	int				lenOfAccNo = 0;
	char				tmpAccNo[16];
	//add end
	
	// PIN����
	if ((ret = UnionReadRequestXMLPackageValue("body/plainPin",plainPin,sizeof(plainPin))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE140:: UnionReadRequestXMLPackageValue[%s]!\n","body/plainPin");
		return(ret);
	}
	
	//���PIN����
	UnionFilterHeadAndTailBlank(plainPin);
	// 20150910 zhangyd 
	lenOfPin = strlen(plainPin);
	if (lenOfPin == 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE140:: plainPin can not be null!\n");
                UnionSetResponseRemark("PIN���Ĳ���Ϊ��!");
                return(errCodeParameter);
	}

	if (plainPin[lenOfPin - 1] == 'F')
		plainPin[--lenOfPin] = 0;
	if (!UnionIsDigitString(plainPin))
	{
		UnionUserErrLog("in UnionDealServiceCodeE140:: plainPin is error!\n");
		UnionSetResponseRemark("PIN���ķǷ�,����Ϊ����");
		return(errCodeHsmCmdMDL_InvalidPinInputData);
	}
	
	// �˺�
	if ((ret = UnionReadRequestXMLPackageValue("body/accNo",accNo,sizeof(accNo))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE140:: UnionReadRequestXMLPackageValue[%s]!\n","body/accNo");
		return(ret);
	}
	
	// ����˺�
	UnionFilterHeadAndTailBlank(accNo);
	
	//modify by zhouxw 20151029
        //if (strlen(accNo) == 0)
	if ((lenOfAccNo = strlen(accNo)) == 0)
	//modify end
        {
                UnionUserErrLog("in UnionDealServiceCodeE140:: accNo can not be null!\n");
                UnionSetResponseRemark("�˺Ų���Ϊ��!");
                return(errCodeParameter);
        }
	// add by zhouxw 20151029
	// �������֤��Ϊ�˺�ʱ�����һλΪ'X'�����񱨴�
	// �˺Ŵ���12λʱ��ָ����Ὣ���һλ�����������ڷ����ڽ����һλ��Ϊ0������޸Ĳ���Ӱ��ԭ�й���
	// modify by zhouxw 20151110
	// ��Ϊ������˺�λ������12λʱ��ȡ12λ��Ч�˺ţ����������ж�
	/*
	if(lenOfAccNo > 12)
                accNo1[lenOfAccNo - 1] = '0';
	*/
	if(lenOfAccNo > 12)
	{
		if((ret = UnionForm12LenAccountNumber(accNo, lenOfAccNo, tmpAccNo)) != 12)
		{
			UnionUserErrLog("in UnionDealServiceCodeE140:: UnionForm12LenAccountNumber err\n");
			UnionSetResponseRemark("ȡ12λ�˺�ʱ����!");
			return(errCodeParameter);
		}
		memcpy(accNo, tmpAccNo, 12);
		accNo[12] = 0;
	}
	// modify end
	//add end
	if (!UnionIsDigitString(accNo))
	{
		UnionUserErrLog("in UnionDealServiceCodeE140:: accNo[%s] is error!\n",accNo);
		UnionSetResponseRemark("�˺ŷǷ�[%s],����Ϊ����",accNo);
		return(errCodeHsmCmdMDL_InvalidAcc);
	}


	// ģʽ
	// 0��LMK02-03��������PIN(����)
	// 1��LMK02-03��������PIN
	// 2��ָ��zpk����pin
	if ((ret = UnionReadRequestXMLPackageValue("body/mode",tmpBuf,sizeof(tmpBuf))) < 0)
		mode = 1;
	else
	{
		if (atoi(tmpBuf) != 1 && atoi(tmpBuf) != 2 && atoi(tmpBuf) != 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE140:: mode[%s] is error!\n",tmpBuf);
                	UnionSetResponseRemark(" ģʽ[%s],����Ϊ0,1����2",tmpBuf);
                	return(errCodeParameter); 
		}
		mode = atoi(tmpBuf);
	}
	
	if (mode == 2)
	{
		// ZPK��Կ����
		if ((ret = UnionReadRequestXMLPackageValue("body/keyName",keyName,sizeof(keyName))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE140:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName");
			return(ret);
		}
		keyName[ret] = 0;

		UnionFilterHeadAndTailBlank(keyName);
		if (strlen(keyName) == 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE140:: keyName can not be null!\n");
			UnionSetResponseRemark("��Կ���Ʋ���Ϊ��");
			return(errCodeParameter);
		}
	
		// ��ǰ����format�������ݻᱨ�� , �޸����=0������� bychengj 20160721 	
		if ((ret = UnionReadRequestXMLPackageValue("body/format",format,sizeof(format))) <= 0)
			strcpy(format,"01");
		//end

		if ((ret =  UnionReadSymmetricKeyDBRec(keyName,1,&zpkKeyDB)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeE140:: UnionReadSymmetricKeyDBRec keyName[%s]!\n",keyName);
			return(ret);
		}
		if (zpkKeyDB.keyType != conZPK)
		{
			UnionUserErrLog("in UnionDealServiceCodeE140:: is not zpk [%s]!\n",keyName);
			return(errCodeEsscMDL_WrongUsageOfKey);
		}

		// ��ȡ��Կֵ
		if ((pzpkKeyValue = UnionGetSymmetricKeyValue(&zpkKeyDB,phsmGroupRec->lmkProtectMode)) == NULL)
		{
			UnionUserErrLog("in UnionDealServiceCodeE140:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
			UnionSetResponseRemark("��ȡ��Կֵʧ��");
			return(errCodeParameter);
		}
	}
	

	if (mode == 1 || (zpkKeyDB.algorithmID == conSymmetricAlgorithmIDOfDES && mode == 2))
	{
		switch(phsmGroupRec->hsmCmdVersionID)
		{
			case conHsmCmdVerRacalStandardHsmCmd:
			case conHsmCmdVerSJL06StandardHsmCmd:
				if ((ret = UnionHsmCmdBA(lenOfPin+1,lenOfPin,plainPin,accNo,strlen(accNo),pinBlock)) < 0)
				{  
					UnionUserErrLog("in UnionDealServiceCodeE140:: UnionHsmCmdBA!\n");
					return(ret);
				}
				break;
			default:
				UnionUserErrLog("in UnionDealServiceCodeE140:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
				UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
				return(errCodeParameter);
		}
	}
	else if (mode == 0 || (zpkKeyDB.algorithmID == conSymmetricAlgorithmIDOfSM4 && mode == 2))
	{
		switch(phsmGroupRec->hsmCmdVersionID)
		{
			case	conHsmCmdVerRacalStandardHsmCmd:
			case	conHsmCmdVerSJL06StandardHsmCmd:
				if ((ret = UnionHsmCmdWW(0,plainPin,accNo,strlen(accNo),pinBlock,sizeof(pinBlock),NULL,0)) < 0)
				{
					UnionUserErrLog("in UnionDealServiceCodeE140:: UnionHsmCmdWW!\n");
					return(ret);
				}
				break;
			default:	
				UnionUserErrLog("in UnionDealServiceCodeE140:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
				UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
				return(errCodeParameter);
		}
	}
	
	switch(mode)
	{
		case	0:		//LMK02-03��������PIN(����)
			break;
		case	1:		//LMK02-03��������PIN
			break;
		case	2:		// ָ��zpk����pin
			switch(zpkKeyDB.algorithmID)
			{
				case	conSymmetricAlgorithmIDOfDES:	// DES�㷨
					switch(phsmGroupRec->hsmCmdVersionID)
					{
						case	conHsmCmdVerRacalStandardHsmCmd:
						case	conHsmCmdVerSJL06StandardHsmCmd:
							if ((ret = UnionHsmCmdJG(zpkKeyDB.keyLen/4,pzpkKeyValue->keyValue,format,lenOfPin+1,pinBlock,accNo,strlen(accNo),pinBlock)) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE140:: UnionHsmCmdJG ret = [%d]!\n",ret);
								return(ret);
							}
							break;
						default:	
							UnionUserErrLog("in UnionDealServiceCodeE140:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
							UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
							return(errCodeParameter);
					}
					break;
				case	conSymmetricAlgorithmIDOfSM4:	// SM4�㷨
					switch(phsmGroupRec->hsmCmdVersionID)
					{
						case	conHsmCmdVerRacalStandardHsmCmd:
						case	conHsmCmdVerSJL06StandardHsmCmd:
							if ((ret = UnionHsmCmdWX(0,pzpkKeyValue->keyValue,pinBlock,format,accNo,strlen(accNo),pinBlock,sizeof(pinBlock))) < 0)
							{
								UnionUserErrLog("in UnionDealServiceCodeE140:: UnionHsmCmdWX ret = [%d]!\n",ret);
								return(ret);
							}
							break;
						default:	
							UnionUserErrLog("in UnionDealServiceCodeE140:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
							UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
							return(errCodeParameter);
					}
					break;
				default:
					UnionUserErrLog("in UnionDealServiceCodeE140:: zpkKeyDB.algorithmID[%d] error!\n",zpkKeyDB.algorithmID);
					return(errCodeEsscMDL_InvalidAlgorithmID);
			}
			break;
		default:
			UnionUserErrLog("in UnionDealServiceCodeE140:: mode[%d] error!\n",mode);
			return(errCodeParameter);
	}
	// ����PIN����
	if ((ret = UnionSetResponseXMLPackageValue("body/pinBlock",pinBlock)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE140:: UnionSetResponseXMLPackageValue pinBlock[%s]!\n",pinBlock);
		return(ret);
	}
	
	return(0);
}
