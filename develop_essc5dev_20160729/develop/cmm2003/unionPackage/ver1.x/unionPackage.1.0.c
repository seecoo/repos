// Author:	zhangyd
// Date:	2015/1/8

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "UnionLog.h"
#include "unionPackage.h"
#include "unionErrCode.h"
#include "UnionLog.h"

int UnionPackageConvertIntStringToInt(unsigned char *str,int lenOfStr)
{
	/*
	char	buf[PACKAGE_MEMORY_BLOCK_SIZE];
	
	memcpy(buf,str,lenOfStr);
	buf[lenOfStr] = 0;
	return(atoi(buf));
	*/
	int	i;
	int	tmpInt = 0;
	
	for (i = 0; i < lenOfStr; i++)
		tmpInt = tmpInt * 10 + str[i] - '0';
		
	return(tmpInt);
	
	/*
	int	i;
	int	tmpInt = 0;
	int	isMinus = 1;
	
	for (i = 0; i < lenOfStr; i++)
	{
		if (!isdigit(str[i]))
		{
			if (i != 0)
				return(tmpInt*isMinus);
			switch (str[i])
			{
				case '-':
					isMinus = -1;
					break;
				case '+':
					isMinus = 1;
					break;
				default:
					return(tmpInt*isMinus);
			}
			continue;
		}
		
		if (tmpInt >= 100000000)
			return(tmpInt * isMinus);
		tmpInt = tmpInt * 10 + str[i] - '0';
	}
	return(tmpInt * isMinus);
	*/
}

/*
����
	��һ�����ݴ������
�������
	ppackage	��ָ��
	pelement	Ԫ��ָ��
	buf		����ָ��
	len		���ݴ�С
�������

����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionPackageFormatBuffer(PUnionPackage ppackage,PUnionPackageElement pelement,unsigned char *buf,int len)
{
	int	offset = 0;
	if (ppackage->buf_remain_len <= len)
	{
		unsigned char 	*buf;
		long		bufSize = ppackage->bufSize + PACKAGE_BUF_SIZE;

		if ((buf = (unsigned char *)realloc(ppackage->bufbase,bufSize * sizeof(unsigned char))) == NULL)
		{
			UnionUserErrLog("in UnionPackageFormatBuffer:: realloc[%ld] error\n",bufSize);
			return(errCodeUseOSErrCode);
		}
		ppackage->buf_remain_len = ppackage->buf_remain_len + PACKAGE_BUF_SIZE;
		ppackage->bufptr = ppackage->bufptr - ppackage->bufbase + buf ;
		ppackage->bufbase = buf ;
		ppackage->bufSize = bufSize ;	
	}
	pelement->pos = ppackage->bufptr;

	offset = (len / PACKAGE_MEMORY_BLOCK_SIZE + 1) * PACKAGE_MEMORY_BLOCK_SIZE; 

	memcpy(pelement->pos,buf,len);
	ppackage->bufptr[len] = 0;
	ppackage->buf_remain_len -= offset; 
	ppackage->bufptr += offset;

	return(offset);
}

/*
����
	��һ��Ԫ�ش��뵽����,���ش��뵽���е����ݵĳ���
�������
	pelement	Ԫ��ָ��
	elementType	Ԫ������
	sizeOfBuf	��������С
�������
	buf		����
����ֵ
	>=0	���ڰ��е��ֽ���
	<0	������
*/
int UnionPutPackageElementIntoStr(PUnionPackageElement pelement,TUnionPackageElementType elementType,char *buf,int sizeOfBuf)
{
	if ((pelement == NULL) || (buf == NULL))
		return(errCodeParameter);
		
	if (pelement->len + elementType >= sizeOfBuf)
	{
		UnionUserErrLog("in UnionPutPackageElementIntoStr:: sizeOfBuf [%d] too small!\n",sizeOfBuf);
		return(errCodeSmallBuffer);
	}
	sprintf(buf,"%0*d",elementType,pelement->len);
	memcpy(buf+elementType,pelement->pos,pelement->len);
	return(elementType + pelement->len);
}

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
	int	ret;
	int	thisLen;
	
	if ((pfld == NULL) || (buf == NULL))
		return(errCodeParameter);

	// TAG
	if ((ret = UnionPutPackageElementIntoStr(&pfld->tag,PACKAGE_ELEMENT_TAG,buf,sizeOfBuf)) < 0)
	{
		UnionUserErrLog("in UnionPutPackageFldIntoStr:: UnionPutPackageElementIntoStr!\n");
		return(ret);
	}
	thisLen = ret;
	
	// VALUE
	if ((ret = UnionPutPackageElementIntoStr(&pfld->value,PACKAGE_ELEMENT_VALUE,buf + thisLen,sizeOfBuf - thisLen)) < 0)
	{
		UnionUserErrLog("in UnionPutPackageFldIntoStr:: UnionPutPackageElementIntoStr!\n");
		return(ret);
	}
	thisLen += ret;
	return(thisLen);
}

