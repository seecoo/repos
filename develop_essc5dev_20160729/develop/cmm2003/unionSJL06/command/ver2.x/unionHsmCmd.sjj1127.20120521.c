//	Author:		������
//	Copyright:	Union Tech. Guangzhou
//	Date:		2012-05-25

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "UnionLog.h"
#include "UnionStr.h"
#include "unionErrCode.h"
#include "unionDesKey.h"
#include "unionSJJ1127Cmd.h"
#include "commWithHsmSvr.h"

// �㷨��ʶ��1��DES�㷨��2��SM1�㷨
int gunionAlgorithmFlag = 1;

// ����ʹ��DES�㷨
int UnionSetUseDesAlgorithm()
{
	gunionAlgorithmFlag = 1;
	return(0);
}

// ����ʹ��SM1�㷨
int UnionSetUseSM1Algorithm()
{
	gunionAlgorithmFlag = 2;
	return(0);
}

// ת����Կ���ȣ�������Կ�ĳ���
int UnionTranslateSJJ1127HsmKeyLength(TUnionDesKeyLength length,char *lenStr)
{       
	switch (length)
	{
		case    con64BitsDesKey:
		case    64:
		case    16:
			memcpy(lenStr,"01",2);
			return(16);
		case    con128BitsDesKey:
		case    128:
		case    32:
			memcpy(lenStr,"02",2);
			return(32);
		case    con192BitsDesKey:
		case    192:
		case    48:
			memcpy(lenStr,"03",2);
			return(64);
		default:
			UnionUserErrLog("in UnionTranslateSJJ1127HsmKeyLength:: invalid key length [%d]!\n",length);
			return(errCodeParameter);
	}
}

// �����ܻ�ָ�����
int UnionFillSJJ1127HsmCommandHeader(char *oldCmd,int lenOfOldCmd,char *newCmd)
{
	int	offset = 0;
		
	if (gunionAlgorithmFlag == 1)
	{
		// �����һ���ַ���"!"��������䣬��ȥ��"!"
		if (memcmp(oldCmd,"!",1) == 0)
		{
			memcpy(newCmd,oldCmd+1,lenOfOldCmd - 1);
			return(lenOfOldCmd - 1);
		}

		offset = 2;
		if (memcmp(oldCmd,"50",2) == 0)
			memcpy(newCmd,"A0",2);
		else if (memcmp(oldCmd,"10",2) == 0)
			memcpy(newCmd,"B0",2);
		else if (memcmp(oldCmd,"12",2) == 0)
			memcpy(newCmd,"B0",2);
		else
			memcpy(newCmd,"00",2);
	}
	else if (gunionAlgorithmFlag == 2)
	{
		// �����һ���ַ���"!"��������䣬��ȥ��"!"
		if (memcmp(oldCmd,"!",1) == 0)
		{
			memcpy(newCmd,oldCmd+1,lenOfOldCmd - 1);
			return(lenOfOldCmd - 1);
		}

		offset = 2;
		if ((memcmp(oldCmd,"00",2) == 0) ||
			(memcmp(oldCmd,"02",2) == 0) ||
			(memcmp(oldCmd,"04",2) == 0) ||
			(memcmp(oldCmd,"62",2) == 0))
		{
			memcpy(newCmd,"00",2);
		}
		else if (memcmp(oldCmd,"10",2) == 0)
			memcpy(newCmd,"B0",2);
		else if (memcmp(oldCmd,"12",2) == 0)
			memcpy(newCmd,"B0",2);
		else
			memcpy(newCmd,"S0",2);
	}

	// ���ü��ܻ�ָ��ͷ�ĳ���Ϊ4
	UnionSetLenOfHsmCmdHeader(4);
	
	memcpy(newCmd + offset, oldCmd, lenOfOldCmd);
	return(lenOfOldCmd + offset);
}

