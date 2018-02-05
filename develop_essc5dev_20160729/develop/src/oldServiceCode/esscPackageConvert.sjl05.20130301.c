#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include "unionREC.h"
#include "UnionStr.h"
#include "UnionLog.h"
#include "unionErrCode.h"

#include "esscPackage.h"
#include "unionRealBaseDB.h"
#include "unionXMLPackage.h"
#include "symmetricKeyDB.h"
#include "unionTableData.h"
#include "accessCheck.h"


// ���� SJL05 ָ�ת��Ϊ��Ӧ�ķ�����
static int UnionConvertSJL05CmdToServer(unsigned char *order, int lenOfOrder, char *serverCode)
{
	if (memcmp(order, "\x01", 1) == 0)		// ���ر�������Կ״̬<0X01>
	{
		memcpy(serverCode, "E100", 5);
	}
	else if (lenOfOrder > 1 && memcmp(order, "\x04\x06", 2) == 0)	// PINBLOCKת�������ⳤ��MMK����ʽ��<0X0406>
	{
		memcpy(serverCode, "EEP1", 5);
	}
	else if (memcmp(order, "\x81", 1) == 0)		// ��֤MAC<0X81>
	{
		memcpy(serverCode, "E151", 5);
	}
	else if (memcmp(order, "\x72", 1) == 0)		// ����������Կ�����ݼ�/����<0X72>
	{
		if (lenOfOrder > 11 && order[11] == 0x00) // ����
			memcpy(serverCode, "E161", 5);
		else if (lenOfOrder > 11 && order[11] == 0x01) // ����
			memcpy(serverCode, "E160", 5);
		else
		{
			UnionUserErrLog("in UnionConvertSJL05CmdToServer:: order[%s] error!\n", order);
			return(errCodeSJL05MDL_HsmCmdFormatError);
		}
	}
	else if (memcmp(order, "\x10", 1) == 0)		// ����һ��������Կ������BMK���ܺ󷵻�<0X10>
	{
		memcpy(serverCode, "E110", 5);
	}
	else
	{
		if (lenOfOrder == 1)
			UnionUserErrLog("in UnionConvertSJL05CmdToServer:: order[0x%02X] not support!\n", order[0]);
		else
			UnionUserErrLog("in UnionConvertSJL05CmdToServer:: order[0x%02X] or [0x%02X%02X] not support!\n", order[0], order[0], order[1]);
		return(errCodeSJL05MDL_HsmCmdFormatError);
	}
	return(4);
}

int UnionSetBodyOfSJL05Cmd0x01(unsigned char *buf, int lenOfBuf)
{
	return(0);
}

