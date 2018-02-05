//	Wolfgang Wang, 2008/9/26

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "unionCmmPackData.h"
#include "unionErrCode.h"
#include "UnionLog.h"

// 将一个报文域打入到包中,返回打入到包中的数据的长度
/*
输入参数
	tag		域标识
	value		域值
	sizeOfBuf	接收域值的缓冲大小
输出参数
	buf		将域值置入的包
返回值
	>=0		域值在包在占的长度
	<0		出错代码
*/
int UnionPutCharTypeCmmPackFldIntoStr(int tag,char value,int sizeOfBuf,char *buf)
{
	int 	lenOfValue;
	char	tmpBuf[100];
	
	sprintf(tmpBuf,"%c",value);
	lenOfValue = strlen(tmpBuf);
	
	return(UnionPutCmmPackFldIntoStr(tag,tmpBuf,lenOfValue,sizeOfBuf,buf));
}

// 将一个报文域打入到包中,返回打入到包中的数据的长度
/*
输入参数
	tag		域标识
	value		域值
	sizeOfBuf	接收域值的缓冲大小
输出参数
	buf		将域值置入的包
返回值
	>=0		域值在包在占的长度
	<0		出错代码
*/
int UnionPutDoubleTypeCmmPackFldIntoStr(int tag,double value,int sizeOfBuf,char *buf)
{
	int 	lenOfValue;
	char	tmpBuf[100];
	
	sprintf(tmpBuf,"%lf",value);
	lenOfValue = strlen(tmpBuf);
	
	return(UnionPutCmmPackFldIntoStr(tag,tmpBuf,lenOfValue,sizeOfBuf,buf));
}

// 将一个报文域打入到包中,返回打入到包中的数据的长度
/*
输入参数
	tag		域标识
	value		域值
	sizeOfBuf	接收域值的缓冲大小
输出参数
	buf		将域值置入的包
返回值
	>=0		域值在包在占的长度
	<0		出错代码
*/
int UnionPutLongTypeCmmPackFldIntoStr(int tag,long value,int sizeOfBuf,char *buf)
{
	int 	lenOfValue;
	char	tmpBuf[100];
	
	sprintf(tmpBuf,"%ld",value);
	lenOfValue = strlen(tmpBuf);
	
	return(UnionPutCmmPackFldIntoStr(tag,tmpBuf,lenOfValue,sizeOfBuf,buf));
}

// 将一个报文域打入到包中,返回打入到包中的数据的长度
/*
输入参数
	tag		域标识
	value		域值
	sizeOfBuf	接收域值的缓冲大小
输出参数
	buf		将域值置入的包
返回值
	>=0		域值在包在占的长度
	<0		出错代码
*/
int UnionPutIntTypeCmmPackFldIntoStr(int tag,int value,int sizeOfBuf,char *buf)
{
	int 	lenOfValue;
	char	tmpBuf[100];
	
	sprintf(tmpBuf,"%d",value);
	lenOfValue = strlen(tmpBuf);
	
	return(UnionPutCmmPackFldIntoStr(tag,tmpBuf,lenOfValue,sizeOfBuf,buf));
}

// 将一个报文域打入到包中,返回打入到包中的数据的长度
/*
输入参数
	tag		域标识
	value		域值
	lenOfValue	域值长度
	sizeOfBuf	接收域值的缓冲大小
输出参数
	buf		将域值置入的包
返回值
	>=0		域值在包在占的长度
	<0		出错代码
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

// 从一个报文中读取一个域，返回域在包中占的长度
/*
输入参数
	data		域所在的数据串
	len		数据串的长度
	sizeOfBuf	接收域值的缓冲大小
输出参数
	tag		域标识
	value		域值
	fldLen		域值长度
返回值
	>=0		域值在包中占的长度
	<0		出错代码
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

// 从一个报文中读取一个指定标识的域，返回域值的长度
/*
输入参数
	data		域所在的数据串
	len		数据串的长度
	tag		域标识
输出参数
	value		域值
返回值
	>=0		域值的长度
	<0		出错代码
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

// 从一个报文中读取一个指定标识的域，返回域值的长度
/*
输入参数
	data		域所在的数据串
	len		数据串的长度
	tag		域标识
输出参数
	value		域值
返回值
	>=0		域值的长度
	<0		出错代码
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

// 从一个报文中读取一个指定标识的域，返回域值的长度
/*
输入参数
	data		域所在的数据串
	len		数据串的长度
	tag		域标识
输出参数
	value		域值
返回值
	>=0		域值的长度
	<0		出错代码
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

// 从一个报文中读取一个指定标识的域，返回域值的长度
/*
输入参数
	data		域所在的数据串
	len		数据串的长度
	tag		域标识
输出参数
	value		域值
返回值
	>=0		域值的长度
	<0		出错代码
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

// 从一个报文中读取一个指定标识的域，返回域值的长度
/*
输入参数
	data		域所在的数据串
	len		数据串的长度
	tag		域标识
	sizeOfBuf	接收域值的缓冲大小
输出参数
	value		域值
返回值
	>=0		域值的长度
	<0		出错代码
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
		if (tmpTagFlag != tag)	// 不是要读的域
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

// 初始化域值位置表
/*
输入参数
	data		域所在的数据串
	len		数据串的长度
	tag		域标识
	sizeOfBuf	接收域值的缓冲大小
输出参数
	value		域值
返回值
	>=0		域值的长度
	<0		出错代码
*/
int UnionInitCmmPackFldDataList(PUnionCmmPackData pposList)
{
	if (pposList == NULL)
		return(errCodeParameter);
	pposList->fldNum = 0;
	pposList->offset = 0;
	return(0);
}
	
