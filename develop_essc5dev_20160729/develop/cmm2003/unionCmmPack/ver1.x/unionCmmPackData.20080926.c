//	Wolfgang Wang, 2008/9/26

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "unionCmmPackData.h"
#include "unionErrCode.h"
#include "UnionLog.h"

// ��һ����������뵽����,���ش��뵽���е����ݵĳ���
/*
�������
	tag		���ʶ
	value		��ֵ
	sizeOfBuf	������ֵ�Ļ����С
�������
	buf		����ֵ����İ�
����ֵ
	>=0		��ֵ�ڰ���ռ�ĳ���
	<0		�������
*/
int UnionPutCharTypeCmmPackFldIntoStr(int tag,char value,int sizeOfBuf,char *buf)
{
	int 	lenOfValue;
	char	tmpBuf[100];
	
	sprintf(tmpBuf,"%c",value);
	lenOfValue = strlen(tmpBuf);
	
	return(UnionPutCmmPackFldIntoStr(tag,tmpBuf,lenOfValue,sizeOfBuf,buf));
}

// ��һ����������뵽����,���ش��뵽���е����ݵĳ���
/*
�������
	tag		���ʶ
	value		��ֵ
	sizeOfBuf	������ֵ�Ļ����С
�������
	buf		����ֵ����İ�
����ֵ
	>=0		��ֵ�ڰ���ռ�ĳ���
	<0		�������
*/
int UnionPutDoubleTypeCmmPackFldIntoStr(int tag,double value,int sizeOfBuf,char *buf)
{
	int 	lenOfValue;
	char	tmpBuf[100];
	
	sprintf(tmpBuf,"%lf",value);
	lenOfValue = strlen(tmpBuf);
	
	return(UnionPutCmmPackFldIntoStr(tag,tmpBuf,lenOfValue,sizeOfBuf,buf));
}

// ��һ����������뵽����,���ش��뵽���е����ݵĳ���
/*
�������
	tag		���ʶ
	value		��ֵ
	sizeOfBuf	������ֵ�Ļ����С
�������
	buf		����ֵ����İ�
����ֵ
	>=0		��ֵ�ڰ���ռ�ĳ���
	<0		�������
*/
int UnionPutLongTypeCmmPackFldIntoStr(int tag,long value,int sizeOfBuf,char *buf)
{
	int 	lenOfValue;
	char	tmpBuf[100];
	
	sprintf(tmpBuf,"%ld",value);
	lenOfValue = strlen(tmpBuf);
	
	return(UnionPutCmmPackFldIntoStr(tag,tmpBuf,lenOfValue,sizeOfBuf,buf));
}

// ��һ����������뵽����,���ش��뵽���е����ݵĳ���
/*
�������
	tag		���ʶ
	value		��ֵ
	sizeOfBuf	������ֵ�Ļ����С
�������
	buf		����ֵ����İ�
����ֵ
	>=0		��ֵ�ڰ���ռ�ĳ���
	<0		�������
*/
int UnionPutIntTypeCmmPackFldIntoStr(int tag,int value,int sizeOfBuf,char *buf)
{
	int 	lenOfValue;
	char	tmpBuf[100];
	
	sprintf(tmpBuf,"%d",value);
	lenOfValue = strlen(tmpBuf);
	
	return(UnionPutCmmPackFldIntoStr(tag,tmpBuf,lenOfValue,sizeOfBuf,buf));
}

// ��һ����������뵽����,���ش��뵽���е����ݵĳ���
/*
�������
	tag		���ʶ
	value		��ֵ
	lenOfValue	��ֵ����
	sizeOfBuf	������ֵ�Ļ����С
�������
	buf		����ֵ����İ�
����ֵ
	>=0		��ֵ�ڰ���ռ�ĳ���
	<0		�������
*/
int UnionPutCmmPackFldIntoStr(int tag,char *value,int lenOfValue,int sizeOfBuf,char *buf)
{
	if ((buf == NULL) || (lenOfValue < 0) || (value == NULL))
		return(errCodeParameter);

	if ( tag < 0 || tag > 999 || lenOfValue > 9999)
		return(errCodeParameter);
		
	if (lenOfValue + 3 + 4 >= sizeOfBuf)
	{
		UnionUserErrLog("in UnionPutCmmPackFldIntoStr:: sizeOfBuf [%d] too small!\n",sizeOfBuf);
		return(errCodeSmallBuffer);
	}
	sprintf(buf,"%03d%04d",tag,lenOfValue);
	memcpy(buf+7,value,lenOfValue);
	return(7+lenOfValue);
}

