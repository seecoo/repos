//	Wolfgang Wang, 2006/8/9

/* 2007-10-25�����������޸���	UnionPutEsscPackageFldIntoStr
	���򳤶���0ʱ��Ҳ������뵽���С�
*/
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "esscPackage.h"
#include "unionErrCode.h"
#ifndef _UnionLogMDL_3_x_
#define _UnionLogMDL_3_x_
#endif
#include "UnionLog.h"
#include "unionREC.h"

#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_
#endif

int gunionIsSpierClientPackage = 0;

TUnionEsscPackage	gunionRequestPackage;
TUnionEsscPackage	gunionResponsePackage;
int			gunionLogEsscPackageAnyway = 0;

long			gunionSsnFromClient = -1;	// 2009/7/8,����������

int UnionGetVersionOfEsscPackage()
{
	return(3);
}

// 2009/11/18,�����������������
// ��ȡһ����Կ��������ص���������ID
int UnionGetPrimaryFldTagOfSpecKMSvrService(int serviceID)
{
	char		varName[128];
	int		fldTag;

	sprintf(varName,"primaryFldOfKMService%03d",serviceID);
	if ((fldTag = UnionReadIntTypeRECVar(varName)) < 0)
		fldTag = conEsscFldKeyName;
	return(fldTag);
}

void UnionSetLogEsscPackageAnyway()
{
	gunionLogEsscPackageAnyway = 1;
}

void UnionCloseLogEsscPackageAnyway()
{
	gunionLogEsscPackageAnyway = 0;
}


// ���ý���ĳ����Ǽ�ؿͻ���
int UnionSetEsscPackageAsSpierClientPackage()
{
	gunionIsSpierClientPackage = 1;
        return(0);
}

// ��һ��ESSC��������뵽����,���ش��뵽���е����ݵĳ���
int UnionPutEsscPackageFldIntoStr(char *serviceID,int index,PUnionEsscPackageFld pfld,char *buf,int sizeOfBuf)
{
	if ((pfld == NULL) || (buf == NULL) || (pfld->len < 0) || (pfld->value == NULL))
		return(errCodeParameter);
	//if (pfld->len == 0)	// 2007/10/25����
	//	return(0);
	if (pfld->len + 3 + 4 >= sizeOfBuf)
	{
		UnionUserErrLog("in UnionPutEsscPackageFldIntoStr:: sizeOfBuf [%d] too small!\n",sizeOfBuf);
		return(errCodeParameter);
	}
	sprintf(buf,"%03d%04d",pfld->tag,pfld->len);
	memcpy(buf+7,pfld->value,pfld->len);
	return(7+pfld->len);
}


// ��һ��ESSC�����ж�ȡһ���򣬷������ڰ���ռ�ĳ���
int UnionReadEsscPackageFldFromStr(char *serviceID,int index,char *data,int len,PUnionEsscPackageFld pfld,int sizeOfFldValue)
{
	char	tmpBuf[10];
	int	copyDataLen;
			
	if ((pfld == NULL) || (data == NULL) || (len < 7) || (pfld->value == NULL))
		return(errCodeParameter);
	memcpy(tmpBuf,data,3);
	tmpBuf[3] = 0;	
	pfld->tag = atoi(tmpBuf);
	memcpy(tmpBuf,data+3,4);
	tmpBuf[4] = 0;	
	pfld->len = atoi(tmpBuf);
	if (pfld->len < 0)
	{
		UnionUserErrLog("in UnionReadEsscPackageFldFromStr:: pfld->len = [%d] for fldIndex = [%03d] error!\n",pfld->len,pfld->tag);
		return(errCodeEsscMDL_EsscPackageFldLength);
	}
	if (pfld->len + 7 > len)
	{
		if (!gunionIsSpierClientPackage)	// ���Ǽ�ؿͻ��˱���
		{
			UnionUserErrLog("in UnionReadEsscPackageFldFromStr:: dataLen = [%04d] shorter than expected [%d]!\n",
					len,pfld->len+7);
			return(errCodeEsscMDL_EsscPackageLength);
		}
		copyDataLen = len - 7;
	}
	else
		copyDataLen = pfld->len;
	if (copyDataLen >= sizeOfFldValue)
	{
		UnionUserErrLog("in UnionReadEsscPackageFldFromStr:: sizeOfFldValue [%d] smaller than expected [%04d]\n",
				sizeOfFldValue,copyDataLen);
		return(errCodeSmallBuffer);
	}
	memcpy(pfld->value,data+7,copyDataLen);
	pfld->value[copyDataLen] = 0;
	return(7+copyDataLen);
}

