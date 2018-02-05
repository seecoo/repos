//	Author:		张永定
//	Copyright:	Union Tech. Guangzhou
//	Date:		2013-08-21

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include "unionREC.h"
#include "UnionStr.h"
#include "UnionLog.h"
#include "unionErrCode.h"

#include "esscPackage.h"
#include "unionRealBaseDB.h"
#include "unionXMLPackage.h"
#include "symmetricKeyDB.h"
#include "unionTableData.h"
#include "accessCheck.h"
#include "commWithHsmSvr.h"

int	gunionIsNotConvertPackage = 1;
int	gunionIsUnionPayPackage = 0;
char	gunionOldAppID[32];
char	gunionOldServiceCode[32];

#define	SERVICE_FIELD_NUM_MAX	20
#define OLD_SERVICECODE_MAXNUM 	512

typedef struct
{
	char	fieldName[64];
	int	method;
	char	parameter[160];
}TUnionOldServiceFieldREC;
typedef TUnionOldServiceFieldREC	*PUnionOldServiceFieldREC;

typedef struct
{	
	int	num;
	TUnionOldServiceFieldREC	rec[SERVICE_FIELD_NUM_MAX];
}TUnionOldServiceFieldTBL;
typedef TUnionOldServiceFieldTBL	*PUnionOldServiceFieldTBL;

typedef struct
{
	char	oldServiceCode[8];
	int	version;
	// char	currServiceCode[8];
	int	flag;
	
	TUnionOldServiceFieldTBL	fieldTBL;
}TUnionOldServiceCodeTBL;
typedef TUnionOldServiceCodeTBL		*PUnionOldServiceCodeTBL;

TUnionOldServiceCodeTBL			gunionOldServiceCodeTBL[OLD_SERVICECODE_MAXNUM];
/*
//add by zhouxw 20150826
void			*handle;
int (*UnionDealKMSvrCode)(char *);
//add end
*/
//char	gunionOldServiceFieldName[OLD_SERVICECODE_MAXNUM][SERVICE_FIELD_NUM_MAX*40+40];
int	current_oldServiceCode_num = 0;
int	isKMSvr = 0;

int UnionIsCheckUnionPayPackage()
{
	return gunionIsUnionPayPackage;
}