/*      
�������ܣ�
	18ָ���ָ����Կ����ת����
���������      
	transformID��ת����ʽ
	oriPKIndex: Դ������Կ����
	lenOfOriPK: Դ������Կ����
	oriPKValue: Դ������Կ
	desPKIndex: Ŀ�ı�����Կ����
	lenOfDesPK: Ŀ�ı�����Կ����
	desPKValue: Ŀ�ı�����Կ
	lenOfOriKey����ת���ܵ���Կ����
	oriKeyValue����ת���ܵ���Կ����
	oriKeyCheckValue����ԿУ��ֵ
		
���������	      
	desKeyValue��ת���ܺ�����
	desKeyCheckValue����ԿУ��ֵ
*/	      
int UnionSJJ1127Cmd18(char *transformID,int oriPKIndex,TUnionDesKeyLength lenOfOriPK,char *oriPKValue,int desPKIndex,TUnionDesKeyLength lenOfDesPK,char *desPKValue,TUnionDesKeyLength lenOfOriKey,char *oriKeyValue,char *oriKeyCheckValue,char *desKeyValue,char *desKeyCheckValue)
{
	int     ret;
	int     len;
	char    hsmCmdBuf[1024*8+1];
	char    tmpBuf[128+1];
	int     hsmCmdLen = 0;
	int     offset = 0;

	memset(hsmCmdBuf, 0, sizeof(hsmCmdBuf));

	// �������
	if ((ret = UnionFillSJJ1127HsmCommandHeader("18",2,hsmCmdBuf)) < 0)
	{
		UnionUserErrLog("in UnionSJJ1127Cmd18:: UnionFillSJJ1127HsmCommandHeader!\n");
		return(ret);
	}
	hsmCmdLen += 4;

	// ת����ʽ
	memcpy(hsmCmdBuf+hsmCmdLen, transformID, 2);
	hsmCmdLen += 2;

	if ((memcmp(transformID,"02",2) == 0) ||
		(memcmp(transformID,"03",2) == 0))
	{
		// Դ������Կ����
		sprintf(hsmCmdBuf+hsmCmdLen, "%04X", oriPKIndex);
		memset(tmpBuf,0,sizeof(tmpBuf));
		memcpy(tmpBuf, hsmCmdBuf+hsmCmdLen, 4);
		hsmCmdLen += 4;

		if (memcmp(tmpBuf,"FFFF",4) == 0)
		{
			// Դ������Կ����
			len = UnionTranslateSJJ1127HsmKeyLength(lenOfOriPK,hsmCmdBuf+hsmCmdLen);
			hsmCmdLen += 2;
			// Դ������Կ
			memcpy(hsmCmdBuf+hsmCmdLen, oriPKValue, len);
			hsmCmdLen += len;
		}
	}

	if ((memcmp(transformID,"01",2) == 0) ||
		(memcmp(transformID,"03",2) == 0))
	{
		// Ŀ�ı�����Կ����
		sprintf(hsmCmdBuf+hsmCmdLen, "%04X", desPKIndex);
		memset(tmpBuf,0,sizeof(tmpBuf));
		memcpy(tmpBuf, hsmCmdBuf+hsmCmdLen, 4);
		hsmCmdLen += 4;

		if (memcmp(tmpBuf,"FFFF",4) == 0)
		{
			// Ŀ�ı�����Կ����
			len = UnionTranslateSJJ1127HsmKeyLength(lenOfDesPK,hsmCmdBuf+hsmCmdLen);
			hsmCmdLen += 2;
			// Ŀ�ı�����Կ
			memcpy(hsmCmdBuf+hsmCmdLen, desPKValue, len);
			hsmCmdLen += len;
		}
	}

	// ��ת���ܵ���Կ����
	len = UnionTranslateSJJ1127HsmKeyLength(lenOfOriKey,hsmCmdBuf+hsmCmdLen);
	hsmCmdLen += 2;

	// ��ת���ܵ���Կ����
	memcpy(hsmCmdBuf+hsmCmdLen, oriKeyValue, len);
	hsmCmdLen += len;

	// ��ԿУ��ֵ
	memcpy(hsmCmdBuf+hsmCmdLen, oriKeyCheckValue, 16);
	hsmCmdLen += 16;

	// �������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionSJJ1127Cmd18:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	offset = 6;
	// ת���ܺ�����
	memcpy(desKeyValue, hsmCmdBuf+offset, len);
	offset += len;

	// ��ԿУ��ֵ
	memcpy(desKeyCheckValue, hsmCmdBuf+offset, 16);
	offset += 16;

	return(0);
}