/*
����
	��һ�������ж�ȡһ��Ԫ�أ��������ڰ���ռ�ĳ���
�������
	ppackage	��ָ��
	data		���ݰ�
	len		���ݰ�����
	elementType	Ԫ������
�������
	pelement	Ԫ��ָ��
����ֵ
	>=0	���ڰ��е��ֽ���
	<0	������
*/
int UnionReadPackageElementFromStr(PUnionPackage ppackage,unsigned char *data,int len,TUnionPackageElementType elementType,PUnionPackageElement pelement)
{
	int	ret;
	char	tmpBuf[PACKAGE_MEMORY_BLOCK_SIZE];
	
	if (len <= 0)
	{
		return(errCodeParameter);
	}
	
	memcpy(tmpBuf,data,elementType);
	tmpBuf[elementType] = 0;
	pelement->len = atoi(tmpBuf);
	//pelement->len = UnionPackageConvertIntStringToInt(data,elementType);
	if ((ret = UnionPackageFormatBuffer(ppackage,pelement,data + elementType,pelement->len)) < 0)
	{
		UnionUserErrLog("in UnionReadPackageElementFromStr:: UnionPackageFormatBuffer!\n");
		return(ret);
	}

	return(elementType + pelement->len);
}
 
/*
����
	��һ�������ж�ȡһ���򣬷������ڰ���ռ�ĳ���
�������
	data		���ݰ�
	len		���ݰ�����
�������
	pfld		��ָ��
����ֵ
	>=0	���ڰ��е��ֽ���
	<0	������
*/
int UnionReadPackageFldFromStr(PUnionPackage ppackage,unsigned char *data,int len,PUnionPackageFld pfld)
{
	int	ret;
	int	thisLen;
	
	// ��ȡTAG
	if ((thisLen = UnionReadPackageElementFromStr(ppackage,data,len,PACKAGE_ELEMENT_TAG,&pfld->tag)) < 0)
	{
		UnionUserErrLog("in UnionReadPackageFldFromStr:: UnionReadPackageElementFromStr tag data[%s]!\n",data);
		return(thisLen);
	}
	
	// ��ȡVALUE
	if ((ret = UnionReadPackageElementFromStr(ppackage,data + thisLen,len - thisLen,PACKAGE_ELEMENT_VALUE,&pfld->value)) < 0)
	{
		UnionUserErrLog("in UnionReadPackageFldFromStr:: UnionReadPackageElementFromStr value! data[%s]\n",data + thisLen);
		return(ret);
	}
	thisLen += ret;
	return(thisLen);
}