// 从一个域值位置清单中读一个域定义
/*
输入参数
	pposList	域值位置清单
	sizeOfBuf	域值缓中大小
	tag		域标识
输出参数
	value		域的值
返回值
	>=0		域值的长度
	<0		出错代码
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

// 从一个域值位置清单中读一个域定义
/*
输入参数
	pposList	域值位置清单
	tag		域标识
输出参数
	value		域的值
返回值
	>=0		域值的长度
	<0		出错代码
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

// 从一个域值位置清单中读一个域定义
/*
输入参数
	pposList	域值位置清单
	tag		域标识
输出参数
	value		域的值
返回值
	>=0		域值的长度
	<0		出错代码
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

// 从一个域值位置清单中读一个域定义
/*
输入参数
	pposList	域值位置清单
	tag		域标识
输出参数
	value		域的值
返回值
	>=0		域值的长度
	<0		出错代码
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

// 从一个域值位置清单中读一个域定义
/*
输入参数
	pposList	域值位置清单
	tag		域标识
输出参数
	value		域的值
返回值
	>=0		域值的长度
	<0		出错代码
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

// 从一个域值位置清单中读一个域定义
/*
输入参数
	pposList	域值位置清单
	tag		域标识
输出参数
	value		域的值
返回值
	>=0		域值的长度
	<0		出错代码
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

// 从一个域值位置清单中读一个域定义
/*
输入参数
	pposList	域值位置清单
	tag		域标识
输出参数
	value		域的值
返回值
	>=0		域值的长度
	<0		出错代码
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

// 从一个域值位置清单中读一个域定义
/*
输入参数
	pposList	域值位置清单
	tag		域标识
输出参数
	value		域的值
返回值
	>=0		域值的长度
	<0		出错代码
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

// 从一个域值位置清单中读一个域定义
/*
输入参数
	pposList	域值位置清单
	tag		域标识
输出参数
	value		域的值
返回值
	>=0		域值的长度
	<0		出错代码
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


// 将域值列解包到一个域值位置清单中
/*
输入参数
	data		域所在的数据串
	len		数据串的长度
输出参数
	pposList	域值位置清单
返回值
	>=0		域值的数目
	<0		出错代码
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

// 解包，将一个域值写到一个域值位置清单中
/*
输入参数
	len		数据串的长度
	tag		域标识
	value		数据串的值
输出参数
	pposList	域值位置清单
返回值
	>=0		域值的数目
	<0		出错代码
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


// 将一个域值写到一个域值位置清单中
/*
输入参数
	tag		域标识
	value		数据值
输出参数
	pposList	域值位置清单
返回值
	>=0		域值的数目
	<0		出错代码
*/
int UnionPutIntTypeCmmPackFldIntoFldDataList(int tag,int value,PUnionCmmPackData pposList)
{
	char	tmpBuf[100+1];
	int	len;
	
	sprintf(tmpBuf,"%d",value);
	len = strlen(tmpBuf);
	return(UnionPutCmmPackFldIntoFldDataList(tag,len,tmpBuf,pposList));
}

// 将一个域值写到一个域值位置清单中
/*
输入参数
	tag		域标识
	value		数据值
输出参数
	pposList	域值位置清单
返回值
	>=0		域值的数目
	<0		出错代码
*/
int UnionPutLongTypeCmmPackFldIntoFldDataList(int tag,long value,PUnionCmmPackData pposList)
{
	char	tmpBuf[100+1];
	int	len;
	
	sprintf(tmpBuf,"%ld",value);
	len = strlen(tmpBuf);
	return(UnionPutCmmPackFldIntoFldDataList(tag,len,tmpBuf,pposList));
}