static int UnionSetBodyOfSJL05Cmd0x0406(unsigned char *buf, int lenOfBuf)
{
	int	ret;
	int	offset = 0;
	unsigned char	ascBuf[8192];

	int	length = 0;
	char	index1[16] ={0};
	char	pik1Len[16] ={0};
	char	pik1[256] = {0};
	char	index2[16] = {0};
	char	pik2Len[16] = {0};
	char	pik2[256] = {0};
	char	pinblockType1[16] = {0};
	char	pinblockType2[16] = {0};
	char	pinblock[32] = {0};
	char	accNo1[40] = {0};
	char	accNo2[40] = {0};
	char	fillCode[32] = {0};
	char	*ptr1 = NULL;
	char	*ptr2 = NULL;
	
	
	if (lenOfBuf >= sizeof(ascBuf) || lenOfBuf < 34)
	{
		UnionUserErrLog("in UnionSetBodyOfSJL05Cmd0x0406 ascBuf len[%zd] is small then lenOfBuf[%d] or lenOfBuf[%d] not enough len[34]\n", sizeof(ascBuf), lenOfBuf, lenOfBuf);
		return(errCodeSJL05MDL_HsmCmdFormatError);
	}

	bcdhex_to_aschex((char *)buf, lenOfBuf, (char *)ascBuf);
	ascBuf[lenOfBuf * 2] = 0;
	
	// ��������ͷ 4 �ַ�
	offset = 4;

	//��ȡת��ǰ��������Կ������ת����Կ����
	memcpy(index1,ascBuf + offset,4);
	offset += 4;
	if ((ret = UnionSetRequestXMLPackageValue("body/index1",index1)) < 0)
        {
                UnionUserErrLog("in UnionSetBodyOfSJL05Cmd0x0406:: UnionSetRequestXMLPackageValue [body/index1]!\n");
                return(errCodeSJL05MDL_HsmCmdFormatError);
        }   

	//��ȡpik1��Կ����
	memcpy(pik1Len,ascBuf + offset,2);
	offset += 2;
	memcpy(pik1,ascBuf + offset,(int)strtol(pik1Len, NULL, 16) * 2);
	offset += (int)strtol(pik1Len, NULL, 16) * 2;
	if ((ret = UnionSetRequestXMLPackageValue("body/pik1", pik1)) < 0)
        {
                UnionUserErrLog("in UnionSetBodyOfSJL05Cmd0x0406:: UnionSetRequestXMLPackageValue [body/pik1]!\n");
                return(errCodeSJL05MDL_HsmCmdFormatError);
        }   

	//��ȡת������������Կ����
	memcpy(index2,ascBuf + offset,4);
	offset += 4;
	if ((ret = UnionSetRequestXMLPackageValue("body/index2",index2)) < 0)
        {
                UnionUserErrLog("in UnionSetBodyOfSJL05Cmd0x0406:: UnionSetRequestXMLPackageValue [body/index2]!\n");
                return(errCodeSJL05MDL_HsmCmdFormatError);
        }   

	//��ȡpik2��Կ����
	memcpy(pik2Len,ascBuf + offset,2);
	offset += 2;
	memcpy(pik2,ascBuf + offset,(int)strtol(pik2Len, NULL, 16) * 2);
	offset +=(int)strtol(pik2Len, NULL, 16) * 2;
	if ((ret = UnionSetRequestXMLPackageValue("body/pik2", pik2)) < 0)
        {
                UnionUserErrLog("in UnionSetBodyOfSJL05Cmd0x0406:: UnionSetRequestXMLPackageValue [body/pik2]!\n");
                return(errCodeSJL05MDL_HsmCmdFormatError);
        }   

	//��ȡת��ǰPINBLOCK��ʽ
	memcpy(pinblockType1,ascBuf + offset,2);
	offset += 2;
	if ((ret = UnionSetRequestXMLPackageValue("body/pinblockType1", pinblockType1)) < 0)
        {
                UnionUserErrLog("in UnionSetBodyOfSJL05Cmd0x0406:: UnionSetRequestXMLPackageValue [body/pinblockType1]!\n");
                return(errCodeSJL05MDL_HsmCmdFormatError);
        }   

	//��ȡת�����PINBLOCK��ʽ
	memcpy(pinblockType2,ascBuf + offset,2);
	offset += 2;
	if ((ret = UnionSetRequestXMLPackageValue("body/pinblockType2", pinblockType2)) < 0)
        {
                UnionUserErrLog("in UnionSetBodyOfSJL05Cmd0x0406:: UnionSetRequestXMLPackageValue [body/pinblockType2]!\n");
                return(errCodeSJL05MDL_HsmCmdFormatError);
        }   

	//��ȡת��ǰ��PINBLOCK
	memcpy(pinblock,ascBuf + offset,16);
	offset += 16;
	if ((ret = UnionSetRequestXMLPackageValue("body/pinblock", pinblock)) < 0)
        {
                UnionUserErrLog("in UnionSetBodyOfSJL05Cmd0x0406:: UnionSetRequestXMLPackageValue [body/pinblock]!\n");
                return(errCodeSJL05MDL_HsmCmdFormatError);
        }   

	//�ж��Ƿ����ת��ǰ���˺�
	//if((atoi(pinblockType1) == 1 || atoi(pinblockType1) == 4) && (memcmp(pinblockType1,pinblockType2,sizeof(pinblockType1)) != 0))
	if(atoi(pinblockType1) == 1 || atoi(pinblockType1) == 4)
	{
		//��ȡת��ǰ���˺� 
		if((ptr1 = strstr((char *)(ascBuf + offset),"3B")) !=NULL)
		{
			length = ptr1 - (char *)(ascBuf + offset);
			memcpy(accNo1,ascBuf + offset,length);
			offset += (length + 2);    //���ָ��;�ĳ��������
			if ((ret = UnionSetRequestXMLPackageValue("body/accNo1", accNo1)) < 0)
			{
				UnionUserErrLog("in UnionSetBodyOfSJL05Cmd0x0406:: UnionSetRequestXMLPackageValue [body/accNo1]!\n");
				return(errCodeSJL05MDL_HsmCmdFormatError);
			}   
		}
		else
		{
			UnionUserErrLog("in UnionSetBodyOfSJL05Cmd0x0406:: accNo1 error!\n");
			return(errCodeSJL05MDL_HsmCmdFormatError);
		}
	}
	if(atoi(pinblockType2) == 1 || atoi(pinblockType2) == 4)
	{
		//��ȡת��ǰ���˺�  
		if((ptr2 = strstr((char*)(ascBuf + offset),"3B")) != NULL)
		{
			length = ptr2 - (char *)(ascBuf + offset);
			memcpy(accNo2,ascBuf + offset,length);
			offset += (length + 2);    //���ָ��;�ĳ��������
			if ((ret = UnionSetRequestXMLPackageValue("body/accNo2", accNo2)) < 0)
			{
				UnionUserErrLog("in UnionSetBodyOfSJL05Cmd0x0406:: UnionSetRequestXMLPackageValue [body/accNo2]!\n");
				return(errCodeSJL05MDL_HsmCmdFormatError);
			}   
		}
		else
		{
			UnionUserErrLog("in UnionSetBodyOfSJL05Cmd0x0406:: accNo1 error!\n");
			return(errCodeSJL05MDL_HsmCmdFormatError);
		}
	}
	//if((atoi(pinblockType2) == 2) && (strcmp(pinblockType1,pinblockType2,sizeof(pinblockType2)) != 0))
	if(atoi(pinblockType2) == 2)
	{
		memcpy(fillCode,ascBuf + offset,12);
		offset += 12;
		offset += 2;       //���;�ĳ���                   
		if ((ret = UnionSetRequestXMLPackageValue("body/fillCode", fillCode)) < 0)
		{
			UnionUserErrLog("in UnionSetBodyOfSJL05Cmd0x0406:: UnionSetRequestXMLPackageValue [body/fillCode]!\n");
			return(errCodeSJL05MDL_HsmCmdFormatError);
		}   
	}

	return(0);
	/*
	int	ret;
	unsigned char	ascBuf[8192];

	if (lenOfBuf >= sizeof(ascBuf) || lenOfBuf < 34)
	{
		UnionUserErrLog("in UnionSetBodyOfSJL05Cmd0x0406 ascBuf len[%zd] is small then lenOfBuf[%d] or lenOfBuf[%d] not enough len[34]\n", sizeof(ascBuf), lenOfBuf, lenOfBuf);
		return(errCodeSJL05MDL_HsmCmdFormatError);
	}

	bcdhex_to_aschex((char *)buf, lenOfBuf, (char *)ascBuf);
	ascBuf[lenOfBuf * 2] = 0;
	if ((ret = UnionSetRequestXMLPackageValue("body/requestOfSjl05",(char *)ascBuf)) < 0)
        {
                UnionUserErrLog("in UnionSetBodyOfSJL05Cmd0x0406:: UnionSetRequestXMLPackageValue [body/requestOfSjl05]!\n");
                return(errCodeSJL05MDL_HsmCmdFormatError);
        }   
	return(0);
	*/
}