/*
�������ܣ�
	50ָ���ָ����Կ��ָ�����ݽ��мӽ���
���������
	algorithmID���㷨��ʶ
		ֵ00- SM1
		ֵ 01- DES ECB
		ֵ 02- DES CBC
		ֵ03- DES CFB
		ֵ04- DES OFB
	encryptID����/���ܱ�־��1�����ܣ�0�����ܣ�
	indexOfKey: ��Կ������
	lenOfKey: ��Կ����
	keyValue: ��Կ����
	iv����ʼ����
	lenOfData: ���ݿ鳤��
	data�����ݿ�
���������
	lenOfResData: ���ݿ鳤��
	resData: ���ݿ�
*/
int UnionSJJ1127Cmd50(char *algorithmID,char *encryptID,int indexOfKey,TUnionDesKeyLength lenOfKey,char *keyValue,char *iv,int lenOfData,char *data,int *lenOfResData,char *resData)
{
	int     ret;
	int     len;
	char    hsmCmdBuf[1024*8+1];
	char    tmpBuf[128+1];
	int     hsmCmdLen = 0;
	int     offset = 0;

	memset(hsmCmdBuf, 0, sizeof(hsmCmdBuf));

	// �������
	if ((ret = UnionFillSJJ1127HsmCommandHeader("50",2,hsmCmdBuf)) < 0)
	{
		UnionUserErrLog("in UnionSJJ1127Cmd50:: UnionFillSJJ1127HsmCommandHeader!\n");
		return(ret);
	}
	hsmCmdLen += 4;

	// �㷨��ʶ
	memcpy(hsmCmdBuf+hsmCmdLen, algorithmID, 2);
	hsmCmdLen += 2;

	// �㷨��ʶ
	memcpy(hsmCmdBuf+hsmCmdLen, encryptID, 1);
	hsmCmdLen += 1;

	// ��Կ������
	sprintf(hsmCmdBuf+hsmCmdLen, "%04X", indexOfKey);
	memset(tmpBuf,0,sizeof(tmpBuf));
	memcpy(tmpBuf, hsmCmdBuf+hsmCmdLen, 4);
	hsmCmdLen += 4;

	// ��Կ����
	if (memcmp(tmpBuf,"FFFF",4) == 0)
	{
		// ��Կ����
		len = UnionTranslateSJJ1127HsmKeyLength(lenOfKey,hsmCmdBuf+hsmCmdLen);
		hsmCmdLen += 2;

		memcpy(hsmCmdBuf+hsmCmdLen, keyValue, len);
		hsmCmdLen += len;
	}

	// ��ʼ����
	if ((memcmp(algorithmID,"02",2) == 0) ||		// DES CBC
		(memcmp(algorithmID,"03",2) == 0) ||	    // DES CFB
		(memcmp(algorithmID,"04",2) == 0))	      // DES OFB
	{
		memcpy(hsmCmdBuf+hsmCmdLen, iv, 16);
		hsmCmdLen += 16;
	}

	// ���ݿ鳤��
	sprintf(hsmCmdBuf+hsmCmdLen, "%04X", lenOfData / 2);
	hsmCmdLen += 4;

	// ���ݿ�
	memcpy(hsmCmdBuf+hsmCmdLen, data, lenOfData);
	hsmCmdLen += lenOfData;
	// �������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionSJJ1127Cmd50:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	offset = 6;
	// �����Կ
	memcpy(tmpBuf, hsmCmdBuf+offset, 4);
	offset += 4;
	tmpBuf[4] = 0;
	sscanf(tmpBuf, "%X", lenOfResData);
	ret = (*lenOfResData) *2;
	// ���ݿ�
	memcpy(resData, hsmCmdBuf+offset, ret);
	return(ret);
}