// 将一个域值写到一个域值位置清单中
/*
输入参数
	tag		域标识
	value		数据值
输出参数
	pposList	域值位置清单
返回值
	>=0		域值的数目
	<0		出错代码
*/
int UnionPutCharTypeCmmPackFldIntoFldDataList(int tag,char value,PUnionCmmPackData pposList)
{
	char	tmpBuf[100+1];
	int	len;
	
	sprintf(tmpBuf,"%c",value);
	len = strlen(tmpBuf);
	return(UnionPutCmmPackFldIntoFldDataList(tag,len,tmpBuf,pposList));
}

// 将一个域值写到一个域值位置清单中
/*
输入参数
	tag		域标识
	value		数据值
输出参数
	pposList	域值位置清单
返回值
	>=0		域值的数目
	<0		出错代码
*/
int UnionPutUnsignedIntTypeCmmPackFldIntoFldDataList(int tag,unsigned int value,PUnionCmmPackData pposList)
{
	char	tmpBuf[100+1];
	int	len;
	
	sprintf(tmpBuf,"%d",value);
	len = strlen(tmpBuf);
	return(UnionPutCmmPackFldIntoFldDataList(tag,len,tmpBuf,pposList));
}

// 将一个域值写到一个域值位置清单中
/*
输入参数
	tag		域标识
	value		数据值
输出参数
	pposList	域值位置清单
返回值
	>=0		域值的数目
	<0		出错代码
*/
int UnionPutUnsignedLongTypeCmmPackFldIntoFldDataList(int tag,unsigned long value,PUnionCmmPackData pposList)
{
	char	tmpBuf[100+1];
	int	len;
	
	sprintf(tmpBuf,"%ld",value);
	len = strlen(tmpBuf);
	return(UnionPutCmmPackFldIntoFldDataList(tag,len,tmpBuf,pposList));
}

// 将一个域值写到一个域值位置清单中
/*
输入参数
	tag		域标识
	value		数据值
输出参数
	pposList	域值位置清单
返回值
	>=0		域值的数目
	<0		出错代码
*/
int UnionPutUnsignedCharTypeCmmPackFldIntoFldDataList(int tag,unsigned char value,PUnionCmmPackData pposList)
{
	char	tmpBuf[100+1];
	int	len;
	
	sprintf(tmpBuf,"%c",value);
	len = strlen(tmpBuf);
	return(UnionPutCmmPackFldIntoFldDataList(tag,len,tmpBuf,pposList));
}

// 将一个域值写到一个域值位置清单中
/*
输入参数
	tag		域标识
	value		数据值
输出参数
	pposList	域值位置清单
返回值
	>=0		域值的数目
	<0		出错代码
*/
int UnionPutDoubleTypeCmmPackFldIntoFldDataList(int tag,double value,PUnionCmmPackData pposList)
{
	char	tmpBuf[100+1];
	int	len;
	
	sprintf(tmpBuf,"%lf",value);
	len = strlen(tmpBuf);
	return(UnionPutCmmPackFldIntoFldDataList(tag,len,tmpBuf,pposList));
}

// 将一个域值写到一个域值位置清单中
/*
输入参数
	tag		域标识
	value		数据值
输出参数
	pposList	域值位置清单
返回值
	>=0		域值的数目
	<0		出错代码
*/
int UnionPutStringTypeCmmPackFldIntoFldDataList(int tag,char *value,PUnionCmmPackData pposList)
{
	char	tmpBuf[2048+1];
	int	len;
	
	sprintf(tmpBuf,"%s",value);
	len = strlen(tmpBuf);
	return(UnionPutCmmPackFldIntoFldDataList(tag,len,tmpBuf,pposList));
}


// 解包，将域值列解包到一个域值位置清单中
/*
输入参数
	pposList	域值位置清单
	sizeOfBuf	缓冲的大小
输出参数
	data		域所在的数据串
返回值
	>=0		数据包的长度
	<0		出错代码
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

// 将报文数据写入到日志中
/*
输入参数
	title		标题
	pposList	域值位置清单
输出参数
	无
返回值
	无
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

// 测试函数
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

// 复制报文
/*
输入参数
	poriList	源包
输出参数
	pdesList	目标包
返回值
	>=0		域数目
	<0		出错代码
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

