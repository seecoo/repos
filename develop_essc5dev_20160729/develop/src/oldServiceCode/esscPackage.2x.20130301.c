//	Author:		张永定
//	Copyright:	Union Tech. Guangzhou
//	Date:		2013-08-21

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <ctype.h>

#include "esscPackage.h"
#include "unionErrCode.h"
#ifndef _UnionLogMDL_3_x_
#define _UnionLogMDL_3_x_
#endif
#include "UnionLog.h"
#include "unionREC.h"
#include "UnionStr.h"

#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_
#endif

int gunionIsSpierClientPackage = 0;
char gunionPackageVariable = 0;

TUnionEsscPackage	gunionRequestPackage;
TUnionEsscPackage	gunionResponsePackage;
int			gunionLogEsscPackageAnyway = 0;

long			gunionSsnFromClient = -1;	// 2009/7/8,王纯军增加

int UnionGetVersionOfEsscPackage()
{
	return(2);
}

void UnionSetLogEsscPackageAnyway()
{
	gunionLogEsscPackageAnyway = 1;
}

void UnionCloseLogEsscPackageAnyway()
{
	gunionLogEsscPackageAnyway = 0;
}

// 将一个ESSC报文域打入到包中,返回打入到包中的数据的长度
int UnionPutEsscPackageFldIntoStr(char *serviceID,int index,PUnionEsscPackageFld pfld,char *buf,int sizeOfBuf)
{
	if ((serviceID == NULL) || (pfld == NULL) || (buf == NULL))
	{
		UnionUserErrLog("in UnionPutEsscPackageFldIntoStr:: serviceID or pfld or data is null!\n");
		return(errCodeParameter);
	}
	
	if ((pfld->len <= 0) || (pfld->value == NULL))
		return(0);

	switch(pfld->tag)
	{
		case	conEsscFldKeyValue:
			if (UnionIsCheckUnionPayPackage())
			{
				memcpy(buf,pfld->value,pfld->len);
				return(pfld->len);
			}
			else	
			{
				sprintf(buf,"%02d%s",pfld->len,pfld->value);
				return(2+pfld->len);
			}
		default:
			memcpy(buf,pfld->value,pfld->len);
			UnionLog("in UnionPutEsscPackageFldIntoStr:: buf[%s]!\n",buf);
			return(pfld->len);
	}
	return(0);
}

// 设置一个域标签
int UnionSetOneEsscPackageFldValue(int tag,int len,char *data,PUnionEsscPackageFld pfld)
{
	pfld->tag = tag;
	pfld->len = len;
	memcpy(pfld->value,data,pfld->len);
	pfld->value[len] = 0;
	return(len);
}