/*
�������ܣ�
	60ָ����ɲ���ӡ��Կ
���������
	lenOfKey: ��Կ���ȱ�־
	numOfComponent: �ɷ���
	keyName����Կ����
���������
	keyValue: �����Կ
	keyCheckValue: ��Կ��У��ֵ
*/
int UnionSJJ1127Cmd60(TUnionDesKeyLength lenOfKey,int numOfComponent,char *keyName,char *keyValue,char *keyCheckValue)
{
	int     ret;
	int     len;
	char    hsmCmdBuf[1024*8+1];
	int     hsmCmdLen = 0;
	int     offset = 0;

	memset(hsmCmdBuf, 0, sizeof(hsmCmdBuf));

	// �������
	if ((ret = UnionFillSJJ1127HsmCommandHeader("60",2,hsmCmdBuf)) < 0)
	{
		UnionUserErrLog("in UnionSJJ1127Cmd60:: UnionFillSJJ1127HsmCommandHeader!\n");
		return(ret);
	}
	hsmCmdLen += 4;

	// ��Կ����
	len = UnionTranslateSJJ1127HsmKeyLength(lenOfKey,hsmCmdBuf+hsmCmdLen);
	hsmCmdLen += 2;

	// �ɷ���
	sprintf(hsmCmdBuf+hsmCmdLen,"%d",numOfComponent);
	hsmCmdLen += 1;

	// ������Ϣ
	memcpy(hsmCmdBuf+hsmCmdLen, keyName, strlen(keyName));
	hsmCmdLen += strlen(keyName);

	// �������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionSJJ1127Cmd60:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	offset = 6;
	// �����Կ
	memcpy(keyValue, hsmCmdBuf+offset, len);
	offset += len;

	// ��Կ��У��ֵ
	memcpy(keyCheckValue, hsmCmdBuf+offset, 16);
	offset += 16;
	return(len);
}


/*
�������ܣ�
	62ָ�װ�ش�ӡ��ʽ
���������
	formatType: ��ʽ����
	format: ��ӡ��ʽ
���������
	��
*/
int UnionSJJ1127Cmd62(char *formatType,char *format)
{
	int     ret;
	char    hsmCmdBuf[1024*8+1];
	int     hsmCmdLen = 0;

	memset(hsmCmdBuf, 0, sizeof(hsmCmdBuf));

	// �������
	if ((ret = UnionFillSJJ1127HsmCommandHeader("62",2,hsmCmdBuf)) < 0)
	{
		UnionUserErrLog("in UnionSJJ1127Cmd62:: UnionFillSJJ1127HsmCommandHeader!\n");
		return(ret);
	}
	hsmCmdLen += 4;

	// ��ʽ����
	memcpy(hsmCmdBuf+hsmCmdLen, formatType, 1);
	hsmCmdLen += 1;

	// ��ʽ����
	sprintf(hsmCmdBuf+hsmCmdLen,"%04d",(int)strlen(format));
	hsmCmdLen += 4;

	// ��ԿУ��ֵ
	memcpy(hsmCmdBuf+hsmCmdLen, format, strlen(format));
	hsmCmdLen += strlen(format);

	// �������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionSJJ1127Cmd62:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	return(0);
}


/*
�������ܣ�
	16ָ���ȡָ����Կ��������Կ
���������
	indexOfKey: ��Կ������
���������
	keyValue: �����Կ
	keyCheckValue: ��Կ��У��ֵ
����ֵ��
	<0������ִ��ʧ��
	=0���ɹ�
*/
int UnionSJJ1127Cmd16(int indexOfKey,char *keyValue,char *keyCheckValue)
{
	int     ret;
	int     len;
	char    hsmCmdBuf[1024*8+1];
	char    tmpBuf[128+1];
	int     hsmCmdLen = 0;
	int     offset = 0;

	memset(hsmCmdBuf, 0, sizeof(hsmCmdBuf));

	// �������
	if ((ret = UnionFillSJJ1127HsmCommandHeader("16",2,hsmCmdBuf)) < 0)
	{
		UnionUserErrLog("in UnionSJJ1127Cmd16:: UnionFillSJJ1127HsmCommandHeader!\n");
		return(ret);
	}
	hsmCmdLen += 4;

	// ��Կ������
	sprintf(hsmCmdBuf+hsmCmdLen, "%04X",indexOfKey);
	hsmCmdLen += 4;

	// �������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionSJJ1127Cmd16:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	offset = 6;

	// ��Կ����
	memset(tmpBuf,0,sizeof(tmpBuf));
	memcpy(tmpBuf, hsmCmdBuf+offset, 2);
	len = atoi(tmpBuf) * 16;
	offset += 2;

	// �����Կ
	memcpy(keyValue, hsmCmdBuf+offset, len);
	offset += len;

	// ��Կ��У��ֵ
	memcpy(keyCheckValue, hsmCmdBuf+offset, 16);
	offset += 16;

	return(len);
}