/*
����
	��ʼ������
�������
	ppackage		���Ľṹ��ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionInitPackage(PUnionPackage ppackage)
{
	int		bufSize = 0;
	
	if (ppackage == NULL)
		return(errCodeParameter);

	if (ppackage->bufSize == 0)
	{
		bufSize = PACKAGE_BUF_SIZE;
		if ((ppackage->bufbase = (unsigned char *)malloc(bufSize * sizeof(unsigned char))) == NULL)
		{
			UnionUserErrLog("in UnionInitPackage:: malloc[%d] error\n",bufSize);
			return(errCodeUseOSErrCode);
		}
		ppackage->bufptr = ppackage->bufbase;
		ppackage->bufSize = bufSize;
	}
	
	ppackage->bufptr = ppackage->bufbase;
	ppackage->num = 0;
	ppackage->index = 0;
	ppackage->buf_remain_len = ppackage->bufSize - PACKAGE_MEMORY_BLOCK_SIZE;
	return(0);
}

/*
����
	�ͷű���
�������
	ppackage		���Ľṹ��ָ��
�������
	��
����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionFreePackage(PUnionPackage ppackage)
{
	if (ppackage == NULL)
		return(0);
	
	if ((ppackage->bufSize > 0) && (ppackage->bufbase))
	{
		free(ppackage->bufbase);
	}
	ppackage->bufbase = NULL;
	ppackage->bufptr = NULL;
	ppackage->num = 0;
	ppackage->index = 0;
	ppackage->bufSize= 0;
	ppackage->buf_remain_len = 0;
	return(0);
}

/*
����
	��һ��Ԫ���õ�����
�������
	ppackage	��ָ��
	pelement	Ԫ��ָ��
	element		Ԫ��
	lenOfElement	Ԫ�صĳ���
�������
	ppackage	��ָ��
	pelement	Ԫ��ָ��
����ֵ
	>=0	�ɹ���ƫ����
	<0	������
*/
int UnionPutPackageElement(PUnionPackage ppackage,PUnionPackageElement pelement,char *element,int lenOfElement)
{
	int	ret;

	if ((ret = UnionPackageFormatBuffer(ppackage,pelement,(unsigned char *)element,lenOfElement)) < 0)
	{
		UnionUserErrLog("in UnionPutPackageElement:: UnionPackageFormatBuffer!\n");
		return(ret);
	}
	pelement->len = lenOfElement;

	return(lenOfElement);
}

/*
����
	��һ�����õ�����
�������
	ppackage	��ָ��
	tag		TAG
	lenOfTag	TAG�ĳ���
	value		VALUE
	lenOfValue	VALUE�ĳ���
�������
	ppackage	��ָ��
����ֵ
	>=0	�ɹ���ƫ����
	<0	������
*/
int UnionPutPackageFld(PUnionPackage ppackage,char *tag,int lenOfTag,char *value,int lenOfValue)
{
	int	i = 0,j = 0;
	int	ret;
	int	lenOfTagPre;
	int	lenOfTagAfter;
	int	grpFound = 0;
	int	fldFound = 0;
	char	*ptr = NULL;
	
	if (ppackage == NULL)
	{
		UnionUserErrLog("in UnionPutPackageFld:: ppackage is NULL!\n");
		return(errCodeParameter);
	}
	if ((tag == NULL) || (lenOfTag <= 0))
	{
		UnionUserErrLog("in UnionPutPackageFld:: tag is null or lenOfTag[%d] <= 0!\n",lenOfTag);
		return(errCodeParameter);
	}
	if (value == NULL)
	{
		UnionUserErrLog("in UnionPutPackageFld:: value is null!\n");
		return(errCodeParameter);
	}
	
	if (ppackage->num >= conMaxNumOfPackageFld)
	{
		UnionUserErrLog("in UnionPutPackageFld:: too much flds! num = [%d]\n",ppackage->num);
		return(errCodeEsscMDL_TooMuchEsscPackageFld);
	}
	if (ppackage->num < 0)	// �޸�����Ŀ
	{
		ppackage->num = 0;
		ppackage->index = 0;
	}
	
	if ((ptr = memchr(tag,'/',lenOfTag)) == NULL)
	{
		UnionUserErrLog("in UnionPutPackageFld:: tag[%s] error!\n",tag);
		return(errCodeParameter);
	}
	lenOfTagPre = ptr - tag;
	lenOfTagAfter = lenOfTag - lenOfTagPre - 1;
	ptr ++;
	
	// �������Ƿ����
	for (i = 0; i < ppackage->num; i++)
	{
		if ((lenOfTagPre == ppackage->fldGrp[i].fldGrpID.len) && 
			(memcmp(ppackage->fldGrp[i].fldGrpID.pos,tag,lenOfTagPre) == 0))
		{
			grpFound = 1;
			ppackage->index = i;
			break;
		}
	}
	
	if (ppackage->fldGrp[i].num < 0)	// �޸�����Ŀ
		ppackage->fldGrp[i].num = 0;

	if (!grpFound)
	{
		// ������ID
		if ((ret = UnionPutPackageElement(ppackage,&ppackage->fldGrp[i].fldGrpID,tag,lenOfTagPre)) < 0)
		{
			UnionUserErrLog("in UnionPutPackageFld:: UnionPutPackageElement tag[%s] error!\n",tag);
			return(ret);
		}
		ppackage->num ++;
		ppackage->index = i;
		ppackage->fldGrp[i].num = 0;
		ppackage->fldGrp[i].index = 0;
	}
	else
	{
		// ����TAG�Ƿ����
		for (j = 0; j < ppackage->fldGrp[i].num; j++)
		{
			if ((lenOfTagAfter == ppackage->fldGrp[i].fld[j].tag.len) && 
				(memcmp(ppackage->fldGrp[i].fld[j].tag.pos,ptr,lenOfTagAfter) == 0))
			{
				fldFound = 1;
				ppackage->fldGrp[i].index = j;
				break;
			}
		}
	}
	
	// ����TAG
	if ((ret = UnionPutPackageElement(ppackage,&ppackage->fldGrp[i].fld[j].tag,ptr,lenOfTagAfter)) < 0)
	{
		UnionUserErrLog("in UnionPutPackageFld:: UnionPutPackageElement tag[%s] error!\n",tag);
		return(ret);
	}

	// ����VALUE
	if ((ret = UnionPutPackageElement(ppackage,&ppackage->fldGrp[i].fld[j].value,value,lenOfValue)) < 0)
	{
		UnionUserErrLog("in UnionPutPackageFld:: UnionPutPackageElement value[%s] error!\n",value);
		return(ret);
	}
	
	if (!fldFound)
		ppackage->fldGrp[i].num++;
	
	return(ret);
}