static int UnionSetBodyOfSJL05Cmd0x81(unsigned char *buf, int lenOfBuf)
{
	int	ret;
	int	dataLen;
	char	zmkName[128];
	char	tmpBuf[128];
	char	data[8192];

	if (lenOfBuf < 17)
	{
                UnionUserErrLog("in UnionSetBodyOfSJL05Cmd0x81:: lenOfBuf[%d] mast bigger then 17!\n", lenOfBuf);
                return(errCodeSJL05MDL_HsmCmdFormatError);
	}

	//modeΪ��ѡ��Ĭ��Ϊ1
	if ((ret = UnionSetRequestXMLPackageValue("body/mode","2")) < 0)
        {
                UnionUserErrLog("in UnionSetBodyOfSJL05Cmd0x81:: UnionSetRequestXMLPackageValue [body/mode]!\n");
                return(errCodeSJL05MDL_HsmCmdFormatError);
        }   

        //ƴװkeyName��modeΪ1ʱ��Ϊzak��Կ����  modeΪ2ʱ��Ϊzmk��Կ����
	bcdhex_to_aschex((char *)(buf + 9), 2, tmpBuf);
	tmpBuf[4] = 0;
	snprintf(zmkName, sizeof(zmkName), "ATM.%sSJL05.zmk", tmpBuf);
	if ((ret = UnionSetRequestXMLPackageValue("body/keyName", zmkName)) < 0)
        {
                UnionUserErrLog("in UnionSetBodyOfSJL05Cmd0x81:: UnionSetRequestXMLPackageValue [body/keyName]!\n");
                return(errCodeSJL05MDL_HsmCmdFormatError);
        }   

        //keyValue��ѡ��modeΪ2�Ǵ��ڣ���ZMK����
	bcdhex_to_aschex((char *)(buf + 1), 8, tmpBuf);
	tmpBuf[16] = 0;
	if ((ret = UnionSetRequestXMLPackageValue("body/keyValue", tmpBuf)) < 0)
        {
                UnionUserErrLog("in UnionSetBodyOfSJL05Cmd0x81:: UnionSetRequestXMLPackageValue [body/keyValue]!\n");
                return(errCodeSJL05MDL_HsmCmdFormatError);
        }   

        // algorithmID��ѡ��Ĭ��Ϊ1, 1��ANSIX9.19 2���й�������׼ 3������POS��׼

	// fillMode �Զ���䷽ʽ  ������䷽ʽ��������ʱĬ��Ϊ1�� 1�����0x00��������鵽����

	// dataType �������� ��ѡ��Ĭ��Ϊ2 1��asc�ַ� 2��ʮ��������
	if ((ret = UnionSetRequestXMLPackageValue("body/dataType", "2")) < 0)
        {
                UnionUserErrLog("in UnionSetBodyOfSJL05Cmd0x81:: UnionSetRequestXMLPackageValue [body/dataType]!\n");
                return(errCodeSJL05MDL_HsmCmdFormatError);
        }   

	// ����
	bcdhex_to_aschex((char *)(buf + 15), 2, tmpBuf);
	tmpBuf[4] = 0;
	dataLen = (int)strtol(tmpBuf, NULL, 16);
	if (dataLen / 2 >= sizeof(data) || dataLen != lenOfBuf - 17)
	{
		UnionUserErrLog("in UnionSetBodyOfSJL05Cmd0x81:: data len[%d] is small or request string len[%d] not equal [%d]!\n", (int)sizeof(data), lenOfBuf, dataLen + 17);
		return(errCodeSJL05MDL_HsmCmdFormatError);
	}
	bcdhex_to_aschex((char *)(buf + 17), dataLen, data);
	data[dataLen * 2] = 0;
	if ((ret = UnionSetRequestXMLPackageValue("body/data", data)) < 0)
        {
                UnionUserErrLog("in UnionSetBodyOfSJL05Cmd0x81:: UnionSetRequestXMLPackageValue [body/data]!\n");
                return(errCodeSJL05MDL_HsmCmdFormatError);
        }   

	// mac
	bcdhex_to_aschex((char *)(buf + 11), 4, tmpBuf);
	tmpBuf[8] = 0;
	if ((ret = UnionSetRequestXMLPackageValue("body/mac", tmpBuf)) < 0)
        {
                UnionUserErrLog("in UnionSetBodyOfSJL05Cmd0x81:: UnionSetRequestXMLPackageValue [body/mac]!\n");
                return(errCodeSJL05MDL_HsmCmdFormatError);
        }   

	return(0);
}

