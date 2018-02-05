//	Author:		ÕÅÓÀ¶¨
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
	
	// ¶ÁÈ¡¼ÇÂ¼ÊýÁ¿
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
	
	// ¶ÁÈ¡¼ÇÂ¼ÊýÁ¿
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

// ¾ÉESSC±¨ÎÄ×ª³Éµ±Ç°ESSC±¨ÎÄ
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
	char	hsmGroupIDList[32];//¼ÓÃÜ»ú×éºÅ
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

	// ÊÇXML±¨ÎÄ
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
	
	// ½âÑ¹ÇëÇó°ü
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
		
		// ÉèÖÃ·þÎñ´úÂë
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
			UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: oldServiceCode_1¤7 version [%s] not found!\n",currServiceCode);
			return(errCodeRecordNotExists);
		}

		if ((ret = UnionReadRecFldFromRecStr(ptr,strlen(ptr),"currServiceCode",currServiceCode,sizeof(currServiceCode))) < 0)
		{
			UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionReadRecFldFromRecStr[currServiceCode][%s]!\n",ptr);
			return(ret);
		}
		currServiceCode[ret] = 0;
	}
	
	// ÉèÖÃÇëÇó±¨ÎÄÍ·
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
		// ÉèÖÃÏµÍ³ID
		if ((ret = UnionSetRequestXMLPackageValue("head/sysID","unionPay")) < 0)
		{
			UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionSetRequestXMLPackageValue [head/sysID][unionPay]!\n");
			return(ret);
		}
		// ÉèÖÃÓ¦ÓÃID
		if ((ret = UnionSetRequestXMLPackageValue("head/appID","unionPay")) < 0)
		{
			UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionSetRequestXMLPackageValue [head/appID][unionPay]!\n");
			return(ret);
		}

		// »ñÈ¡¼ÓÃÜ»ú×éºÅ   
		if ((ret = UnionReadHsmGroupIDListByAppID("unionPay",hsmGroupIDList,sizeof(hsmGroupIDList))) < 0)
		{
			UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionReadHsmGroupIDListByAppID[unionPay]!\n");
			return(ret);
		}
		hsmGroupIDList[ret] = 0;
	}
	else
	{
		// ÉèÖÃÏµÍ³ID
		if ((ret = UnionSetRequestXMLPackageValue("head/sysID",gunionOldAppID)) < 0)
		{
			UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionSetRequestXMLPackageValue [head/sysID][%s]!\n",gunionOldAppID);
			return(ret);
		}
		// ÉèÖÃÓ¦ÓÃID
		if ((ret = UnionSetRequestXMLPackageValue("head/appID",gunionOldAppID)) < 0)
		{
			UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionSetRequestXMLPackageValue [head/appID][%s]!\n",gunionOldAppID);
			return(ret);
		}

		// »ñÈ¡¼ÓÃÜ»ú×éºÅ   
		if ((ret = UnionReadHsmGroupIDListByAppID(gunionOldAppID,hsmGroupIDList,sizeof(hsmGroupIDList))) < 0)
		{
			UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionReadHsmGroupIDListByAppID[%s]!\n",gunionOldAppID);
			return(ret);
		}
		hsmGroupIDList[ret] = 0;
	}

	// ÉèÖÃIPµØÖ·
	if ((ret = UnionSetRequestXMLPackageValue("head/clientIPAddr",cliIPAddr)) < 0)
	{
		UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionSetRequestXMLPackageValue [head/cliIPAddr][%s]!\n",cliIPAddr);
		return(ret);
	}
	// ÉèÖÃ½»Ò×Ê±¼ä
	if ((ret = UnionSetRequestXMLPackageValue("head/transTime",UnionGetCurrentFullSystemDateTime())) < 0)
	{
		UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionSetRequestXMLPackageValue [head/transTime][%s]!\n",UnionGetCurrentFullSystemDateTime());
		return(ret);
	}
	// ÉèÖÃ½»Ò×±êÊ¶
	if ((ret = UnionSetRequestXMLPackageValue("head/transFlag","1")) < 0)
	{
		UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionSetRequestXMLPackageValue [head/transFlag][%s]!\n","1");
		return(ret);
	}
	// ÉèÖÃ±¨ÎÄÌå
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
		// 1=¹Ì¶¨Öµ,2=È¡±¨ÎÄÖµ,3=È¡±¨ÎÄÖµ²¢À©Õ¹,4=È¡±¨ÎÄÖµ²¢Ñ¹Ëõ
		switch(fieldTBL.rec[i].method)
		{
			case	convertMethodOfReadFixedValue:	// ¹Ì¶¨Öµ
				strcpy(fieldValue,fieldTBL.rec[i].parameter);
				break;	
			case	convertMethodOfReadPackageValue:	// È¡±¨ÎÄÖµ
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

				// »ÆºÓÒøÐÐÐèÒª´«ËÍÊý¾ÝÀàÐÍ,À´Çø·Ö¶þ½øÖÆÓëÊ®Áù½øÖÆ
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
			case	convertMethodOfReadPackageValueAndExpand:	// È¡±¨ÎÄÖµ²¢À©Õ¹
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
			case	convertMethodOfReadPackageValueAndCompress:	// È¡±¨ÎÄÖµ²¢Ñ¹Ëõ
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
				fieldValue[ret/2] = 0;	// modify by leipp 20160201	ÓÉret*2 -> ret/2
				break;
			case	convertMethodOfReadZMKNameBySameAppNoAndNode:	// ¶ÁÈ¡Í¬Ó¦ÓÃºÍÊôÖ÷µÄZMKÃÜÔ¿Ãû³Æ
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
				// 20150529 zhangyd ÐÞ¸Ä
				if ((*(ptr + 1) >= 'A') && (*(ptr + 1) <= 'Z'))
					snprintf(fieldValue,sizeof(fieldValue),"%s.ZMK", tmpBuf);
				else
					snprintf(fieldValue,sizeof(fieldValue),"%s.zmk", tmpBuf);
				break;
			case	convertMethodOfCheckFieldIsExist:	// ÅÐ¶ÏÓòÊÇ·ñ´æÔÚ£¬Èç¹û´æÔÚ¸³Öµ1£¬²»´æÔÚ¸³Öµ0
				if ((ret = UnionReadEsscRequestPackageFld(atoi(fieldTBL.rec[i].parameter),tmpBuf,sizeof(tmpBuf))) < 0)
					snprintf(fieldValue,sizeof(fieldValue),"0");
				else
					snprintf(fieldValue,sizeof(fieldValue),"1");
				break;
			case	convertMethodOfReadKeyNameByHsmGroupIDAndIndex:	//ÒÔXXXÊÇÃÜÂë»ú×éºÅ£¬YYÊÇË½Ô¿ÔÚÃÜÂë»úÖÐµÄ´æ´¢Ë÷Òý£¬È¡¹«Ô¿Ãû³Æ£ºGrpXXX.vkIndeYY.pk£»
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
			case	convertMethodOfReadKeyNameByAppNoAndHsmGroupID:	//ÒÔidOfAppÊÇÓ¦ÓÃ±àºÅ£¬»ñÈ¡XXXÊÇÃÜÂë»ú×éºÅ¡£ È¡¹«Ô¿Ãû³ÆÎª£ºidOfApp.hsmGrpXXX.pk£»
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
				// »ñÈ¡¼ÓÃÜ»ú×éºÅ   
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
			case	convertMethodOfReadKeyNameByAppNo:	//ÒÔappNameÊÇÓ¦ÓÃ±àºÅ£¬È¡¹«Ô¿Ãû³Æ£ºappName.pkcs10.pk
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
			case	convertMethodOfReadPackageValueByRequest:	// ¸ù¾ÝÇëÇó±¨ÎÄÅÐ¶ÏÐÔ¶ÁÈ¡
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
			case	convertMethodOfReadFourBytesOfPackageValue:	//¶ÁÈ¡ÈÕÆÚ
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
		// modify by leipp 20150806, Èç¹ûËÍ²»¿É¼û×Ö·û£¬»ñÈ¡³¤¶ÈÓÐ¿ÉÄÜÊ§°Ü£¬²»ÄÜÓÃstrlenÀ´¶ÁÈ¡
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

// µ±Ç°ESSC±¨ÎÄ×ª³É¾ÉESSC±¨ÎÄ
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
	
	// ÊÇXML±¨ÎÄ
	if (gunionIsNotConvertPackage)
		return(0);

	UnionLocateResponseXMLPackage("",0);
	
	// ¶ÁÈ¡ÏìÓ¦Âë
	if ((ret = UnionReadResponseXMLPackageValue("head/responseCode",responseCode,sizeof(responseCode))) < 0)
	{
		UnionUserErrLog("in UnionConvertXMLPackageToOtherPackage:: UnionReadResponseXMLPackageValue[head/responseCode]!\n");
		UnionLogResponseXMLPackage();
		return(ret);
	}
	responseCode[ret] = 0;
	errCode = 0 - atoi(responseCode);
	
	// ¶ÁÈ¡ÏìÓ¦ÃèÊö
	if ((ret = UnionReadResponseXMLPackageValue("head/responseRemark",responseRemark,sizeof(responseRemark))) < 0)
	{
		UnionUserErrLog("in UnionConvertXMLPackageToOtherPackage:: UnionReadResponseXMLPackageValue[head/responseRemark]!\n");
		return(ret);
	}
	responseRemark[ret] = 0;
	
	UnionInitEsscResponsePackage();
	
	// Ö´ÐÐ´íÎó
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
				
				// 1=¹Ì¶¨Öµ,2=È¡±¨ÎÄÖµ,3=È¡±¨ÎÄÖµ²¢À©Õ¹,4=È¡±¨ÎÄÖµ²¢Ñ¹Ëõ
				switch(fieldTBL.rec[i].method)
				{
					case	convertMethodOfReadFixedValue:	// ¹Ì¶¨Öµ
						lenOfValue = snprintf(fieldValue,sizeof(fieldValue),"%s",fieldTBL.rec[i].parameter);
						break;	
					case	convertMethodOfReadPackageValue:	// È¡±¨ÎÄÖµ
						if ((lenOfValue = UnionReadResponseXMLPackageValue(xmlFieldName,fieldValue,sizeof(fieldValue))) < 0)
						{
							//UnionUserErrLog("in UnionConvertXMLPackageToOtherPackage:: UnionReadResponseXMLPackageValue [%s][%s]!\n",fieldName,parameter);
							//return(lenOfValue);
							//break;
							continue;
						}
						break;
					case	convertMethodOfReadPackageValueAndExpand:	// È¡±¨ÎÄÖµ²¢À©Õ¹
						if ((ret = UnionReadResponseXMLPackageValue(xmlFieldName,tmpBuf,sizeof(tmpBuf))) < 0)
						{
							//UnionUserErrLog("in UnionConvertXMLPackageToOtherPackage:: UnionReadResponseXMLPackageValue [%s][%s]!\n",fieldName,parameter);
							//return(ret);
							break;
						}
						tmpBuf[ret] = 0;
						lenOfValue = bcdhex_to_aschex(tmpBuf,ret,fieldValue);
						break;
					case	convertMethodOfReadPackageValueAndCompress:	// È¡±¨ÎÄÖµ²¢Ñ¹Ëõ
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
					case	convertMethodOfReadZMKNameBySameAppNoAndNode:	// ¶ÁÈ¡Í¬Ó¦ÓÃºÍÊôÖ÷µÄZMKÃÜÔ¿Ãû³Æ
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
						// 20150529 zhangyd ÐÞ¸Ä
						if ((*(ptr + 1) >= 'A') && (*(ptr + 1) <= 'Z'))
							lenOfValue = snprintf(fieldValue,sizeof(fieldValue),"%s.ZMK", tmpBuf);
						else
							lenOfValue = snprintf(fieldValue,sizeof(fieldValue),"%s.zmk", tmpBuf);
						break;
					case	convertMethodOfCheckFieldIsExist:	// ÅÐ¶ÏÓòÊÇ·ñ´æÔÚ£¬Èç¹û´æÔÚ¸³Öµ1£¬²»´æÔÚ¸³Öµ0
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
		isKMSvr = 0; //´ËÎªÈ«¾Ö±äÁ¿£¬ÏÂ´Î²»Éè»ØÎªµÄ»°£¬Ôì³ÉºóÃæÊ¹ÓÃ´Ë½ø³ÌµÄ½»Ò×±¨´í
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