// ��һ�������ж�ȡһ���򣬷������ڰ���ռ�ĳ���
/*
�������
	data		�����ڵ����ݴ�
	len		���ݴ��ĳ���
	sizeOfBuf	������ֵ�Ļ����С
�������
	tag		���ʶ
	value		��ֵ
	fldLen		��ֵ����
����ֵ
	>=0		��ֵ�ڰ���ռ�ĳ���
	<0		�������
*/
int UnionReadCmmPackFldFromStr(char *data,int len,int sizeOfBuf,int *tag,int *fldLen,char *value)
{
	char	tmpBuf[10];
	int	copyDataLen;
			
	if (len == 0)
		return(0);
	if ((tag == NULL) || (fldLen == NULL) || (data == NULL) || (len < 7))
		return(errCodeParameter);
	memcpy(tmpBuf,data,3);
	tmpBuf[3] = 0;	
	*tag = atoi(tmpBuf);
	memcpy(tmpBuf,data+3,4);
	tmpBuf[4] = 0;	
	*fldLen = atoi(tmpBuf);
	if (*fldLen < 0)
	{
		UnionUserErrLog("in UnionReadCmmPackFldFromStr:: len = [%d] for fldIndex = [%03d] error!\n",*fldLen,tag);
		return(errCodeCmmPackMDL_DatatFldLength);
	}
	if (*fldLen + 7 > len)
	{
		UnionUserErrLog("in UnionReadCmmPackFldFromStr:: dataLen = [%04d] shorter than expected [%04d]!\n",len,*fldLen+7);
		return(errCodeCmmPackMDL_DatatLengthToShort);
	}
	copyDataLen = *fldLen;
	if (value != NULL)
	{
		if (copyDataLen >= sizeOfBuf)
		{
			UnionUserErrLog("in UnionReadCmmPackFldFromStr:: sizeOfBuf [%d] smaller than expected [%04d]\n",
					sizeOfBuf,copyDataLen);
			return(errCodeSmallBuffer);
		}
		memcpy(value,data+7,copyDataLen);
		value[copyDataLen] = 0;
	}
	return(7+copyDataLen);
}

// ��һ�������ж�ȡһ��ָ����ʶ���򣬷�����ֵ�ĳ���
/*
�������
	data		�����ڵ����ݴ�
	len		���ݴ��ĳ���
	tag		���ʶ
�������
	value		��ֵ
����ֵ
	>=0		��ֵ�ĳ���
	<0		�������
*/
int UnionReadSpecCharTypeCmmPackFldFromStr(char *data,int len,int tag,char *value)
{
	int	ret = 0;
	char	tmpBuf[128+1];
	
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadSpecCmmPackFldFromStr(data,len,tag,sizeof(tmpBuf),tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionReadSpecCharTypeCmmPackFldFromStr:: UnionReadSpecCmmPackFldFromStr [%d] !\n",tag);
		return(ret);
	}
	*value = tmpBuf[0];
	return(ret);
}

// ��һ�������ж�ȡһ��ָ����ʶ���򣬷�����ֵ�ĳ���
/*
�������
	data		�����ڵ����ݴ�
	len		���ݴ��ĳ���
	tag		���ʶ
�������
	value		��ֵ
����ֵ
	>=0		��ֵ�ĳ���
	<0		�������
*/
int UnionReadSpecDoubleTypeCmmPackFldFromStr(char *data,int len,int tag,double *value)
{
	int	ret = 0;
	char	tmpBuf[128+1];
	
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadSpecCmmPackFldFromStr(data,len,tag,sizeof(tmpBuf),tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionReadSpecDoubleTypeCmmPackFldFromStr:: UnionReadSpecCmmPackFldFromStr [%d] !\n",tag);
		return(ret);
	}
	*value = atof(tmpBuf);
	return(ret);
}

