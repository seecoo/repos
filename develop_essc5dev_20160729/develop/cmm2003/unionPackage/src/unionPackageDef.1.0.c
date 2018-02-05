//	Wolfgang Wang, 2010-6-11

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "UnionLog.h"
#include "unionPackageDef.h"
#include "unionErrCode.h"
#include "UnionLog.h"

/*
����
	��һ����������뵽����,���ش��뵽���е����ݵĳ���
�������
	pfld		��ָ��
	sizeOfBuf	��������С
�������
	buf		����
����ֵ
	>=0	���ڰ��е��ֽ���
	<0	������
*/
int UnionPutPackageFldIntoStr(PUnionPackageFld pfld,char *buf,int sizeOfBuf)
{
	if ((pfld == NULL) || (buf == NULL) || (pfld->len < 0) || (pfld->value == NULL))
		return(errCodeParameter);
	if (pfld->len + 8 >= sizeOfBuf)
	{
		UnionUserErrLog("in UnionPutPackageFldIntoStr:: sizeOfBuf [%d] too small!\n",sizeOfBuf);
		return(errCodeSmallBuffer);
	}
	sprintf(buf,"%03d%d%04d",pfld->tag,pfld->dataFormat,pfld->len);
	memcpy(buf+8,pfld->value,pfld->len);
	return(8+pfld->len);
}
 
/*
����
	��һ�������ж�ȡһ���򣬷������ڰ���ռ�ĳ���
�������
	data		���ݰ�
	len		���ݰ�����
	sizeOfFldValue	pfld��valueֵ�Ļ����С
�������
	pfld		��ָ��
����ֵ
	>=0	���ڰ��е��ֽ���
	<0	������
*/
int UnionReadPackageFldFromStr(char *data,int len,PUnionPackageFld pfld,int sizeOfFldValue)
{
	int	copyDataLen;
			
	if ((pfld == NULL) || (data == NULL) || (len < 8) || (pfld->value == NULL))
		return(errCodeParameter);
	pfld->tag = UnionConvertIntStringToInt(data,3);
	pfld->dataFormat = UnionConvertIntStringToInt(data+3,1);
	pfld->len = UnionConvertIntStringToInt(data+4,4);
	if (pfld->len < 0)
	{
		UnionUserErrLog("in UnionReadPackageFldFromStr:: pfld->len = [%d] for fldIndex = [%03d] error!\n",pfld->len,pfld->tag);
		return(errCodePackageDefMDL_PackFldLength);
	}
	if (pfld->len + 8 > len)
		copyDataLen = len - 8;
	else
		copyDataLen = pfld->len;
	if (copyDataLen >= sizeOfFldValue)
	{
		UnionUserErrLog("in UnionReadPackageFldFromStr:: sizeOfFldValue [%d] smaller than expected [%04d]\n",
				sizeOfFldValue,copyDataLen);
		return(errCodeSmallBuffer);
	}
	memcpy(pfld->value,data+8,copyDataLen);
	pfld->value[copyDataLen] = 0;
	return(8+copyDataLen);
}

/*
����
	��ʼ������
�������
	pfld		��ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionInitPackage(PUnionPackage ppackage)
{
	if (ppackage == NULL)
		return(errCodeParameter);
	memset(ppackage,0,sizeof(*ppackage));
	ppackage->fldNum = 0;
	ppackage->offset = 0;
	return(0);
}

/*
����
	��ʼ�����ĳ�һ��������
�������
	pfld		��ָ��
	serviceID	�������
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionInitPackageAsRequest(PUnionPackage ppackage,int serviceID)
{
	if (ppackage == NULL)
		return(errCodeParameter);
	UnionInitPackage(ppackage);
	ppackage->serviceID = serviceID;
	ppackage->direction = conPackIsRequest;
	return(0);
}

/*
����
	��ʼ�����ĳ�һ����Ӧ����
�������
	pfld		��ָ��
	serviceID	�������
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionInitPackageAsResponse(PUnionPackage ppackage,int serviceID)
{
	if (ppackage == NULL)
		return(errCodeParameter);
	UnionInitPackage(ppackage);
	ppackage->serviceID = serviceID;
	ppackage->direction = conPackIsResponse;
	return(0);
}

/*
����
	��ʼ�����ĳ�һ�����ݱ���
�������
	pfld		��ָ��
	serviceID	�������
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionInitPackageAsDataBlock(PUnionPackage ppackage,int serviceID)
{
	if (ppackage == NULL)
		return(errCodeParameter);
	UnionInitPackage(ppackage);
	ppackage->serviceID = serviceID;
	ppackage->direction = conPackIsData;
	return(0);
}

/*
����
	��һ�����õ�����
�������
	ppackage	��ָ��
	fldTag		���ʶ
	dataFormat	��ı��뷽ʽ
	len		��ĳ���
	value		���ֵ
�������
	ppackage	��ָ��
����ֵ
	>=0	�ɹ�������Ŀ
	<0	������
*/
int UnionPutBitsTypePackageFld(PUnionPackage ppackage,int fldTag,int len,char *value)
{
	return(UnionPutPackageFld(ppackage,fldTag,conDataFormatIsBinary,len,value));
}