/*
����
	�Ӱ��ж�һ��Ԫ��
�������
	pelement	Ԫ��ָ��
	element		Ԫ��
	sizeOfBuf	Ԫ�ص�ֵ�Ļ����С
�������
	value		Ԫ�ص�ֵ	
����ֵ
	>=0	�ɹ�������Ԫ�س���
	<0	������
*/
int UnionReadPackageElement(PUnionPackageElement pelement,char *value,int sizeOfBuf)
{
	if (pelement->len >= sizeOfBuf)
	{
		UnionUserErrLog("in UnionReadPackageElement:: sizeOfBuf [%d] too small!\n",sizeOfBuf);
		return(errCodeSmallBuffer);
	}
	memcpy(value,pelement->pos,pelement->len);
	value[pelement->len] = 0;
	return(pelement->len);
}

/*
����
	�Ӱ��ж�һ����
�������
	ppackage	��ָ��
	tag		TAG��ʶ
	lenOfTag	TAG�ĳ���
	sizeOfBuf	VALUE��ֵ�Ļ����С
�������
	value		VALUE��ֵ	
����ֵ
	>=0	�ɹ��������򳤶�
	<0	������
*/
int UnionReadPackageFld(PUnionPackage ppackage,char *tag,int lenOfTag,char *value,int sizeOfBuf)
{
	int	i,j;
	int	index;
	int	lenOfTagPre;
	int	lenOfTagAfter;
	int	grpFound = 0;
	int	fldFound = 0;
	char	*ptr = NULL;
	
	if ((ptr = memchr(tag,'/',lenOfTag)) == NULL)
	{
		UnionUserErrLog("in UnionPutPackageFld:: tag[%s] error!\n",tag);
		return(errCodeParameter);
	}
	lenOfTagPre = ptr - tag;
	lenOfTagAfter = lenOfTag - lenOfTagPre - 1;
	ptr ++;
	
	// �������Ƿ����
	for (i = 0; i < ppackage->num; i++)
	{
		index = (ppackage->index + i) % ppackage->num;
		if ((lenOfTagPre == ppackage->fldGrp[index].fldGrpID.len) && 
			(memcmp(ppackage->fldGrp[index].fldGrpID.pos,tag,lenOfTagPre) == 0))
		{
			grpFound = 1;
			ppackage->index = index;
			i = index;
			break;
		}
	}
	
	if (!grpFound)
	{
		//UnionProgramerLog("in UnionReadPackageFld:: tag = [%s] not defined in this package!\n",tag);
		return(errCodeEsscMDL_EsscPackageFldNotFound);
	}

	// ����TAG�Ƿ����
	for (j = 0; j < ppackage->fldGrp[i].num; j++)
	{
		index = (ppackage->fldGrp[i].index + j) % ppackage->fldGrp[i].num;
		if ((lenOfTagAfter == ppackage->fldGrp[i].fld[index].tag.len) && 
			(memcmp(ppackage->fldGrp[i].fld[index].tag.pos,ptr,lenOfTagAfter) == 0))
		{
			fldFound = 1;
			ppackage->fldGrp[i].index = index;
			j = index;
			break;
		}
	}

	if (!fldFound)
	{
		//UnionProgramerLog("in UnionReadPackageFld:: tag = [%s] not defined in this package!\n",tag);
		return(errCodeEsscMDL_EsscPackageFldNotFound);
	}

	if (ppackage->fldGrp[i].fld[j].value.len >= sizeOfBuf)
	{
		UnionUserErrLog("in UnionReadPackageFld:: sizeOfBuf [%d] too small to read tag [%s]!\n",sizeOfBuf,tag);
		return(errCodeSmallBuffer);
	}

	memcpy(value,ppackage->fldGrp[i].fld[j].value.pos,ppackage->fldGrp[i].fld[j].value.len);
	value[ppackage->fldGrp[i].fld[j].value.len] = 0;
	return(ppackage->fldGrp[i].fld[j].value.len);
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
	int	i,j;
	char	tmpBuf[8192*2];
	int	len = 0;
	
	if (ppackage == NULL)
		return;

	len = snprintf(tmpBuf,sizeof(tmpBuf),"\nversion:: %s\ngroup num:: %d\n",ppackage->version,ppackage->num);
	for (i = 0; i < ppackage->num; i++)
	{
		len += snprintf(tmpBuf + len,sizeof(tmpBuf) - len,"  fldGrpID:: %s fldNum:: %2d\n",ppackage->fldGrp[i].fldGrpID.pos,ppackage->fldGrp[i].num);
		for (j = 0; j < ppackage->fldGrp[i].num;j++)
		{
			len += snprintf(tmpBuf + len,sizeof(tmpBuf) - len,"    [%16s] [%4d] [%s]\n",
					ppackage->fldGrp[i].fld[j].tag.pos,
					ppackage->fldGrp[i].fld[j].value.len,
					ppackage->fldGrp[i].fld[j].value.pos);
		}
	}
	UnionLog("%s", tmpBuf);
#endif
	return;
}