int UnionSetEsscPackageFldByServiceCode(char *serviceID,int index,char *data,int len,PUnionEsscPackageFld pfld,int sizeOfFldValue)
{
	char	tmpNum[16];
	int	keyLen = 0;
	int	tag = 0;
	int	serviceCode = 0;
	int	offset = 0;

	serviceCode = atoi(serviceID);

	switch(serviceCode)
	{
		case	274:
		case	275:
			switch(index)
			{
				case	0:
					memcpy(tmpNum,data,2);
					tmpNum[2] = 0;
					UnionSetOneEsscPackageFldValue(conEsscFldAccNo,atoi(tmpNum),data+2,pfld);
					offset = 2 + 20;
					return(offset);
				case	1:
					return(UnionSetOneEsscPackageFldValue(conEsscFldFirstNodeID,4,data,pfld));
				default:
					return(0);
			}
			break;
		case	281:
		case	284:
		case	291:
		case	294:
		case	283:
		case	285:
		case	293:
		case	295:
			switch(index)
			{
				case	0:
					memcpy(tmpNum,data,2);		
					tmpNum[2] = 0;
					UnionSetOneEsscPackageFldValue(conEsscFldAccNo,atoi(tmpNum),data+2,pfld);
					offset = 2 + 20;
					return(offset);
				case	1:
					return(UnionSetOneEsscPackageFldValue(conEsscFldEncryptedPinByZPK,16,data,pfld));
				case	2:
					return(UnionSetOneEsscPackageFldValue(conEsscFldFirstNodeID,4,data,pfld));
				case	3:
					return(UnionSetOneEsscPackageFldValue(conEsscFldSecondNodeID,4,data,pfld));
				default:
					return(0);
			}
			break;	
		case	250:
			switch(index)
			{
				case	0:
					memcpy(tmpNum,data,4);		
					tmpNum[4] = 0;
					offset = 4;
					offset += UnionSetOneEsscPackageFldValue(conEsscFldMacData,atoi(tmpNum),data+4,pfld);
					return(offset);
				case	1:
					return(UnionSetOneEsscPackageFldValue(conEsscFldFirstNodeID,4,data,pfld));
				default:
					return(0);
			}
			break;
		case	251:
			switch(index)
			{
				case	0:
					memcpy(tmpNum,data,4);
					tmpNum[4] = 0;
					offset = 4;
					offset += UnionSetOneEsscPackageFldValue(conEsscFldMacData,atoi(tmpNum),data+4,pfld);
					return(offset);
				case	1:
					return(UnionSetOneEsscPackageFldValue(conEsscFldMac,8,data,pfld));
				case	2:
					return(UnionSetOneEsscPackageFldValue(conEsscFldNodeID,4,data,pfld));
				default:
					return(0);
			}
			break;
		case	240:
		case	241:
			switch(index)
			{
				case	0:
					memcpy(tmpNum,data,2);		
					tmpNum[2] = 0;
					UnionSetOneEsscPackageFldValue(conEsscFldAccNo,atoi(tmpNum),data+2,pfld);
					offset = 2 + 20;
					return(offset);
				case	1:
					return(UnionSetOneEsscPackageFldValue(conEsscFldCardPeriod,4,data,pfld));
				case	2:
					return(UnionSetOneEsscPackageFldValue(conEsscFldServiceID,3,data,pfld));
				case	3:
					return(UnionSetOneEsscPackageFldValue(conEsscFldNodeID,4,data,pfld));
				default:
					return(0);
			}
			break;
		case	261:
		case	260:
			if (serviceCode == 261)
				tag = conEsscFldCiperData;
			else	
				tag = conEsscFldData;

			switch(index)
			{
				case	0:
					memcpy(tmpNum, data, 4);
					tmpNum[4] = 0;
					offset = 4;
					offset += UnionSetOneEsscPackageFldValue(tag, atoi(tmpNum), data+4, pfld);
					return(offset);
				case	1:
					return(UnionSetOneEsscPackageFldValue(conEsscFldNodeID, 4, data, pfld));
				default:
					return(0);
			}
			break;
		case	202:
		case	203:
		case	204:
		case	205:
		case	255:
		case	256:
			if (index == 0)
				return(UnionSetOneEsscPackageFldValue(conEsscFldNodeID,4,data,pfld));
			else
				return(0);
			break;
		case	200:
		case	201:
			switch(index)
			{
				case	0:
					if (len == 60)
						keyLen = 48;	
					else if (len == 44)
						keyLen = 32;
					else if (len == 28)
						keyLen = 16;
					return(UnionSetOneEsscPackageFldValue(conEsscFldKeyValue,keyLen,data,pfld));
				case	1:
					return(UnionSetOneEsscPackageFldValue(conEsscFldKeyCheckValue,8,data,pfld));
				case	2:
					return(UnionSetOneEsscPackageFldValue(conEsscFldNodeID,4,data,pfld));
				default:
					return(0);
			}
			break;
		case	540:
			switch(index)
			{
				case	0:
					return(UnionSetOneEsscPackageFldValue(conEsscFldMacData,256,data,pfld));
				case	1:
					return(UnionSetOneEsscPackageFldValue(conEsscFldNodeID,4,data,pfld));
				default:
					return(0);
			}
			break;
		case	541:
			switch(index)
			{
				case	0:
					return(UnionSetOneEsscPackageFldValue(conEsscFldMacData,256,data,pfld));
				case	1:
					return(UnionSetOneEsscPackageFldValue(conEsscFldKeyValue,16,data,pfld));
				case	2:
					return(UnionSetOneEsscPackageFldValue(conEsscFldMac,16,data,pfld));
				case	3:
					return(UnionSetOneEsscPackageFldValue(conEsscFldNodeID,4,data,pfld));
				default:
					return(0);
			}
			break;
		case	252:
		case	253:
		case	254:
			switch(index)
			{
				case	0:
					memcpy(tmpNum,data,4);		
					tmpNum[4] = 0;
					offset = 4;
					offset += UnionSetOneEsscPackageFldValue(conEsscFldMacData,atoi(tmpNum),data+4,pfld);
					return(offset);
				case	1:
					return(UnionSetOneEsscPackageFldValue(conEsscFldNodeID,4,data,pfld));
				default:
					return(0);
			}
			break;
		case	100:
			if (index == 0)
				return(UnionSetOneEsscPackageFldValue(conEsscFldData,len,data,pfld));
			else
				return(0);
		case	671:
		case	672:
			if (index == 0)
				return(UnionSetOneEsscPackageFldValue(conEsscFldNodeID,4,data,pfld));
			else
				return(0);
			break;
		case	673:
			if (index == 0)
				return(UnionSetOneEsscPackageFldValue(conEsscFldKeyValue,32,data,pfld));
			else if (index == 1)
				return(UnionSetOneEsscPackageFldValue(conEsscFldNodeID,4,data,pfld));
			else
				return(0);
			break;
		case	674:
			switch(index)
			{
				case	0:
					gunionPackageVariable = data[0];
					return(UnionSetOneEsscPackageFldValue(conEsscFldAlgorithmMode,1,data,pfld));
				case	1:
					return(UnionSetOneEsscPackageFldValue(conEsscFldAccNo,19,data,pfld));
				case	2:
					return(UnionSetOneEsscPackageFldValue(conEsscFldCardPeriod,4,data,pfld));
				case	3:
					if ((gunionPackageVariable == '2') || (gunionPackageVariable == '3'))
						return(UnionSetOneEsscPackageFldValue(conEsscFldServiceID,3,data,pfld));
					else
						return(UnionSetOneEsscPackageFldValue(conEsscFldNodeID,4,data,pfld));
				case	4:
					return(UnionSetOneEsscPackageFldValue(conEsscFldNodeID,4,data,pfld));
				default:
					return(0);
			}
			break;
		case	675:
			switch(index)
			{
				case	0:
					gunionPackageVariable = data[0];
					return(UnionSetOneEsscPackageFldValue(conEsscFldAlgorithmMode,1,data,pfld));
				case	1:
					return(UnionSetOneEsscPackageFldValue(conEsscFldAccNo,19,data,pfld));
				case	2:
					return(UnionSetOneEsscPackageFldValue(conEsscFldCardPeriod,4,data,pfld));
				case	3:
					if (gunionPackageVariable != '0')
						return(UnionSetOneEsscPackageFldValue(conEsscFldServiceID,3,data,pfld));
					else
						return(UnionSetOneEsscPackageFldValue(conEsscFldRandNum1,5,data,pfld));
				case	4:
					if (gunionPackageVariable == '2')
						return(UnionSetOneEsscPackageFldValue(conEsscFldRandNum1,5,data,pfld));
					else if (gunionPackageVariable == '0')
						return(UnionSetOneEsscPackageFldValue(conEsscFldRandNum2,4,data,pfld));
					else
						return(UnionSetOneEsscPackageFldValue(conEsscFldRandNum3,3,data,pfld));
				case	5:
					if (gunionPackageVariable == '2')
						return(UnionSetOneEsscPackageFldValue(conEsscFldRandNum2,4,data,pfld));
					else if (gunionPackageVariable == '0')
						return(UnionSetOneEsscPackageFldValue(conEsscFldRandNum3,3,data,pfld));
					else
						return(UnionSetOneEsscPackageFldValue(conEsscFldNodeID,4,data,pfld));
				case	6:
					if (gunionPackageVariable == '2')
						return(UnionSetOneEsscPackageFldValue(conEsscFldRandNum3,3,data,pfld));
					else
						return(UnionSetOneEsscPackageFldValue(conEsscFldNodeID,4,data,pfld));
				case	7:
					return(UnionSetOneEsscPackageFldValue(conEsscFldNodeID,4,data,pfld));
				default:
					return(0);
			}
			break;
		default:
			return(0);
	}

	return 0;
}