// ��ʼ��������
int UnionInitEsscRequestPackage()
{
	//memset(&gunionRequestPackage,0,sizeof(gunionRequestPackage));
	gunionRequestPackage.fldNum = 0;
	gunionRequestPackage.offset = 0;
	return(0);
}

// ������������
int UnionSetEsscRequestPackageFld(int fldTag,int len,char *value)
{
	if ((len < 0) || (value == NULL))
	{
		UnionUserErrLog("in UnionSetEsscRequestPackageFld:: fldTag = [%03d] parameter error!\n",fldTag);
		return(errCodeParameter);
	}
	if (len + gunionRequestPackage.offset >= sizeof(gunionRequestPackage.dataBuf))
	{
		UnionUserErrLog("in UnionSetEsscRequestPackageFld:: dataBuf = [%04zu] offset = [%04d] fldLen = [%04d]\n",
			sizeof(gunionRequestPackage.dataBuf),gunionRequestPackage.offset,len);
		return(errCodeSmallBuffer);
	}
	if (gunionRequestPackage.fldNum >= conMaxNumOfEsscPackageFld)
	{
		UnionUserErrLog("in UnionSetEsscRequestPackageFld:: too much flds! fldNum = [%03d]\n",gunionRequestPackage.fldNum);
		return(errCodeEsscMDL_TooMuchEsscPackageFld);
	}
	gunionRequestPackage.fldGrp[gunionRequestPackage.fldNum].len = len;
	gunionRequestPackage.fldGrp[gunionRequestPackage.fldNum].tag = fldTag;
	gunionRequestPackage.fldGrp[gunionRequestPackage.fldNum].value = gunionRequestPackage.dataBuf + gunionRequestPackage.offset;
	memcpy(gunionRequestPackage.fldGrp[gunionRequestPackage.fldNum].value,value,len);
	gunionRequestPackage.fldGrp[gunionRequestPackage.fldNum].value[len] = 0;
	gunionRequestPackage.offset += (len+1);
	++gunionRequestPackage.fldNum;
	return(0);
}

// ��ȡ��������
int UnionReadEsscRequestPackageFld(int fldTag,char *value,int sizeOfBuf)
{
	int	index;
	
	if (value == NULL)
	{
		UnionUserErrLog("in UnionReadEsscRequestPackageFld:: fldTag = [%03d] parameter error!\n",fldTag);
		return(errCodeParameter);
	}
	
	for (index = 0; index < gunionRequestPackage.fldNum; index++)
	{
		if (gunionRequestPackage.fldGrp[index].tag == fldTag)
		{
			if (gunionRequestPackage.fldGrp[index].len >= sizeOfBuf)
			{
				UnionUserErrLog("in UnionReadEsscRequestPackageFld:: sizeOfBuf [%d] too small to read fld [%03d]!\n",sizeOfBuf,fldTag);
				return(errCodeSmallBuffer);
			}
			if (gunionRequestPackage.fldGrp[index].len < 0)
			{
				UnionUserErrLog("in UnionReadEsscRequestPackageFld:: fldLen = [%03d] error!\n",gunionRequestPackage.fldGrp[index].len);
				return(errCodeEsscMDL_PackageFldValueLen);
			}
			memcpy(value,gunionRequestPackage.fldGrp[index].value,gunionRequestPackage.fldGrp[index].len);
			return(gunionRequestPackage.fldGrp[index].len);
		}
	}
	//UnionAuditLog("in UnionReadEsscRequestPackageFld:: fldTag = [%03d] not defined in this package!\n",fldTag);
	UnionProgramerLog("in UnionReadEsscRequestPackageFld:: fldTag = [%03d] not defined in this package!\n",fldTag);
	return(errCodeEsscMDL_EsscPackageFldNotFound);
}