/*
����
	��һ�����õ�����
�������
	ppackage	��ָ��
	fldTag		���ʶ
	dataFormat	��ı��뷽ʽ
	len		��ĳ���
	value		���ֵ
�������
	ppackage	��ָ��
����ֵ
	>=0	�ɹ�������Ŀ
	<0	������
*/
int UnionPutPackageFld(PUnionPackage ppackage,int fldTag,int dataFormat,int len,char *value)
{
	if (ppackage == NULL)
	{
		UnionUserErrLog("in UnionPutPackageFld:: ppackage is NULL!\n");
		return(errCodeParameter);
	}
	if (len < 0)
	{
		UnionUserErrLog("in UnionPutPackageFld:: fldTag = [%03d] parameter error!\n",fldTag);
		return(errCodeParameter);
	}
	if (ppackage->fldNum >= conMaxNumOfPackageFld)
	{
		UnionUserErrLog("in UnionPutPackageFld:: too much flds! fldNum = [%03d]\n",ppackage->fldNum);
		return(errCodeEsscMDL_TooMuchEsscPackageFld);
	}
	if (ppackage->fldNum < 0)	// �޸�����Ŀ
		ppackage->fldNum = 0;
	if (ppackage->fldNum == 0)	// �޸�ƫ��ֵ
		ppackage->offset = 0;
	ppackage->fldGrp[ppackage->fldNum].len = len;
	ppackage->fldGrp[ppackage->fldNum].tag = fldTag;
	ppackage->fldGrp[ppackage->fldNum].dataFormat = dataFormat;
	if (len + ppackage->offset + 1 >= sizeof(ppackage->dataBuf))
	{
		UnionUserErrLog("in UnionSetEsscPackageFld:: dataBuf = [%04d] offset = [%04d] fldLen = [%04d]\n",
				sizeof(ppackage->dataBuf),ppackage->offset,len);
		return(errCodeSmallBuffer);
	}
	ppackage->fldGrp[ppackage->fldNum].value = ppackage->dataBuf + ppackage->offset;
	memcpy(ppackage->fldGrp[ppackage->fldNum].value,value,len);
	ppackage->fldGrp[ppackage->fldNum].value[len] = 0;
	ppackage->offset += (len+1);
	++ppackage->fldNum;
	return(ppackage->fldNum);
}

/*
����
	��һ�����õ�����
�������
	ppackage	��ָ��
	fldTag		���ʶ
	value		���ֵ
�������
	ppackage	��ָ��
����ֵ
	>=0	�ɹ�������Ŀ
	<0	������
*/
int UnionPutStrTypePackageFld(PUnionPackage ppackage,int fldTag,char *value)
{
	return(UnionPutPackageFld(ppackage,fldTag,conDataFormatIsAscii,strlen(value),value));
}

/*
����
	��һ�����õ�����
�������
	ppackage	��ָ��
	fldTag		���ʶ
	value		���ֵ
	len		���ݵĳ���
�������
	ppackage	��ָ��
����ֵ
	>=0	�ɹ�������Ŀ
	<0	������
*/
int UnionPutStrTypePackageFldWithLen(PUnionPackage ppackage,int fldTag,int len,char *value)
{
	return(UnionPutPackageFld(ppackage,fldTag,conDataFormatIsAscii,len,value));
}

/*
����
	��һ�����õ�����
�������
	ppackage	��ָ��
	fldTag		���ʶ
	value		���ֵ
�������
	ppackage	��ָ��
����ֵ
	>=0	�ɹ�������Ŀ
	<0	������
*/
int UnionPutIntTypePackageFld(PUnionPackage ppackage,int fldTag,int value)
{
	char	tmpBuf[20];
	
	sprintf(tmpBuf,"%d",value);
	return(UnionPutPackageFld(ppackage,fldTag,conDataFormatIsAscii,strlen(tmpBuf),tmpBuf));
}

