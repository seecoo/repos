/*
 * Copyright (c) 2011 Union Tech.
 * All rights reserved.
 *
 * Created on: 2016-01-27
 *   @Author: chenwd
 *   @Version 1.0
 */

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
#include "unionHsmCmd.h"
#include "symmetricKeyDB.h"
#include "asymmetricKeyDB.h"
#include "UnionXOR.h"
#include "unionHsmCmdVersion.h"
/*
���ܣ�ָ��WH��ɢ��Ƭ��Կ���԰�ȫ���ķ�ʽ������SM4��
���������
securityMech		1A	��ȫ����		R��SM4���ܺ�MAC
mode			1H	ģʽ��־		0-������ 1-���ܲ�����MAC 2-���ܲ�ʹ�ü��ܺ����ݼ���У��ֵ
id			1N	����ID			��Կ����ģʽ��PBOC3.0ģʽ 0 = CBCģʽ��ǿ�����X80 1 = ������ָ����ECB 2 = ECBģʽ������������
mkType			3H	����Կ����		109= MK-AC 209= MK-SMI 309= MK-SMC  509= MK-DN
mk			32H	����Կ	
mkIndex			1A+3H	����Կ����	
mkCheckValue		16H	����ԿУ��ֵ		������ʽû�и���
mkDvsNum		1N	��ɢ����		ָ���Ը���Կ��ɢ�Ĵ�����1-3�Σ�
mkDvsData		n*16H	��ɢ����		��ɢ��Ƭ��Կ�����ݣ�����n������ɢ����
pkType			1N	������Կ����		���ܱ��������Ŀ�Ƭ��Կ����Կ���� 0=TK��������Կ�� 1=DK-SMC��ʹ��MK-SMCʵʱ��ɢ���ɵ�����Կ��
pk			32H	������Կ	
pkIndex			1A+3H	������Կ		������Կ����
pkCheckValue		16H	������ԿУ��ֵ		������ԿΪ������ʽû�и���
pkDvsNum		1N	������Կ��ɢ����	������������Կ���͡�Ϊ1ʱ�� ��MK-SMC��ɢ�õ�DK-SMC����ɢ���� ��ΧΪ0-3
pkDvsData		n*16H	������Կ��ɢ����	������������Կ���͡�Ϊ1ʱ�� ������Կ����ɢ���ݣ�����nΪ��������Կ��ɢ������
proKeyFlag		1A	������Կ��ʶ		�Ƿ�Ա�����Կ���й��̼���,����Ҫ���������Կʱ,ʹ�ù�����Կ��������. Y:���������Կ N:�����������Կ ��ѡ��:��û�и���ʱȱʡΪN
proFactor		32H	��������		���������Կ������	��ѡ��:����������Կ��־ΪYʱ�� ʹ�ñ�����Կ����ɢ����Կ�Դ����ݽ��й��̼���õ�������Կ��
ivCbc			32H	IV-CBC			����������ID��Ϊ0ʱ��
encryptFillData		N*16H	�����������		����������ID��Ϊ2ʱ�к���Կ����һ����м���
encryptFillOffset	4H	�����������ƫ����	����������ID��Ϊ2ʱ�н���Կ���Ĳ��뵽����������ݵ�λ����ֵ������0������������ݳ���֮��
ivMac			32H	IV-MAC			������ģʽ��־��Ϊ1ʱ��
macData			N*16H	MAC�������		������ģʽ��־��Ϊ1ʱ�� ����Կ����һ�����MAC���������
macOffset		4H	ƫ����			������ģʽ��־��Ϊ1ʱ�н���Կ���Ĳ��뵽MAC������ݵ�λ����ֵ������0��MAC������ݳ���֮��

���������
mac			16H	MACֵ			������ģʽ��־��Ϊ1ʱ��
criperDataLen		4H	�������ݳ���		�������ݳ���(������16�ı�����������ǰ׺���ȡ���׺���ȡ���Կ���ȵĺ�)
criperData		N*16H	��������		�������������
checkValue		16H	�������ݵ�У��ֵ	������ģʽ��־��Ϊ2ʱ�д���
*/
int UnionDealServiceCodeEB0B(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret = 0,mkIndex = -1,mkDvsNum = 1,len = 0,pkIndex = -1,pkDvsNum = 0,encryptFillOffset = 0,macOffset = 0, criperDataLen = 0;
	char	securityMech[32];
	char	mode[32];
	char	mk[64];
	char	id[32];
	char	mkCheckValue[32];
	char	mkType[32];
	char	mkDvsData[64];
	char	pk[64];
	char	pkCheckValue[32];
	char	pkType[32];
	char	pkDvsData[64];
	char	proKeyFlag[32];
	char	proFactor[64];
	char	ivCbc[64];
	char	encryptFillData[1056];
	char	ivMac[64];
	char	macData[1056];
	char	mac[64];
	char	tmpBuf[64];
	char	criperData[1056];
	char	checkValue[32];
	
	//��ȫ����
	if ((ret = UnionReadRequestXMLPackageValue("body/securityMech",securityMech,sizeof(securityMech))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB0B:: UnionReadRequestXMLPackageValue[%s]!\n","body/securityMech");
		return(ret);
	}
	securityMech[ret] = 0;

	//ģʽ��־
	if ((ret = UnionReadRequestXMLPackageValue("body/mode",mode,sizeof(mode))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB0B:: UnionReadRequestXMLPackageValue[%s]!\n","body/mode");
		return(ret);
	}
	mode[ret] = 0;	

	//����ID
	if ((ret = UnionReadRequestXMLPackageValue("body/id",id,sizeof(id))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB0B:: UnionReadRequestXMLPackageValue[%s]!\n","body/id");
		return(ret);
	}
	id[ret] = 0;	
			
	//����Կ
	memset(mk,0,sizeof(mk));
	if ((ret = UnionReadRequestXMLPackageValue("body/mk",mk,sizeof(mk))) < 0)
	{
		//����Կ����		4A
		if ((ret = UnionReadRequestXMLPackageValue("body/mkIndex",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeEB0B:: UnionReadRequestXMLPackageValue[%s]!\n","body/mk and body/mkIndex");
			return(ret);
		}
		tmpBuf[ret] = 0;
		mkIndex = atoi(tmpBuf);
	}
	else
	{
		//����ԿУ��ֵ	
		if ((ret = UnionReadRequestXMLPackageValue("body/mkCheckValue",mkCheckValue,sizeof(mkCheckValue))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeEB0B:: UnionReadRequestXMLPackageValue[%s]!\n","body/mkCheckValue");
			return(ret);
		}
		mkCheckValue[ret] = 0;
	}

	//����Կ����
	if ((ret = UnionReadRequestXMLPackageValue("body/mkType",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB0B:: UnionReadRequestXMLPackageValue[%s]!\n","body/mkType");
		return(ret);
	}
	tmpBuf[ret] = 0;

	memset(mkType,0,sizeof(mkType));
	UnionTranslateDesKeyTypeTo3CharFormat(UnionConvertSymmetricKeyKeyType(tmpBuf), mkType);

	//��ɢ����
	if ((ret = UnionReadRequestXMLPackageValue("body/mkDvsNum",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB0B:: UnionReadRequestXMLPackageValue[%s]!\n","body/mkDvsNum");
		return(ret);
	}
	tmpBuf[ret] = 0;
	mkDvsNum = atoi(tmpBuf);
	
	if(mkDvsNum < 1 || mkDvsNum > 3)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB0B:: the mkDvsNum not 1 or 2 or 3!\n");
		return errCodePackageDefMDL_InvalidMaxFldNum;
	}
	
	//��ɢ����	
	if ((len = UnionReadRequestXMLPackageValue("body/mkDvsData",mkDvsData,sizeof(mkDvsData))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB0B:: UnionReadRequestXMLPackageValue[%s]!\n","body/mkDvsData");
		return(ret);
	}
	mkDvsData[len] = 0;
	
	if((len/16) != mkDvsNum)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB0B:: UnionReadRequestXMLPackageValue mkDvsData[%s] len[%d] != %d*16!\n",mkDvsData,len,mkDvsNum);
		return(-1);
	}	
	
	//������Կ
	memset(pk,0,sizeof(pk));
	memset(pkCheckValue,0,sizeof(pkCheckValue));
	if ((ret = UnionReadRequestXMLPackageValue("body/pk",pk,sizeof(pk))) < 0)
	{
		//������Կ����
		if ((ret = UnionReadRequestXMLPackageValue("body/mkIndex",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeEB0B:: UnionReadRequestXMLPackageValue[%s]!\n","body/mk and body/mkIndex");
			return(ret);
		}
		tmpBuf[ret] = 0;
		mkIndex = atoi(tmpBuf);
	}
	else
	{
		//������ԿУ��ֵ
		if ((ret = UnionReadRequestXMLPackageValue("body/pkCheckValue",pkCheckValue,sizeof(pkCheckValue))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeEB0B:: UnionReadRequestXMLPackageValue[%s]!\n","body/pkCheckValue");
			return(ret);
		}
		pkCheckValue[ret] = 0;
	}

	//������Կ����
	if ((ret = UnionReadRequestXMLPackageValue("body/pkType",pkType,sizeof(pkType))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB0B:: UnionReadRequestXMLPackageValue[%s]!\n","body/pkType");
		return(ret);
	}
	pkType[ret] = 0;

	if(strcmp(pkType,"1") == 0)
	{
		//������Կ��ɢ����
		if ((ret = UnionReadRequestXMLPackageValue("body/pkDvsNum",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeEB0B:: UnionReadRequestXMLPackageValue[%s]!\n","body/pkDvsNum");
			return(ret);
		}
		tmpBuf[ret] = 0;
		pkDvsNum = atoi(tmpBuf);
		
		if(pkDvsNum < 0 || pkDvsNum > 3)
		{
			UnionUserErrLog("in UnionDealServiceCodeEB0B:: the pkDvsNum not 1 or 2 or 3!\n");
			return errCodePackageDefMDL_InvalidMaxFldNum;
		}
		
		if(pkDvsNum > 0)
		{
			//������Կ��ɢ����	
			if ((len = UnionReadRequestXMLPackageValue("body/pkDvsData",pkDvsData,sizeof(pkDvsData))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeEB0B:: UnionReadRequestXMLPackageValue[%s]!\n","body/pkDvsData");
				return(len);
			}
			pkDvsData[len] = 0;
			
			if((len/16) != pkDvsNum)
			{
				UnionUserErrLog("in UnionDealServiceCodeEB0B:: UnionReadRequestXMLPackageValue mkDvsData[%s] len[%d] != %d*16!\n",pkDvsData,len,pkDvsNum);
				return(-1);
			}
		}
	}

	//������Կ��ʶ
	memset(proKeyFlag,0,sizeof(proKeyFlag));
	if ((ret = UnionReadRequestXMLPackageValue("body/proKeyFlag",proKeyFlag,sizeof(proKeyFlag))) < 0)
	{
		strcpy(proKeyFlag,"N");
	}
	else if((strcmp(proKeyFlag,"Y") != 0)  && (strcmp(proKeyFlag,"N") != 0))
	{
		UnionUserErrLog("in UnionDealServiceCodeEB0B:: UnionReadRequestXMLPackageValue[%s][%s] != Y or != N !\n","body/proKeyFlag",proKeyFlag);
		return -1;
	}
	
	memset(proFactor,0,sizeof(proFactor));
	if(strcmp(proKeyFlag,"Y") == 0)
	{
		//��������
		if ((ret = UnionReadRequestXMLPackageValue("body/proFactor",proFactor,sizeof(proFactor))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeEB0B:: UnionReadRequestXMLPackageValue[%s]!\n","body/proFactor");
			return(ret);
		}
	}
	
	memset(ivCbc,0,sizeof(ivCbc));
	memset(encryptFillData,0,sizeof(encryptFillData));
	if(strcmp(id,"0") == 0)
	{
		//ivCbc	
		if ((ret = UnionReadRequestXMLPackageValue("body/ivCbc",ivCbc,sizeof(ivCbc))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeEB0B:: UnionReadRequestXMLPackageValue[%s]!\n","body/ivCbc");
			return(ret);
		}
	}
	else if(strcmp(id,"2") == 0)
	{
		//�����������
		if ((ret = UnionReadRequestXMLPackageValue("body/encryptFillData",encryptFillData,sizeof(encryptFillData))) > 0)
		{
			//�����������ƫ����
			if ((ret = UnionReadRequestXMLPackageValue("body/encryptFillOffset",tmpBuf,sizeof(tmpBuf))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCodeEB0B:: UnionReadRequestXMLPackageValue[%s]!\n","body/encryptFillOffset");
				return(ret);
			}
			tmpBuf[ret] = 0;
			encryptFillOffset = atoi(tmpBuf);
		}
	}
				
	memset(ivMac,0,sizeof(ivMac));
	memset(macData,0,sizeof(macData));
	if(strcmp(mode,"1") == 0)
	{
		//IV-MAC	
		if ((ret = UnionReadRequestXMLPackageValue("body/ivMac",ivMac,sizeof(ivMac))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeEB0B:: UnionReadRequestXMLPackageValue[%s]!\n","body/ivMac");
			return(ret);
		}
		
		//MAC�������	
		if ((ret = UnionReadRequestXMLPackageValue("body/macData",macData,sizeof(macData))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeEB0B:: UnionReadRequestXMLPackageValue[%s]!\n","body/macData");
			return(ret);
		}
		
		//ƫ����
		if ((ret = UnionReadRequestXMLPackageValue("body/macOffset",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeEB0B:: UnionReadRequestXMLPackageValue[%s]!\n","body/macOffset");
			return(ret);
		}
		tmpBuf[ret] = 0;
		macOffset = atoi(tmpBuf);
	}
	
	memset(mac,0,sizeof(mac));
	memset(criperData,0,sizeof(criperData));
	memset(checkValue,0,sizeof(checkValue));
	if ((ret = UnionHsmCmdWH(securityMech, mode, id, mkType, mk, mkIndex, mkCheckValue, mkDvsNum, mkDvsData, pkType, pk, pkIndex, pkDvsNum, pkDvsData, pkCheckValue, proKeyFlag, proFactor, ivCbc, strlen(encryptFillData), encryptFillData, encryptFillOffset, ivMac, strlen(macData), macData, macOffset, mac, &criperDataLen, criperData, checkValue)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB0B:: UnionHsmCmdWH err!\n");
		return(ret);
	}

	if(strcmp(mode,"1") == 0)
	{
		// ����MACֵ
		if ((ret = UnionSetResponseXMLPackageValue("body/mac",mac)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeEB0B:: UnionSetResponseXMLPackageValue newSeed[%s]!\n",mac);
			return(ret);
		}
	}
	else if(strcmp(mode,"2") == 0)
	{
		// ���ؼ������ݵ�У��ֵ
		if ((ret = UnionSetResponseXMLPackageValue("body/checkValue",checkValue)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCodeEB0B:: UnionSetResponseXMLPackageValue checkValue[%s]!\n",checkValue);
			return(ret);
		}
	}

        len = sprintf(tmpBuf,"%d",criperDataLen);
        tmpBuf[len] = 0;
        // �����������ݳ���	
        if ((ret = UnionSetResponseXMLPackageValue("body/criperDataLen",tmpBuf)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCodeEB0B:: UnionSetResponseXMLPackageValue criperDataLen[%s]!\n",tmpBuf);
                return(ret);
        }

	// ������������
	if ((ret = UnionSetResponseXMLPackageValue("body/criperData",criperData)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCodeEB0B:: UnionSetResponseXMLPackageValue criperData[%s]!\n",criperData);
		return(ret);
	}
	
	return(0);
}