// �������д����־
void UnionLogEsscRequestPackage()
{
#ifndef _WIN32_
	int	index;

	UnionNullLogWithTime("Request fldNum = [%04d]\n",gunionRequestPackage.fldNum);
	for (index = 0; index < gunionRequestPackage.fldNum; index++)
	{
		switch (gunionRequestPackage.fldGrp[index].tag)
		{
			case	conEsscFldPlainPin:
			case	conEsscFldPlainData:
				UnionNullLogWithTimeAnyway("[%03d] [%04d] [%s]\n",gunionRequestPackage.fldGrp[index].tag,gunionRequestPackage.fldGrp[index].len,"******");
				break;
			default:
				UnionNullLogWithTimeAnyway("[%03d] [%04d] [%s]\n",gunionRequestPackage.fldGrp[index].tag,gunionRequestPackage.fldGrp[index].len,gunionRequestPackage.fldGrp[index].value);
				break;
		}
	}
	//UnionMerelyNullLog("\n");
#endif
	return;
}

// �������д����־
void UnionLogEsscRequestPackageAnyway(char *idOfApp,char *serviceID)
{
#ifndef _WIN32_
	int	index;

	UnionNullLogWithTimeAnyway("Req[%s%s1] fldNum = [%04d]\n",idOfApp,serviceID,gunionRequestPackage.fldNum);
	for (index = 0; index < gunionRequestPackage.fldNum; index++)
	{
		switch (gunionRequestPackage.fldGrp[index].tag)
		{
			case	conEsscFldPlainPin:
			case	conEsscFldPlainData:
				UnionNullLogWithTimeAnyway("[%03d] [%04d] [%s]\n",gunionRequestPackage.fldGrp[index].tag,gunionRequestPackage.fldGrp[index].len,"******");
				break;
			default:
				UnionNullLogWithTimeAnyway("[%03d] [%04d] [%s]\n",gunionRequestPackage.fldGrp[index].tag,gunionRequestPackage.fldGrp[index].len,gunionRequestPackage.fldGrp[index].value);
				break;
		}
	}
	//UnionMerelyNullLogAnyway("\n");
#endif
	return;
}

// ��һ�������
int UnionPackEsscRequestPackage(char *idOfApp,char *serviceID,char *buf,int sizeOfBuf)
{
	int	offset;
	int	thisLen;
	int	index;
	
	if ((idOfApp == NULL) || (serviceID == NULL) || (buf == NULL))
	{
		UnionUserErrLog("in UnionPackEsscRequestPackage:: null pointer!\n");
		return(errCodeParameter);
	}
	if (sizeOfBuf < 2 + 3 + 1 + 4)
	{
		UnionUserErrLog("in UnionPackEsscRequestPackage:: sizeOfBuf [%d] too small!\n",sizeOfBuf);
		return(errCodeSmallBuffer);
	}
	memcpy(buf,idOfApp,2);
	memcpy(buf+2,serviceID,3);
	memcpy(buf+2+3,"1",1);
	offset = 2+3+1;
	sprintf(buf+offset,"%03d",gunionRequestPackage.fldNum);
	offset += 3;
	if (gunionLogEsscPackageAnyway)
		UnionLogEsscRequestPackageAnyway(idOfApp,serviceID);
	else
	{
		UnionNullLogWithTime("before pack [%s%s1]::\n",idOfApp,serviceID);
		UnionLogEsscRequestPackage();
	}
	for (index = 0; index < gunionRequestPackage.fldNum; index++)
	{
		if ((thisLen = UnionPutEsscPackageFldIntoStr(serviceID,index,&(gunionRequestPackage.fldGrp[index]),buf+offset,sizeOfBuf-offset)) < 0)
		{
			UnionUserErrLog("in UnionPackEsscRequestPackage:: UnionPutEsscPackageFldIntoStr [%03d]\n",gunionRequestPackage.fldGrp[index].tag);
			return(thisLen);
		}
		offset += thisLen;
	}
	return(offset);
}