static int UnionSetBodyOfSJL05Cmd0x72(unsigned char *buf, int lenOfBuf)
{
	int	ret;
	int	dataLen;
	char	zekName[128];
	char	tmpBuf[128];
	char	data[8192];


	if ((ret = UnionSetRequestXMLPackageValue("body/mode", "1")) < 0)
        {
                UnionUserErrLog("in UnionSetBodyOfSJL05Cmd0x72:: UnionSetRequestXMLPackageValue [body/mode]!\n");
                return(errCodeSJL05MDL_HsmCmdFormatError);
        }   

	//ƴװ��Կ����keyName
	bcdhex_to_aschex((char *)(buf + 1), 2, tmpBuf);
	tmpBuf[4] = 0;
	snprintf(zekName,sizeof(zekName),"ATM.%sSJL05.zek",tmpBuf);
	if ((ret = UnionSetRequestXMLPackageValue("body/keyName", zekName)) < 0)
        {
                UnionUserErrLog("in UnionSetBodyOfSJL05Cmd0x72:: UnionSetRequestXMLPackageValue [body/keyName]!\n");
                return(errCodeSJL05MDL_HsmCmdFormatError);
        }   

	//ƴװ��ʼ����
	bcdhex_to_aschex((char *)(buf + 3), 8, tmpBuf);
	tmpBuf[16] = 0;
	if ((ret = UnionSetRequestXMLPackageValue("body/iv", tmpBuf)) < 0)
        {
                UnionUserErrLog("in UnionSetBodyOfSJL05Cmd0x72:: UnionSetRequestXMLPackageValue [body/iv]!\n");
                return(errCodeSJL05MDL_HsmCmdFormatError);
        }   

	//��ȡ�㷨��ʶ
	bcdhex_to_aschex((char *)(buf + 12), 1, tmpBuf);
	tmpBuf[2] = 0;
	if ((tmpBuf[0] != '0') && (tmpBuf[0] != '1') && (tmpBuf[1] != '0') && (tmpBuf[1] != '1'))
	{
		UnionUserErrLog("in UnionSetBodyOfSJL05Cmd0x72:: Algorithm flag error!\n");
		return(errCodeSJL05MDL_HsmCmdFormatError);
	}
	if(tmpBuf[0] == '0') // ECB
	{
		if ((ret = UnionSetRequestXMLPackageValue("body/algorithmID", "0")) < 0)
		{
			UnionUserErrLog("in UnionSetBodyOfSJL05Cmd0x72:: UnionSetRequestXMLPackageValue [body/algorithmID]!\n");
			return(errCodeSJL05MDL_HsmCmdFormatError);
		}   
	}
	else if(tmpBuf[0] == '1') // CBC
	{
		if ((ret = UnionSetRequestXMLPackageValue("body/algorithmID", "1")) < 0)
		{
			UnionUserErrLog("in UnionSetBodyOfSJL05Cmd0x72:: UnionSetRequestXMLPackageValue [body/algorithmID]!\n");
			return(errCodeSJL05MDL_HsmCmdFormatError);
		}   
	}

	// dataType
	if ((ret = UnionSetRequestXMLPackageValue("body/dataType", "1")) < 0)
	{
		UnionUserErrLog("in UnionSetBodyOfSJL05Cmd0x72:: UnionSetRequestXMLPackageValue [body/dataType]!\n");
		return(errCodeSJL05MDL_HsmCmdFormatError);
	}   

	// exportFlag
	if ((ret = UnionSetRequestXMLPackageValue("body/exportFlag", "1")) < 0)
	{
		UnionUserErrLog("in UnionSetBodyOfSJL05Cmd0x72:: UnionSetRequestXMLPackageValue [body/exportFlag]!\n");
		return(errCodeSJL05MDL_HsmCmdFormatError);
	}   
	
	//��ȡ����
	bcdhex_to_aschex((char *)(buf + 13), 2, tmpBuf);
	tmpBuf[4] = 0;
	dataLen = (int)strtol(tmpBuf, NULL, 16);
	if (dataLen / 2 >= (int)sizeof(data))
	{
		UnionUserErrLog("in UnionSetBodyOfSJL05Cmd0x72:: data len[%d] is small!\n", (int)sizeof(data));
		return(errCodeSJL05MDL_HsmCmdFormatError);
	}
	bcdhex_to_aschex((char *)(buf + 15), dataLen, data);
	data[dataLen * 2] = 0;
	if ((ret = UnionSetRequestXMLPackageValue("body/data", data)) < 0)
	{
		UnionUserErrLog("in UnionSetBodyOfSJL05Cmd0x72:: UnionSetRequestXMLPackageValue [body/data]!\n");
		return(errCodeSJL05MDL_HsmCmdFormatError);
	}   

	//ƴװ�������ݸ�ʽ
	if ((ret = UnionSetRequestXMLPackageValue("body/format", "2")) < 0)
	{
		UnionUserErrLog("in UnionSetBodyOfSJL05Cmd0x72:: UnionSetRequestXMLPackageValue [body/format]!\n");
		return(errCodeSJL05MDL_HsmCmdFormatError);
	}   
	return(0);
}