/*
����
	��һ�����õ�����
�������
	ppackage	��ָ��
	fldTag		���ʶ
	value		���ֵ
�������
	ppackage	��ָ��
����ֵ
	>=0	�ɹ�������Ŀ
	<0	������
*/
int UnionPutLongTypePackageFld(PUnionPackage ppackage,int fldTag,long value)
{
	char	tmpBuf[20];
	
	sprintf(tmpBuf,"%ld",value);
	return(UnionPutPackageFld(ppackage,fldTag,conDataFormatIsAscii,strlen(tmpBuf),tmpBuf));
}

/*
����
	��һ�����õ�����
�������
	ppackage	��ָ��
	fldTag		���ʶ
	value		���ֵ
�������
	ppackage	��ָ��
����ֵ
	>=0	�ɹ�������Ŀ
	<0	������
*/
int UnionPutCharTypePackageFld(PUnionPackage ppackage,int fldTag,char value)
{
	char	tmpBuf[20];
	
	sprintf(tmpBuf,"%c",value);
	return(UnionPutPackageFld(ppackage,fldTag,conDataFormatIsAscii,strlen(tmpBuf),tmpBuf));
}

/*
����
	�Ӱ��ж�һ����
�������
	ppackage	��ָ��
	fldTag		���ʶ
	sizeOfBuf	���ֵ�Ļ����С
�������
	dataFormat	��ı��뷽ʽ
	value		���ֵ	
����ֵ
	>=0	�ɹ��������򳤶�
	<0	������
*/
int UnionReadPackageFld(PUnionPackage ppackage,int fldTag,int *dataFormat,char *value,int sizeOfBuf)
{
	int	index;
	
	for (index = 0; index < ppackage->fldNum; index++)
	{
		if (ppackage->fldGrp[index].tag != fldTag)
			continue;
		if (ppackage->fldGrp[index].len >= sizeOfBuf)
		{
			UnionUserErrLog("in UnionReadPackageFld:: sizeOfBuf [%d] too small to read fld [%03d]!\n",sizeOfBuf,fldTag);
			return(errCodeSmallBuffer);
		}
		if (ppackage->fldGrp[index].len < 0)
		{
			UnionUserErrLog("in UnionReadPackageFld:: fldLen = [%03d] error!\n",ppackage->fldGrp[index].len);
			return(errCodeEsscMDL_PackageFldValueLen);
		}
		if (dataFormat != NULL)
			*dataFormat = ppackage->fldGrp[index].dataFormat;
		if (value == NULL)
			return(0);
		memcpy(value,ppackage->fldGrp[index].value,ppackage->fldGrp[index].len);
		value[ppackage->fldGrp[index].len] = 0;
		return(ppackage->fldGrp[index].len);
	}
	//UnionAuditLog("in UnionReadPackageFld:: fldTag = [%03d] not defined in this package!\n",fldTag);
	UnionProgramerLog("in UnionReadPackageFld:: fldTag = [%03d] not defined in this package!\n",fldTag);
	return(errCodeEsscMDL_EsscPackageFldNotFound);
}

/*
����
	�Ӱ��ж�һ����
�������
	ppackage	��ָ��
	fldTag		���ʶ
	sizeOfBuf	���ֵ�Ļ����С
�������
	value		���ֵ	
����ֵ
	>=0	�ɹ��������򳤶�
	<0	������
*/
int UnionReadStrTypePackageFld(PUnionPackage ppackage,int fldTag,char *value,int sizeOfBuf)
{
	return(UnionReadPackageFld(ppackage,fldTag,NULL,value,sizeOfBuf));
}

/*
����
	�Ӱ��ж�һ����
�������
	ppackage	��ָ��
	fldTag		���ʶ
	sizeOfBuf	���ֵ�Ļ����С
�������
	value		���ֵ	
����ֵ
	>=0	�ɹ��������򳤶�
	<0	������
*/
int UnionReadBitsTypePackageFld(PUnionPackage ppackage,int fldTag,char *value,int sizeOfBuf)
{
	return(UnionReadPackageFld(ppackage,fldTag,NULL,value,sizeOfBuf));
}

/*
����
	�Ӱ��ж�һ����
�������
	ppackage	��ָ��
	fldTag		���ʶ
	sizeOfBuf	���ֵ�Ļ����С
�������
	value		���ֵ	
����ֵ
	>=0	�ɹ��������򳤶�
	<0	������
*/
int UnionReadIntTypePackageFld(PUnionPackage ppackage,int fldTag,int *value)
{
	char	tmpBuf[20];
	int	ret;
	
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadPackageFld(ppackage,fldTag,NULL,tmpBuf,sizeof(tmpBuf))) < 0)
		return(ret);
	*value = atoi(tmpBuf);
	return(ret);
}