/*      
�������ܣ�
        64ָ���ָ����Կ����ɢ����������Կ
���������
        mkIndex:	����Կ����
		FFFF��ʹ��ָ���д���ĸ���Կ   
		0000-03E8����Կ����    
		�������Ƿ�
        lenOfMK:	����Կ����
		������Կ����ΪFFFFʱ�д���
		ֵ01- 64bits   
		ֵ02- 128bits    
		ֵ03- 192bits
        mk:		����Կ
		������Կ����ΪFFFFʱ�д���
		MK�����µĸ���Կ������
        mkDvsNum:	��ɢ����
        mkDvsData:	��ɢ����
	
��������� 
	
        criperData:	������������� 
		MK�����µĹ�����Կ/����Կ����
        keyCheckValue:	��Կ��У��ֵ
		������Կ/����Կ��У��ֵ
*/     
int UnionSJJ1127Cmd64(int mkIndex,TUnionDesKeyLength lenOfMK,char *mk, int mkDvsNum, char *mkDvsData, char *criperData, char *keyCheckValue)
{
        int     ret;
        int     lenOfDvsData = 16;
        int     len = 0;
        char    hsmCmdBuf[1024*8+1];
        int     hsmCmdLen = 0;
        char    tmpBuf[4+1];
        int     offset = 0;

        memset(hsmCmdBuf, 0, sizeof(hsmCmdBuf));

	// �������
        if ((ret = UnionFillSJJ1127HsmCommandHeader("64",2,hsmCmdBuf)) < 0)
	{
	        UnionUserErrLog("in UnionSJJ1127Cmd64:: UnionFillSJJ1127HsmCommandHeader!\n");
	        return(ret);
	}
        hsmCmdLen += 4;

	// ����Կ����
        sprintf(hsmCmdBuf+hsmCmdLen, "%04X", mkIndex);        
        memset(tmpBuf,0,sizeof(tmpBuf));
        memcpy(tmpBuf, hsmCmdBuf+hsmCmdLen, 4);  
        hsmCmdLen += 4;      

        if (memcmp(tmpBuf,"FFFF",4) == 0)
	{
		// ����Կ����
	        len = UnionTranslateSJJ1127HsmKeyLength(lenOfMK,hsmCmdBuf+hsmCmdLen);
	        hsmCmdLen += 2;
		// ����Կ
	        memcpy(hsmCmdBuf+hsmCmdLen, mk, len);
	        hsmCmdLen += len;
	}
	
	// ��ɢ����
        sprintf(hsmCmdBuf+hsmCmdLen, "%02d", mkDvsNum);
        hsmCmdLen += 2;

	// ��ɢ����
        memcpy(hsmCmdBuf+hsmCmdLen, mkDvsData, mkDvsNum*lenOfDvsData);
        hsmCmdLen += mkDvsNum*lenOfDvsData;
        
	// �������ͨѶ
        if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
	        UnionUserErrLog("in UnionSJJ1127Cmd64:: UnionDirectHsmCmd!\n");
	        return(ret);
	}
        offset = 6;
	
	// ��������
	memcpy(criperData, hsmCmdBuf+offset, 32);

	// ��Կ��У��ֵ
	offset += 32;
        memcpy(keyCheckValue, hsmCmdBuf+offset, 16);        
        return(0);
}

/*
�������ܣ�
	14ָ�����Կ����д��ָ����Կ����
���������
	lenOfKey: ��Կ���ȱ�־
	keyValue: �����Կ
	keyCheckValue: ��Կ��У��ֵ
	indexOfKey: ��Կ������
���������
	��
*/
int UnionSJJ1127Cmd14(TUnionDesKeyLength lenOfKey,char *keyValue,char *keyCheckValue,int indexOfKey)
{
	int     ret;
	int     len;
	char    hsmCmdBuf[1024*8+1];
	int     hsmCmdLen = 0;

	memset(hsmCmdBuf, 0, sizeof(hsmCmdBuf));

	// �������
	if ((ret = UnionFillSJJ1127HsmCommandHeader("14",2,hsmCmdBuf)) < 0)
	{
		UnionUserErrLog("in UnionSJJ1127Cmd14:: UnionFillSJJ1127HsmCommandHeader!\n");
		return(ret);
	}
	hsmCmdLen += 4;

	// ��Կ����
	len = UnionTranslateSJJ1127HsmKeyLength(lenOfKey,hsmCmdBuf+hsmCmdLen);
	hsmCmdLen += 2;

	// ��Կ����
	memcpy(hsmCmdBuf+hsmCmdLen, keyValue, len);
	hsmCmdLen += len;

	// ��ԿУ��ֵ
	memcpy(hsmCmdBuf+hsmCmdLen, keyCheckValue, 16);
	hsmCmdLen += 16;

	// ��Կ������
	sprintf(hsmCmdBuf+hsmCmdLen, "%04X",indexOfKey);
	hsmCmdLen += 4;

	// �������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionSJJ1127Cmd14:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	return(0);
}