// ��һ�������
int UnionUnpackEsscRequestPackage(char *data,int lenOfData,char *idOfApp,char *serviceID)
{
	int	offset;
	int	thisLen;
	int	index = 0;
	char	tmpBuf[10];

	int	isSetMask = 0;
	char	tmpValue[1024];
	
	gunionSsnFromClient = -1;	// 2009/7/8,Wolfgang Wang added
	
	if ((idOfApp == NULL) || (serviceID == NULL) || (data == NULL) || (lenOfData <= 0))
	{
		UnionUserErrLog("in UnionUnpackEsscRequestPackage:: null pointer!\n");
		return(errCodeParameter);
	}
	UnionInitEsscRequestPackage();
	if (lenOfData < 2 + 3 + 1 + 3)
	{
		UnionUserErrLog("in UnionUnpackEsscRequestPackage:: lenOfData [%d] too small!\n",lenOfData);
		return(errCodeSmallBuffer);
	}
	memcpy(idOfApp,data,2);
	idOfApp[2] = 0;
	memcpy(serviceID,data+2,3);
	serviceID[3] = 0;
	if (data[2+3] != '1')
	{
		UnionUserErrLog("in UnionUnpackEsscRequestPackage:: this is not a request package!\n");
		return(errCodeEsscMDL_NotEsscRequestPackage);
	}
	memcpy(tmpBuf,data+2+3+1,3);
	tmpBuf[3] = 0;
	gunionRequestPackage.fldNum = atoi(tmpBuf);
	offset = 2 + 3 + 1 + 3;
	for (index = 0; index < gunionRequestPackage.fldNum; index++)
	{
		gunionRequestPackage.fldGrp[index].value = gunionRequestPackage.dataBuf+gunionRequestPackage.offset;
		if ((thisLen = UnionReadEsscPackageFldFromStr(serviceID,index,data+offset,lenOfData-offset,
				&(gunionRequestPackage.fldGrp[index]),
				sizeof(gunionRequestPackage.dataBuf)-gunionRequestPackage.offset)) < 0)
		{
			UnionUserErrLog("in UnionUnpackEsscRequestPackage:: UnionReadEsscPackageFldFromStr [%03d]\n",index);
			return(thisLen);
		}
		// 2009/7/8,Wolfgang Wang added
		if (gunionRequestPackage.fldGrp[index].tag == conEsscFldMessageFlag)
		{
			gunionSsnFromClient = atol((char *)gunionRequestPackage.fldGrp[index].value);
		}
		// 2009/7/8 added end
		offset += thisLen;
		gunionRequestPackage.offset += (gunionRequestPackage.fldGrp[index].len + 1);
	}
	// add by zhouxw 20160421 begin ��֧������һ����
	if(memcmp(serviceID, "516", 3) == 0)
		isSetMask = 1;
	if(!isSetMask)
		goto NormalLog;
	else
	{
		for (index = 0; index < gunionRequestPackage.fldNum; index ++)
		{
			if(memcmp(serviceID, "516", 3) == 0 && gunionRequestPackage.fldGrp[index].tag == conEsscFldSecondWKName)
			{
				memcpy(tmpValue, gunionRequestPackage.fldGrp[index].value, gunionRequestPackage.fldGrp[index].len);
				tmpValue[gunionRequestPackage.fldGrp[index].len] = 0;
				memset(gunionRequestPackage.fldGrp[index].value, '*', gunionRequestPackage.fldGrp[index].len);
			}
		}
	}
	// add by zhouxw 20160421 end 
NormalLog:
	if (gunionLogEsscPackageAnyway)
		UnionLogEsscRequestPackageAnyway(idOfApp,serviceID);
	else
	{
		UnionNullLogWithTime("after unpack [%s%s1]::\n",idOfApp,serviceID);
		UnionLogEsscRequestPackage();
	}
	// add by zhouxw 20160421 begin
	if(isSetMask)
	{
		for (index = 0; index < gunionRequestPackage.fldNum; index ++)
		{
			if(memcmp(serviceID, "516", 3) == 0 && gunionRequestPackage.fldGrp[index].tag == conEsscFldSecondWKName)
			{
				memcpy(gunionRequestPackage.fldGrp[index].value, tmpValue, gunionRequestPackage.fldGrp[index].len);
			}
		}
	}	
	// add by zhouxw 20160421 end
	return(0);
}

// ��ʼ����Ӧ����
int UnionInitEsscResponsePackage()
{
	//memset(&gunionResponsePackage,0,sizeof(gunionResponsePackage));
	gunionResponsePackage.fldNum = 0;
	gunionResponsePackage.offset = 0;
	return(0);
}