/*
����
	�Ӱ��ж�һ����
�������
	ppackage	��ָ��
	fldTag		���ʶ
	sizeOfBuf	���ֵ�Ļ����С
�������
	value		���ֵ	
����ֵ
	>=0	�ɹ��������򳤶�
	<0	������
*/
int UnionReadLongTypePackageFld(PUnionPackage ppackage,int fldTag,long *value)
{
	char	tmpBuf[20];
	int	ret;
	
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadPackageFld(ppackage,fldTag,NULL,tmpBuf,sizeof(tmpBuf))) < 0)
		return(ret);
	*value = atol(tmpBuf);
	return(ret);
}

/*
����
	�Ӱ��ж�һ����
�������
	ppackage	��ָ��
	fldTag		���ʶ
	sizeOfBuf	���ֵ�Ļ����С
�������
	value		���ֵ	
����ֵ
	>=0	�ɹ��������򳤶�
	<0	������
*/
int UnionReadCharTypePackageFld(PUnionPackage ppackage,int fldTag,char value)
{
	char	tmpBuf[20];
	int	ret;
	
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadPackageFld(ppackage,fldTag,NULL,tmpBuf,sizeof(tmpBuf))) < 0)
		return(ret);
	value = tmpBuf[0];
	return(ret);
}

/*
����
	�Ӱ��ж�һ����
�������
	ppackage	��ָ��
	fldTag		���ʶ
	sizeOfBuf	���ֵ�Ļ����С
�������
	value		���ֵ	
����ֵ
	>=0	�ɹ��������򳤶�
	<0	������
*/
int UnionReadDoubleTypePackageFld(PUnionPackage ppackage,int fldTag,double *value)
{
	char	tmpBuf[20];
	int	ret;
	
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadPackageFld(ppackage,fldTag,NULL,tmpBuf,sizeof(tmpBuf))) < 0)
		return(ret);
	*value = atof(tmpBuf);
	return(ret);
}
/*
����
	����д����־
�������
	ppackage	��ָ��
�������
	��
����ֵ
	��
*/
void UnionLogPackage(PUnionPackage ppackage)
{
#ifndef _WIN32
	int	index;
	char	tmpBuf[conMaxPackSizeOfEngine*2+1];
	int	len;
	
	if (ppackage == NULL)
		return;
	if (ppackage->direction == conPackIsRequest)
		UnionNullLogWithTime("request:: service=%03d\n",ppackage->serviceID);
	else if (ppackage->direction == conPackIsResponse)
		UnionNullLogWithTime("response:: service=%03d responseCode=%d\n",ppackage->serviceID,ppackage->responseCode);
	else
		UnionNullLogWithTime("dataBlock:: service=%03d responseCode=%d\n",ppackage->serviceID,ppackage->responseCode);
	UnionNullLogWithTime("fldNum = [%04d]\n",ppackage->fldNum);
	for (index = 0; index < ppackage->fldNum; index++)
	{
		UnionNullLog("[%03d] [%d] [%04d] ",ppackage->fldGrp[index].tag,ppackage->fldGrp[index].dataFormat,ppackage->fldGrp[index].len);
		if (ppackage->fldGrp[index].dataFormat == conDataFormatIsAscii)
			UnionRealNullLog("[%s]\n",ppackage->fldGrp[index].value);
		else
		{
			if ((len = ppackage->fldGrp[index].len) >= sizeof(tmpBuf))
				len = sizeof(tmpBuf)/2;
			bcdhex_to_aschex(ppackage->fldGrp[index].value,len,tmpBuf);
			tmpBuf[len*2] = 0;
			UnionRealNullLog("[%s]\n",tmpBuf);
		}
	}
#endif
	return;
}