// 从一个ESSC报文中读取一个域，返回域在包中占的长度
int UnionReadEsscPackageFldFromStr(char *serviceID,int index,char *data,int len,PUnionEsscPackageFld pfld,int sizeOfFldValue)
{
	int	offset = 0;
	char	tmpBuf[128];
	int	ret = 0;
	int	i = 0;

	if ((serviceID == NULL) || (pfld == NULL) || (data == NULL) || (pfld->value == NULL))
	{
		UnionUserErrLog("in UnionReadEsscPackageFldFromStr:: serviceID or pfld or data or pfld->value is null!\n");
		return(errCodeParameter);
	}
	
	if (len <= 0)
		return(0);

	if (strcasecmp(UnionGetIDOfCustomization(),"UnionPay") == 0)
		return(UnionSetEsscPackageFldByServiceCode(serviceID,index,data,len,pfld,sizeOfFldValue));
	
	if (strcmp(serviceID,"001") == 0)
	{
		switch(index + 1)
		{
			case	1:
				pfld->tag = conEsscFldPinByRsaPK;
				if (len <= 200)
				{
					pfld->len = 128;
					data[128+1] = 0;
				}
				else
					pfld->len = 256;
				memcpy(pfld->value,data,pfld->len);
				pfld->value[pfld->len] = 0;
				if (pfld->len == 128)
					offset = len;
				else
					offset = pfld->len;
				return(offset);
			case	2:
				pfld->tag = conEsscFldRandNum;
				pfld->len = len;
				if (len < 6)
					pfld->len = 0;
				else
				{
					pfld->len = 6;
					for (i = 0; i < 6; i++)
					{
						if (!isprint(data[i]))
						{
							pfld->len = 0;
							break;
						}
					}
				}
				memcpy(pfld->value,data,pfld->len);
				pfld->value[pfld->len] = 0;
				offset = len;
				if (data[offset] == ';')
					offset ++;
				return(offset);

			default:
				return(0);
		}
	}
	else if (strcmp(serviceID,"002") == 0)
	{
		switch(index + 1)
		{
			case	1:
				pfld->tag = conEsscFldPinByRsaPK;
				memcpy(tmpBuf,data+128+19,10);
				tmpBuf[10] = 0;
				UnionFilterHeadAndTailBlank(tmpBuf);
				if (strcasecmp(tmpBuf,"s0102") == 0 || strcasecmp(tmpBuf,"s0101") == 0)
				{
					pfld->len = 128;
					data[128+19+10] = 0;
				}
				else
					pfld->len = 256;
				
				memcpy(pfld->value,data,pfld->len);
				pfld->value[pfld->len] = 0;
				offset = pfld->len;
				if (data[offset] == ';')
					offset ++;
				return(offset);
			case	2:
				pfld->tag = conEsscFldAccNo;
				pfld->len = 19;
				memcpy(pfld->value,data,pfld->len);
				pfld->value[pfld->len] = 0;
				offset = pfld->len;
				if (data[offset] == ';')
					offset ++;
				return(offset);
			case	3:
				pfld->tag = conEsscFldKeyName;
				memcpy(tmpBuf,data,10);
				tmpBuf[10] = 0;
				UnionFilterHeadAndTailBlank(tmpBuf);
				if (strcasecmp(tmpBuf,"s0102") == 0)
					ret = sprintf(tmpBuf,"H2.NIBS000000000.ZPK");
				else if (strcasecmp(tmpBuf,"s0101") == 0)
					ret = sprintf(tmpBuf,"NS.S010100000000.ZPK");
				tmpBuf[ret] = 0;
				pfld->len = ret;
				memcpy(pfld->value,tmpBuf,pfld->len);
				pfld->value[pfld->len] = 0;
				offset = 10;
				if (data[offset] == ';')
					offset ++;
				return(offset);
			case	4:
				pfld->tag = conEsscFldRandNum;
				pfld->len = len;
				if (len < 6)
					pfld->len = 0;
				else
				{
					pfld->len = 6;
					for (i = 0; i < 6; i++)
					{
						if (!isprint(data[i]))
						{
							pfld->len = 0;
							break;
						}
					}
				}
				memcpy(pfld->value,data,pfld->len);
				pfld->value[pfld->len] = 0;
				offset = len;
				if (data[offset] == ';')
					offset ++;
				return(offset);
			default:
				return(0);
		}
	}
	else if (strcmp(serviceID,"201") == 0)
	{
		switch(index + 1)
		{
			case	1:
				pfld->tag = conEsscFldKeyName;
				if ((pfld->len = UnionConvertIntoLen(data,2)) <= 0)
					return(0);
				memcpy(pfld->value,data+2,pfld->len);
				pfld->value[pfld->len] = 0;
				offset = 2 + pfld->len;
				if (data[offset] == ';')
					offset ++;	
				return(offset);
			case	2:
				pfld->tag = conEsscFldZMKName;
				if ((pfld->len = UnionConvertIntoLen(data,2)) <= 0)
					return(0);
				memcpy(pfld->value,data+2,pfld->len);
				pfld->value[pfld->len] = 0;
				offset = 2 + pfld->len;
				return(offset);
			default:
				return(0);
		}
	}
	else if (strcmp(serviceID,"302") == 0)
	{
		switch(index + 1)
		{
			case	1:
				pfld->tag = conEsscFldKeyName;
				if ((pfld->len = UnionConvertIntoLen(data,2)) <= 0)
					return(0);
				memcpy(pfld->value,data+6,pfld->len);
				pfld->value[pfld->len] = 0;
				offset = 2;
				if (data[offset] == ';')
					offset ++;
				return(offset);
			case	2:
				pfld->tag = conEsscFldMacData;
				if ((offset = UnionConvertIntoLen(data-2,2)) <= 0)
					return(0);
				if ((pfld->len = UnionConvertIntoLen(data,4)) <= 0)
					return(0);
				memcpy(pfld->value,data+offset+4,pfld->len);
				pfld->value[pfld->len] = 0;
				offset += pfld->len+4;
				return(offset);
			default:
				return(0);
		}
	}
	else if (strcmp(serviceID,"332") == 0)
	{
		switch(index + 1)
		{
			case	1:
				pfld->tag = conEsscFldMac;
				pfld->len = 16;
				memcpy(pfld->value,data,pfld->len);
				pfld->value[pfld->len] = 0;
				offset = pfld->len;
				if (data[offset] == ';')
					offset ++;
				return(offset);
			case	2:
				pfld->tag = conEsscFldKeyName;
				if ((pfld->len = UnionConvertIntoLen(data,2)) <= 0)
					return(0);
				memcpy(pfld->value,data+6,pfld->len);
				pfld->value[pfld->len] = 0;
				offset = 2;
				if (data[offset] == ';')
					offset ++;
				return(offset);
			case	3:
				pfld->tag = conEsscFldMacData;
				if ((offset = UnionConvertIntoLen(data-2,2)) <= 0)
					return(0);
				if ((pfld->len = UnionConvertIntoLen(data,4)) <= 0)
					return(0);
				memcpy(pfld->value,data+offset+4,pfld->len);
				pfld->value[pfld->len] = 0;
				offset += pfld->len+4;
				return(offset);
			default:
				return(0);
		}
	}
	else if (strcmp(serviceID,"404") == 0)
	{
		switch(index + 1)
		{
			case	1:
				pfld->tag = conEsscFldEncryptedPinByZPK;
				pfld->len = 16;
				memcpy(pfld->value,data,pfld->len);
				pfld->value[pfld->len] = 0;
				offset = pfld->len;
				if (data[offset] == ';')
					offset ++;
				return(offset);
			case	2:
				pfld->tag = conEsscFldAccNo;
				if ((pfld->len = UnionConvertIntoLen(data,2)) <= 0)
					return(0);
				memcpy(pfld->value,data+6,pfld->len);
				pfld->value[pfld->len] = 0;
				offset = 2;
				if (data[offset] == ';')
					offset ++;
				return(offset);
			case	3:
				pfld->tag = conEsscFldFirstWKName;
				if ((offset = UnionConvertIntoLen(data-2,2)) <= 0)
					return(0);
				if ((pfld->len = UnionConvertIntoLen(data,2)) <= 0)
					return(0);
				memcpy(pfld->value,data+offset+4,pfld->len);
				pfld->value[pfld->len] = 0;
				offset = 2;
				if (data[offset] == ';')
					offset ++;
				return(offset);
			case	4:
				pfld->tag = conEsscFldSecondWKName;
				if ((offset = UnionConvertIntoLen(data-4,2)) <= 0)
					return(0);
				if ((offset += UnionConvertIntoLen(data-2,2)) <= 0)
					return(0);
				if ((pfld->len = UnionConvertIntoLen(data,2)) <= 0)
					return(0);
				memcpy(pfld->value,data+offset+2,pfld->len);
				pfld->value[pfld->len] = 0;
				offset += pfld->len+2;
				return(offset);
			default:
				return(0);
		}
	}
	else if (strcmp(serviceID,"462") == 0)
	{
		switch(index + 1)
		{
			case	1:
				pfld->tag = conEsscFldEncryptedPinByZPK;
				pfld->len = 16;
				memcpy(pfld->value,data,pfld->len);
				pfld->value[pfld->len] = 0;
				offset = pfld->len;
				if (data[offset] == ';')
					offset ++;
				return(offset);
			case	2:
				pfld->tag = conEsscFldIBMPinOffset;
				pfld->len = 12;
				memcpy(pfld->value,data,pfld->len);
				pfld->value[pfld->len] = 0;
				offset = pfld->len;
				if (data[offset] == ';')
					offset ++;
				return(offset);
			case	3:
				pfld->tag = conEsscFldAccNo;
				if ((pfld->len = UnionConvertIntoLen(data,2)) <= 0)
					return(0);
				memcpy(pfld->value,data+6,pfld->len);
				pfld->value[pfld->len] = 0;
				offset = 2;
				if (data[offset] == ';')
					offset ++;
				return(offset);
			case	4:
				pfld->tag = conEsscFldFirstWKName;
				if ((offset = UnionConvertIntoLen(data-2,2)) <= 0)
					return(0);
				if ((pfld->len = UnionConvertIntoLen(data,2)) <= 0)
					return(0);
				memcpy(pfld->value,data+offset+4,pfld->len);
				pfld->value[pfld->len] = 0;
				offset = 2;
				if (data[offset] == ';')
					offset ++;
				return(offset);
			case	5:
				pfld->tag = conEsscFldSecondWKName;
				if ((offset = UnionConvertIntoLen(data-4,2)) <= 0)
					return(0);
				if ((offset += UnionConvertIntoLen(data-2,2)) <= 0)
					return(0);
				if ((pfld->len = UnionConvertIntoLen(data,2)) <= 0)
					return(0);
				memcpy(pfld->value,data+offset+2,pfld->len);
				pfld->value[pfld->len] = 0;
				offset += pfld->len+2;
				return(offset);
			default:
				return(0);
		}
	}

	return(0);
}