// ������Ӧ������
int UnionSetEsscResponsePackageFld(int fldTag,int len,char *value)
{
	if ((len < 0) || (value == NULL))
	{
		UnionUserErrLog("in UnionSetEsscResponsePackageFld:: fldTag = [%03d] parameter error!\n",fldTag);
		return(errCodeParameter);
	}
	if (len + gunionResponsePackage.offset >= sizeof(gunionResponsePackage.dataBuf))
	{
		UnionUserErrLog("in UnionSetEsscResponsePackageFld:: dataBuf = [%04zu] offset = [%04d] fldLen = [%04d]\n",
			sizeof(gunionResponsePackage.dataBuf),gunionResponsePackage.offset,len);
		return(errCodeSmallBuffer);
	}
	if (gunionResponsePackage.fldNum >= conMaxNumOfEsscPackageFld)
	{
		UnionUserErrLog("in UnionSetEsscResponsePackageFld:: too much flds! fldNum = [%03d]\n",gunionResponsePackage.fldNum);
		return(errCodeEsscMDL_TooMuchEsscPackageFld);
	}
	gunionResponsePackage.fldGrp[gunionResponsePackage.fldNum].len = len;
	gunionResponsePackage.fldGrp[gunionResponsePackage.fldNum].tag = fldTag;
	gunionResponsePackage.fldGrp[gunionResponsePackage.fldNum].value = gunionResponsePackage.dataBuf + gunionResponsePackage.offset;
	memcpy(gunionResponsePackage.fldGrp[gunionResponsePackage.fldNum].value,value,len);
	gunionResponsePackage.fldGrp[gunionResponsePackage.fldNum].value[len] = 0;
	gunionResponsePackage.offset += (len+1);
	++gunionResponsePackage.fldNum;
	return(0);
}

// ��ȡ��Ӧ������
int UnionReadEsscResponsePackageFld(int fldTag,char *value,int sizeOfBuf)
{
	int	index;
	
	if (value == NULL)
	{
		UnionUserErrLog("in UnionReadEsscResponsePackageFld:: fldTag = [%03d] parameter error!\n",fldTag);
		return(errCodeParameter);
	}
	
	for (index = 0; index < gunionResponsePackage.fldNum; index++)
	{
		if (gunionResponsePackage.fldGrp[index].tag == fldTag)
		{
			if (gunionResponsePackage.fldGrp[index].len >= sizeOfBuf)
			{
				UnionUserErrLog("in UnionReadEsscResponsePackageFld:: sizeOfBuf [%d] too small to read fld [%03d]!\n",sizeOfBuf,fldTag);
				return(errCodeSmallBuffer);
			}
			if (gunionResponsePackage.fldGrp[index].len < 0)
			{
				UnionUserErrLog("in UnionReadEsscResponsePackageFld:: fldLen = [%03d] error!\n",gunionResponsePackage.fldGrp[index].len);
				return(errCodeEsscMDL_PackageFldValueLen);
			}
			memcpy(value,gunionResponsePackage.fldGrp[index].value,gunionResponsePackage.fldGrp[index].len);
			return(gunionResponsePackage.fldGrp[index].len);
		}
	}
	UnionAuditLog("in UnionReadEsscResponsePackageFld:: fldTag = [%03d] not defined in this package!\n",fldTag);
	return(errCodeEsscMDL_EsscPackageFldNotFound);
}

// ����Ӧ��д����־
void UnionLogEsscResponsePackage()
{
#ifndef _WIN32_
	int	index;
	
	UnionNullLogWithTime("Response fldNum = [%04d]\n",gunionResponsePackage.fldNum);
	for (index = 0; index < gunionResponsePackage.fldNum; index++)
	{
		switch (gunionResponsePackage.fldGrp[index].tag)
		{
			case	conEsscFldPlainPin:
			case	conEsscFldPlainData:
				UnionNullLogWithTimeAnyway("[%03d] [%04d] [%s]\n",gunionResponsePackage.fldGrp[index].tag,gunionResponsePackage.fldGrp[index].len,"******");
				break;
			default:
				UnionNullLogWithTimeAnyway("[%03d] [%04d] [%s]\n",gunionResponsePackage.fldGrp[index].tag,gunionResponsePackage.fldGrp[index].len,gunionResponsePackage.fldGrp[index].value);
				break;
		}
	}
	//UnionMerelyNullLog("\n");
#endif
	return;
}