/*
����
	�������뵽����
�������
	ppackage	��ָ��
	sizeOfBuf	�����С
�������
	buf		����ɵĴ�
����ֵ
	>=0	�ɹ�������ɵĴ�����
	<0	������
*/
int UnionPackPackage(PUnionPackage ppackage,char *buf,int sizeOfBuf)
{
	int	ret;
	int	offset = 0;
	int	thisLen;
	int	index;
	
	if ((buf == NULL) || (ppackage == NULL))
	{
		UnionUserErrLog("in UnionPackPackage:: null pointer!\n");
		return(errCodeNullPointer);
	}

	UnionLogPackage(ppackage);

	if (sizeOfBuf < 1 + 3)
	{
		UnionUserErrLog("in UnionPackPackage:: sizeOfBuf [%d] too small!\n",sizeOfBuf);
		return(errCodeSmallBuffer);
	}
	sprintf(buf+offset,"%d",ppackage->direction % 10);
	offset++;
	sprintf(buf+offset,"%03d",ppackage->serviceID % 1000);
	offset += 3;
	if ((ppackage->direction != conPackIsRequest) && (ppackage->direction != conPackIsData))
	{
		if (sizeOfBuf < offset + 8)
		{
			UnionUserErrLog("in UnionPackPackage:: sizeOfBuf [%d] too small!\n",sizeOfBuf);
			return(errCodeSmallBuffer);
		}
		sprintf(buf+offset,"%08d",ppackage->responseCode);
		offset += 8;
	}
	if (sizeOfBuf < offset + 3)
	{
		UnionUserErrLog("in UnionPackPackage:: sizeOfBuf [%d] too small!\n",sizeOfBuf);
		return(errCodeSmallBuffer);
	}
	sprintf(buf+offset,"%03d",ppackage->fldNum);
	offset += 3;
	for (index = 0; index < ppackage->fldNum; index++)
	{
		if ((thisLen = UnionPutPackageFldIntoStr(&(ppackage->fldGrp[index]),buf+offset,sizeOfBuf-offset)) < 0)
		{
			UnionUserErrLog("in UnionPackPackage:: UnionPutPackageFldIntoStr [%03d]\n",ppackage->fldGrp[index].tag);
			return(thisLen);
		}
		offset += thisLen;
	}
	return(offset);
}

/*
����
	��һ�����⵽����
�������
	data		��
	lenOfData	������
�������
	ppackage	��ָ��
����ֵ
	>=0	�ɹ�������Ŀ
	<0	������
*/
int UnionUnpackPackage(char *data,int lenOfData,PUnionPackage ppackage)
{
	int	ret;
	int	offset = 0;
	int	thisLen;
	int	index = 0;
	char	tmpBuf[10];
	
	if ((ppackage == NULL) || (data == NULL) || (lenOfData <= 0))
	{
		UnionUserErrLog("in UnionUnpackPackage:: null pointer!\n");
		return(errCodeParameter);
	}
	UnionInitPackage(ppackage);
	if (lenOfData < 4)
	{
		UnionUserErrLog("in UnionUnpackPackage:: lenOfData [%d] too small!\n",lenOfData);
		return(errCodeSmallBuffer);
	}
	ppackage->direction = UnionConvertIntStringToInt(data+offset,1);
	offset++;
	ppackage->serviceID = UnionConvertIntStringToInt(data+offset,3);
	offset += 3;
	if ((ppackage->direction != conPackIsRequest) && (ppackage->direction != conPackIsData))
	{
		if (lenOfData < offset + 8)
		{
			UnionUserErrLog("in UnionUnpackPackage:: lenOfData [%d] too small!\n",lenOfData);
			return(errCodeSmallBuffer);
		}
		ppackage->responseCode = UnionConvertIntStringToInt(data+offset,8);
		offset += 8;
	}
	if (lenOfData < offset + 3)
	{
		UnionUserErrLog("in UnionUnpackPackage:: lenOfData [%d] too small!\n",lenOfData);
		return(errCodeSmallBuffer);
	}
	ppackage->fldNum = UnionConvertIntStringToInt(data+offset,3);
	offset += 3;
	for (index = 0; index < ppackage->fldNum; index++)
	{
		ppackage->fldGrp[index].value = ppackage->dataBuf+ppackage->offset;
		if ((thisLen = UnionReadPackageFldFromStr(data+offset,lenOfData-offset,
				&(ppackage->fldGrp[index]),
				sizeof(ppackage->dataBuf)-ppackage->offset)) < 0)
		{
			UnionUserErrLog("in UnionUnpackPackage:: UnionReadPackageFldFromStr [%03d]\n",index);
			return(thisLen);
		}
		offset += thisLen;
		ppackage->offset += (ppackage->fldGrp[index].len + 1);
	}
	UnionLogPackage(ppackage);
	if (ppackage->responseCode < 0)
	{
		UnionUserErrLog("in UnionUnpackPackage:: ppackage->responseCode = [%d]!\n",ppackage->responseCode);
		return(ppackage->responseCode);
	}
	return(ppackage->fldNum);
}