// ��һ�������ж�ȡһ��ָ����ʶ���򣬷�����ֵ�ĳ���
/*
�������
	data		�����ڵ����ݴ�
	len		���ݴ��ĳ���
	tag		���ʶ
�������
	value		��ֵ
����ֵ
	>=0		��ֵ�ĳ���
	<0		�������
*/
int UnionReadSpecLongTypeCmmPackFldFromStr(char *data,int len,int tag,long *value)
{
	int	ret = 0;
	char	tmpBuf[128+1];
	
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadSpecCmmPackFldFromStr(data,len,tag,sizeof(tmpBuf),tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionReadSpecLongTypeCmmPackFldFromStr:: UnionReadSpecCmmPackFldFromStr [%d] !\n",tag);
		return(ret);
	}
	*value = atol(tmpBuf);
	return(ret);
}

// ��һ�������ж�ȡһ��ָ����ʶ���򣬷�����ֵ�ĳ���
/*
�������
	data		�����ڵ����ݴ�
	len		���ݴ��ĳ���
	tag		���ʶ
�������
	value		��ֵ
����ֵ
	>=0		��ֵ�ĳ���
	<0		�������
*/
int UnionReadSpecIntTypeCmmPackFldFromStr(char *data,int len,int tag,int *value)
{
	int	ret = 0;
	char	tmpBuf[128+1];
	
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadSpecCmmPackFldFromStr(data,len,tag,sizeof(tmpBuf),tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionReadSpecIntTypeCmmPackFldFromStr:: UnionReadSpecCmmPackFldFromStr [%d] !\n",tag);
		return(ret);
	}
	*value = atoi(tmpBuf);
	return(ret);
}

// ��һ�������ж�ȡһ��ָ����ʶ���򣬷�����ֵ�ĳ���
/*
�������
	data		�����ڵ����ݴ�
	len		���ݴ��ĳ���
	tag		���ʶ
	sizeOfBuf	������ֵ�Ļ����С
�������
	value		��ֵ
����ֵ
	>=0		��ֵ�ĳ���
	<0		�������
*/
int UnionReadSpecCmmPackFldFromStr(char *data,int len,int tag,int sizeOfBuf,char *value)
{
	int	fldLenInStr,tmpTagFlag,fldLen;
	int	offset = 0;
	char	tmpBuf[1024+1];
	
	for (;;)
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((fldLenInStr = UnionReadCmmPackFldFromStr(data+offset,len-offset,sizeof(tmpBuf),&tmpTagFlag,&fldLen,tmpBuf)) < 0)
		{
			UnionUserErrLog("in UnionReadSpecCmmPackFldFromStr:: UnionReadCmmPackFldFromStr [%d] !\n",tag);
			return(fldLenInStr);
		}
		offset += fldLenInStr;
		if (tmpTagFlag != tag)	// ����Ҫ������
			continue;
		if (fldLen >= sizeOfBuf)
		{
			UnionUserErrLog("in UnionReadSpecCmmPackFldFromStr:: sizeOfBuf [%d] smaller than expected [%04d]\n",
					sizeOfBuf,fldLen);
			return(errCodeSmallBuffer);
		}
		if (value != NULL)
		{
			memcpy(value,tmpBuf,fldLen);
			value[fldLen] = 0;
		}
		return(fldLen);
	}
}

// ��ʼ����ֵλ�ñ�
/*
�������
	data		�����ڵ����ݴ�
	len		���ݴ��ĳ���
	tag		���ʶ
	sizeOfBuf	������ֵ�Ļ����С
�������
	value		��ֵ
����ֵ
	>=0		��ֵ�ĳ���
	<0		�������
*/
int UnionInitCmmPackFldDataList(PUnionCmmPackData pposList)
{
	if (pposList == NULL)
		return(errCodeParameter);
	pposList->fldNum = 0;
	pposList->offset = 0;
	return(0);
}
	