// ����Ӧ��д����־
void UnionLogEsscResponsePackageAnyway(char *idOfApp,char *serviceID,int responseCode)
{
#ifndef _WIN32_
	int	index;
	
	UnionNullLogWithTime("Res[%s%s0%06d] fldNum = [%04d]\n",idOfApp,serviceID,responseCode,gunionResponsePackage.fldNum);
	for (index = 0; index < gunionResponsePackage.fldNum; index++)
	{
		switch (gunionResponsePackage.fldGrp[index].tag)
		{
			case	conEsscFldPlainPin:
			case	conEsscFldPlainData:
				UnionNullLogWithTimeAnyway("[%03d] [%04d] [%s]\n",gunionResponsePackage.fldGrp[index].tag,gunionResponsePackage.fldGrp[index].len,"******");
				break;
			default:
				UnionNullLogWithTimeAnyway("[%03d] [%04d] [%s]\n",gunionResponsePackage.fldGrp[index].tag,gunionResponsePackage.fldGrp[index].len,gunionResponsePackage.fldGrp[index].value);
				break;
		}
	}
	//UnionMerelyNullLog("\n");
#endif
	return;
}

// ��һ����Ӧ��
int UnionPackEsscResponsePackage(char *idOfApp,char *serviceID,int responseCode,char *buf,int sizeOfBuf)
{
	int	ret;
	int	offset;
	int	thisLen;
	int	index;
	char	tmpBuf[128];
	
	int	isSetMask = 0;
	char	tmpValue[1024];
	
	if ((idOfApp == NULL) || (serviceID == NULL) || (buf == NULL))
	{
		UnionUserErrLog("in UnionPackEsscResponsePackage:: null pointer!\n");
		return(errCodeParameter);
	}
	if (sizeOfBuf < 2 + 3 + 1 + 4)
	{
		UnionUserErrLog("in UnionPackEsscResponsePackage:: sizeOfBuf [%d] too small!\n",sizeOfBuf);
		return(errCodeSmallBuffer);
	}
	memcpy(buf,idOfApp,2);
	memcpy(buf+2,serviceID,3);
	memcpy(buf+2+3,"0",1);
	offset = 2+3+1;
	sprintf(buf+offset,"%06d",responseCode);
	offset += 6;
	// 2009/7/8,Wolfang Wang added
	if (gunionSsnFromClient >= 0)
	{
		sprintf(tmpBuf,"%ld",gunionSsnFromClient);
		if ((ret = UnionSetEsscResponsePackageFld(conEsscFldMessageFlag,strlen(tmpBuf),tmpBuf)) <  0)
		{
			UnionUserErrLog("in UnionPackEsscResponsePackage:: UnionSetEsscResponsePackageFld [%03d]!\n",conEsscFldMessageFlag);
			return(ret);
		}
	}
	// add by zhouxw 20160421 begin һ�������֧������һ����
	if(memcmp(serviceID, "518", 3) == 0)
		isSetMask = 1;
	if(!isSetMask)
		goto NormalLog;
	else
	{
		for (index = 0; index < gunionResponsePackage.fldNum; index ++)
		{
			if(memcmp(serviceID, "518", 3) == 0 && gunionResponsePackage.fldGrp[index].tag == conEsscFldEncryptedPinByZPK2)
			{
				memcpy(tmpValue, gunionResponsePackage.fldGrp[index].value, gunionResponsePackage.fldGrp[index].len);
				tmpValue[gunionResponsePackage.fldGrp[index].len] = 0;
				memset(gunionResponsePackage.fldGrp[index].value, '*', gunionResponsePackage.fldGrp[index].len);
			}
		}
	}
	// add by zhouxw 20160421 end 
	// 2009/7/8, addition ended
NormalLog:
	if (gunionLogEsscPackageAnyway)
		UnionLogEsscResponsePackageAnyway(idOfApp,serviceID,responseCode);
	else
	{
		UnionNullLogWithTime("before pack [%s%s0%06d]::\n",idOfApp,serviceID,responseCode);
	//if (responseCode < 0)
	//	return(offset);
		UnionLogEsscResponsePackage();
	}
	// add by zhouxw 20160421 begin
	if(isSetMask)
	{
		for (index = 0; index < gunionResponsePackage.fldNum; index ++)
		{
			if(memcmp(serviceID, "518", 3) == 0 && gunionResponsePackage.fldGrp[index].tag == conEsscFldEncryptedPinByZPK2)
			{
				memcpy(gunionResponsePackage.fldGrp[index].value, tmpValue, gunionResponsePackage.fldGrp[index].len);
			}
		}
	}
	// add by zhouxw 20160421 end
	sprintf(buf+offset,"%03d",gunionResponsePackage.fldNum);
	offset += 3;
	for (index = 0; index < gunionResponsePackage.fldNum; index++)
	{
		if ((thisLen = UnionPutEsscPackageFldIntoStr(serviceID,index,&(gunionResponsePackage.fldGrp[index]),buf+offset,sizeOfBuf-offset)) < 0)
		{
			UnionUserErrLog("in UnionPackEsscResponsePackage:: UnionPutEsscPackageFldIntoStr [%03d]\n",gunionResponsePackage.fldGrp[index].tag);
			return(thisLen);
		}
		offset += thisLen;
	}
	//UnionNullLog("in UnionPackEsscResponsePackage:: len = [%04d]\n",offset);
	return(offset);
}