/*  2015-08-26
����
	��հ���ĳ����ֶ�����
�������
	ppackage	��ָ��
	groupName	����
�������
	��
����ֵ
	>=0	�ɹ�������ɵĴ�����
	<0	������
*/
int UnionClearPackageGrpByName(PUnionPackage ppackage, char *groupName)
{
	int	i;
	
	if ((groupName == NULL) || (ppackage == NULL))
	{
		UnionUserErrLog("in UnionClearPackageGrpByName:: null pointer!\n");
		return(errCodeNullPointer);
	}

	for (i = 0; i < ppackage->num; i++)
	{
		if(memcmp(ppackage->fldGrp[i].fldGrpID.pos, groupName, ppackage->fldGrp[i].fldGrpID.len) == 0)
		{
			ppackage->fldGrp[i].num = 0;
			return(0);
		}
	}
	return(-1);
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
	int	i,j;
	int	ret;
	int	offset = 0;
	
	if ((buf == NULL) || (ppackage == NULL))
	{
		UnionUserErrLog("in UnionPackPackage:: null pointer!\n");
		return(errCodeNullPointer);
	}

	//UnionLogPackage(ppackage);

	// ��version
	offset = snprintf(buf,sizeOfBuf,"%4s",ppackage->version);
	
	// ��1���ֽڵ�������
	offset += snprintf(buf + offset,sizeOfBuf - offset,"%d",ppackage->num);
	
	// ��ppackage->num * 2���������������ʽ��2�ֽ�������+2�ֽ���ID����+��ID
	for (i = 0; i < ppackage->num; i++)
	{
		offset += snprintf(buf + offset,sizeOfBuf - offset,"%02d",ppackage->fldGrp[i].num);
		// ����ID
		if ((ret = UnionPutPackageElementIntoStr(&ppackage->fldGrp[i].fldGrpID,PACKAGE_ELEMENT_TAG,buf + offset,sizeOfBuf - offset)) < 0)
		{
			UnionUserErrLog("in UnionPackPackage:: UnionPutPackageElementIntoStr!\n");
			return(ret);
		}
		offset += ret;
	}
	
	for (i = 0; i < ppackage->num; i++)
	{
		// ����ĳ���
		for (j = 0; j < ppackage->fldGrp[i].num; j++)
		{
			if ((ret = UnionPutPackageFldIntoStr(&ppackage->fldGrp[i].fld[j],buf + offset,sizeOfBuf - offset)) < 0)
			{
				UnionUserErrLog("in UnionPackPackage:: UnionPutPackageFldIntoStr!\n");
				return(ret);
			}
			offset += ret;
		}
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
int UnionUnpackPackage(unsigned char *data,int lenOfData,PUnionPackage ppackage)
{
	int	i,j;
	int	ret;
	int	offset = 0;
	int	thisLen;
	int	num = 0;
	
	if ((ppackage == NULL) || (data == NULL) || (lenOfData <= 0))
	{
		UnionUserErrLog("in UnionUnpackPackage:: null pointer!\n");
		return(errCodeParameter);
	}
	UnionInitPackage(ppackage);
		
	// 4���ֽڱ��İ汾��
	memcpy(ppackage->version,data,4);
	ppackage->version[4] = 0;
	/*
	if (strcmp(ppackage->version,PACKAGE_VERSION_001) != 0)
	{
		UnionUserErrLog("in UnionUnpackPackage:: ppackage->version [%s] error!\n",ppackage->version);
		return(errCodePackageDefMDL_IDOfPackageNotDefined);
	}
	*/
	offset += 4;

	// 1���ֽڵ�������
	ppackage->num = UnionPackageConvertIntStringToInt(data+offset,1);
	offset++;
	
	// ppackage->num * 2���������������ʽ��2�ֽ�������+2�ֽ���ID����+��ID
	for (i = 0; i < ppackage->num; i++)
	{
		ppackage->fldGrp[i].num = UnionPackageConvertIntStringToInt(data+offset,2);
		offset += 2;
		// ��ID
		if ((ret = UnionReadPackageElementFromStr(ppackage,data + offset,lenOfData - offset,PACKAGE_ELEMENT_TAG,&ppackage->fldGrp[i].fldGrpID)) < 0)
		{
			UnionUserErrLog("in UnionUnpackPackage:: UnionReadPackageElementFromStr!\n");
			return(ret);
		}
		ppackage->fldGrp[i].index = 0; // added 2015-08-13
		offset += ret;		
	}
	
	for (i = 0; i < ppackage->num; i++)
	{
		// ���ڵ�ֵ
		for (j = 0; j < ppackage->fldGrp[i].num; j++)
		{
			if ((thisLen = UnionReadPackageFldFromStr(ppackage,data+offset,lenOfData-offset,
				&(ppackage->fldGrp[i].fld[j]))) < 0)
			{
				UnionUserErrLog("in UnionUnpackPackage:: UnionReadPackageFldFromStr j = [%d]\n",j);
				return(thisLen);
			}
			offset += thisLen;
		}
		num += ppackage->fldGrp[i].num;	
	}
	return(num);
}

/*
����
	���ñ��İ汾
�������
	ppackage	��ָ��
	version		�汾
	len		�汾����
�������

����ֵ
	>=0	�ɹ�
	<0	������
*/
int UnionPackageSetVersion(PUnionPackage ppackage,char *version,int len)
{
	if ((ppackage == NULL) || (version == NULL))
	{
		UnionUserErrLog("in UnionUnpackPackage:: null pointer!\n");
		return(errCodeParameter);
	}
		
	if (len != 4)
	{
		UnionUserErrLog("in UnionUnpackPackage:: version length error!\n");
		return(errCodeParameter);
	}
	
	memcpy(ppackage->version,version,len);
	ppackage->version[len] = 0;
	return(0);
}