// ��һ����ֵλ���嵥�ж�һ������
/*
�������
	pposList	��ֵλ���嵥
	sizeOfBuf	��ֵ���д�С
	tag		���ʶ
�������
	value		���ֵ
����ֵ
	>=0		��ֵ�ĳ���
	<0		�������
*/
int UnionReadCmmPackFldFromFldDataList(PUnionCmmPackData pposList,int tag,int sizeOfBuf,char *value)
{
	PUnionCmmPackFldData	ppos;
	int			ret;
	int			index;
	
	if ((value == NULL) || (sizeOfBuf < 0) || (pposList == NULL) || (tag < 0))
		return(errCodeParameter);
	if ((pposList->fldNum >= conMaxNumOfFldPerCmmPack) || (pposList->fldNum < 0))
	{
		UnionUserErrLog("in UnionReadCmmPackFldFromFldDataList:: posList->fldNum [%d] error!\n",pposList->fldNum);
		return(errCodeParameter);
	}
	for (index = 0; index < pposList->fldNum; index++)
	{
		if (pposList->fldGrp[index].tag == tag)
		{
			if ((pposList->fldGrp[index].len > sizeOfBuf) || (pposList->fldGrp[index].len < 0))
			{
				UnionUserErrLog("in UnionReadCmmPackFldFromFldDataList:: pposList->fldGrp[index].len [%d] error!\n",pposList->fldGrp[index].len);
				return(errCodeCmmPackMDL_DatatFldLength);
			}
			//UnionLog("in UnionReadCmmPackFldFromFldDataList:: [%03d][%04d][%s]\n",tag,pposList->fldGrp[index].len,pposList->fldGrp[index].value);
			memcpy(value,pposList->fldGrp[index].value,pposList->fldGrp[index].len);
			value[pposList->fldGrp[index].len] = 0;
			return(pposList->fldGrp[index].len);
		}
	}
	UnionAuditLog("in UnionReadCmmPackFldFromFldDataList:: tag [%03d] not defined!\n",tag);
	return(errCodeCmmPackMDL_FldNotDefined);
}

// ��һ����ֵλ���嵥�ж�һ������
/*
�������
	pposList	��ֵλ���嵥
	tag		���ʶ
�������
	value		���ֵ
����ֵ
	>=0		��ֵ�ĳ���
	<0		�������
*/
int UnionReadIntTypeCmmPackFldFromFldDataList(PUnionCmmPackData pposList,int tag,int *value)
{
	char	tmpBuf[100];
	int	ret;
	
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadCmmPackFldFromFldDataList(pposList,tag,sizeof(tmpBuf),tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionReadIntTypeCmmPackFldFromFldDataList:: UnionReadCmmPackFldFromFldDataList! ret = [%d]!\n",ret);
		return(ret);
	}
	*value = atoi(tmpBuf);
	return(ret);
}

// ��һ����ֵλ���嵥�ж�һ������
/*
�������
	pposList	��ֵλ���嵥
	tag		���ʶ
�������
	value		���ֵ
����ֵ
	>=0		��ֵ�ĳ���
	<0		�������
*/
int UnionReadLongTypeCmmPackFldFromFldDataList(PUnionCmmPackData pposList,int tag,long *value)
{
	char	tmpBuf[100];
	int	ret;
	
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadCmmPackFldFromFldDataList(pposList,tag,sizeof(tmpBuf),tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionReadLongTypeCmmPackFldFromFldDataList:: UnionReadCmmPackFldFromFldDataList! ret = [%d]!\n",ret);
		return(ret);
	}
	*value = atol(tmpBuf);
	return(ret);
}

// ��һ����ֵλ���嵥�ж�һ������
/*
�������
	pposList	��ֵλ���嵥
	tag		���ʶ
�������
	value		���ֵ
����ֵ
	>=0		��ֵ�ĳ���
	<0		�������
*/
int UnionReadCharTypeCmmPackFldFromFldDataList(PUnionCmmPackData pposList,int tag,char *value)
{
	char	tmpBuf[100];
	int	ret;
	
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadCmmPackFldFromFldDataList(pposList,tag,sizeof(tmpBuf),tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionReadCharTypeCmmPackFldFromFldDataList:: UnionReadCmmPackFldFromFldDataList! ret = [%d]!\n",ret);
		return(ret);
	}
	*value = tmpBuf[0];
	return(ret);
}

// ��һ����ֵλ���嵥�ж�һ������
/*
�������
	pposList	��ֵλ���嵥
	tag		���ʶ
�������
	value		���ֵ
����ֵ
	>=0		��ֵ�ĳ���
	<0		�������
*/
int UnionReadUnsignedIntTypeCmmPackFldFromFldDataList(PUnionCmmPackData pposList,int tag,unsigned int *value)
{
	char	tmpBuf[100];
	int	ret;
	
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadCmmPackFldFromFldDataList(pposList,tag,sizeof(tmpBuf),tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionReadUnsignedIntTypeCmmPackFldFromFldDataList:: UnionReadCmmPackFldFromFldDataList! ret = [%d]!\n",ret);
		return(ret);
	}
	*value = atoi(tmpBuf);
	return(ret);
}