static int UnionSetBodyOfSJL05Cmd0x10(unsigned char *buf, int lenOfBuf)
{
	int	ret;
	char	tmpBuf[128];
	char	protectKey[128];

	if (lenOfBuf != 3)
	{	
                UnionUserErrLog("in UnionSetBodyOfSJL05Cmd0x10:: reqStr len[%d] is not 3!\n", lenOfBuf);
                return(errCodeSJL05MDL_HsmCmdFormatError);
	}
	// keyName	128A	��Կ����
	snprintf(tmpBuf, sizeof(tmpBuf), "%s", "ATM.9999999tmp.zak");
	if ((ret = UnionSetRequestXMLPackageValue("body/keyName", tmpBuf)) < 0)
        {
                UnionUserErrLog("in UnionSetBodyOfSJL05Cmd0x10:: UnionSetRequestXMLPackageValue [body/keyName]!\n");
                return(errCodeSJL05MDL_HsmCmdFormatError);
        }   

	// keyGroup	16A	��Կ��

	//algorithmID	10A	�㷨��ʶ
	snprintf(tmpBuf, sizeof(tmpBuf), "%s", "DES");
	if ((ret = UnionSetRequestXMLPackageValue("body/algorithmID", tmpBuf)) < 0)
        {
                UnionUserErrLog("in UnionSetBodyOfSJL05Cmd0x10:: UnionSetRequestXMLPackageValue [body/algorithmID]!\n");
                return(errCodeSJL05MDL_HsmCmdFormatError);
	}
	
	// keyType	10A	��Կ����
	snprintf(tmpBuf, sizeof(tmpBuf), "%s", "ZAK");
	if ((ret = UnionSetRequestXMLPackageValue("body/keyType", tmpBuf)) < 0)
        {
                UnionUserErrLog("in UnionSetBodyOfSJL05Cmd0x10:: UnionSetRequestXMLPackageValue [body/keyType]!\n");
                return(errCodeSJL05MDL_HsmCmdFormatError);
	}

	// keyLen	3N	��Կ����
	snprintf(tmpBuf, sizeof(tmpBuf), "%s", "64");
	if ((ret = UnionSetRequestXMLPackageValue("body/keyLen", tmpBuf)) < 0)
        {
                UnionUserErrLog("in UnionSetBodyOfSJL05Cmd0x10:: UnionSetRequestXMLPackageValue [body/keyLen]!\n");
                return(errCodeSJL05MDL_HsmCmdFormatError);
	}

	// updateKeyFlag	1N	������Կ��ʶ	Ĭ��Ϊ0�� ��Ϊ1ʱ������Կ�����Ѿ����ڣ��������Կ
	snprintf(tmpBuf, sizeof(tmpBuf), "%s", "1");
	if ((ret = UnionSetRequestXMLPackageValue("body/updateKeyFlag", tmpBuf)) < 0)
        {
                UnionUserErrLog("in UnionSetBodyOfSJL05Cmd0x10:: UnionSetRequestXMLPackageValue [body/updateKeyFlag]!\n");
                return(errCodeSJL05MDL_HsmCmdFormatError);
	}

	// outputFlag	1N	��������ʶ
	snprintf(tmpBuf, sizeof(tmpBuf), "%s", "1");
	if ((ret = UnionSetRequestXMLPackageValue("body/outputFlag", tmpBuf)) < 0)
        {
                UnionUserErrLog("in UnionSetBodyOfSJL05Cmd0x10:: UnionSetRequestXMLPackageValue [body/outputFlag]!\n");
                return(errCodeSJL05MDL_HsmCmdFormatError);
	}

	// effectiveDays	5N	��Ч����
	snprintf(tmpBuf, sizeof(tmpBuf), "%s", "36500");
	if ((ret = UnionSetRequestXMLPackageValue("body/effectiveDays", tmpBuf)) < 0)
        {
                UnionUserErrLog("in UnionSetBodyOfSJL05Cmd0x10:: UnionSetRequestXMLPackageValue [body/effectiveDays]!\n");
                return(errCodeSJL05MDL_HsmCmdFormatError);
	}

	// enabled	1N	���ñ�ʶ
	snprintf(tmpBuf, sizeof(tmpBuf), "%s", "1");
	if ((ret = UnionSetRequestXMLPackageValue("body/enabled", tmpBuf)) < 0)
        {
                UnionUserErrLog("in UnionSetBodyOfSJL05Cmd0x10:: UnionSetRequestXMLPackageValue [body/enabled]!\n");
                return(errCodeSJL05MDL_HsmCmdFormatError);
	}

	// exportFlag	1N �����ʶ
	snprintf(tmpBuf, sizeof(tmpBuf), "%s", "1");
	if ((ret = UnionSetRequestXMLPackageValue("body/exportFlag", tmpBuf)) < 0)
        {
                UnionUserErrLog("in UnionSetBodyOfSJL05Cmd0x10:: UnionSetRequestXMLPackageValue [body/exportFlag]!\n");
                return(errCodeSJL05MDL_HsmCmdFormatError);
	}

	// protectKey	128A/16H/32H/48H	������Կ
	bcdhex_to_aschex((char *)(buf + 1), 2, tmpBuf);
	tmpBuf[4] = 0;
	snprintf(protectKey, sizeof(protectKey), "ATM.%sSJL05.zmk", tmpBuf);
	if ((ret = UnionSetRequestXMLPackageValue("body/protectKey", protectKey)) < 0)
        {
                UnionUserErrLog("in UnionSetBodyOfSJL05Cmd0x10:: UnionSetRequestXMLPackageValue [body/protectKey]!\n");
                return(errCodeSJL05MDL_HsmCmdFormatError);
	}
	return(0);
}
static int UnionConvertResponseToSJL050Cmd0x01(char *responseCode, unsigned char *buf, int lenOfBuf)
{
	int	ret;

	if (memcmp(responseCode, "000000", 6) == 0)
	{
		ret = snprintf((char *)buf, lenOfBuf, "%s", "A");
	}
	else
	{
		ret = snprintf((char *)buf, lenOfBuf, "%s", "E\x01");
	}
		
	return(ret);
}

static int UnionConvertResponseToSJL050Cmd0x0406(char *responseCode, unsigned char *buf, int lenOfBuf)
{
	int	ret;
	int	offset = 0;
	char	pinBlock[32];
	char	resStr[128];
	
	if (memcmp(responseCode, "000000", 6) == 0)
        {
                ret = snprintf(resStr, sizeof(resStr), "%s", "A");
                offset += ret;

                if ((ret = UnionReadResponseXMLPackageValue("body/pinBlock", pinBlock, sizeof(pinBlock))) < 0)
                {
                        UnionUserErrLog("in UnionConvertResponseToSJL050Cmd0x0406:: UnionReadResponseXMLPackageValue[body/pinBlock]!\n");
                        return(errCodeSJL05MDL_HsmCmdFormatError);
                }
                aschex_to_bcdhex(pinBlock, ret, resStr + offset);
                offset += ret / 2;
        }
        else if (memcmp(responseCode, "001023", 6) == 0) // ��Ч��PIN���ʽ����
        {
                ret = snprintf(resStr, sizeof(resStr), "%s", "E\x14");
                offset += ret;
        }
        else if (memcmp(responseCode, "006009", 6) == 0) // ��Կ���� zmk ����
        {
                // �Ƿ���������Կ������
                ret = snprintf(resStr, sizeof(resStr), "%s", "E\x0C");
                offset += ret;
        }
        else
        {
                // MAC��֤ʧ��
                ret = snprintf(resStr, sizeof(resStr), "%s", "E\x2D");
                offset += ret;
        }

        if (lenOfBuf < offset)
        {
                UnionUserErrLog("in UnionConvertResponseToSJL050Cmd0x0406:: buf lenOfBuf[%d] too small!\n", lenOfBuf);
                return(errCodeSJL05MDL_HsmCmdFormatError);
        }
        memcpy(buf, resStr, offset);
	return (offset);
}