// ��һ����Ӧ��
int UnionUnpackEsscResponsePackage(char *data,int lenOfData,char *idOfApp,char *serviceID)
{
	int	offset;
	int	thisLen;
	int	index = 0;
	char	tmpBuf[10];
	int	responseCode;

	if ((idOfApp == NULL) || (serviceID == NULL) || (data == NULL) || (lenOfData <= 0))
	{
		UnionUserErrLog("in UnionUnpackEsscResponsePackage:: null pointer!\n");
		return(errCodeParameter);
	}
	UnionInitEsscResponsePackage();
	if (lenOfData < 2 + 3 + 1 + 3)
	{
		UnionUserErrLog("in UnionUnpackEsscResponsePackage:: lenOfData [%d] too small!\n",lenOfData);
		return(errCodeSmallBuffer);
	}
	memcpy(idOfApp,data,2);
	memcpy(serviceID,data+2,3);
	if (data[2+3] != '0')
	{
		UnionUserErrLog("in UnionUnpackEsscResponsePackage:: this is not a request package!\n");
		return(errCodeEsscMDL_NotEsscResponsePackage);
	}
	offset = 2+3+1;
	memcpy(tmpBuf,data+offset,6);
	tmpBuf[6] = 0;
	responseCode = atoi(tmpBuf);
	//if ((responseCode = atoi(tmpBuf)) < 0)	// ��Ӧ�벻Ϊ0
	//	return(responseCode);
	offset += 6;
	memcpy(tmpBuf,data+offset,3);
	tmpBuf[3] = 0;
	gunionResponsePackage.fldNum = atoi(tmpBuf);
	offset += 3;
	for (index = 0; index < gunionResponsePackage.fldNum; index++)
	{
		gunionResponsePackage.fldGrp[index].value = gunionResponsePackage.dataBuf+gunionResponsePackage.offset;
		if ((thisLen = UnionReadEsscPackageFldFromStr(serviceID,index,data+offset,lenOfData-offset,
				&(gunionResponsePackage.fldGrp[index]),
				sizeof(gunionResponsePackage.dataBuf)-gunionResponsePackage.offset)) < 0)
		{
			UnionUserErrLog("in UnionUnpackEsscResponsePackage:: UnionReadEsscPackageFldFromStr [%03d]\n",index);
			return(thisLen);
		}
		offset += thisLen;
		gunionResponsePackage.offset += (gunionResponsePackage.fldGrp[index].len + 1);
	}
	if (gunionLogEsscPackageAnyway)
		UnionLogEsscResponsePackageAnyway(idOfApp,serviceID,responseCode);
	else
	{
		UnionNullLogWithTime("after unpack [%s%s0%05d]::\n",idOfApp,serviceID,responseCode);
		UnionLogEsscResponsePackage();
	}
	return(responseCode);
}
// ��ȡ��Ӧ��������Ŀ
int UnionGetMaxFldNumOfEsscResponsePackage()
{
	return(gunionResponsePackage.fldNum);
}