// ��һ����ֵλ���嵥�ж�һ������
/*
�������
	pposList	��ֵλ���嵥
	tag		���ʶ
�������
	value		���ֵ
����ֵ
	>=0		��ֵ�ĳ���
	<0		�������
*/
int UnionReadUnsignedLongTypeCmmPackFldFromFldDataList(PUnionCmmPackData pposList,int tag,unsigned long *value)
{
	char	tmpBuf[100];
	int	ret;
	
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadCmmPackFldFromFldDataList(pposList,tag,sizeof(tmpBuf),tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionReadUnsignedLongTypeCmmPackFldFromFldDataList:: UnionReadCmmPackFldFromFldDataList! ret = [%d]!\n",ret);
		return(ret);
	}
	*value = atol(tmpBuf);
	return(ret);
}

// ��һ����ֵλ���嵥�ж�һ������
/*
�������
	pposList	��ֵλ���嵥
	tag		���ʶ
�������
	value		���ֵ
����ֵ
	>=0		��ֵ�ĳ���
	<0		�������
*/
int UnionReadUnsignedCharTypeCmmPackFldFromFldDataList(PUnionCmmPackData pposList,int tag,unsigned char *value)
{
	char	tmpBuf[100];
	int	ret;
	
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadCmmPackFldFromFldDataList(pposList,tag,sizeof(tmpBuf),tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionReadUnsignedCharTypeCmmPackFldFromFldDataList:: UnionReadCmmPackFldFromFldDataList! ret = [%d]!\n",ret);
		return(ret);
	}
	*value = tmpBuf[0];
	return(ret);
}

// ��һ����ֵλ���嵥�ж�һ������
/*
�������
	pposList	��ֵλ���嵥
	tag		���ʶ
�������
	value		���ֵ
����ֵ
	>=0		��ֵ�ĳ���
	<0		�������
*/
int UnionReadDoubleTypeCmmPackFldFromFldDataList(PUnionCmmPackData pposList,int tag,double *value)
{
	char	tmpBuf[100];
	int	ret;
	
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadCmmPackFldFromFldDataList(pposList,tag,sizeof(tmpBuf),tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionReadDoubleTypeCmmPackFldFromFldDataList:: UnionReadCmmPackFldFromFldDataList! ret = [%d]!\n",ret);
		return(ret);
	}
	*value = atof(tmpBuf);
	return(ret);
}

// ��һ����ֵλ���嵥�ж�һ������
/*
�������
	pposList	��ֵλ���嵥
	tag		���ʶ
�������
	value		���ֵ
����ֵ
	>=0		��ֵ�ĳ���
	<0		�������
*/
int UnionReadStringTypeCmmPackFldFromFldDataList(PUnionCmmPackData pposList,int tag,char *value)
{
	char	tmpBuf[2048+1];
	int	ret;
	
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadCmmPackFldFromFldDataList(pposList,tag,sizeof(tmpBuf),tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionReadStringTypeCmmPackFldFromFldDataList:: UnionReadCmmPackFldFromFldDataList! ret = [%d]!\n",ret);
		return(ret);
	}
	strcpy(value,tmpBuf);
	return(ret);
}


// ����ֵ�н����һ����ֵλ���嵥��
/*
�������
	data		�����ڵ����ݴ�
	len		���ݴ��ĳ���
�������
	pposList	��ֵλ���嵥
����ֵ
	>=0		��ֵ����Ŀ
	<0		�������
*/
int UnionUnpackCmmPackIntoFldDataList(char *data,int len,PUnionCmmPackData pposList)
{
	int	offset = 0;
	int	fldLenInStr;
	PUnionCmmPackFldData	ppos;
	int	fldNum;
	
	UnionInitCmmPackFldDataList(pposList);
	if (len < 3)
	{
		UnionUserErrLog("in UnionUnpackCmmPackIntoFldDataList:: lenOfData = [%d] too short!\n",len);
		return(errCodeAPIPackageTooShort);
	}
	fldNum = UnionConvertIntStringToInt(data,3);
	offset = 3;
	pposList->fldNum = 0;
	for (;;)
	{
		//if (offset == len)
		//	break;
		if (pposList->fldNum >= fldNum)
			return(pposList->fldNum);
		ppos = &(pposList->fldGrp[pposList->fldNum]);
		ppos->value = pposList->data + pposList->offset;
		if ((fldLenInStr = UnionReadCmmPackFldFromStr(data+offset,len-offset,sizeof(pposList->data)-pposList->offset,&(ppos->tag),&(ppos->len),ppos->value)) < 0)
		{
			UnionUserErrLog("in UnionUnpackCmmPackIntoFldDataList:: UnionReadCmmPackFldFromStr! len = [%04d][%s] fldIndex = [%d] offset = [%d]\n",len,data,pposList->fldNum,offset);
			return(fldLenInStr);
		}
		if (fldLenInStr == 0)
			break;
		pposList->offset += (ppos->len + 1);
		pposList->fldNum += 1;
		offset += fldLenInStr;
	}
	if (fldNum != pposList->fldNum)
	{
		UnionAuditLog("in UnionUnpackCmmPackIntoFldDataList:: real fldNum [%d] != that in package [%d]!\n",pposList->fldNum,fldNum);
	}
	//UnionLogCmmPackData("after unpack::",pposList);
	return(pposList->fldNum);
}