static int UnionConvertResponseToSJL050Cmd0x81(char *responseCode, unsigned char *buf, int lenOfBuf)
{
	int	ret;
	int	offset = 0;
	char	resStr[128];
	
	if (memcmp(responseCode, "000000", 6) == 0)
        {
                ret = snprintf(resStr, sizeof(resStr), "%s", "A");
                offset += ret;
        }
        else if (memcmp(responseCode, "001023", 6) == 0) // ��Ч��PIN���ʽ����
        {
                ret = snprintf(resStr, sizeof(resStr), "%s", "E\x14");
                offset += ret;
        }
        else if (memcmp(responseCode, "006009", 6) == 0) // ��Կ���� zmk ����
        {
                // �Ƿ���������Կ������
                ret = snprintf(resStr, sizeof(resStr), "%s", "E\x0C");
                offset += ret;
        }
        else
        {
                // MAC��֤ʧ��
                ret = snprintf(resStr, sizeof(resStr), "%s", "E\x2D");
                offset += ret;
        }

        if (lenOfBuf < offset)
        {
                UnionUserErrLog("in UnionConvertResponseToSJL050Cmd0x81:: buf lenOfBuf[%d] too small!\n", lenOfBuf);
                return(errCodeSJL05MDL_HsmCmdFormatError);
        }
        memcpy(buf, resStr, offset);
	return (offset);
}

static int UnionConvertResponseToSJL050Cmd0x72(char *responseCode, unsigned char *buf, int lenOfBuf)
{
	int	ret;
	int	offset = 0;
	char	resStr[8192];
	char	lenData[8];
	char	data[8192];
	
	if (memcmp(responseCode, "000000", 6) == 0)
        {
                ret = snprintf(resStr, sizeof(resStr), "%s", "A");
                offset += ret;

		if ((ret = UnionReadResponseXMLPackageValue("body/data", data, sizeof(data))) < 0)
		{
			UnionUserErrLog("in UnionConvertResponseToSJL050Cmd0x72:: UnionReadResponseXMLPackageValue[body/data]!\n");
			return(errCodeSJL05MDL_HsmCmdFormatError);
		}
		snprintf(lenData, sizeof(lenData), "%04X", ret / 2);
		aschex_to_bcdhex(lenData, 4, resStr + offset);
		offset += 2;

		aschex_to_bcdhex(data, ret, resStr + offset);
		offset += ret / 2;
        }
        else if (memcmp(responseCode, "001023", 6) == 0) // ��Ч��PIN���ʽ����
        {
                ret = snprintf(resStr, sizeof(resStr), "%s", "E\x14");
                offset += ret;
        }
        else if (memcmp(responseCode, "006009", 6) == 0) // ��Կ���� zmk ����
        {
                // �Ƿ���������Կ������
                ret = snprintf(resStr, sizeof(resStr), "%s", "E\x0C");
                offset += ret;
        }
        else
        {
                // MAC��֤ʧ��
                ret = snprintf(resStr, sizeof(resStr), "%s", "E\x2D");
                offset += ret;
        }

        if (lenOfBuf < offset)
        {
                UnionUserErrLog("in UnionConvertResponseToSJL050Cmd0x72:: buf lenOfBuf[%d] too small!\n", lenOfBuf);
                return(errCodeSJL05MDL_HsmCmdFormatError);
        }
        memcpy(buf, resStr, offset);
	return (offset);
}

static int UnionConvertResponseToSJL050Cmd0x10(char *responseCode, unsigned char *buf, int lenOfBuf)
{
	int	ret;
	int	offset = 0;
	char	resStr[128];
	char	keyValue[64];
	char	checkValue[64];
	
	if (memcmp(responseCode, "000000", 6) == 0)
        {
                ret = snprintf(resStr, sizeof(resStr), "%s", "A");
                offset += ret;

		if ((ret = UnionReadResponseXMLPackageValue("body/keyValue", keyValue, sizeof(keyValue))) < 0)
		{
			UnionUserErrLog("in UnionConvertResponseToSJL050Cmd0x10:: UnionReadResponseXMLPackageValue[body/keyValue]!\n");
			return(errCodeSJL05MDL_HsmCmdFormatError);
		}
		keyValue[ret] = 0;
		aschex_to_bcdhex(keyValue, ret, resStr + offset);
		offset += ret / 2;

		if ((ret = UnionReadResponseXMLPackageValue("body/checkValue", checkValue, sizeof(checkValue))) < 0)
		{
			UnionUserErrLog("in UnionConvertResponseToSJL050Cmd0x10:: UnionReadResponseXMLPackageValue[body/checkValue]!\n");
			return(errCodeSJL05MDL_HsmCmdFormatError);
		}
		checkValue[ret] = 0;
		aschex_to_bcdhex(checkValue, ret, resStr + offset);
		offset += ret / 2;
        }
        else if (memcmp(responseCode, "001023", 6) == 0) // ��Ч��PIN���ʽ����
        {
                ret = snprintf(resStr, sizeof(resStr), "%s", "E\x14");
                offset += ret;
        }
        else if (memcmp(responseCode, "006009", 6) == 0) // ��Կ���� zmk ����
        {
                // �Ƿ���������Կ������
                ret = snprintf(resStr, sizeof(resStr), "%s", "E\x0C");
                offset += ret;
        }
        else
        {
                // MAC��֤ʧ��
                ret = snprintf(resStr, sizeof(resStr), "%s", "E\x2D");
                offset += ret;
        }

        if (lenOfBuf < offset)
        {
                UnionUserErrLog("in UnionConvertResponseToSJL050Cmd0x10:: buf lenOfBuf[%d] too small!\n", lenOfBuf);
                return(errCodeSJL05MDL_HsmCmdFormatError);
        }
        memcpy(buf, resStr, offset);
	return (offset);
}

