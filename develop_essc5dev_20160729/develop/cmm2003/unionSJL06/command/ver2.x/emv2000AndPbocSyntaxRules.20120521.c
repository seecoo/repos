#include <stdio.h>
#include <string.h>

#ifndef _UnionLogMDL_3_x_
#define _UnionLogMDL_3_x_
#endif

#include "UnionLog.h"
#include "unionErrCode.h"

// PBOC mode='0'方式处理PAN
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
功能
	根据规范将数据形成16字节的离散数据
输入参数
	method：离散数据组成方法，0：EMV2000 V4.1选项A；1：EMV2000 V4.1选项B；2：PBOC2.0
	lenOfData：数据长度
	data：数据
输出参数
	disperseData：16字节的离散数据
返回值
	<0：函数执行失败，值为失败的错误码
	>0：函数执行成功，返回disperseData的长度
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
// PBOC明文数据填充
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