// �������һ����ֵд��һ����ֵλ���嵥��
/*
�������
	len		���ݴ��ĳ���
	tag		���ʶ
	value		���ݴ���ֵ
�������
	pposList	��ֵλ���嵥
����ֵ
	>=0		��ֵ����Ŀ
	<0		�������
*/
int UnionPutCmmPackFldIntoFldDataList(int tag,int len,char *value,PUnionCmmPackData pposList)
{
	PUnionCmmPackFldData	ppos;
	int			ret;
	
	if ((value == NULL) || (len < 0) || (pposList == NULL) || (tag < 0))
		return(errCodeParameter);
		
	if ((pposList->fldNum >= conMaxNumOfFldPerCmmPack) || (pposList->fldNum < 0))
	{
		UnionUserErrLog("in UnionPutCmmPackFldIntoFldDataList:: posList->fldNum [%d] error!\n",pposList->fldNum);
		return(errCodeParameter);
	}
	ppos = &(pposList->fldGrp[pposList->fldNum]);
	ppos->value = pposList->data + pposList->offset;
	ppos->tag = tag;
	ppos->len = len;
	if ((ppos->len + pposList->offset) >= sizeof(pposList->data))
	{
		UnionUserErrLog("in UnionPutCmmPackFldIntoFldDataList:: posList->data size [%d] <= expected [%d] + [%d]!\n",sizeof(pposList->data),ppos->len,pposList->offset);
		return(errCodeParameter);
	}
	memcpy(ppos->value,value,len);
	ppos->value[len] = 0;
	pposList->offset += (ppos->len + 1);
	pposList->fldNum += 1;
	return(pposList->fldNum);
}


// ��һ����ֵд��һ����ֵλ���嵥��
/*
�������
	tag		���ʶ
	value		����ֵ
�������
	pposList	��ֵλ���嵥
����ֵ
	>=0		��ֵ����Ŀ
	<0		�������
*/
int UnionPutIntTypeCmmPackFldIntoFldDataList(int tag,int value,PUnionCmmPackData pposList)
{
	char	tmpBuf[100+1];
	int	len;
	
	sprintf(tmpBuf,"%d",value);
	len = strlen(tmpBuf);
	return(UnionPutCmmPackFldIntoFldDataList(tag,len,tmpBuf,pposList));
}

// ��һ����ֵд��һ����ֵλ���嵥��
/*
�������
	tag		���ʶ
	value		����ֵ
�������
	pposList	��ֵλ���嵥
����ֵ
	>=0		��ֵ����Ŀ
	<0		�������
*/
int UnionPutLongTypeCmmPackFldIntoFldDataList(int tag,long value,PUnionCmmPackData pposList)
{
	char	tmpBuf[100+1];
	int	len;
	
	sprintf(tmpBuf,"%ld",value);
	len = strlen(tmpBuf);
	return(UnionPutCmmPackFldIntoFldDataList(tag,len,tmpBuf,pposList));
}

// ��һ����ֵд��һ����ֵλ���嵥��
/*
�������
	tag		���ʶ
	value		����ֵ
�������
	pposList	��ֵλ���嵥
����ֵ
	>=0		��ֵ����Ŀ
	<0		�������
*/
int UnionPutCharTypeCmmPackFldIntoFldDataList(int tag,char value,PUnionCmmPackData pposList)
{
	char	tmpBuf[100+1];
	int	len;
	
	sprintf(tmpBuf,"%c",value);
	len = strlen(tmpBuf);
	return(UnionPutCmmPackFldIntoFldDataList(tag,len,tmpBuf,pposList));
}