/*
�������ܣ�
	90ָ������������Կ
���������
	lenOfKey: ��Կ���ȱ�־
	indexOfKey: ��Կ������
���������
	keyValue: �����Կ
	keyCheckValue: ��Կ��У��ֵ
*/
int UnionSJJ1127Cmd90(TUnionDesKeyLength lenOfKey,int indexOfKey,char *keyValue,char *keyCheckValue)
{
	int     ret;
	int     len;
	char    hsmCmdBuf[1024*8+1];
	int     hsmCmdLen = 0;
	int     offset = 0;

	memset(hsmCmdBuf, 0, sizeof(hsmCmdBuf));

	// �������
	if ((ret = UnionFillSJJ1127HsmCommandHeader("90",2,hsmCmdBuf)) < 0)
	{
		UnionUserErrLog("in UnionSJJ1127Cmd90:: UnionFillSJJ1127HsmCommandHeader!\n");
		return(ret);
	}
	hsmCmdLen += 4;

	// ��Կ����
	len = UnionTranslateSJJ1127HsmKeyLength(lenOfKey,hsmCmdBuf+hsmCmdLen);
	hsmCmdLen += 2;

	// ��Կ������
	sprintf(hsmCmdBuf+hsmCmdLen, "%04X", indexOfKey);
	hsmCmdLen += 4;
	// �������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionSJJ1127Cmd90:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	offset = 6;
	// �����Կ
	memcpy(keyValue, hsmCmdBuf+offset, len);
	offset += len;

	// ��Կ��У��ֵ
	memcpy(keyCheckValue, hsmCmdBuf+offset, 16);
	offset += 16;

	return(len);
}