static int getOldServiceFieldName(char *req_oldServiceCode,int req_version,int req_flag)
{
	int	i;
	int	ret;
	int	totalNum = 0;
	char	sql[128];
	char	tmpBuf[32];
	char	oldServiceCode[8];
	//char	tmpOldServiceCode[8];
	int	version = 0;
	//int	tmpVersion = 0;
	int	flag = 0;
	//int	tmpFlag = 0;
	int	fieldNum = 0;
	PUnionOldServiceFieldTBL	pfieldTBL = NULL;
	PUnionOldServiceCodeTBL		pcodeTBL = NULL;

	if (req_oldServiceCode != NULL && strlen(req_oldServiceCode) > 0)
		snprintf(sql,sizeof(sql),"select oldServiceCode,version,flag,fieldName,method,parameter from oldServiceFiled where oldServiceCode = '%s' and version = %d and flag = %d order by seqNo",req_oldServiceCode,req_version,req_flag);
	else
		snprintf(sql,sizeof(sql),"select oldServiceCode,version,flag,fieldName,method,parameter from oldServiceFiled order by oldServiceCode,version,flag,seqNo");

	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in getOldServiceFieldName:: UnionSelectRealDBRecord[%s]! ret = [%d]\n",sql,ret);
		return(ret);
	}
	if (ret == 0)
		return(0);
	
	// 读取记录数量
	if ((ret = UnionReadXMLPackageValue("totalNum", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in getOldServiceFieldName:: UnionReadXMLPackageValue[%s]!\n","totalNum");
		return(ret);
	}
	totalNum = atoi(tmpBuf);
	UnionLog("in getOldServiceFieldName:: totalNum[%d]\n",totalNum);

	//tmpOldServiceCode[0] = 0;
	//tmpVersion = 0;
	//tmpFlag = 0;
	
	pcodeTBL = &gunionOldServiceCodeTBL[current_oldServiceCode_num];

	for (i = 0; i < totalNum; i++)
	{
		if ((ret = UnionLocateXMLPackage("detail", i+1)) < 0)
		{
			UnionUserErrLog("in getOldServiceFieldName:: UnionLocateXMLPackage[%d]!\n",i);
			return(ret);
		}
		
		if ((ret = UnionReadXMLPackageValue("oldServiceCode", oldServiceCode, sizeof(oldServiceCode))) < 0)
		{
			UnionUserErrLog("in getOldServiceFieldName:: UnionReadXMLPackageValue[%d][%s]!\n",i,"oldServiceCode");
			return(ret);
		}

		if ((ret = UnionReadXMLPackageValue("version", tmpBuf, sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in getOldServiceFieldName:: UnionReadXMLPackageValue[%d][%s]!\n",i,"version");
			return(ret);
		}
		version = atoi(tmpBuf);

		if ((ret = UnionReadXMLPackageValue("flag", tmpBuf, sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in getOldServiceFieldName:: UnionReadXMLPackageValue[%d][%s]!\n",i,"");
			return(ret);
		}
		flag = atoi(tmpBuf);
		
		if ((strcmp(pcodeTBL->oldServiceCode,oldServiceCode) != 0) ||
			(pcodeTBL->version != version) ||
			(pcodeTBL->flag != flag))
		{
			pcodeTBL = &gunionOldServiceCodeTBL[current_oldServiceCode_num];
			strcpy(pcodeTBL->oldServiceCode,oldServiceCode);
			pcodeTBL->version = version;
			pcodeTBL->flag = flag;

			if (++current_oldServiceCode_num > OLD_SERVICECODE_MAXNUM)
			{
				UnionUserErrLog("in getOldServiceFieldName:: OLD_SERVICECODE_MAXNUM[%d] too small!\n",OLD_SERVICECODE_MAXNUM);
				return(-1);
			}
			fieldNum = 0;
		}

		pfieldTBL = &gunionOldServiceCodeTBL[current_oldServiceCode_num-1].fieldTBL;

		if ((ret = UnionReadXMLPackageValue("fieldName", pfieldTBL->rec[fieldNum].fieldName, sizeof(pfieldTBL->rec[fieldNum].fieldName))) < 0)
		{
			UnionUserErrLog("in getOldServiceFieldName:: UnionReadXMLPackageValue[%d][%s]!\n",fieldNum,"fieldName");
			return(ret);
		}
			
		if ((ret = UnionReadXMLPackageValue("method", tmpBuf, sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in getOldServiceFieldName:: UnionReadXMLPackageValue[%d][%s]!\n",fieldNum,"method");
			return(ret);
		}
		pfieldTBL->rec[fieldNum].method = atoi(tmpBuf);
	
		if ((ret = UnionReadXMLPackageValue("parameter", pfieldTBL->rec[fieldNum].parameter, sizeof(pfieldTBL->rec[fieldNum].parameter))) < 0)
		{
			UnionUserErrLog("in getOldServiceFieldName:: UnionReadXMLPackageValue[%d][%s]!\n",fieldNum,"parameter");
			return(ret);
		}
		
		fieldNum ++;
		pfieldTBL->num = fieldNum;
	}


	UnionLog("in getOldServiceFieldName:: current_oldServiceCode_num[%d]!\n",current_oldServiceCode_num);
	return(current_oldServiceCode_num);
}

static int getOldServiceFiledREC(char *oldServiceCode, int version,int flag,PUnionOldServiceFieldTBL pfieldTBL)
{
	int	i;
	int	ret;
	
	if (current_oldServiceCode_num == 0)
	{
		if ((ret = getOldServiceFieldName(NULL,0,0)) < 0)
		{
			UnionUserErrLog("in getOldServiceFiledREC:: getOldServiceFieldName! ret = [%d]\n",ret);
			return(ret);
		}
		else if (ret == 0)
		{
			UnionUserErrLog("in getOldServiceFiledREC:: getOldServiceFieldName! ret = [%d]\n",ret);
			return 0;
		}
	}
	
	for (i = 0; i < current_oldServiceCode_num; i++)
	{
		if ((strcmp(gunionOldServiceCodeTBL[i].oldServiceCode,oldServiceCode) == 0) &&
			(gunionOldServiceCodeTBL[i].version == version) &&
			(gunionOldServiceCodeTBL[i].flag == flag))
		{
			memcpy(pfieldTBL,&gunionOldServiceCodeTBL[i].fieldTBL,sizeof(TUnionOldServiceFieldTBL));
			return(gunionOldServiceCodeTBL[i].fieldTBL.num);
		}
	}
	if (i == current_oldServiceCode_num)
	{
		/*
		if ((ret = getOldServiceFieldName(oldServiceCode,version,flag)) < 0)
		{
			UnionUserErrLog("in getOldServiceFiledREC:: getOldServiceFieldName oldServiceCode[%s]! ret = [%d]\n",oldServiceCode,ret);
			return(ret);
		}
		else if (ret == 0)
		{
			UnionUserErrLog("in getOldServiceFiledREC:: getOldServiceFieldName oldServiceCode[%s] not found!\n",oldServiceCode);
			return 0;
		}
		memcpy(pfieldTBL,&gunionOldServiceCodeTBL[i].fieldTBL,sizeof(TUnionOldServiceFieldTBL));
		*/
		return(0);
	}

	return(gunionOldServiceCodeTBL[i].fieldTBL.num);
}
/*static int getOldServiceFiledREC(char *oldServiceCode, int version,int flag,PUnionOldServiceFieldTBL pfieldTBL)
{
	int	i;
	int	ret;
	int	totalNum = 0;
	char	sql[128];
	char	tmpBuf[32];

	snprintf(sql,sizeof(sql),"select * from oldServiceFiled where oldServiceCode = '%s' and version = %d and flag = %d order by seqNo",oldServiceCode,version,flag);
	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in getOldServiceFiledREC:: UnionSelectRealDBRecord[%s]! ret = [%d]\n",sql,ret);
		return(ret);
	}
	if (ret == 0)
		return(0);
	
	// 读取记录数量
	if ((ret = UnionReadXMLPackageValue("totalNum", tmpBuf, sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in getOldServiceFiledREC:: UnionReadXMLPackageValue[%s]!\n","totalNum");
		return(ret);
	}
	tmpBuf[ret] = 0;
	if ((totalNum = atoi(tmpBuf)) > SERVICE_FIELD_NUM_MAX)
	{
		UnionUserErrLog("in getOldServiceFiledREC:: totalNum[%d] > [%d]!\n",totalNum,SERVICE_FIELD_NUM_MAX);
		return(errCodeParameter);
	}

	for (i = 0; i < totalNum; i++)
	{
		if ((ret = UnionLocateXMLPackage("detail", i+1)) < 0)
		{
			UnionUserErrLog("in getOldServiceFiledREC:: UnionLocateXMLPackage!\n");
			return(ret);
		}
	
		if ((ret = UnionReadXMLPackageValue("fieldName", pfieldTBL->rec[i].fieldName, sizeof(pfieldTBL->rec[i].fieldName))) < 0)
		{
			UnionUserErrLog("in getOldServiceFiledREC:: UnionReadXMLPackageValue[%d][%s]!\n",i,"fieldName");
			return(ret);
		}
		pfieldTBL->rec[i].fieldName[ret] = 0;
			
		if ((ret = UnionReadXMLPackageValue("method", tmpBuf, sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in getOldServiceFiledREC:: UnionReadXMLPackageValue[%d][%s]!\n",i,"method");
			return(ret);
		}
		tmpBuf[ret] = 0;
		pfieldTBL->rec[i].method = atoi(tmpBuf);
	
		if ((ret = UnionReadXMLPackageValue("parameter", pfieldTBL->rec[i].parameter, sizeof(pfieldTBL->rec[i].parameter))) < 0)
		{
			UnionUserErrLog("in getOldServiceFiledREC:: UnionReadXMLPackageValue[%d][%s]!\n",i,"parameter");
			return(ret);
		}
		pfieldTBL->rec[i].parameter[ret] = 0;
	}
	return(totalNum);
}*/

// 旧ESSC报文转成当前ESSC报文
int UnionConvertOtherPackageToXMLPackage(unsigned char *buf,int lenOfBuf,char *cliIPAddr)
{
	int	i;
	int	ret;
	int	totalNum = 0;
	char	sql[1024];
	char	currServiceCode[32];
	char	tmpBuf[4096];
	char	fieldName[64];
	char	fieldValue[4096];
	char	*ptr = NULL;
	char	hsmGroupIDList[32];//加密机组号
	int	isBitData = 0;
	int	lenOfBitData = 0;
	TUnionOldServiceFieldTBL	fieldTBL;
	char	tmpOldServiceCode[32];

	if ((ptr = UnionReadStringTypeRECVar("abOfMyself")) == NULL)
	{
		UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionReadStringTypeRECVar[%s] error!\n","abOfMyself");
		return(errCodeRECMDL_VarNotExists);
	}
	if (strcasecmp(UnionGetIDOfCustomization(),"UnionPay") == 0)
		gunionIsUnionPayPackage = 1;
	else
		gunionIsUnionPayPackage = 0;

	// 是XML报文
	if (memcmp(buf,PACKAGE_VERSION_001,4) == 0)
	{
		UnionSetPackageType(PACKAGE_TYPE_V001);
		gunionIsNotConvertPackage = 1;
		return(0);
	}
	else if (memcmp(buf,"<?xml",5) == 0)
	{
		UnionSetPackageType(PACKAGE_TYPE_XML);
		gunionIsNotConvertPackage = 1;
		return(0);
	}
	else
	{
		UnionSetPackageType(PACKAGE_TYPE_V001);
		gunionIsNotConvertPackage = 0;
	}
	
	// 解压请求包
	if ((ret = UnionUnpackEsscRequestPackage((char *)buf,lenOfBuf,gunionOldAppID,gunionOldServiceCode)) < 0)
	{
		UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionUnpackEsscRequestPackage!\n");
		return(ret);
	}
	//add by zhouxw 20150826
	if(strcmp(gunionOldAppID, "KM") == 0)
	{
		strcpy(tmpOldServiceCode, gunionOldServiceCode);
		strcpy(gunionOldServiceCode, "KM");
		strcat(gunionOldServiceCode, tmpOldServiceCode);
		isKMSvr = 1;
		//snprintf(currServiceCode,sizeof(currServiceCode),"%s%s",gunionOldAppID, gunionOldServiceCode);
	}
	else	
	//add end
		snprintf(currServiceCode,sizeof(currServiceCode),"%s%d",gunionOldServiceCode,UnionGetVersionOfEsscPackage());
	if ((ptr = UnionFindTableValue("oldServiceCode", currServiceCode)) == NULL)
	{
		snprintf(sql,sizeof(sql),"select * from oldServiceCode where oldServiceCode = '%s' and version = %d",gunionOldServiceCode,UnionGetVersionOfEsscPackage());
		if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
		{
			UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionSelectRealDBRecord[%s]! ret = [%d]\n",sql,ret);
			return(ret);
		}
		if (ret == 0)
		{
			UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: oldServiceCode[%s] not found! sql = [%s]\n",gunionOldServiceCode,sql);
			return(errCodeInvalidService);
		}
			
		UnionLocateXMLPackage("detail", 1);
		
		// 设置服务代码
		if ((ret = UnionReadXMLPackageValue("currServiceCode", currServiceCode, sizeof(currServiceCode))) < 0)
		{
			UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionReadXMLPackageValue[currServiceCode]!\n");
			return(ret);
		}
		currServiceCode[ret] = 0;
	}
	else
	{
		if (strlen(ptr) == 0)
		{
			UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: oldServiceCode_1�7 version [%s] not found!\n",currServiceCode);
			return(errCodeRecordNotExists);
		}

		if ((ret = UnionReadRecFldFromRecStr(ptr,strlen(ptr),"currServiceCode",currServiceCode,sizeof(currServiceCode))) < 0)
		{
			UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionReadRecFldFromRecStr[currServiceCode][%s]!\n",ptr);
			return(ret);
		}
		currServiceCode[ret] = 0;
	}
	
	// 设置请求报文头
	if ((ret = UnionInitRequestXMLPackage(NULL,NULL,0)) < 0)
	{
		UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionInitRequestXMLPackage!\n");
		return(ret);
	}
		
	if ((ret = UnionSetRequestXMLPackageValue("head/serviceCode",currServiceCode)) < 0)
	{
		UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionSetRequestXMLPackageValue [head/serviceCode][%s]!\n",currServiceCode);
		return(ret);
	}
	if (gunionIsUnionPayPackage)
	{
		// 设置系统ID
		if ((ret = UnionSetRequestXMLPackageValue("head/sysID","unionPay")) < 0)
		{
			UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionSetRequestXMLPackageValue [head/sysID][unionPay]!\n");
			return(ret);
		}
		// 设置应用ID
		if ((ret = UnionSetRequestXMLPackageValue("head/appID","unionPay")) < 0)
		{
			UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionSetRequestXMLPackageValue [head/appID][unionPay]!\n");
			return(ret);
		}

		// 获取加密机组号   
		if ((ret = UnionReadHsmGroupIDListByAppID("unionPay",hsmGroupIDList,sizeof(hsmGroupIDList))) < 0)
		{
			UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionReadHsmGroupIDListByAppID[unionPay]!\n");
			return(ret);
		}
		hsmGroupIDList[ret] = 0;
	}
	else
	{
		// 设置系统ID
		if ((ret = UnionSetRequestXMLPackageValue("head/sysID",gunionOldAppID)) < 0)
		{
			UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionSetRequestXMLPackageValue [head/sysID][%s]!\n",gunionOldAppID);
			return(ret);
		}
		// 设置应用ID
		if ((ret = UnionSetRequestXMLPackageValue("head/appID",gunionOldAppID)) < 0)
		{
			UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionSetRequestXMLPackageValue [head/appID][%s]!\n",gunionOldAppID);
			return(ret);
		}

		// 获取加密机组号   
		if ((ret = UnionReadHsmGroupIDListByAppID(gunionOldAppID,hsmGroupIDList,sizeof(hsmGroupIDList))) < 0)
		{
			UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionReadHsmGroupIDListByAppID[%s]!\n",gunionOldAppID);
			return(ret);
		}
		hsmGroupIDList[ret] = 0;
	}

	// 设置IP地址
	if ((ret = UnionSetRequestXMLPackageValue("head/clientIPAddr",cliIPAddr)) < 0)
	{
		UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionSetRequestXMLPackageValue [head/cliIPAddr][%s]!\n",cliIPAddr);
		return(ret);
	}
	// 设置交易时间
	if ((ret = UnionSetRequestXMLPackageValue("head/transTime",UnionGetCurrentFullSystemDateTime())) < 0)
	{
		UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionSetRequestXMLPackageValue [head/transTime][%s]!\n",UnionGetCurrentFullSystemDateTime());
		return(ret);
	}
	// 设置交易标识
	if ((ret = UnionSetRequestXMLPackageValue("head/transFlag","1")) < 0)
	{
		UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionSetRequestXMLPackageValue [head/transFlag][%s]!\n","1");
		return(ret);
	}
	// 设置报文体
	/*
	//add by zhouxw 20150826
	if(isKMSvr)
	{
		if((ret = UnionSetKMSvrRequestPackage(gunionOldServiceCode)) < 0)
		{
			UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionSetKMSvrRequestPackage serviceCode[%s]!\n", gunionOldServiceCode);
			return(ret);
		}
		return(1);
	}
	*/
	//add end
	if ((totalNum = getOldServiceFiledREC(gunionOldServiceCode, UnionGetVersionOfEsscPackage(),1,&fieldTBL)) < 0)
	{
		UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: getOldServiceFiledREC ret = [%d]!\n",totalNum);
		return(totalNum);
	}
	else if (totalNum == 0)
		return(0);
	
	for (i = 0; i < totalNum; i++)
	{
		// 1=固定值,2=取报文值,3=取报文值并扩展,4=取报文值并压缩
		switch(fieldTBL.rec[i].method)
		{
			case	convertMethodOfReadFixedValue:	// 固定值
				strcpy(fieldValue,fieldTBL.rec[i].parameter);
				break;	
			case	convertMethodOfReadPackageValue:	// 取报文值
				if ((ret = UnionReadEsscRequestPackageFld(atoi(fieldTBL.rec[i].parameter),fieldValue,sizeof(fieldValue))) < 0)
				{
					if (ret != errCodeEsscMDL_EsscPackageFldNotFound)
					{
						UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionReadEsscRequestPackageFld [%d][%s]!\n",i,fieldTBL.rec[i].parameter);
						return(ret);
					}
					else
						continue;
				}
				fieldValue[ret] = 0;
				// add by leipp 20150806
				if (ret != strlen(fieldValue))
				{
					lenOfBitData = ret;
					isBitData = 1;
				}

				// 黄河银行需要传送数据类型,来区分二进制与十六进制
				if (atoi(fieldTBL.rec[i].parameter) == 204)
				{
					if (((memcmp(currServiceCode,"E150",4) == 0) || (memcmp(currServiceCode,"E151",4) == 0)))
					{
						ret = atoi(fieldValue);	
						snprintf(fieldValue,sizeof(fieldValue),"%d",ret+1);
					}
				}
				// add by leipp end
				break;
			case	convertMethodOfReadPackageValueAndExpand:	// 取报文值并扩展
				if ((ret = UnionReadEsscRequestPackageFld(atoi(fieldTBL.rec[i].parameter),tmpBuf,sizeof(tmpBuf))) < 0)
				{
					if (ret != errCodeEsscMDL_EsscPackageFldNotFound)
					{
						UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionReadEsscRequestPackageFld [%d][%s]!\n",i,fieldTBL.rec[i].parameter);
						return(ret);
					}
					else
						continue;
				}
				tmpBuf[ret] = 0;
				bcdhex_to_aschex(tmpBuf,ret,fieldValue);
				fieldValue[ret*2] = 0;
				break;
			case	convertMethodOfReadPackageValueAndCompress:	// 取报文值并压缩
				if ((ret = UnionReadEsscRequestPackageFld(atoi(fieldTBL.rec[i].parameter),tmpBuf,sizeof(tmpBuf))) < 0)
				{
					if (ret != errCodeEsscMDL_EsscPackageFldNotFound)
					{
						UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionReadEsscRequestPackageFld [%d][%s]!\n",i,fieldTBL.rec[i].parameter);
						return(ret);
					}
					else
						continue;
				}
				tmpBuf[ret] = 0;
				aschex_to_bcdhex(tmpBuf,ret,fieldValue);
				fieldValue[ret/2] = 0;	// modify by leipp 20160201	由ret*2 -> ret/2
				break;
			case	convertMethodOfReadZMKNameBySameAppNoAndNode:	// 读取同应用和属主的ZMK密钥名称
				if ((ret = UnionReadEsscRequestPackageFld(atoi(fieldTBL.rec[i].parameter),tmpBuf,sizeof(tmpBuf))) < 0)
				{
					if (ret != errCodeEsscMDL_EsscPackageFldNotFound)
					{
						UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionReadEsscRequestPackageFld [%d][%s]!\n",i,fieldTBL.rec[i].parameter);
						return(ret);
					}
					else
						continue;
				}
				tmpBuf[ret] = 0;
				if((ptr = strrchr(tmpBuf, '.')) == NULL)
				{
					UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: keyName [%s] illegal!\n",tmpBuf);
					return(errCodeParameter);
				}
				*ptr = 0;
				// 20150529 zhangyd 修改
				if ((*(ptr + 1) >= 'A') && (*(ptr + 1) <= 'Z'))
					snprintf(fieldValue,sizeof(fieldValue),"%s.ZMK", tmpBuf);
				else
					snprintf(fieldValue,sizeof(fieldValue),"%s.zmk", tmpBuf);
				break;
			case	convertMethodOfCheckFieldIsExist:	// 判断域是否存在，如果存在赋值1，不存在赋值0
				if ((ret = UnionReadEsscRequestPackageFld(atoi(fieldTBL.rec[i].parameter),tmpBuf,sizeof(tmpBuf))) < 0)
					snprintf(fieldValue,sizeof(fieldValue),"0");
				else
					snprintf(fieldValue,sizeof(fieldValue),"1");
				break;
			case	convertMethodOfReadKeyNameByHsmGroupIDAndIndex:	//以XXX是密码机组号，YY是私钥在密码机中的存储索引，取公钥名称：GrpXXX.vkIndeYY.pk；
				if ((ret = UnionReadEsscRequestPackageFld(atoi(fieldTBL.rec[i].parameter),tmpBuf,sizeof(tmpBuf))) < 0)
                                {
                                        if (ret != errCodeEsscMDL_EsscPackageFldNotFound)
                                        {
                                                UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionReadEsscRequestPackageFld [%d][%s]!\n",i,fieldTBL.rec[i].parameter);
                                                return(ret);
                                        }
					else
						continue;
                                }
				tmpBuf[ret] = 0;
				if(strstr(hsmGroupIDList, ","))
				{
					 UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: Have many hsmGroupID [%s]!\n",hsmGroupIDList);
                                         return(ret);
				}
                                snprintf(fieldValue,sizeof(fieldValue),"Grp%s.vkInde%s.pk",hsmGroupIDList, tmpBuf);
                                break;
			case	convertMethodOfReadKeyNameByAppNoAndHsmGroupID:	//以idOfApp是应用编号，获取XXX是密码机组号。 取公钥名称为：idOfApp.hsmGrpXXX.pk；
				if ((ret = UnionReadEsscRequestPackageFld(atoi(fieldTBL.rec[i].parameter),tmpBuf,sizeof(tmpBuf))) < 0)
                                {
                                        if (ret != errCodeEsscMDL_EsscPackageFldNotFound)
                                        {
                                                UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionReadEsscRequestPackageFld [%d][%s]!\n",i,fieldTBL.rec[i].parameter);
                                                return(ret);
                                        }
					else
						continue;
                                }
                                tmpBuf[ret] = 0;
				// 获取加密机组号   
				if ((ret = UnionReadHsmGroupIDListByAppID(tmpBuf,hsmGroupIDList,sizeof(hsmGroupIDList))) < 0)
				{
					UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionReadHsmGroupIDListByAppID[%s]!\n",tmpBuf);
					return(ret);
				}
				hsmGroupIDList[ret] = 0;
				if(strstr(hsmGroupIDList, ","))
                                {
                                         UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: Have many hsmGroupID [%s]!\n",hsmGroupIDList);
                                         return(ret);
                                }
				snprintf(fieldValue,sizeof(fieldValue),"%s.hsmGrp%s.pk", tmpBuf,hsmGroupIDList);
				break;
			case	convertMethodOfReadKeyNameByAppNo:	//以appName是应用编号，取公钥名称：appName.pkcs10.pk
				if ((ret = UnionReadEsscRequestPackageFld(atoi(fieldTBL.rec[i].parameter),tmpBuf,sizeof(tmpBuf))) < 0)
                                {
                                        if (ret != errCodeEsscMDL_EsscPackageFldNotFound)
                                        {
                                                UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionReadEsscRequestPackageFld [%d][%s]!\n",i,fieldTBL.rec[i].parameter);
                                                return(ret);
                                        }
					else
						continue;
                                }
                                tmpBuf[ret] = 0;
                                snprintf(fieldValue,sizeof(fieldValue),"%s.pkcs10.pk", tmpBuf);
				break;
			case	convertMethodOfReadPackageValueByRequest:	// 根据请求报文判断性读取
				if ((ret = UnionReadEsscRequestPackageFld(atoi(fieldTBL.rec[i].parameter),fieldValue,sizeof(fieldValue))) < 0)
				{
					if (ret == errCodeEsscMDL_EsscPackageFldNotFound)
					{
						UnionLog("in UnionConvertOtherPackageToXMLPackage:: UnionReadEsscRequestPackageFld [%d][%s]!\n",i,fieldTBL.rec[i].parameter);
						continue;
					}
				}
				fieldValue[ret] = 0;
                                break;
			//add by zhouxw 20150909
			case	convertMethodOfReadFourBytesOfPackageValue:	//读取日期
				if((ret = UnionReadEsscRequestPackageFld(atoi(fieldTBL.rec[i].parameter), tmpBuf, sizeof(tmpBuf))) < 0)
				{
					if (ret != errCodeEsscMDL_EsscPackageFldNotFound)
					{
						UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionReadEsscRequestPackageFld [%d][%s]!\n",i,fieldTBL.rec[i].parameter);
						return(ret);
					}
					else
						continue;
				}
				tmpBuf[ret] = 0;
				memcpy(fieldValue, tmpBuf, 4);
				fieldValue[4] = 0;
				break;
			//end
			default:
				UnionLog("in UnionConvertOtherPackageToXMLPackage:: method[%d] error",fieldTBL.rec[i].method);
				strcpy(fieldValue,fieldTBL.rec[i].parameter);
				break;
		}
		
		snprintf(fieldName,sizeof(fieldName),"body/%s",fieldTBL.rec[i].fieldName);
		// modify by leipp 20150806, 如果送不可见字符，获取长度有可能失败，不能用strlen来读取
		if (isBitData)
		{
			isBitData = 0;
			if ((ret = UnionSetRequestXMLPackageBitValue(fieldName,lenOfBitData,fieldValue)) < 0)
			{
				UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionSetRequestXMLPackageBitValue [%d][%s]!\n",i,fieldTBL.rec[i].parameter);
				return(ret);
			}
		}
		else
		{
			if ((ret = UnionSetRequestXMLPackageValue(fieldName,fieldValue)) < 0)
			{
				UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionSetRequestXMLPackageValue [%d][%s]!\n",i,fieldTBL.rec[i].parameter);
				return(ret);
			}
		}
		// modify by leipp end
	}

	if (gunionIsUnionPayPackage)
	{
		if ((ret = UnionSetConvertPackageKeyName(gunionOldServiceCode,gunionOldAppID)) < 0)
		{
			UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionSetConvertPackageKeyName [%s][%s]!\n",gunionOldAppID,gunionOldServiceCode);
			return(ret);
		}
	}
	
	return(1);
}

// 当前ESSC报文转成旧ESSC报文
int UnionConvertXMLPackageToOtherPackage(unsigned char *buf,int sizeOfBuf)
{
	int	i;
	int	ret;
	int	lenOfValue = 0;
	int	totalNum = 0;
	int	errCode = 0;
	char	tmpBuf[4112];
//	char	parameter[128];
	char	xmlFieldName[64];
	char	fieldValue[4096];
	char	responseCode[32];
	char	responseRemark[128];
	char	*ptr = NULL;
	TUnionOldServiceFieldTBL	fieldTBL;
	
	// 是XML报文
	if (gunionIsNotConvertPackage)
		return(0);

	UnionLocateResponseXMLPackage("",0);
	
	// 读取响应码
	if ((ret = UnionReadResponseXMLPackageValue("head/responseCode",responseCode,sizeof(responseCode))) < 0)
	{
		UnionUserErrLog("in UnionConvertXMLPackageToOtherPackage:: UnionReadResponseXMLPackageValue[head/responseCode]!\n");
		UnionLogResponseXMLPackage();
		return(ret);
	}
	responseCode[ret] = 0;
	errCode = 0 - atoi(responseCode);
	
	// 读取响应描述
	if ((ret = UnionReadResponseXMLPackageValue("head/responseRemark",responseRemark,sizeof(responseRemark))) < 0)
	{
		UnionUserErrLog("in UnionConvertXMLPackageToOtherPackage:: UnionReadResponseXMLPackageValue[head/responseRemark]!\n");
		return(ret);
	}
	responseRemark[ret] = 0;
	
	UnionInitEsscResponsePackage();
	
	// 执行错误
	if (errCode < 0)
	{
		UnionSetEsscResponsePackageFld(conEsscFldErrorCodeRemark,strlen(responseRemark),responseRemark);
	}
	else
	{
		if ((totalNum = getOldServiceFiledREC(gunionOldServiceCode, UnionGetVersionOfEsscPackage(),0,&fieldTBL)) < 0)
		{
			UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: getOldServiceFiledREC ret = [%d]!\n",totalNum);
			return(totalNum);
		}

		if (totalNum > 0)
		{
			for (i = 0; i < totalNum; i++)
			{			
				snprintf(xmlFieldName,sizeof(xmlFieldName),"body/%s",fieldTBL.rec[i].parameter);
				
				// 1=固定值,2=取报文值,3=取报文值并扩展,4=取报文值并压缩
				switch(fieldTBL.rec[i].method)
				{
					case	convertMethodOfReadFixedValue:	// 固定值
						lenOfValue = snprintf(fieldValue,sizeof(fieldValue),"%s",fieldTBL.rec[i].parameter);
						break;	
					case	convertMethodOfReadPackageValue:	// 取报文值
						if ((lenOfValue = UnionReadResponseXMLPackageValue(xmlFieldName,fieldValue,sizeof(fieldValue))) < 0)
						{
							//UnionUserErrLog("in UnionConvertXMLPackageToOtherPackage:: UnionReadResponseXMLPackageValue [%s][%s]!\n",fieldName,parameter);
							//return(lenOfValue);
							//break;
							continue;
						}
						break;
					case	convertMethodOfReadPackageValueAndExpand:	// 取报文值并扩展
						if ((ret = UnionReadResponseXMLPackageValue(xmlFieldName,tmpBuf,sizeof(tmpBuf))) < 0)
						{
							//UnionUserErrLog("in UnionConvertXMLPackageToOtherPackage:: UnionReadResponseXMLPackageValue [%s][%s]!\n",fieldName,parameter);
							//return(ret);
							break;
						}
						tmpBuf[ret] = 0;
						lenOfValue = bcdhex_to_aschex(tmpBuf,ret,fieldValue);
						break;
					case	convertMethodOfReadPackageValueAndCompress:	// 取报文值并压缩
						if ((ret = UnionReadResponseXMLPackageValue(xmlFieldName,tmpBuf,sizeof(tmpBuf))) < 0)
						{
							//UnionUserErrLog("in UnionConvertXMLPackageToOtherPackage:: UnionReadResponseXMLPackageValue [%s][%s]!\n",fieldName,parameter);
							//return(ret);
							break;
						}
						tmpBuf[ret] = 0;
						lenOfValue = aschex_to_bcdhex(tmpBuf,ret,fieldValue);
						fieldValue[lenOfValue] = 0;
						break;
					case	convertMethodOfReadZMKNameBySameAppNoAndNode:	// 读取同应用和属主的ZMK密钥名称
						if ((ret = UnionReadResponseXMLPackageValue(xmlFieldName,tmpBuf,sizeof(tmpBuf))) < 0)
						{
							//UnionUserErrLog("in UnionConvertXMLPackageToOtherPackage:: UnionReadResponseXMLPackageValue [%s][%s]!\n",fieldName,parameter);
							//return(ret);
							break;
						}
						tmpBuf[ret] = 0;
						if((ptr = strchr(tmpBuf, '.')) == NULL || (ptr + 1) == NULL || (ptr = strchr(ptr + 1, '.')) == NULL)
						{
							UnionUserErrLog("in UnionConvertXMLPackageToOtherPackage:: keyName [%s] illegal!\n",tmpBuf);
							return(errCodeParameter);
								
						}
						*ptr = 0;
						// 20150529 zhangyd 修改
						if ((*(ptr + 1) >= 'A') && (*(ptr + 1) <= 'Z'))
							lenOfValue = snprintf(fieldValue,sizeof(fieldValue),"%s.ZMK", tmpBuf);
						else
							lenOfValue = snprintf(fieldValue,sizeof(fieldValue),"%s.zmk", tmpBuf);
						break;
					case	convertMethodOfCheckFieldIsExist:	// 判断域是否存在，如果存在赋值1，不存在赋值0
						if ((ret = UnionReadResponseXMLPackageValue(xmlFieldName,tmpBuf,sizeof(tmpBuf))) < 0)
							lenOfValue = snprintf(fieldValue,sizeof(fieldValue),"0");
						else
							lenOfValue = snprintf(fieldValue,sizeof(fieldValue),"1");
						break;
					case	convertMethodOfReadPackageValueByRequest:
						if ((lenOfValue = UnionReadResponseXMLPackageValue(xmlFieldName,fieldValue,sizeof(fieldValue))) < 0)
						{
							continue;
						}
						break;
					case	convertMethodOfReadPackageResponseDataLen:
						if ((lenOfValue = UnionReadResponseXMLPackageValue(xmlFieldName,fieldValue,sizeof(fieldValue))) < 0)
							continue;
						lenOfValue = snprintf(fieldValue,sizeof(fieldValue),"%04d",lenOfValue);
						break;
					default:
						UnionLog("in UnionConvertXMLPackageToOtherPackage:: method[%d] error",fieldTBL.rec[i].method);
						lenOfValue = snprintf(fieldValue,sizeof(fieldValue),"%s",fieldTBL.rec[i].parameter);
						break;	
				}
				
				if ((ret = UnionSetEsscResponsePackageFld(atoi(fieldTBL.rec[i].fieldName),lenOfValue,fieldValue)) < 0)
				{
					UnionUserErrLog("in UnionConvertXMLPackageToOtherPackage:: UnionSetEsscResponsePackageFld i[%d][%s][%d][%s]!\n",i,fieldTBL.rec[i].fieldName,lenOfValue,fieldValue);
					return(ret);
				}
			}
		}
	}
	if(isKMSvr)
	{
		strcpy(tmpBuf, gunionOldServiceCode+2);
		strcpy(gunionOldServiceCode, tmpBuf);
		//add by linxj 20151028
		isKMSvr = 0; //此为全局变量，下次不设回为的话，造成后面使用此进程的交易报错
		//add end 20151028
	}


	if ((ret = UnionPackEsscResponsePackage(gunionOldAppID,gunionOldServiceCode,errCode,(char *)buf,sizeOfBuf)) < 0)
	{
		UnionUserErrLog("in UnionConvertXMLPackageToOtherPackage:: UnionPackEsscResponsePackage!\n");
		return(ret);
	}
	buf[ret] = 0;

	return(ret);
}