// ��һ����ֵд��һ����ֵλ���嵥��
/*
�������
	tag		���ʶ
	value		����ֵ
�������
	pposList	��ֵλ���嵥
����ֵ
	>=0		��ֵ����Ŀ
	<0		�������
*/
int UnionPutUnsignedIntTypeCmmPackFldIntoFldDataList(int tag,unsigned int value,PUnionCmmPackData pposList)
{
	char	tmpBuf[100+1];
	int	len;
	
	sprintf(tmpBuf,"%d",value);
	len = strlen(tmpBuf);
	return(UnionPutCmmPackFldIntoFldDataList(tag,len,tmpBuf,pposList));
}

// ��һ����ֵд��һ����ֵλ���嵥��
/*
�������
	tag		���ʶ
	value		����ֵ
�������
	pposList	��ֵλ���嵥
����ֵ
	>=0		��ֵ����Ŀ
	<0		�������
*/
int UnionPutUnsignedLongTypeCmmPackFldIntoFldDataList(int tag,unsigned long value,PUnionCmmPackData pposList)
{
	char	tmpBuf[100+1];
	int	len;
	
	sprintf(tmpBuf,"%ld",value);
	len = strlen(tmpBuf);
	return(UnionPutCmmPackFldIntoFldDataList(tag,len,tmpBuf,pposList));
}

// ��һ����ֵд��һ����ֵλ���嵥��
/*
�������
	tag		���ʶ
	value		����ֵ
�������
	pposList	��ֵλ���嵥
����ֵ
	>=0		��ֵ����Ŀ
	<0		�������
*/
int UnionPutUnsignedCharTypeCmmPackFldIntoFldDataList(int tag,unsigned char value,PUnionCmmPackData pposList)
{
	char	tmpBuf[100+1];
	int	len;
	
	sprintf(tmpBuf,"%c",value);
	len = strlen(tmpBuf);
	return(UnionPutCmmPackFldIntoFldDataList(tag,len,tmpBuf,pposList));
}

// ��һ����ֵд��һ����ֵλ���嵥��
/*
�������
	tag		���ʶ
	value		����ֵ
�������
	pposList	��ֵλ���嵥
����ֵ
	>=0		��ֵ����Ŀ
	<0		�������
*/
int UnionPutDoubleTypeCmmPackFldIntoFldDataList(int tag,double value,PUnionCmmPackData pposList)
{
	char	tmpBuf[100+1];
	int	len;
	
	sprintf(tmpBuf,"%lf",value);
	len = strlen(tmpBuf);
	return(UnionPutCmmPackFldIntoFldDataList(tag,len,tmpBuf,pposList));
}

// ��һ����ֵд��һ����ֵλ���嵥��
/*
�������
	tag		���ʶ
	value		����ֵ
�������
	pposList	��ֵλ���嵥
����ֵ
	>=0		��ֵ����Ŀ
	<0		�������
*/
int UnionPutStringTypeCmmPackFldIntoFldDataList(int tag,char *value,PUnionCmmPackData pposList)
{
	char	tmpBuf[2048+1];
	int	len;
	
	sprintf(tmpBuf,"%s",value);
	len = strlen(tmpBuf);
	return(UnionPutCmmPackFldIntoFldDataList(tag,len,tmpBuf,pposList));
}


// ���������ֵ�н����һ����ֵλ���嵥��
/*
�������
	pposList	��ֵλ���嵥
	sizeOfBuf	����Ĵ�С
�������
	data		�����ڵ����ݴ�
����ֵ
	>=0		���ݰ��ĳ���
	<0		�������
*/
int UnionPackFldDataListIntoCmmPack(PUnionCmmPackData pposList,int sizeOfBuf,char *data)
{
	int	offset = 0;
	int	fldLenInStr;
	int	index = 0;
	PUnionCmmPackFldData	ppos;
	
	//UnionLogCmmPackData("before pack::",pposList);
	if (sizeOfBuf <= 3)
	{
		UnionUserErrLog("in UnionPackFldDataListIntoCmmPack:: sizeOfBuf [%d] too small!\n",sizeOfBuf);
		return(errCodeSmallBuffer);
	}
	sprintf(data,"%03d",pposList->fldNum);
	offset = 3;
	for (;index < pposList->fldNum; index++)
	{
		ppos = &(pposList->fldGrp[index]);
		if ((fldLenInStr = UnionPutCmmPackFldIntoStr(ppos->tag,ppos->value,ppos->len,sizeOfBuf-offset,data+offset)) < 0)
		{
			UnionUserErrLog("in UnionPackFldDataListIntoCmmPack:: UnionPutCmmPackFldIntoStr! fldIndex = [%d] offset = [%d]\n",index,offset);
			return(fldLenInStr);
		}
		offset += fldLenInStr;
	}
	return(offset);
}

