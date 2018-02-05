#include <stdio.h>
#include <string.h>

#ifndef _UnionLogMDL_3_x_
#define _UnionLogMDL_3_x_
#endif

#include "UnionLog.h"
#include "unionErrCode.h"

// PBOC mode='0'��ʽ����PAN
int UnionForm16BytesDisperseDataOfV41A(int lenOfData, char *data, char *disperseData)
{
	char	tmpStr[16+1];

	memset(tmpStr, 0, sizeof tmpStr);

	if ((data == NULL) || (disperseData == NULL))
	{
		UnionUserErrLog("in UnionForm16BytesDisperseData:: Null Pointer!\n");
                return(errCodeParameter);
	}

	if(lenOfData >= 16)
	{
		memcpy(tmpStr, data+lenOfData-16, 16);
		memcpy(disperseData, tmpStr, 16);
	}
	else
	{
		memset(tmpStr, '0', sizeof tmpStr);
		memcpy(tmpStr+16-lenOfData, data, lenOfData);
		memcpy(disperseData, tmpStr, 16);
	}
	return 16;
}

/*
����
	���ݹ淶�������γ�16�ֽڵ���ɢ����
�������
	method����ɢ������ɷ�����0��EMV2000 V4.1ѡ��A��1��EMV2000 V4.1ѡ��B��2��PBOC2.0
	lenOfData�����ݳ���
	data������
�������
	disperseData��16�ֽڵ���ɢ����
����ֵ
	<0������ִ��ʧ�ܣ�ֵΪʧ�ܵĴ�����
	>0������ִ�гɹ�������disperseData�ĳ���
*/
int UnionForm16BytesDisperseData(int iMode, int lenOfData, char *data, char *disperseData)
{
	switch(iMode)
	{
	case 0:
		return (UnionForm16BytesDisperseDataOfV41A(lenOfData, data, disperseData));
	default:
		{
			UnionUserErrLog("in UnionForm16BytesDisperseData:: iMode = [%d] not supported!\n", iMode);
			return (UnionSetUserDefinedErrorCode(errCodeParameter));
		}
	}
	return 0;
}

/*********
// PBOC�����������
int UnionPBOCEMVFormPlainDataBlock(int lenOfDataLen,int lenOfData,unsigned char *data,unsigned char *dataBlock)
{
	int	ret,offset=0;
	int	i;
	unsigned char	tmpBuf[4096];
	char	tmpStr[10];

	memset(tmpBuf, 0, sizeof tmpBuf);
	memset(tmpStr, 0, sizeof tmpStr);

	if ((data == NULL) || (dataBlock == NULL))
	{
		UnionUserErrLog("in UnionPBOCEMVFormPlainDataBlock:: Null Pointer!\n");
                return(errCodeParameter);
	}
	memset(tmpBuf, '0', lenOfDataLen);
	sprintf(tmpStr,"%d", lenOfData);
	if (lenOfDataLen-strlen(tmpStr) < 0)
	{
		UnionUserErrLog("in UnionPBOCEMVFormPlainDataBlock:: lenOfDataLen err[%d]!\n",lenOfDataLen);
                return(errCodeParameter);
	}
	else
		memcpy(tmpBuf+lenOfDataLen-strlen(tmpStr), tmpStr, strlen(tmpStr));
	offset += lenOfDataLen;
	memcpy(tmpBuf+offset, data, lenOfData);
	offset += lenOfData;

	ret = offset%8;
	for(i=0;i<(8-ret);i++)
	{
		if (i==0)
			*(tmpBuf+offset) = 0x80;
		else
			*(tmpBuf+offset) = 0x00;
		offset++;
	}
	memcpy(dataBlock, tmpBuf, offset);
	return offset;
}
*********/

int UnionPBOCEMVFormPlainDataBlock(int lenOfData,unsigned char *data,unsigned char *dataBlock)
{
	int	ret,offset=0;
	int	i;
	unsigned char	tmpBuf[4096];

	memset(tmpBuf, 0, sizeof tmpBuf);

	if ((data == NULL) || (dataBlock == NULL))
	{
		UnionUserErrLog("in UnionPBOCEMVFormPlainDataBlock:: Null Pointer!\n");
                return(errCodeParameter);
	}

	memcpy(tmpBuf+offset, data, lenOfData);
	offset += lenOfData;

	ret = offset%8;
	if (ret == 0)
	{
		memcpy(dataBlock, tmpBuf, offset);
		return(offset);
	}

	for(i=0;i<(8-ret);i++)
	{
		if (i==0)
			*(tmpBuf+offset) = 0x80;
		else
			*(tmpBuf+offset) = 0x00;
		offset++;
	}
	memcpy(dataBlock, tmpBuf, offset);
	return offset;
}

int UnionPBOCEMVFormPlainDataBlockForSM4(int lenOfData,unsigned char *data,unsigned char *dataBlock)
{
        int     ret,offset=0;
        int     i;
        unsigned char   tmpBuf[4096];

        memset(tmpBuf, 0, sizeof tmpBuf);

        if ((data == NULL) || (dataBlock == NULL))
        {
                UnionUserErrLog("in UnionPBOCEMVFormPlainDataBlockForSM4:: Null Pointer!\n");
                return(errCodeParameter);
        }

        memcpy(tmpBuf+offset, data, lenOfData);
        offset += lenOfData;

        ret = offset%16;
        if (ret == 0)
        {
                memcpy(dataBlock, tmpBuf, offset);
                return(offset);
        }

        for(i=0;i<(16-ret);i++)
        {
                if (i==0)
                        *(tmpBuf+offset) = 0x80;
                else
                        *(tmpBuf+offset) = 0x00;
                offset++;
        }
        memcpy(dataBlock, tmpBuf, offset);
        return offset;
}