// ���ݱ���ָ�·�ɲ�ͬ�ı�����
static int UnionSetBodyOfSJL05FromString(unsigned char *buf, int lenOfBuf)
{
	int	ret;

	if (memcmp(buf, "\x01", 1) == 0)		// ���ر�������Կ״̬<0X01>
	{
		if ((ret = UnionSetBodyOfSJL05Cmd0x01(buf, lenOfBuf)) < 0)
		{
			UnionUserErrLog("in UnionSetBodyOfSJL05FromString:: UnionSetBodyOfSJL05Cmd0x01!\n");
			return(errCodeSJL05MDL_HsmCmdFormatError);
		}
	}
	else if (lenOfBuf > 1 && memcmp(buf, "\x04\x06", 2) == 0)	// PINBLOCKת�������ⳤ��MMK����ʽ��<0X0406>
	{
		if ((ret = UnionSetBodyOfSJL05Cmd0x0406(buf, lenOfBuf)) < 0)
		{
			UnionUserErrLog("in UnionSetBodyOfSJL05FromString:: UnionSetBodyOfSJL05Cmd0x0406!\n");
			return(errCodeSJL05MDL_HsmCmdFormatError);
		}
	}
	else if (memcmp(buf, "\x81", 1) == 0)		// ��֤MAC<0X81>
	{
		if ((ret = UnionSetBodyOfSJL05Cmd0x81(buf, lenOfBuf)) < 0)
		{
			UnionUserErrLog("in UnionSetBodyOfSJL05FromString:: UnionSetBodyOfSJL05Cmd0x81!\n");
			return(errCodeSJL05MDL_HsmCmdFormatError);
		}
	}
	else if (memcmp(buf, "\x72", 1) == 0)		// ����������Կ�����ݼ�/����<0X72>
	{
		if ((ret = UnionSetBodyOfSJL05Cmd0x72(buf, lenOfBuf)) < 0)
		{
			UnionUserErrLog("in UnionSetBodyOfSJL05FromString:: UnionSetBodyOfSJL05Cmd0x72!\n");
			return(errCodeSJL05MDL_HsmCmdFormatError);
		}
	}
	else if (memcmp(buf, "\x10", 1) == 0)		// ����һ��������Կ������BMK���ܺ󷵻�<0X10>
	{
		if ((ret = UnionSetBodyOfSJL05Cmd0x10(buf, lenOfBuf)) < 0)
		{
			UnionUserErrLog("in UnionSetBodyOfSJL05FromString:: UnionSetBodyOfSJL05Cmd0x10!\n");
			return(errCodeSJL05MDL_HsmCmdFormatError);
		}
	}
	else
	{
		UnionUserErrLog("in UnionSetBodyOfSJL05FromString:: command[0x%X%X] not support!\n", buf[0], buf[1]);
		return(errCodeSJL05MDL_HsmCmdFormatError);
	}
	return(0);
	
}

// ��ִ�н����ת��Ϊ SJL05 ��Ҫ�Ľ��
static int UnionConvertResponseToSJL05String(char *serverCode, char *responseCode, unsigned char *buf, int lenOfBuf)
{
	int	ret = -1;

	if (memcmp(serverCode, "E100", 4) == 0)		// ���ر�������Կ״̬<0X01>
	{
		ret = UnionConvertResponseToSJL050Cmd0x01(responseCode, buf, lenOfBuf);
	}
	else if (memcmp(serverCode, "EEP1", 4) == 0)	// PINBLOCKת�������ⳤ��MMK����ʽ��<0X0406>
	{
		ret = UnionConvertResponseToSJL050Cmd0x0406(responseCode, buf, lenOfBuf);
	}
	else if (memcmp(serverCode, "E151", 4) == 0)		// ��֤MAC<0X81>
	{
		ret = UnionConvertResponseToSJL050Cmd0x81(responseCode, buf, lenOfBuf);
	}
	else if (memcmp(serverCode, "E160", 4) == 0 || memcmp(serverCode, "E161", 4) == 0)		// ����������Կ�����ݼ�/����<0X72>
	{
		ret = UnionConvertResponseToSJL050Cmd0x72(responseCode, buf, lenOfBuf);
	}
	else if (memcmp(serverCode, "E110", 4) == 0)		// ����һ��������Կ������BMK���ܺ󷵻�<0X10>
	{
		ret = UnionConvertResponseToSJL050Cmd0x10(responseCode, buf, lenOfBuf);
	}
	else
	{
		UnionUserErrLog("in UnionSetBodyOfSJL05FromString:: command[0x%X%X] not support!\n", buf[0], buf[1]);
		return(errCodeNoneOperationDefined);
	}
	return(ret);
	
}