// ����������д�뵽��־��
/*
�������
	title		����
	pposList	��ֵλ���嵥
�������
	��
����ֵ
	��
*/
void UnionLogCmmPackData(char *title,PUnionCmmPackData pposList)
{
	int	index;
	
	if (title != NULL)
		UnionNullLogWithTime("%s fldNum=[%03d]\n",title,pposList->fldNum);
	else
		UnionNullLogWithTime("fldNum=[%03d]\n",pposList->fldNum);
	for (index = 0; index < pposList->fldNum; index++)
	{
		UnionNullLogWithTime("[%03d] [%04d] [%s]\n",pposList->fldGrp[index].tag,pposList->fldGrp[index].len,pposList->fldGrp[index].value);
		//UnionNullLogWithTime("[%03d] [%04d] [%0X] [%s]\n",pposList->fldGrp[index].tag,pposList->fldGrp[index].len,pposList->fldGrp[index].value,pposList->fldGrp[index].value);
	}
	//UnionNullLog("data address = [%0X]\n",pposList->data);
	//UnionMemLog("xxxaaa",pposList->data,sizeof(pposList->data));
}

// ���Ժ���
int UnionTestCmmPackDataFun(char *data,int len)
{
	TUnionCmmPackData	oriPosList,desPosList;
	int			ret;
	int			index;
	char			tmpBuf[1024+1];
	int			fldLen;
	
	if ((ret = UnionUnpackCmmPackIntoFldDataList(data,len,&oriPosList)) < 0)
	{
		printf("UnionUnpackCmmPackIntoFldDataList error! ret = [%d]\n",ret);
		return(ret);
	}
	UnionInitCmmPackFldDataList(&desPosList);
	for (index = 0; index < oriPosList.fldNum; index++)
	{
		if ((fldLen = UnionReadCmmPackFldFromFldDataList(&oriPosList,oriPosList.fldGrp[index].tag,sizeof(tmpBuf),tmpBuf)) < 0)
		{
			printf("UnionReadCmmPackFldFromFldDataList error! ret = [%d] index = [%d] tag = [%03d]\n",fldLen,index,oriPosList.fldGrp[index].tag);
			return(fldLen);
		}
		if ((ret = UnionPutCmmPackFldIntoFldDataList(oriPosList.fldGrp[index].tag,fldLen,tmpBuf,&desPosList)) < 0)
		{
			printf("UnionPutCmmPackFldIntoFldDataList error! ret = [%d] index = [%d] tag = [%03d]\n",fldLen,index,oriPosList.fldGrp[index].tag);
			return(ret);
		}
	}
	if ((ret = UnionPackFldDataListIntoCmmPack(&desPosList,sizeof(tmpBuf),tmpBuf)) < 0)
	{
		printf("UnionPackFldDataListIntoCmmPack error! ret = [%d]\n",ret);
		return(ret);
	}
	tmpBuf[ret] = 0;
	printf("[%04d][%s]\n",ret,tmpBuf);
	return(0);
}

// ���Ʊ���
/*
�������
	poriList	Դ��
�������
	pdesList	Ŀ���
����ֵ
	>=0		����Ŀ
	<0		�������
*/
int UnionCopyPackFldDataList(PUnionCmmPackData poriList,PUnionCmmPackData pdesList)
{
	int	index = 0;
	int	offset = 0;
	
	if ((poriList == NULL) || (pdesList == NULL))
		return(errCodeParameter);
	memcpy(pdesList,poriList,sizeof(*poriList));
	for (;index < poriList->fldNum; index++)
	{
		pdesList->fldGrp[index].value = pdesList->data + offset;
		offset += (pdesList->fldGrp[index].len + 1);
	}
	return(pdesList->fldNum);
}