// 初始化请求报文
int UnionInitEsscRequestPackage()
{
	//memset(&gunionRequestPackage,0,sizeof(gunionRequestPackage));
	gunionRequestPackage.fldNum = 0;
	gunionRequestPackage.offset = 0;
	return(0);
}

// 设置请求报文域
int UnionSetEsscRequestPackageFld(int fldTag,int len,char *value)
{
	if ((len < 0) || (value == NULL))
	{
		UnionUserErrLog("in UnionSetEsscRequestPackageFld:: fldTag = [%03d] parameter error!\n",fldTag);
		return(errCodeParameter);
	}
	if (len + gunionRequestPackage.offset >= sizeof(gunionRequestPackage.dataBuf))
	{
		UnionUserErrLog("in UnionSetEsscRequestPackageFld:: dataBuf = [%04d] offset = [%04d] fldLen = [%04d]\n",
			(int)sizeof(gunionRequestPackage.dataBuf),gunionRequestPackage.offset,len);
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

// 读取请求报文域
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

// 将请求包写入日志
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

// 将请求包写入日志
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

// 打一个请求包
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
	if (sizeOfBuf < 2 + 3 + 1)
	{
		UnionUserErrLog("in UnionPackEsscRequestPackage:: sizeOfBuf [%d] too small!\n",sizeOfBuf);
		return(errCodeSmallBuffer);
	}
	memcpy(buf,idOfApp,2);
	idOfApp[2] = 0;
	memcpy(buf+2,serviceID,3);
	serviceID[3] = 0;
	memcpy(buf+2+3,"1",1);
	offset = 2+3+1;
	if (gunionLogEsscPackageAnyway)
		UnionLogEsscRequestPackageAnyway(idOfApp,serviceID);
	else
	{
		UnionNullLogWithTime("before pack [%s%s1]::\n",idOfApp,serviceID);
		UnionLogEsscRequestPackage();
	}
	for (index = 0; ; index++)
	{
		if ((thisLen = UnionPutEsscPackageFldIntoStr(serviceID,index,&(gunionRequestPackage.fldGrp[index]),buf+offset,sizeOfBuf-offset)) < 0)
		{
			UnionUserErrLog("in UnionPackEsscRequestPackage:: UnionPutEsscPackageFldIntoStr [%03d]\n",gunionRequestPackage.fldGrp[index].tag);
			return(thisLen);
		}
		else if (thisLen == 0)
			break;
		offset += thisLen;
	}
	return(offset);
}

// 打一个请求包
int UnionUnpackEsscRequestPackage(char *data,int lenOfData,char *idOfApp,char *serviceID)
{
	int	offset;
	int	thisLen;
	int	index = 0;
	//char	ascBuf[8192+1];

	// 打印请求报文
	data[lenOfData] = 0;
	UnionNullLogWithTime("in UnionUnpackEsscRequestPackage:: Request data[%04d][%s]!\n",lenOfData,data);
	
	gunionSsnFromClient = -1;	// 2009/7/8,Wolfgang Wang added
	
	if ((idOfApp == NULL) || (serviceID == NULL) || (data == NULL) || (lenOfData <= 0))
	{
		UnionUserErrLog("in UnionUnpackEsscRequestPackage:: null pointer!\n");
		return(errCodeParameter);
	}
	UnionInitEsscRequestPackage();
	if (lenOfData < 2 + 3 + 1)
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
	/*
	memcpy(tmpBuf,data+2+3+1,3);
	tmpBuf[3] = 0;
	gunionRequestPackage.fldNum = atoi(tmpBuf);
	offset = 2 + 3 + 1 + 3;
	*/
	gunionRequestPackage.fldNum = 0;
	offset = 2 + 3 + 1;
	//for (index = 0; index < gunionRequestPackage.fldNum; index++)
	for (index = 0; ; index++)
	{
		gunionRequestPackage.fldGrp[index].value = gunionRequestPackage.dataBuf+gunionRequestPackage.offset;
		if ((thisLen = UnionReadEsscPackageFldFromStr(serviceID,index,data+offset,lenOfData-offset,
				&(gunionRequestPackage.fldGrp[index]),
				sizeof(gunionRequestPackage.dataBuf)-gunionRequestPackage.offset)) < 0)
		{
			UnionUserErrLog("in UnionUnpackEsscRequestPackage:: UnionReadEsscPackageFldFromStr [%03d]\n",index);
			return(thisLen);
		}
		else if (thisLen == 0)
			break;
		
		gunionRequestPackage.fldNum ++;
		offset += thisLen;
		gunionRequestPackage.offset += (gunionRequestPackage.fldGrp[index].len + 1);
	}
	if (gunionLogEsscPackageAnyway)
		UnionLogEsscRequestPackageAnyway(idOfApp,serviceID);
	else
	{
		UnionNullLogWithTime("after unpack [%s%s1]::\n",idOfApp,serviceID);
		UnionLogEsscRequestPackage();
	}

	/*
	for (index = 0; index < gunionRequestPackage.fldNum; index++)
	{
		if (gunionRequestPackage.fldGrp[index].tag == conEsscFldPlainPin)
			return 0;
		if (UnionIsPrintByRequestFlag(serviceID,2,gunionRequestPackage.fldGrp[index].tag))
		{
			bcdhex_to_aschex((char *)gunionRequestPackage.fldGrp[index].value,gunionRequestPackage.fldGrp[index].len,ascBuf);
			UnionNullLogWithTimeAnyway("(%03d) (%04d) (%s)\n",gunionRequestPackage.fldGrp[index].tag,gunionRequestPackage.fldGrp[index].len*2,ascBuf);
		}
	}

	*/
	return(0);
}

// 初始化响应报文
int UnionInitEsscResponsePackage()
{
	memset(&gunionResponsePackage,0,sizeof(gunionResponsePackage));
	gunionResponsePackage.fldNum = 0;
	gunionResponsePackage.offset = 0;
	return(0);
}

// 设置响应报文域
int UnionSetEsscResponsePackageFld(int fldTag,int len,char *value)
{
	if (value == NULL)
	{
		UnionUserErrLog("in UnionSetEsscResponsePackageFld:: fldTag = [%03d] parameter error!\n",fldTag);
		return(errCodeParameter);
	}
	if (len <= 0)
		return(0);
		
	if (len + gunionResponsePackage.offset >= sizeof(gunionResponsePackage.dataBuf))
	{
		UnionUserErrLog("in UnionSetEsscResponsePackageFld:: dataBuf = [%04d] offset = [%04d] fldLen = [%04d]\n",
			(int)sizeof(gunionResponsePackage.dataBuf),gunionResponsePackage.offset,len);
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

// 读取响应报文域
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

// 将响应包写入日志
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

// 将响应包写入日志
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

// 打一个响应包
int UnionPackEsscResponsePackage(char *idOfApp,char *serviceID,int responseCode,char *buf,int sizeOfBuf)
{
	int	ret;
	int	offset;
	int	thisLen;
	int	index;
	char	tmpBuf[128];
	
	if ((idOfApp == NULL) || (serviceID == NULL) || (buf == NULL))
	{
		UnionUserErrLog("in UnionPackEsscResponsePackage:: null pointer!\n");
		return(errCodeParameter);
	}
	if (sizeOfBuf < 2 + 3 + 1)
	{
		UnionUserErrLog("in UnionPackEsscResponsePackage:: sizeOfBuf [%d] too small!\n",sizeOfBuf);
		return(errCodeSmallBuffer);
	}
	memcpy(buf,idOfApp,2);
	memcpy(buf+2,serviceID,3);
	memcpy(buf+2+3,"0",1);
	offset = 2+3+1;
	if (responseCode < 0)
	{
		if (strcasecmp(UnionGetIDOfCustomization(),"cgb") == 0)
		{
			if (strcmp(serviceID,"332") == 0)
			{
				if (errCodeEsscMDL_TwoMacNotSame == responseCode)
				{
					sprintf(buf+offset,"%s","VM");
					goto offset;
				}
			}
		}

		if ( responseCode > -2000  && responseCode < errCodeOffsetOfHsmReturnCodeMDL)
		{
			sprintf(tmpBuf,"%06d",responseCode);
			memmove(tmpBuf,tmpBuf+strlen(tmpBuf)-2,2);
			tmpBuf[2] = 0;
			responseCode = atoi(tmpBuf);
			sprintf(buf+offset,"%02d",responseCode);
		}
		else
		{
			if (strcasecmp(UnionGetIDOfCustomization(),"UnionPay") == 0)
				sprintf(buf+offset,"%s","15");
			else
				sprintf(buf+offset,"%s","IE");
		}
offset:
		offset += 2;

		// 打印响应日志
		UnionNullLogWithTime("in UnionPackEsscResponsePackage:: Response data[%04d][%s]!\n",offset,buf);
		// zhangyd 20140612 add
		return(offset);
	}
	else
		sprintf(buf+offset,"00");
	offset += 2;
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
	// 2009/7/8, addition ended
	if (gunionLogEsscPackageAnyway)
		UnionLogEsscResponsePackageAnyway(idOfApp,serviceID,responseCode);
	else
	{
		UnionNullLogWithTime("before pack [%s%s0%06d]::\n",idOfApp,serviceID,responseCode);
		UnionLogEsscResponsePackage();
	}
	for (index = 0; ; index++)
	{
		if ((thisLen = UnionPutEsscPackageFldIntoStr(serviceID,index,&(gunionResponsePackage.fldGrp[index]),buf+offset,sizeOfBuf-offset)) < 0)
		{
			UnionUserErrLog("in UnionPackEsscResponsePackage:: UnionPutEsscPackageFldIntoStr [%03d]\n",gunionResponsePackage.fldGrp[index].tag);
			return(thisLen);
		}
		else if(thisLen == 0)
			break;	
		offset += thisLen;
	}

	// 打印响应日志
	buf[offset] = 0;
	UnionNullLogWithTime("in UnionPackEsscResponsePackage:: Response data[%04d][%s]!\n",offset,buf);
	return(offset);
}

// 解一个响应包
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
	//if ((responseCode = atoi(tmpBuf)) < 0)	// 响应码不为0
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
// 获取响应包的域数目
int UnionGetMaxFldNumOfEsscResponsePackage()
{
	return(gunionResponsePackage.fldNum);
}

// 根据域索引号，读取响应报文域
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
		if (!gunionIsSpierClientPackage)	// 不是监控客户端
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

// 获取请求包的域数目
int UnionGetMaxFldNumOfEsscRequestPackage()
{
	return(gunionRequestPackage.fldNum);
}

// 根据域索引号，读取请求报文域
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
		if (!gunionIsSpierClientPackage)	// 不是监控客户端
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