// �� SJL05 ָ��ת��ΪXML����
int UnionConvertOtherPackageToXMLPackage(unsigned char *buf,int lenOfBuf,char *cliIPAddr)
{
	int	ret;
	char	currServiceCode[32];
	char	tmpBuf[8192 * 2];

	if (buf == NULL || cliIPAddr == NULL || lenOfBuf <= 0)
	{
		UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: Parameter error!\n");
		return(errCodeSJL05MDL_HsmCmdFormatError);
	}

	bcdhex_to_aschex((char *)buf, lenOfBuf, tmpBuf);
	tmpBuf[lenOfBuf * 2] = 0;
	UnionLog("in UnionConvertOtherPackageToXMLPackage:: SJL05 hsm reqStr[%04d][%s]\n", lenOfBuf * 2, tmpBuf);

	// ����XML��ʽΪ V001
	UnionSetPackageType(PACKAGE_TYPE_V001);
	
	// ���� SJL05 ָ�ת��Ϊ��Ӧ�ķ�����
	if ((ret = UnionConvertSJL05CmdToServer(buf, lenOfBuf, currServiceCode)) < 0)
	{
		UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionConvertSJL05CmdToServer!\n");
		return(errCodeSJL05MDL_HsmCmdFormatError);
	}
	currServiceCode[ret] = 0;

	// ����������ͷ
	if ((ret = UnionInitRequestXMLPackage(NULL,NULL,0)) < 0)
	{
		UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionInitRequestXMLPackage!\n");
		return(errCodeSJL05MDL_HsmCmdFormatError);
	}
		
	if ((ret = UnionSetRequestXMLPackageValue("head/serviceCode",currServiceCode)) < 0)
	{
		UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionSetRequestXMLPackageValue [head/serviceCode][%s]!\n",currServiceCode);
		return(errCodeSJL05MDL_HsmCmdFormatError);
	}
	// ����ϵͳID
	if ((ret = UnionSetRequestXMLPackageValue("head/sysID", "SJL05")) < 0)
	{
		UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionSetRequestXMLPackageValue [head/sysID][%s]!\n", "SJL05");
		return(errCodeSJL05MDL_HsmCmdFormatError);
	}
	// ����Ӧ��ID
	if ((ret = UnionSetRequestXMLPackageValue("head/appID", "SJL05")) < 0)
	{
		UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionSetRequestXMLPackageValue [head/appID][%s]!\n","SJL05");
		return(errCodeSJL05MDL_HsmCmdFormatError);
	}

	// ����IP��ַ
	if ((ret = UnionSetRequestXMLPackageValue("head/clientIPAddr",cliIPAddr)) < 0)
	{
		UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionSetRequestXMLPackageValue [head/cliIPAddr][%s]!\n",cliIPAddr);
		return(errCodeSJL05MDL_HsmCmdFormatError);
	}
	// ���ý���ʱ��
	if ((ret = UnionSetRequestXMLPackageValue("head/transTime",UnionGetCurrentFullSystemDateTime())) < 0)
	{
		UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionSetRequestXMLPackageValue [head/transTime][%s]!\n",UnionGetCurrentFullSystemDateTime());
		return(errCodeSJL05MDL_HsmCmdFormatError);
	}
	// ���ý��ױ�ʶ
	if ((ret = UnionSetRequestXMLPackageValue("head/transFlag","1")) < 0)
	{
		UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionSetRequestXMLPackageValue [head/transFlag][%s]!\n","1");
		return(errCodeSJL05MDL_HsmCmdFormatError);
	}


	// ���ñ�����
	// ���ݱ���ָ�·�ɲ�ͬ�ı�����
	if ((ret = UnionSetBodyOfSJL05FromString(buf, lenOfBuf)) < 0)
	{
		UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionSetBodyOfSJL05FromString!\n");
		return(errCodeSJL05MDL_HsmCmdFormatError);
	}

	return(1);
}



// ��ǰESSC����ת���� SJL05 ��Ҫ�ĸ�ʽ
int UnionConvertXMLPackageToOtherPackage(unsigned char *buf,int sizeOfBuf)
{
	int	ret;
	int	errCode = 0;
	char	responseCode[32];
	char	responseRemark[128];
	char	currServerCode[16];
	char	tmpBuf[8192 * 2];

	UnionLocateResponseXMLPackage("",0);
	
	// ��ȡ��Ӧ��
	if ((ret = UnionReadResponseXMLPackageValue("head/responseCode",responseCode,sizeof(responseCode))) < 0)
	{
		UnionUserErrLog("in UnionConvertXMLPackageToOtherPackage:: UnionReadResponseXMLPackageValue[head/responseCode]!\n");
		UnionLogResponseXMLPackage();
		return(errCodeSJL05MDL_HsmCmdFormatError);
	}
	responseCode[ret] = 0;
	errCode = 0 - atoi(responseCode);
	
	// ��ȡ��Ӧ����
	if ((ret = UnionReadResponseXMLPackageValue("head/responseRemark",responseRemark,sizeof(responseRemark))) < 0)
	{
		UnionUserErrLog("in UnionConvertXMLPackageToOtherPackage:: UnionReadResponseXMLPackageValue[head/responseRemark]!\n");
		return(errCodeSJL05MDL_HsmCmdFormatError);
	}
	responseRemark[ret] = 0;

	// ��ȡ������
	if ((ret = UnionReadResponseXMLPackageValue("head/serviceCode",currServerCode,sizeof(currServerCode))) < 0)
	{
		UnionUserErrLog("in UnionConvertXMLPackageToOtherPackage:: UnionReadResponseXMLPackageValue[head/serviceCode]!\n");
		return(errCodeSJL05MDL_HsmCmdFormatError);
	}
	currServerCode[ret] = 0;

	// ��ִ�н����ת��Ϊ SJL05 ��Ҫ�Ľ��
	if ((ret = UnionConvertResponseToSJL05String(currServerCode, responseCode, buf, sizeOfBuf)) < 0)
	{
		UnionUserErrLog("in UnionConvertXMLPackageToOtherPackage:: UnionConvertResponseToSJL05String\n");
		return(errCodeSJL05MDL_HsmCmdFormatError);
	}
	buf[ret] = 0;

	bcdhex_to_aschex((char *)buf, ret, tmpBuf);
	tmpBuf[ret * 2] = 0;
	UnionLog("in UnionConvertXMLPackageToOtherPackage:: SJL05 hsm resStr[%04d][%s]\n", ret * 2, tmpBuf);
	return(ret);
}