/*
�������ܣ�
	U2ָ�ʹ��ָ����Ӧ������Կ����2����ɢ�õ���ƬӦ������Կ��
		ʹ��ָ��������Կ����Կ���м��ܱ������������MAC���㡣
���������
	securityMech: ��ȫ����(S����DES���ܺ�MAC, T����DES���ܺ�MAC)
	mode: ģʽ��־, 0-������ 1-���ܲ�����MAC
	id: ����ID, 0=M/Chip4(CBCģʽ��ǿ�����X80) 1=VISA/PBOC(������ָ����ECB) 2=PBOC1.0ģʽ(ECBģʽ������������)

	mkIndex:	FFFF��ʹ��ָ���д���ĸ���Կ
			0000-03E8����Կ����
			�������Ƿ�
	lenOfMK:	��������Կ����ΪFFFFʱ�д���
			ֵ01- 64bits
			ֵ02- 128bits
			ֵ03- 192bits
	mk:	     MK�����µĸ���Կ������
	mkDvsNum: ����Կ��ɢ����, 1-3��
	mkDvsData: ����Կ��ɢ����, n*16H, n������ɢ����

	pkIndex:	FFFF��ʹ��ָ���д���ı�����Կ
			0000-03E8����Կ����
			�������Ƿ�
	lenOfPK:	����������Կ����ΪFFFFʱ�д���
			ֵ01- 64bits
			ֵ02- 128bits
			ֵ03- 192bits
	pk:	     MK�����µı�����Կ������
	pkDvsNum: ������Կ��ɢ����,������������Կ���͡�Ϊ1ʱ��,��MK-SMC��ɢ�õ�DK-SMC����ɢ����,��ΧΪ1-3
	pkDvsData: ������Կ��ɢ����,n*16H������������Կ���͡�Ϊ1ʱ��,������Կ����ɢ���ݣ�����nΪ��������Կ��ɢ������

	proKeyFlag:������Կ��ʶ��Y:���������Կ��N:�����������Կ����ѡ��:��û�и���ʱȱʡΪN
	proFactor: ��������(16H),��ѡ��:����������Կ��־ΪYʱ��

	ivCbc: IV-CBC,8H ����������ID��Ϊ0ʱ��

	encryptFillDataLen: ����������ݳ���(4H),����������ID��Ϊ2ʱ�У�������1024��
		����Կ����һ����м��ܵ����ݳ���
	encryptFillData: ����������� nB ����������ID��Ϊ2ʱ��,����Կ����һ����м���
	encryptFillOffset: �����������ƫ���� 4H ����������ID��Ϊ2ʱ��
		����Կ���Ĳ��뵽����������ݵ�λ��, ��ֵ������0������������ݳ���֮��

	ivMac: IV-MAC,16H ������ģʽ��־��Ϊ1ʱ��
	macDataLen: MAC������ݳ��� 4H ������ģʽ��־��Ϊ1ʱ��,����Կ����һ�����MAC��������ݳ���
	macData: MAC������� nB ������ģʽ��־��Ϊ1ʱ��,����Կ����һ�����MAC���������
	macOffset: ƫ���� 4H ������ģʽ��־��Ϊ1ʱ��,����Կ���Ĳ��뵽MAC������ݵ�λ��
		��ֵ������0��MAC������ݳ���֮��
���������
	mac: MACֵ 8B ������ģʽ��־��Ϊ1ʱ��
	criperDataLen: �������ݳ��� 4H �������ݳ���(������8�ı�����������ǰ׺���ȡ���׺���ȡ���Կ���ȵĺ�)
	criperData: nB �������������
����ֵ��
	<0������ִ��ʧ��
	=0���ɹ�
*/
int UnionSJJ1127CmdU2(char *securityMech, char *mode, char *id, int mkIndex,TUnionDesKeyLength lenOfMK,char *mk,
	int mkDvsNum, char *mkDvsData, int pkIndex, TUnionDesKeyLength lenOfPK, char *pk,
	int pkDvsNum, char *pkDvsData, char *proKeyFlag, char *proFactor, char *ivCbc,
	int encryptFillDataLen, char *encryptFillData, int encryptFillOffset,
	char *ivMac, int macDataLen, char *macData, int macOffset,
	char *mac, int *criperDataLen, char *criperData)
{
	int     ret;
	int     lenOfDvsData = 16;
	int     len = 0;
	char    hsmCmdBuf[1024*8+1];
	int     hsmCmdLen = 0;
	char    tmpBuf[4+1];
	int     offset = 0;

	memset(hsmCmdBuf, 0, sizeof(hsmCmdBuf));

	// �������
	if ((ret = UnionFillSJJ1127HsmCommandHeader("U2",2,hsmCmdBuf)) < 0)
	{
		UnionUserErrLog("in UnionSJJ1127CmdU2:: UnionFillSJJ1127HsmCommandHeader!\n");
		return(ret);
	}
	hsmCmdLen += 4;

	// ��ȫ����
	memcpy(hsmCmdBuf+hsmCmdLen, securityMech, 1);
	hsmCmdLen += 1;

	// ģʽ��ʶ
	memcpy(hsmCmdBuf+hsmCmdLen, mode, 1);
	hsmCmdLen += 1;

	// ����ID
	memcpy(hsmCmdBuf+hsmCmdLen, id, 1);
	hsmCmdLen += 1;

	// ����Կ����
	sprintf(hsmCmdBuf+hsmCmdLen, "%04X", mkIndex);
	memset(tmpBuf,0,sizeof(tmpBuf));
	memcpy(tmpBuf, hsmCmdBuf+hsmCmdLen, 4);
	hsmCmdLen += 4;

	if (memcmp(tmpBuf,"FFFF",4) == 0)
	{
		// ����Կ����
		len = UnionTranslateSJJ1127HsmKeyLength(lenOfMK,hsmCmdBuf+hsmCmdLen);
		hsmCmdLen += 2;
		// ����Կ
		memcpy(hsmCmdBuf+hsmCmdLen, mk, len);
		hsmCmdLen += len;
	}

	// ��ɢ����
	sprintf(hsmCmdBuf+hsmCmdLen, "%d", mkDvsNum);
	hsmCmdLen += 1;

	// ��ɢ����
	memcpy(hsmCmdBuf+hsmCmdLen, mkDvsData, mkDvsNum*lenOfDvsData);
	hsmCmdLen += mkDvsNum*lenOfDvsData;

	// ��������Կ����
	sprintf(hsmCmdBuf+hsmCmdLen, "%04X", pkIndex);
	memset(tmpBuf,0,sizeof(tmpBuf));
	memcpy(tmpBuf, hsmCmdBuf+hsmCmdLen, 4);
	hsmCmdLen += 4;

	if (memcmp(tmpBuf,"FFFF",4) == 0)
	{
		// ��������Կ����
		len = UnionTranslateSJJ1127HsmKeyLength(lenOfPK,hsmCmdBuf+hsmCmdLen);
		hsmCmdLen += 2;
		// ��������Կ
		memcpy(hsmCmdBuf+hsmCmdLen, pk, len);
		hsmCmdLen += len;
	}

	// ������Կ��ɢ����
	sprintf(hsmCmdBuf+hsmCmdLen, "%d", pkDvsNum);
	hsmCmdLen += 1;

	// ������Կ��ɢ����
	memcpy(hsmCmdBuf+hsmCmdLen, pkDvsData, pkDvsNum*lenOfDvsData);
	hsmCmdLen += pkDvsNum*lenOfDvsData;

	// ������Կ��ʶ
	if( (proKeyFlag != NULL) && (strlen(proKeyFlag) != 0) )
	{
		memcpy(hsmCmdBuf+hsmCmdLen, proKeyFlag, 1);
		hsmCmdLen += 1;
		// ��������
		if (proKeyFlag[0] == 'Y')
		{
			memcpy(hsmCmdBuf+hsmCmdLen, proFactor, lenOfDvsData);
			hsmCmdLen += lenOfDvsData;
		}
	}

	// IV-CBC
	if (id[0] == '0')
	{
		memcpy(hsmCmdBuf+hsmCmdLen, ivCbc, 8);
		hsmCmdLen += 8;
	}

	// ����������ݳ��ȡ�����������ݡ������������ƫ����
	if (id[0] == '2')
	{
		sprintf(hsmCmdBuf+hsmCmdLen,"%04X", encryptFillDataLen/2);
		hsmCmdLen += 4;

		memcpy(hsmCmdBuf+hsmCmdLen, encryptFillData, encryptFillDataLen);
		hsmCmdLen += encryptFillDataLen;

		sprintf(hsmCmdBuf+hsmCmdLen,"%04X", encryptFillOffset/2);
		hsmCmdLen += 4;
	}

	// IV-MAC, MAC������ݳ���, MAC�������, ƫ����
	if (mode[0] == '1')
	{
		if (strlen(ivMac) > 0)
		{
			memcpy(hsmCmdBuf+hsmCmdLen, ivMac, 16);
			hsmCmdLen += 16;
		}

		if (macDataLen > 8000)
		{
			UnionUserErrLog("in UnionSJJ1127CmdU2::macDataLen[%d] too large\n", macDataLen);
			return(errCodeParameter);
		}
		sprintf(hsmCmdBuf+hsmCmdLen, "%04X", macDataLen/2);
		hsmCmdLen += 4;

		memcpy(hsmCmdBuf+hsmCmdLen, macData, macDataLen);
		hsmCmdLen += macDataLen;

		if ((macOffset < 0) || (macOffset > macDataLen))
		{
			UnionUserErrLog("in UnionSJJ1127CmdU2::macOffset[%d] <0 or >macDataLen[%d]\n", macOffset, macDataLen);
			return(errCodeParameter);
		}
		sprintf(hsmCmdBuf+hsmCmdLen, "%04X", macOffset/2);
		hsmCmdLen += 4;
	}

	// �������ͨѶ
	if ((ret = UnionDirectHsmCmd(hsmCmdBuf,hsmCmdLen,hsmCmdBuf,sizeof(hsmCmdBuf))) < 0)
	{
		UnionUserErrLog("in UnionSJJ1127CmdU2:: UnionDirectHsmCmd!\n");
		return(ret);
	}
	offset = 6;
	if (mode[0] == '1') // MACֵ
	{
		memcpy(mac, hsmCmdBuf+offset, 16);
		offset += 16;
	}

	// �������ݳ���
	offset += 4;
	*criperDataLen = ret - offset;

	// ��������
	memcpy(criperData, hsmCmdBuf+offset, *criperDataLen);
	UnionLog("in UnionSJJ1127CmdU2::*criperDataLen=[%d]\n",*criperDataLen);
	return(0);
}