// �����������ţ���ȡ��Ӧ������
int UnionReadEsscResponsePackageFldByIndex(int index,int *fldTag,char *value,int sizeOfBuf)
{
	int	copyDataLen;
	
	if ((value == NULL) || (sizeOfBuf <= 1))
	{
		UnionUserErrLog("in UnionReadEsscResponsePackageFldByIndex:: index = [%03d] parameter error!\n",index);
		return(errCodeParameter);
	}
	
	if ((index >= gunionResponsePackage.fldNum) || (index < 0))
	{
		UnionUserErrLog("in UnionReadEsscResponsePackageFldByIndex:: index = [%03d] out of ranage num [%03d]\n",index,gunionResponsePackage.fldNum);
		return(errCodeEsscMDL_FldIndexOutOfPackageFldNum);
	}
	if (gunionResponsePackage.fldGrp[index].len >= sizeOfBuf)
	{
		if (!gunionIsSpierClientPackage)	// ���Ǽ�ؿͻ���
		{
			UnionUserErrLog("in UnionReadEsscResponsePackageFldByIndex:: sizeOfBuf [%d] too small to read fld [%03d]!\n",sizeOfBuf,index);
			return(errCodeSmallBuffer);
		}
		copyDataLen = sizeOfBuf - 1;
	}
	else
		copyDataLen = gunionResponsePackage.fldGrp[index].len;
	if (copyDataLen < 0)
	{
		UnionUserErrLog("in UnionReadEsscResponsePackageFldByIndex:: copyDataLen = [%d] error!\n",copyDataLen);
		return(errCodeEsscMDL_PackageFldValueLen);
	}
	memcpy(value,gunionResponsePackage.fldGrp[index].value,copyDataLen);
	*fldTag = gunionResponsePackage.fldGrp[index].tag;
	value[copyDataLen] = 0;
	return(gunionResponsePackage.fldGrp[index].len);
}

// ��ȡ�����������Ŀ
int UnionGetMaxFldNumOfEsscRequestPackage()
{
	return(gunionRequestPackage.fldNum);
}

// �����������ţ���ȡ��������
int UnionReadEsscRequestPackageFldByIndex(int index,int *fldTag,char *value,int sizeOfBuf)
{
	int	copyDataLen;
	
	if ((value == NULL) || (sizeOfBuf <= 1))
	{
		UnionUserErrLog("in UnionReadEsscRequestPackageFldByIndex:: index = [%03d] parameter error!\n",index);
		return(errCodeParameter);
	}
	
	if ((index >= gunionRequestPackage.fldNum) || (index < 0))
	{
		UnionUserErrLog("in UnionReadEsscRequestPackageFldByIndex:: index = [%03d] out of ranage num [%03d]\n",index,gunionRequestPackage.fldNum);
		return(errCodeEsscMDL_FldIndexOutOfPackageFldNum);
	}
	if (gunionRequestPackage.fldGrp[index].len >= sizeOfBuf)
	{
		if (!gunionIsSpierClientPackage)	// ���Ǽ�ؿͻ���
		{
			UnionUserErrLog("in UnionReadEsscRequestPackageFldByIndex:: sizeOfBuf [%d] too small to read fld [%03d]!\n",sizeOfBuf,index);
			return(errCodeSmallBuffer);
		}
		copyDataLen = sizeOfBuf - 1;
	}
	else
		copyDataLen = gunionRequestPackage.fldGrp[index].len;
	if (copyDataLen < 0)
	{
		UnionUserErrLog("in UnionReadEsscRequestPackageFldByIndex:: copyDataLen = [%d] error!\n",copyDataLen);
		return(errCodeEsscMDL_PackageFldValueLen);
	}
	memcpy(value,gunionRequestPackage.fldGrp[index].value,copyDataLen);
	value[copyDataLen] = 0;
	*fldTag = gunionRequestPackage.fldGrp[index].tag;
	return(gunionRequestPackage.fldGrp[index].len);
}

int UnionSetConvertPackageKeyName(char *serviceID,char *appNo)
{
	return 0;
}
