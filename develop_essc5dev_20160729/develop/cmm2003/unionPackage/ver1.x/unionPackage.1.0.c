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
功能
	将一个数据打入包中
输入参数
	ppackage	包指针
	pelement	元素指针
	buf		数据指针
	len		数据大小
输出参数

返回值
	>=0	成功
	<0	错误码
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
功能
	将一个元素打入到包中,返回打入到包中的数据的长度
输入参数
	pelement	元素指针
	elementType	元素类型
	sizeOfBuf	缓冲区大小
输出参数
	buf		缓冲
返回值
	>=0	域在包中的字节数
	<0	错误码
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
功能
	将一个报文域打入到包中,返回打入到包中的数据的长度
输入参数
	pfld		域指针
	sizeOfBuf	缓冲区大小
输出参数
	buf		缓冲
返回值
	>=0	域在包中的字节数
	<0	错误码
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
功能
	从一个报文中读取一个元素，返回域在包中占的长度
输入参数
	ppackage	包指针
	data		数据包
	len		数据包长度
	elementType	元素类型
输出参数
	pelement	元素指针
返回值
	>=0	域在包中的字节数
	<0	错误码
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
功能
	从一个报文中读取一个域，返回域在包中占的长度
输入参数
	data		数据包
	len		数据包长度
输出参数
	pfld		域指针
返回值
	>=0	域在包中的字节数
	<0	错误码
*/
int UnionReadPackageFldFromStr(PUnionPackage ppackage,unsigned char *data,int len,PUnionPackageFld pfld)
{
	int	ret;
	int	thisLen;
	
	// 读取TAG
	if ((thisLen = UnionReadPackageElementFromStr(ppackage,data,len,PACKAGE_ELEMENT_TAG,&pfld->tag)) < 0)
	{
		UnionUserErrLog("in UnionReadPackageFldFromStr:: UnionReadPackageElementFromStr tag data[%s]!\n",data);
		return(thisLen);
	}
	
	// 读取VALUE
	if ((ret = UnionReadPackageElementFromStr(ppackage,data + thisLen,len - thisLen,PACKAGE_ELEMENT_VALUE,&pfld->value)) < 0)
	{
		UnionUserErrLog("in UnionReadPackageFldFromStr:: UnionReadPackageElementFromStr value! data[%s]\n",data + thisLen);
		return(ret);
	}
	thisLen += ret;
	return(thisLen);
}

/*
功能
	初始化报文
输入参数
	ppackage		报文结构体指针
输出参数
	无
返回值
	>=0	成功
	<0	错误码
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
功能
	释放报文
输入参数
	ppackage		报文结构体指针
输出参数
	无
返回值
	>=0	成功
	<0	错误码
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
功能
	将一个元素置到包中
输入参数
	ppackage	包指针
	pelement	元素指针
	element		元素
	lenOfElement	元素的长度
输出参数
	ppackage	包指针
	pelement	元素指针
返回值
	>=0	成功，偏移量
	<0	错误码
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
功能
	将一个域置到包中
输入参数
	ppackage	包指针
	tag		TAG
	lenOfTag	TAG的长度
	value		VALUE
	lenOfValue	VALUE的长度
输出参数
	ppackage	包指针
返回值
	>=0	成功，偏移量
	<0	错误码
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
	if (ppackage->num < 0)	// 修复组数目
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
	
	// 查找组是否存在
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
	
	if (ppackage->fldGrp[i].num < 0)	// 修复域数目
		ppackage->fldGrp[i].num = 0;

	if (!grpFound)
	{
		// 加入组ID
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
		// 查找TAG是否存在
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
	
	// 加入TAG
	if ((ret = UnionPutPackageElement(ppackage,&ppackage->fldGrp[i].fld[j].tag,ptr,lenOfTagAfter)) < 0)
	{
		UnionUserErrLog("in UnionPutPackageFld:: UnionPutPackageElement tag[%s] error!\n",tag);
		return(ret);
	}

	// 加入VALUE
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
功能
	从包中读一个元素
输入参数
	pelement	元素指针
	element		元素
	sizeOfBuf	元素的值的缓冲大小
输出参数
	value		元素的值	
返回值
	>=0	成功，返回元素长度
	<0	错误码
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
功能
	从包中读一个域
输入参数
	ppackage	包指针
	tag		TAG标识
	lenOfTag	TAG的长度
	sizeOfBuf	VALUE的值的缓冲大小
输出参数
	value		VALUE的值	
返回值
	>=0	成功，返回域长度
	<0	错误码
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
	
	// 查找组是否存在
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

	// 查找TAG是否存在
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
功能
	将包写入日志
输入参数
	ppackage	包指针
输出参数
	无
返回值
	无
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
功能
	清空包中某组的字段内容
输入参数
	ppackage	包指针
	groupName	组名
输出参数
	无
返回值
	>=0	成功，包组成的串长度
	<0	错误码
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
功能
	将包打入到串中
输入参数
	ppackage	包指针
	sizeOfBuf	缓冲大小
输出参数
	buf		包组成的串
返回值
	>=0	成功，包组成的串长度
	<0	错误码
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

	// 打version
	offset = snprintf(buf,sizeOfBuf,"%4s",ppackage->version);
	
	// 打1个字节的组数量
	offset += snprintf(buf + offset,sizeOfBuf - offset,"%d",ppackage->num);
	
	// 打ppackage->num * 2个报文组个数，格式：2字节组数量+2字节组ID长度+组ID
	for (i = 0; i < ppackage->num; i++)
	{
		offset += snprintf(buf + offset,sizeOfBuf - offset,"%02d",ppackage->fldGrp[i].num);
		// 打组ID
		if ((ret = UnionPutPackageElementIntoStr(&ppackage->fldGrp[i].fldGrpID,PACKAGE_ELEMENT_TAG,buf + offset,sizeOfBuf - offset)) < 0)
		{
			UnionUserErrLog("in UnionPackPackage:: UnionPutPackageElementIntoStr!\n");
			return(ret);
		}
		offset += ret;
	}
	
	for (i = 0; i < ppackage->num; i++)
	{
		// 打组的长度
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
功能
	将一个串解到包中
输入参数
	data		串
	lenOfData	串长度
输出参数
	ppackage	包指针
返回值
	>=0	成功，域数目
	<0	错误码
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
		
	// 4个字节报文版本号
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

	// 1个字节的组数量
	ppackage->num = UnionPackageConvertIntStringToInt(data+offset,1);
	offset++;
	
	// ppackage->num * 2个报文组个数，格式：2字节组数量+2字节组ID长度+组ID
	for (i = 0; i < ppackage->num; i++)
	{
		ppackage->fldGrp[i].num = UnionPackageConvertIntStringToInt(data+offset,2);
		offset += 2;
		// 组ID
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
		// 组内的值
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
功能
	设置报文版本
输入参数
	ppackage	包指针
	version		版本
	len		版本长度
输出参数

返回值
	>=0	成功
	<0	错误码
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

