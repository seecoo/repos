//	Author:		������
//	Copyright:	Union Tech. Guangzhou
//	Date:		2013-01-10

/*
	�޸���:		leipp
	�޸�����:	֧�ֹ��ܸ���
	�޸�ʱ��:	2016-03-05
*/

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "commWithHsmSvr.h"
#include "unionRealBaseDB.h"
#include "UnionLog.h"
#include "essc5Interface.h"
#include "UnionStr.h"
#include "symmetricKeyDB.h"
#include "unionHsmCmd.h"
#include "unionHsmCmdVersion.h"

/*************************************** 
�������:	E203
������:  	E203 �ַ�PINת��(ZPK->ZPK)
��������:       E203 �ַ�PINת��(ZPK->ZPK)
***************************************/
int UnionDealServiceCodeE203(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret;

	char 	pinBlock[80]; 	//PIN����
	char	srcNo[48];		//ԭ�˺�
	char	dstNo[48];		//Ŀ���˺�
	char	zpkName1[136];
	char	zpkName2[136];
	char	format1[8];
	char	format2[8];
	char	flag[8] = "N";

	char	pin[80];
	char	tmpBuf[32];

	int	lenOfPin = 0;	

	TUnionSymmetricKeyDB    srcZpk;
	TUnionSymmetricKeyDB    dstZpk;
	PUnionSymmetricKeyValue		psrcKeyValue = NULL;
	PUnionSymmetricKeyValue		pdstKeyValue = NULL;

	memset(pinBlock,	0,	sizeof(pinBlock));
	memset(srcNo,		0,	sizeof(srcNo));

	memset(dstNo, 		0,	sizeof(dstNo));
	memset(pin,		0,	sizeof(pin));

	memset(&srcZpk,		0,	sizeof(srcZpk));
	memset(&dstZpk,		0,	sizeof(dstZpk));

	if ((ret = UnionReadRequestXMLPackageValue("body/pinBlock", pinBlock, sizeof(pinBlock)))< 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE203:: UnionReadRequestXMLPackageValue[%s]!\n","body/pinBlock");
                return(ret);
        }

	// add by leipp 20160304
	if ((ret = UnionReadRequestXMLPackageValue("body/format1", format1, sizeof(format1))) > 0)
        {
		format1[ret] = 0;
		if ((ret = UnionReadRequestXMLPackageValue("body/format2", format2, sizeof(format2))) > 0)
		{
			snprintf(flag,sizeof(flag),"Y");
			format2[ret] = 0;
		}
        }
	// add by leipp 20160304 end

	if ((ret = UnionReadRequestXMLPackageValue("body/accNo1", srcNo, sizeof(srcNo)))< 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE203:: UnionReadRequestXMLPackageValue[%s]!\n","body/accNo1");
                return(ret);
        }
	
	if ((ret = UnionReadRequestXMLPackageValue("body/accNo2", dstNo, sizeof(dstNo)))< 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE203:: UnionReadRequestXMLPackageValue[%s]!\n","body/accNo1");
                return(ret);
        }

	// ����˺�
	UnionFilterHeadAndTailBlank(srcNo);
        if (strlen(srcNo) == 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE203:: srcNo can not be null!\n");
                UnionSetResponseRemark("Դ�˺Ų���Ϊ��!");
                return(errCodeParameter);
        }
	if (!UnionIsDigitString(srcNo))
	{
		UnionUserErrLog("in UnionDealServiceCodeE203:: accNo1[%s] is error!\n",srcNo);
		UnionSetResponseRemark("Դ�˺ŷǷ�[%s],����Ϊ����",srcNo);
		return(errCodeHsmCmdMDL_InvalidAcc);
	}

	// ����˺�
	UnionFilterHeadAndTailBlank(dstNo);
        if (strlen(dstNo) == 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE203:: dstNo can not be null!\n");
                UnionSetResponseRemark("Ŀ���˺Ų���Ϊ��!");
                return(errCodeParameter);
        }
	if (!UnionIsDigitString(dstNo))
	{
		UnionUserErrLog("in UnionDealServiceCodeE203:: accNo2[%s] is error!\n",dstNo);
		UnionSetResponseRemark("Ŀ���˺ŷǷ�[%s],����Ϊ����",dstNo);
		return(errCodeHsmCmdMDL_InvalidAcc);
	}
	
	// modify by lisq 20141216 �˺�û������Ҫ��
	/*
	if((strlen(srcNo) == 16) && (strlen(dstNo)) == 16)	
	{
		strncpy(srcNo, &srcNo[3], 12);	
		srcNo[12] = '\0';
		strncpy(dstNo, &dstNo[3], 12);	
		dstNo[12] = '\0';
	}
	else
	{
                UnionUserErrLog("in UnionDealServiceCodeE203:: srcNo[%s], strlen(srcNo)=[%d], dstNo[%s],strlen(dstNo)=[%d]!\n",
			srcNo, (int)strlen(srcNo), dstNo, (int)strlen(dstNo));
		return -1;
	}
	*/
	
	if ((strlen(srcNo) < 12) || (strlen(dstNo) < 12))
	{
		UnionUserErrLog("in UnionDealServiceCodeE203:: srcNo[%s], strlen(srcNo)=[%d], dstNo[%s],strlen(dstNo)=[%d]!\n", srcNo, (int)strlen(srcNo), dstNo, (int)strlen(dstNo));
		return(errCodeHsmCmdMDL_InvalidAcc);
	}
	// modify by lisq 20141216 end �˺�û������Ҫ��

	if ((ret = UnionReadRequestXMLPackageValue("body/keyName1", zpkName1, sizeof(zpkName1))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE203:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName1");
                return(ret);
	}
	zpkName1[ret] = 0;

	if ((ret = UnionReadRequestXMLPackageValue("body/keyName2", zpkName2, sizeof(zpkName2))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeE203:: UnionReadRequestXMLPackageValue[%s]!\n","body/keyName2");
                return(ret);
	}
	zpkName2[ret] = 0;

	 // ��ȡ�Գ���Կ
        if ((ret =  UnionReadSymmetricKeyDBRec(zpkName1, 1, &srcZpk)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE203:: UnionReadSymmetricKeyDBRec keyName[%s]!\n", zpkName1);
                return(ret);
        }

	// ��ȡ�Գ���Կֵ
	if ((psrcKeyValue = UnionGetSymmetricKeyValue(&srcZpk,phsmGroupRec->lmkProtectMode)) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCodeE203:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
		UnionSetResponseRemark("��ȡ��Կֵʧ��");
		return(errCodeParameter);
	}
	
	 // ��ȡ�Գ���Կ
	if ((ret = UnionReadSymmetricKeyDBRec(zpkName2, 1, &dstZpk)) < 0)
	{
                UnionUserErrLog("in UnionDealServiceCodeE203:: UnionReadSymmetricKeyDBRec keyName[%s]!\n", zpkName2);
                return(ret);
	}
	// ��ȡ�Գ���Կֵ
	if ((pdstKeyValue = UnionGetSymmetricKeyValue(&dstZpk,phsmGroupRec->lmkProtectMode)) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCodeE203:: UnionGetSymmetricKeyValue lmkProtectMode[%s]!\n",phsmGroupRec->lmkProtectMode);
		UnionSetResponseRemark("��ȡ��Կֵʧ��");
		return(errCodeParameter);
	}

	// modify by leipp 20160305
	switch(phsmGroupRec->hsmCmdVersionID)
	{
		case conHsmCmdVerRacalStandardHsmCmd:
		case conHsmCmdVerSJL06StandardHsmCmd:
			if ((ret = UnionHsmCmdH8(srcZpk.algorithmID,psrcKeyValue->keyValue, dstZpk.algorithmID,pdstKeyValue->keyValue, pinBlock,format1,format2, srcNo, dstNo, &lenOfPin, pin)) < 0)
			{
				if ((ret <= errCodeOffsetOfHsmReturnCodeMDL) && (UnionOldVersionSymmetricKeyIsUsed(&srcZpk)))
				{
					if ((ret = UnionHsmCmdH8(srcZpk.algorithmID,psrcKeyValue->oldKeyValue, dstZpk.algorithmID,pdstKeyValue->keyValue, pinBlock,format1,format2, srcNo, dstNo, &lenOfPin, pin)) < 0)
					{
						UnionUserErrLog("in UnionDealServiceCodeE203: UnionHsmCmdH8 old version return[%d]!\n", ret);
						return(ret);
					}
				}
				else
				{
					UnionUserErrLog("in UnionDealServiceCodeE203: UnionHsmCmdH8 return[%d]!\n", ret);
					return(ret);
				}
			}
			break;
		default:
			UnionUserErrLog("in UnionDealServiceCodeE203:: phsmGroupRec->hsmCmdVersionID[%d]\n",phsmGroupRec->hsmCmdVersionID);
			UnionSetResponseRemark("�Ƿ��ļ��ܻ�ָ������");
			return(errCodeParameter);
	}
	// modify end by leipp 20160305
	
	//���� pinLen
	sprintf(tmpBuf, "%02d", lenOfPin);
	tmpBuf[2] = 0;
        if ((ret = UnionSetResponseXMLPackageValue("body/pinLen", tmpBuf)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE203: UnionSetResponseXMLPackageValue[%s]!\n", "body/pinLen");
                return(ret);
        }
	
	//���� pinBlock
        if ((ret = UnionSetResponseXMLPackageValue("body/pinBlock", pin)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeE203: UnionSetResponseXMLPackageValue[%s]!\n", "body/pinBlock");
                return(ret);
        }

	return 0;
}
