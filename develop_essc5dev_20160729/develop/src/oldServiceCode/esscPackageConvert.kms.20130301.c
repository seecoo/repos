//	Author:		张永定
//	Copyright:	Union Tech. Guangzhou
//	Date:		2013-08-21

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <setjmp.h>

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
#include "UnionSocket.h"
#include "accessCheck.h"
#include "remoteKeyPlatform.h"
#include "esscKmsPackage.h"

// 以下是文件传输命令
#define conResCmdStartSendingData       200     // 开始发送文件数据
#define conResCmdSendingData            201     // 发送数据记录
#define conResCmdFinishSendingData      202     // 结束发送文件数据
#define conResCmdDownloadFile           203     // 下载数据文件
#define conResCmdUploadFile             204     // 上载数据文件
#define conResCmdReloadResFile          205     // 在服务器端重新加载资源文件
#define conResCmdQueryResStatus         206     // 查询资源的状态
#define conResCmdUnlockRes              207     // 解锁资源
#define conResCmdUploadFileOnCondition  208     // 条件上载数据文件
#define conResCmdCreateFileDir          209     // 创建文件目录

int		gunionIsNotConvertPackage = 1;
static int	gunionRemotePackageType = 4;
char		gunionOldAppID[32];
char		gunionOldServiceCode[32];
char		gunionPackageHead[128];

#define	SERVICE_FIELD_NUM_MAX	20
#define OLD_SERVICECODE_MAXNUM 	256

typedef struct
{
        char    fieldName[64];
        int     method;
        char    parameter[160];
}TUnionOldServiceFieldREC;
typedef TUnionOldServiceFieldREC        *PUnionOldServiceFieldREC;

typedef struct
{
        int     num;
        TUnionOldServiceFieldREC        rec[SERVICE_FIELD_NUM_MAX];
}TUnionOldServiceFieldTBL;
typedef TUnionOldServiceFieldTBL        *PUnionOldServiceFieldTBL;

typedef struct
{
	char	tellerNo[128];
	char	resID[8];
	char	serviceID[8];
	int	lenOfPackage;
	char	package[4096];
}TUnionOldKmsPackage;
typedef TUnionOldKmsPackage	*PUnionOldKmsPackage;

char	oldServiceFieldName[OLD_SERVICECODE_MAXNUM][SERVICE_FIELD_NUM_MAX*40+40];
int	current_oldServiceCode_num = 0;

TUnionOldKmsPackage	toldKmsReqPackage;
//TUnionOldKmsPackage	toldKmsResPackage;
TUnionOldKmsPackage	toldKmsRemoteReqPackage;
//TUnionOldKmsPackage	toldKmsRemoteResPackage;

jmp_buf	gunionRemotePlatformJmpEnv;
void	UnionDealRemotePlatformResponseTimeout();

int UnionGetVersionOfEsscPackage()
{
	return 4;
}

char *UnionGetTellerNo()
{
	return(toldKmsReqPackage.tellerNo);
}

int UnionGetResID()
{
	return(atoi(toldKmsReqPackage.resID));
}
static int getOldServiceFieldName(char *oldService)
{
	int	i;
	int	ret;
	int	totalNum = 0;
	char	sql[128];
	char	tmpBuf[32];
	char	fieldName[42];
	char	data[1024];
	int	len = 0;
	int	oldServiceCode = 0;
	int	tmpOldServiceCode = 0;
	int	isNewService = 0;
	int	isNewFlag = 0;
	int	version = 4;	// modify by leipp 20160218
	//int	tmpVersion = 0;
	int	flag = 0;
	int	tmpFlag = 0;

	// modify by leipp 20160218,只保存版本号为4的数据
	if (oldService != NULL && strlen(oldService) > 0)
		snprintf(sql,sizeof(sql),"select oldServiceCode,version,flag,fieldName from oldServiceFiled where oldServiceCode = '%s' and version = %d order by flag,seqNo",oldService,version);
	else
		snprintf(sql,sizeof(sql),"select oldServiceCode,version,flag,fieldName from oldServiceFiled where version = %d order by oldServiceCode,flag,seqNo",version);
	// modify end 20160218

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
	tmpBuf[ret] = 0;
	totalNum = atoi(tmpBuf);

	for (i = 0; i < totalNum; i++)
	{
		if ((ret = UnionLocateXMLPackage("detail", i+1)) < 0)
		{
			UnionUserErrLog("in getOldServiceFieldName:: UnionLocateXMLPackage!\n");
			return(ret);
		}

		if ((ret = UnionReadXMLPackageValue("oldServiceCode", tmpBuf, sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in getOldServiceFieldName:: UnionReadXMLPackageValue[%d][%s]!\n",i,"oldServiceCode");
			return(ret);
		}
		tmpBuf[ret] = 0;
		oldServiceCode = atoi(tmpBuf);
		if (oldServiceCode != tmpOldServiceCode)
		{
			isNewService = 1;
			tmpOldServiceCode = oldServiceCode;
		}
		else
			isNewService = 0;

		if ((ret = UnionReadXMLPackageValue("flag", tmpBuf, sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in getOldServiceFieldName:: UnionReadXMLPackageValue[%d][%s]!\n",i,"");
			return(ret);
		}
		tmpBuf[ret] = 0;
		flag = atoi(tmpBuf);
		if (flag == 1 && flag != tmpFlag && isNewService == 0)
		{
			tmpFlag = flag;
			isNewFlag = 1;
		}
		else
			isNewFlag = 0;

		if ((ret = UnionReadXMLPackageValue("fieldName", fieldName, sizeof(fieldName))) < 0)
		{
			UnionUserErrLog("in getOldServiceFieldName:: UnionReadXMLPackageValue[%d][%s]!\n",i,"fieldName");
			return(ret);
		}
		fieldName[ret] = 0;

		if (isNewService)
		{	
			if (len > 0)
			{
				if (current_oldServiceCode_num >= OLD_SERVICECODE_MAXNUM)
				{
					UnionUserErrLog("in getOldServiceFieldName:: current_oldServiceCode_num[%d] > OLD_SERVICECODE_MAXNUM[%d]!\n",current_oldServiceCode_num+1,OLD_SERVICECODE_MAXNUM);
					return(errCodeSmallBuffer);
				}
				if (len >= sizeof(oldServiceFieldName[0]))
				{
					UnionUserErrLog("in getOldServiceFieldName:: len[%d] >= sizeof oldServiceFieldName[%zu]!\n",len,sizeof(oldServiceFieldName[0]));
					return(errCodeSmallBuffer);
				}
				memcpy(oldServiceFieldName[current_oldServiceCode_num],data,len);
				oldServiceFieldName[current_oldServiceCode_num][len] = 0;
				current_oldServiceCode_num++;
			}

			// modify by leipp 20160218,由%03d改为%06d,服务码是定义的6位.
			len = sprintf(data,"%06d%d%s|",oldServiceCode,version,fieldName);
			// modify end 20160218
			tmpFlag = flag;
		}
		else
		{
			if (1 == isNewFlag)
				len += sprintf(data+len,"=%s|",fieldName);
			else
				len += sprintf(data+len,"%s|",fieldName);
		}
	}

	if (len > 0)
	{
		if (current_oldServiceCode_num >= OLD_SERVICECODE_MAXNUM)
		{
			UnionUserErrLog("in getOldServiceFieldName:: current_oldServiceCode_num[%d] > OLD_SERVICECODE_MAXNUM[%d]!\n",current_oldServiceCode_num+1,OLD_SERVICECODE_MAXNUM);
			return(errCodeSmallBuffer);
		}
		if (len >= sizeof(oldServiceFieldName[0]))
		{
			UnionUserErrLog("in getOldServiceFieldName:: len[%d] >= sizeof oldServiceFieldName[%zu]!\n",len,sizeof(oldServiceFieldName[0]));
			return(errCodeSmallBuffer);
		}
		memcpy(oldServiceFieldName[current_oldServiceCode_num],data,len);
		oldServiceFieldName[current_oldServiceCode_num][len] = 0;
		current_oldServiceCode_num++;
	}

	UnionLog("in getOldServiceFieldName:: current_oldServiceCode_num[%d]!\n",current_oldServiceCode_num);
	return(current_oldServiceCode_num);
}

static int getOldServiceFiledREC(char *oldServiceCode, int version,int flag,PUnionOldServiceFieldTBL pfieldTBL)
{
	int	i;
	int	ret;
	int	totalNum = 0;
	char	sql[128];
	char	tmpBuf[32];
	char	data[16];
	int	len = 0;
	char	oldServiceField[1024];
	char	*precStr = NULL;
	char	fieldName[SERVICE_FIELD_NUM_MAX][128];
	int	fieldNum = 0;
	
	snprintf(data,sizeof(data),"%s%d",oldServiceCode,version);

	if (current_oldServiceCode_num == 0)
	{
		if ((ret = getOldServiceFieldName(NULL)) < 0)
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
		// modify by leipp 20160218,由4改为7,原来服务码是3位，加一位版本号是4位，后服务码为6位，加一个版本号为7位
		if (memcmp(oldServiceFieldName[i],data,7) == 0)
		{
			snprintf(oldServiceField,sizeof(oldServiceField),"%s",oldServiceFieldName[i]+7);
			UnionDebugLog("in getOldServiceFiledREC:: getOldServiceFieldName [%d] oldServiceCode[%s] oldServiceField[%s]!\n",i,oldServiceCode,oldServiceFieldName[i]);
			break;
		}
		// modify end 20160218
	}
	if (i == current_oldServiceCode_num)
	{
		// modify by leipp 20160218
		/*if ((ret = getOldServiceFieldName(oldServiceCode)) < 0)
		{
			UnionUserErrLog("in getOldServiceFiledREC:: getOldServiceFieldName oldServiceCode[%s]! ret = [%d]\n",oldServiceCode,ret);
			return(ret);
		}
		else if (ret == 0)
		{
			UnionLog("in getOldServiceFiledREC:: getOldServiceFieldName oldServiceCode[%s] not found!\n",oldServiceCode);
			return 0;
		}
		
		snprintf(oldServiceField,sizeof(oldServiceField),"%s",oldServiceFieldName[i]+7);
		*/
		
		UnionLog("in getOldServiceFiledREC:: getOldServiceFieldName oldServiceCode[%s] not found!\n",oldServiceCode);
		return 0;
		// modify end 20160218
	}

	if ((precStr = strchr(oldServiceField,'=')) != NULL)
	{
		if (flag == 1)
		{
			len = strlen(oldServiceField) - (precStr - oldServiceField) - 1;
			memmove(oldServiceField,precStr+1,len);
		}
		else
			len = precStr - oldServiceField;
		oldServiceField[len] = 0;
	}

	memset(fieldName,0,sizeof(fieldName));
        if ((fieldNum = UnionSeprateVarStrIntoVarGrp(oldServiceField,strlen(oldServiceField),'|',fieldName,20)) < 0)
        {
                UnionUserErrLog("in getOldServiceFiledREC:: UnionSeprateVarStrIntoVarGrp[%s]!\n",oldServiceField);
                return(fieldNum);
        }

	for (i = 0; i < fieldNum; i++)
	{
		snprintf(data,sizeof(data),"%s%d%d%s",oldServiceCode,version,flag,fieldName[i]);
		if ((precStr = UnionFindTableValue("oldServiceFiled", data)) == NULL)
		{
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
		}
		else
		{
			if (strlen(precStr) == 0)
			{
				UnionUserErrLog("in getOldServiceFiledREC:: oldServiceCode_version[%s] not found!\n",data);
				return(errCodeRecordNotExists);
			}

			snprintf(pfieldTBL->rec[i].fieldName,sizeof(pfieldTBL->rec[i].fieldName),"%s",fieldName[i]);
			
			if ((ret = UnionReadRecFldFromRecStr(precStr,strlen(precStr),"method",tmpBuf,sizeof(tmpBuf))) < 0)
			{
				UnionUserErrLog("in getOldServiceFiledREC:: UnionReadRecFldFromRecStr[method][%s]!\n",precStr);
				return(ret);
			}
			tmpBuf[ret] = 0;
			pfieldTBL->rec[i].method = atoi(tmpBuf);

			if ((ret = UnionReadRecFldFromRecStr(precStr,strlen(precStr),"parameter",pfieldTBL->rec[i].parameter,sizeof(pfieldTBL->rec[i].parameter))) < 0)
			{
				UnionUserErrLog("in getOldServiceFiledREC:: UnionReadRecFldFromRecStr[parameter][%s]!\n",precStr);
				return(ret);
			}
			pfieldTBL->rec[i].parameter[ret] = 0;
		}
	}

	return(fieldNum);
}

// 旧ESSC报文转成当前ESSC报文
int UnionConvertOtherPackageToXMLPackage(unsigned char *buf,int lenOfBuf,char *cliIPAddr)
{
	int	i;
	int	ret;
	int	totalNum = 0;
	char	sql[1024];
	char	oldServiceCode[32];
	char	currServiceCode[32];
	char	tmpBuf[4096];
	char	fieldName[64];
	char	fieldValue[4096];
	char	*ptr = NULL;
	char	hsmGroupIDList[32];//加密机组号
	int	offset = 0;
	int	lenOfPackage = 0;
	//char	resID[8];
	//char	operator[128];
	
	TUnionOldServiceFieldTBL	fieldTBL;

	memset(&toldKmsReqPackage,0,sizeof(TUnionOldKmsPackage));
	
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
		//UnionSetPackageType(PACKAGE_TYPE_V001);
		gunionIsNotConvertPackage = 0;
	}

	// 检查报文格式
	if ((offset = UnionIsCheckKMSPackage('1',(char *)buf,lenOfBuf,toldKmsReqPackage.tellerNo,gunionOldAppID,toldKmsReqPackage.resID,toldKmsReqPackage.serviceID)) < 0)
	{
		UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionIsCheckKMSPackage buf[%s]! ret = [%d]\n",buf,offset);
		return(offset);
	}
	if (offset >= sizeof(gunionPackageHead))
	{
		UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: package head is too long, buf[%d] >=  [%d]\n",offset,(int)sizeof(gunionPackageHead));
		return(errCodeSmallBuffer);
	}

	memcpy(gunionPackageHead,buf,offset);
	gunionPackageHead[offset] = 0;

	memcpy(tmpBuf,buf+offset,4);
	tmpBuf[4] = 0;
	lenOfPackage = atoi(tmpBuf);
	offset += 4;

	toldKmsReqPackage.lenOfPackage = lenOfPackage;
	if (lenOfPackage > 0)
	{
		if (lenOfPackage >= sizeof(toldKmsReqPackage.package))
		{
			UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: package is too long, buf[%d] >=  [%d]\n",offset,(int)sizeof(toldKmsReqPackage.package));
			return(errCodeSmallBuffer);
		}
		memcpy(toldKmsReqPackage.package,buf+offset,lenOfPackage);
		toldKmsReqPackage.package[lenOfPackage] = 0;
	}

	UnionNullLog("in UnionIsCheckKMSPackage:: flag              [1]!\n");
	UnionNullLog("in UnionIsCheckKMSPackage:: tellerNo          [%s]!\n",toldKmsReqPackage.tellerNo);
	UnionNullLog("in UnionIsCheckKMSPackage:: resID             [%s]!\n",toldKmsReqPackage.resID);
	UnionNullLog("in UnionIsCheckKMSPackage:: serviceCode       [%s]!\n",toldKmsReqPackage.serviceID);
	UnionNullLog("in UnionIsCheckKMSPackage:: lenOfParameter    [%04d]!\n",toldKmsReqPackage.lenOfPackage);
	UnionNullLog("in UnionIsCheckKMSPackage:: parameter         [%s]!\n",toldKmsReqPackage.package);

	snprintf(oldServiceCode,sizeof(oldServiceCode),"%s%s",toldKmsReqPackage.resID,toldKmsReqPackage.serviceID);
	snprintf(currServiceCode,sizeof(currServiceCode),"%s%d",oldServiceCode,UnionGetVersionOfEsscPackage());
	if ((ptr = UnionFindTableValue("oldServiceCode", currServiceCode)) == NULL)
	{
		snprintf(sql,sizeof(sql),"select * from oldServiceCode where oldServiceCode = '%s' and version = %d",oldServiceCode,UnionGetVersionOfEsscPackage());
		if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
		{
			UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionSelectRealDBRecord[%s]! ret = [%d]\n",sql,ret);
			return(ret);
		}
		if (ret == 0)
		{
			UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: oldServiceCode[%s] not found! sql = [%s]\n",oldServiceCode,sql);
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
			UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: oldServiceCode_� version [%s] not found!\n",currServiceCode);
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

	// 设置设置旧服务码
	if ((ret = UnionSetRequestXMLPackageValue("head/oldServiceCode",toldKmsReqPackage.serviceID)) < 0)
	{
		UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionSetRequestXMLPackageValue [head/oldServiceCode]!\n");
		return(ret);
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

	// 获取加密机组号   
	if ((ret = UnionReadHsmGroupIDListByAppID(gunionOldAppID,hsmGroupIDList,sizeof(hsmGroupIDList))) < 0)
	{
		UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionReadHsmGroupIDListByAppID[%s]!\n",gunionOldAppID);
		return(ret);
	}
	hsmGroupIDList[ret] = 0;

	// 设置报文体
	if ((totalNum = getOldServiceFiledREC(oldServiceCode, UnionGetVersionOfEsscPackage(),1,&fieldTBL)) < 0)
	{
		UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: getOldServiceFiledREC ret = [%d]!\n",totalNum);
		return(totalNum);
	}
	else if (totalNum == 0)
	{
		if (toldKmsReqPackage.lenOfPackage > 0)
		{
			if ((ret = UnionSetRequestXMLPackageValue("body/fldName",toldKmsReqPackage.package)) < 0)
			{
				UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionSetRequestXMLPackageValue [body/fldName]!\n");
				return(ret);
			}	
			return (1);
		}
		return(1);
	}
	
	// modify by leipp 20160218，修改(char *)buf+offset 为 toldKmsReqPackage.package
	for (i = 0; i < totalNum; i++)
	{
		// 1=固定值,2=取报文值,3=取报文值并扩展,4=取报文值并压缩
		switch(fieldTBL.rec[i].method)
		{
			case	convertMethodOfReadFixedValue:	// 固定值
				snprintf(fieldValue,sizeof(fieldValue),"%s",fieldTBL.rec[i].parameter);
				break;	
			case	convertMethodOfReadPackageValue:	// 取报文值
				//if ((ret = UnionReadRecFldFromRecStr((char *)buf+offset,lenOfPackage,fieldTBL.rec[i].parameter,fieldValue,sizeof(fieldValue))) < 0)
				if ((ret = UnionReadRecFldFromRecStr(toldKmsReqPackage.package,lenOfPackage,fieldTBL.rec[i].parameter,fieldValue,sizeof(fieldValue))) < 0)
				{
					if ((strchr(toldKmsReqPackage.package,'=') == NULL) && (toldKmsReqPackage.lenOfPackage > 0))
					{
						snprintf(fieldValue,sizeof(fieldValue),"%s",toldKmsReqPackage.package);
						break;
					}
					if (ret != errCodeEsscMDL_FldNotExists)
					{
						UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionReadRecFldFromRecStr [%d][%s]!\n",i,fieldTBL.rec[i].parameter);
						return(ret);
					}
					else
						continue;
				}
				fieldValue[ret] = 0;
				break;
			case	convertMethodOfReadPackageValueAndExpand:	// 取报文值并扩展
				//if ((ret = UnionReadRecFldFromRecStr((char *)buf+offset,lenOfPackage,fieldTBL.rec[i].parameter,tmpBuf,sizeof(tmpBuf))) < 0)
				if ((ret = UnionReadRecFldFromRecStr(toldKmsReqPackage.package,lenOfPackage,fieldTBL.rec[i].parameter,tmpBuf,sizeof(tmpBuf))) < 0)
				{
					if (ret != errCodeEsscMDL_FldNotExists)
					{
						UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionReadRecFldFromRecStr [%d][%s]!\n",i,fieldTBL.rec[i].parameter);
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
				//if ((ret = UnionReadRecFldFromRecStr((char *)buf+offset,lenOfPackage,fieldTBL.rec[i].parameter,tmpBuf,sizeof(tmpBuf))) < 0)
				if ((ret = UnionReadRecFldFromRecStr(toldKmsReqPackage.package,lenOfPackage,fieldTBL.rec[i].parameter,tmpBuf,sizeof(tmpBuf))) < 0)
				{
					if (ret != errCodeEsscMDL_FldNotExists)
					{
						UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionReadRecFldFromRecStr [%d][%s]!\n",i,fieldTBL.rec[i].parameter);
						return(ret);
					}
					else
						continue;
				}
				tmpBuf[ret] = 0;
				aschex_to_bcdhex(tmpBuf,ret,fieldValue);
				fieldValue[ret*2] = 0;
				break;
			case	convertMethodOfReadZMKNameBySameAppNoAndNode:	// 读取同应用和属主的ZMK密钥名称
				//if ((ret = UnionReadRecFldFromRecStr((char *)buf+offset,lenOfPackage,fieldTBL.rec[i].parameter,tmpBuf,sizeof(tmpBuf))) < 0)
				if ((ret = UnionReadRecFldFromRecStr(toldKmsReqPackage.package,lenOfPackage,fieldTBL.rec[i].parameter,tmpBuf,sizeof(tmpBuf))) < 0)
				{
					if (ret != errCodeEsscMDL_FldNotExists)
					{
						UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionReadRecFldFromRecStr [%d][%s]!\n",i,fieldTBL.rec[i].parameter);
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
				ret = sprintf(fieldValue,"%s.zmk", tmpBuf);
				fieldValue[ret] = 0;
				break;
			case	convertMethodOfCheckFieldIsExist:	// 判断域是否存在，如果存在赋值1，不存在赋值0
				//if ((ret = UnionReadRecFldFromRecStr((char *)buf+offset,lenOfPackage,fieldTBL.rec[i].parameter,tmpBuf,sizeof(tmpBuf))) < 0)
				if ((ret = UnionReadRecFldFromRecStr(toldKmsReqPackage.package,lenOfPackage,fieldTBL.rec[i].parameter,tmpBuf,sizeof(tmpBuf))) < 0)
					ret = sprintf(fieldValue,"0");
				else
					ret = sprintf(fieldValue,"1");
				fieldValue[ret] = 0;
				break;
			case	convertMethodOfReadKeyNameByHsmGroupIDAndIndex:	//以XXX是密码机组号，YY是私钥在密码机中的存储索引，取公钥名称：GrpXXX.vkIndeYY.pk；
				//if ((ret = UnionReadRecFldFromRecStr((char *)buf+offset,lenOfPackage,fieldTBL.rec[i].parameter,tmpBuf,sizeof(tmpBuf))) < 0)
				if ((ret = UnionReadRecFldFromRecStr(toldKmsReqPackage.package,lenOfPackage,fieldTBL.rec[i].parameter,tmpBuf,sizeof(tmpBuf))) < 0)
                                {
                                        if (ret != errCodeEsscMDL_FldNotExists)
                                        {
                                                UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionReadRecFldFromRecStr [%d][%s]!\n",i,fieldTBL.rec[i].parameter);
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
                                ret = sprintf(fieldValue,"Grp%s.vkInde%s.pk",hsmGroupIDList, tmpBuf);
                                fieldValue[ret] = 0;
                                break;
			case	convertMethodOfReadKeyNameByAppNoAndHsmGroupID:	//以idOfApp是应用编号，获取XXX是密码机组号。 取公钥名称为：idOfApp.hsmGrpXXX.pk；
				//if ((ret = UnionReadRecFldFromRecStr((char *)buf+offset,lenOfPackage,fieldTBL.rec[i].parameter,tmpBuf,sizeof(tmpBuf))) < 0)
				if ((ret = UnionReadRecFldFromRecStr(toldKmsReqPackage.package,lenOfPackage,fieldTBL.rec[i].parameter,tmpBuf,sizeof(tmpBuf))) < 0)
                                {
                                        if (ret != errCodeEsscMDL_FldNotExists)
                                        {
                                                UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionReadRecFldFromRecStr [%d][%s]!\n",i,fieldTBL.rec[i].parameter);
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
				ret = sprintf(fieldValue,"%s.hsmGrp%s.pk", tmpBuf,hsmGroupIDList);
                                fieldValue[ret] = 0;
				break;
			case	convertMethodOfReadKeyNameByAppNo:	//以appName是应用编号，取公钥名称：appName.pkcs10.pk
				//if ((ret = UnionReadRecFldFromRecStr((char *)buf+offset,lenOfPackage,fieldTBL.rec[i].parameter,tmpBuf,sizeof(tmpBuf))) < 0)
				if ((ret = UnionReadRecFldFromRecStr(toldKmsReqPackage.package,lenOfPackage,fieldTBL.rec[i].parameter,tmpBuf,sizeof(tmpBuf))) < 0)
                                {
                                        if (ret != errCodeEsscMDL_FldNotExists)
                                        {
                                                UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionReadRecFldFromRecStr [%d][%s]!\n",i,fieldTBL.rec[i].parameter);
                                                return(ret);
                                        }
					else
						continue;
                                }
                                tmpBuf[ret] = 0;
                                ret = sprintf(fieldValue,"%s.pkcs10.pk", tmpBuf);
                                fieldValue[ret] = 0;
				break;
			case	convertMethodOfReadPackageValueByRequest:	// 可选项
				//if ((ret = UnionReadRecFldFromRecStr((char *)buf+offset,lenOfPackage,fieldTBL.rec[i].parameter,fieldValue,sizeof(fieldValue))) < 0)
				if ((ret = UnionReadRecFldFromRecStr(toldKmsReqPackage.package,lenOfPackage,fieldTBL.rec[i].parameter,fieldValue,sizeof(fieldValue))) < 0)
				{
					if (ret == errCodeEsscMDL_FldNotExists)
					{
						UnionLog("in UnionConvertOtherPackageToXMLPackage:: UnionReadRecFldFromRecStr [%d][%s]!\n",i,fieldTBL.rec[i].parameter);
						continue;
					}
				}
				fieldValue[ret] = 0;
                                break;
			case	convertMethodOfReadPackageValueByAlgorithmID:
				//if ((ret = UnionReadRecFldFromRecStr((char *)buf+offset,lenOfPackage,fieldTBL.rec[i].parameter,fieldValue,sizeof(fieldValue))) > 0)
				if ((ret = UnionReadRecFldFromRecStr(toldKmsReqPackage.package,lenOfPackage,fieldTBL.rec[i].parameter,fieldValue,sizeof(fieldValue))) > 0)
				{
					fieldValue[ret] = 0;
					if ((strcmp(fieldValue,"41") == 0) || (strcmp(fieldValue,"4") == 0))	// 国密标识
						snprintf(fieldValue,sizeof(fieldValue),"SM4");
					else
						snprintf(fieldValue,sizeof(fieldValue),"DES");
				}
				else
					snprintf(fieldValue,sizeof(fieldValue),"DES");
				break;
			default:
				UnionLog("in UnionConvertOtherPackageToXMLPackage:: method[%d] error",fieldTBL.rec[i].method);
				strcpy(fieldValue,fieldTBL.rec[i].parameter);
				break;
		}
		
		ret = sprintf(fieldName,"body/%s",fieldTBL.rec[i].fieldName);
		fieldName[ret] = 0;
		if ((ret = UnionSetRequestXMLPackageValue(fieldName,fieldValue)) < 0)
		{
			UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: UnionSetRequestXMLPackageValue [%d][%s]!\n",i,fieldTBL.rec[i].parameter);
			return(ret);
		}
	}
	// modify end 20160218
		
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
	char	tmpBuf[4096];
	char	parameter[128];
	char	xmlFieldName[64];
	char	fieldValue[4096];
	char	responseCode[32];
	char	responseRemark[128];
	char	*ptr = NULL;
	int	lenOfPackage = 0;
	int	lenOfHead = 0;
	char	oldServiceCode[32];

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
	
	//UnionInitEsscResponsePackage();

	lenOfPackage += sprintf((char *)buf,"%s",gunionPackageHead);
	buf[0] = '0';
	lenOfHead = lenOfPackage;

	// 执行错误
	if (errCode < 0)
	{
		if (responseCode[0] != '0')
			responseCode[0] = '0';
		lenOfPackage += snprintf((char *)buf+lenOfPackage,sizeOfBuf - lenOfPackage,"-%05d%04zu%s",atoi(responseCode),strlen(responseRemark),responseRemark);
		UnionLog("in UnionConvertXMLPackageToOtherPackage:: reponse package [%s]!\n",buf);
		return(lenOfPackage);
	}
	else
	{
		lenOfPackage += 10;
		snprintf(oldServiceCode,sizeof(oldServiceCode),"%s%s",toldKmsReqPackage.resID,toldKmsReqPackage.serviceID);
		if ((totalNum = getOldServiceFiledREC(oldServiceCode, UnionGetVersionOfEsscPackage(),0,&fieldTBL)) < 0)
		{
			UnionUserErrLog("in UnionConvertOtherPackageToXMLPackage:: getOldServiceFiledREC ret = [%d]!\n",totalNum);
			return(totalNum);
		}

		if (totalNum > 0)
		{
			for (i = 0; i < totalNum; i++)
			{			
				ret = sprintf(xmlFieldName,"body/%s",fieldTBL.rec[i].parameter);
				xmlFieldName[ret] = 0;
				
				// 1=固定值,2=取报文值,3=取报文值并扩展,4=取报文值并压缩
				switch(fieldTBL.rec[i].method)
				{
					case	convertMethodOfReadFixedValue:	// 固定值
						strcpy(fieldValue,fieldTBL.rec[i].parameter);
						lenOfValue = strlen(fieldValue);
						break;	
					case	convertMethodOfReadPackageValue:	// 取报文值
						if ((lenOfValue = UnionReadResponseXMLPackageValue(xmlFieldName,fieldValue,sizeof(fieldValue))) < 0)
							break;
						break;
					case	convertMethodOfReadPackageValueAndExpand:	// 取报文值并扩展
						if ((ret = UnionReadResponseXMLPackageValue(xmlFieldName,tmpBuf,sizeof(tmpBuf))) < 0)
							break;
						tmpBuf[ret] = 0;
						lenOfValue = bcdhex_to_aschex(tmpBuf,ret,fieldValue);
						break;
					case	convertMethodOfReadPackageValueAndCompress:	// 取报文值并压缩
						if ((ret = UnionReadResponseXMLPackageValue(xmlFieldName,tmpBuf,sizeof(tmpBuf))) < 0)
							break;
						tmpBuf[ret] = 0;
						lenOfValue = aschex_to_bcdhex(tmpBuf,ret,fieldValue);
						break;
					case	convertMethodOfReadZMKNameBySameAppNoAndNode:	// 读取同应用和属主的ZMK密钥名称
						if ((ret = UnionReadResponseXMLPackageValue(xmlFieldName,tmpBuf,sizeof(tmpBuf))) < 0)
							break;
						tmpBuf[ret] = 0;
						if((ptr = strchr(tmpBuf, '.')) == NULL || (ptr + 1) == NULL || (ptr = strchr(ptr + 1, '.')) == NULL)
						{
							UnionUserErrLog("in UnionConvertXMLPackageToOtherPackage:: keyName [%s][%s] illegal!\n",tmpBuf, parameter);
							return(errCodeParameter);
								
						}
						ptr = 0;
						sprintf(fieldValue,"%s.zmk", tmpBuf);
						break;
					case	convertMethodOfCheckFieldIsExist:	// 判断域是否存在，如果存在赋值1，不存在赋值0
						if ((ret = UnionReadResponseXMLPackageValue(xmlFieldName,tmpBuf,sizeof(tmpBuf))) < 0)
							sprintf(fieldValue,"0");
						else
							sprintf(fieldValue,"1");
						break;
					case	convertMethodOfReadPackageValueByRequest:
						if ((lenOfValue = UnionReadResponseXMLPackageValue(xmlFieldName,fieldValue,sizeof(fieldValue))) < 0)
						{
							continue;
						}
						break;
					default:
						UnionLog("in UnionConvertXMLPackageToOtherPackage:: method[%d] error",fieldTBL.rec[i].method);
						strcpy(fieldValue,parameter);
						lenOfValue = strlen(fieldValue);
						break;	
				}
				fieldValue[lenOfValue] = 0;
				lenOfPackage += snprintf((char *)buf+lenOfPackage,sizeOfBuf - lenOfPackage,"%s=%s|",fieldTBL.rec[i].fieldName,fieldValue);
			}
		}
	}
	ret = lenOfPackage - lenOfHead - 10;
	snprintf(tmpBuf,sizeof(tmpBuf),"%06d%04d",ret,ret);
	memcpy(buf+lenOfHead,tmpBuf,10);
	buf[lenOfPackage] = 0;
	UnionLog("in UnionConvertXMLPackageToOtherPackage:: reponse package [%s]!\n",buf);
	return(lenOfPackage);
}


/* 
功能：	转发报文到远程平台
参数：	[in]
返回值：>=0			成功
	<0			失败，返回错误码
*/
int UnionSendPackageToRemotePlatform(char *ipAddr,int port,int timeout,unsigned char *buf,int lenOfBuf)
{
	int		ret = 0;
	int		len = 0;
	int		socketHDL = -1;
	unsigned char	lenBuf[8];
	unsigned char	resStr[81920];
	char		fileName[128];
	int		recvFileFlag = 0;
	int		dataRecvFinished = 0;
	char		resID[8];
	char		resCmd[8];
	FILE		*fp = NULL;
	int		mngSvrResCmd = 0;
	int		tmpFileIndex = 0;
	
	
	if (buf == NULL)
	{
		UnionUserErrLog("in UnionSendPackageToRemotePlatform:: reqStr is null!\n");
		return(errCodeParameter);
	}
	
	UnionNullLog("***** START 远程平台[%s:%d] *****\n",ipAddr,port);
	UnionNullLog("Request Package::[%d][%s]\n",lenOfBuf-2 ,buf+2);
	
	len = lenOfBuf;

	if ((socketHDL = UnionCreateSocketClient(ipAddr,port)) < 0)
	{
		UnionUserErrLog("in UnionSendPackageToRemotePlatform:: UnionCreateSocketClient[%s][%d]!\n",ipAddr,port);
		ret = errCodeUseOSErrCode;
		goto errorExit;
	}

	alarm(0);
#ifdef _LINUX_
	if (sigsetjmp(gunionRemotePlatformJmpEnv,1) != 0)
#else
	if (setjmp(gunionRemotePlatformJmpEnv) != 0)
#endif
	{
		UnionUserErrLog("in UnionSendPackageToRemotePlatform:: Timeout!\n");
		UnionSetResponseRemark("远程平台,地址[%s:%d]响应超时",ipAddr,port);
		ret = errCodeSocketMDL_Timeout;
		goto errorExit;
	}
	alarm(timeout);
	signal(SIGALRM,UnionDealRemotePlatformResponseTimeout);

	if ((ret = UnionSendToSocket(socketHDL,buf,len)) < 0)
	{
		UnionUserErrLog("in UnionSendPackageToRemotePlatform:: UnionSendToSocket buf[%d][%s]!\n",len,buf);
		goto errorExit;
	}
	// 接收数据
	recvFileFlag = 0;
	for (;;)
	{
		// 接收一个数据包
		memset(lenBuf,0,2);
		if ((ret = UnionReceiveFromSocketUntilLen(socketHDL,lenBuf,2)) < 0)
		{
			UnionUserErrLog("in UnionSendPackageToRemotePlatform:: UnionReceiveFromSocketUntilLen!\n");
			goto errorExit;
		}
		if (ret == 0)
		{
			UnionDebugLog("in UnionSendPackageToRemotePlatform:: svr close socket!\n");
			ret = errCodeEsscMDL_NoResponsePackFromMngSvr;
			goto errorExit;
		}
		if (((len = lenBuf[0] * 256 + lenBuf[1]) < 0) || (len > sizeof(resStr)))
		{
			UnionUserErrLog("in UnionSendPackageToRemotePlatform:: len [%d] too large or too small!\n",len);
			ret = errCodeEsscMDL_DataLenFromMngSvr;
			goto errorExit;
		}
		if (len == 0)
			continue;

		if ((ret = UnionReceiveFromSocketUntilLen(socketHDL,resStr,len)) < 0)
		{
			UnionUserErrLog("in UnionSendPackageToRemotePlatform:: UnionReceiveFromSocketUntilLen!\n");
			goto errorExit;
		}
		if (ret == 0)
		{
			UnionDebugLog("in UnionSendPackageToRemotePlatform:: svr close socket!\n");
			ret = errCodeEsscMDL_NoResponsePackFromMngSvr;
			goto errorExit;
		}
		resStr[len] = 0;
		// 判断是不是响应包
		if (resStr[0] == '0')		// 是响应包
		{
			if (recvFileFlag != 0)		// 是否有数据传输
			{
				if (!dataRecvFinished)		// 有数据传，是否传输结束
				{
					UnionUserErrLog("in UnionSendPackageToRemotePlatform:: data send not finished yet!\n");
					ret = errCodeEsscMDL_NotFinishSendingData;
					goto errorExit;
				}
			}
			break;
		}
		// 是请求包，解这个请求包
		if ((ret = UnionUnpackMngSvrRequestPackage((char *)resStr,len,resID,resID,resCmd,(char *)resStr,sizeof(resStr))) < 0)
		{
			UnionUserErrLog("in UnionSendPackageToRemotePlatform:: UnionUnpackMngSvrRequestPackage!\n");
			goto errorExit;
		}
		mngSvrResCmd = atoi(resCmd);
		// 判断是不是数据开始传送包
		if (mngSvrResCmd == conResCmdStartSendingData) 
		{
			if (recvFileFlag != 0)	// 已经收到过一个数据传输开始包
			{
				UnionUserErrLog("in UnionSendPackageToRemotePlatform:: another data start sending pack received again!\n");
				ret = errCodeEsscMDL_AnotherStartingSendDataCmd;
				goto errorExit;
			}

			tmpFileIndex ++;

			if (UnionGenerateTempFile(fileName,sizeof(fileName),tmpFileIndex) < 0)	// 不能获得一个暂时存储文件名
			{
				UnionUserErrLog("in UnionSendPackageToRemotePlatform:: UnionGenerateMngSvrTempFile!\n");
				ret = errCodeEsscMDL_FailToGetTempFile;
				goto errorExit;
			}
			recvFileFlag = 1;
			
			if ((fp = fopen(fileName,"wb")) == NULL)	// 打不开暂时文件
			{
				UnionSystemErrLog("in UnionSendPackageToRemotePlatform:: fopen [%s]!\n",fileName);
				ret = errCodeUseOSErrCode;
				goto errorExit;
			}
			continue;
		}
		// 判断是不是数据结束传送包
		if (mngSvrResCmd == conResCmdFinishSendingData) 
		{
			if (recvFileFlag == 0)	// 没有收到过数据传输开始包
			{
				UnionUserErrLog("in UnionSendPackageToRemotePlatform:: no start sending pack received ever!\n");
				ret = errCodeEsscMDL_NoStartingSendDataCmdBefore;
				goto errorExit;
			}
			dataRecvFinished = 1;
			continue;
		}	
		// 判断是不是数据包
		if (mngSvrResCmd == conResCmdSendingData)
		{
			if ((recvFileFlag == 0) || (fp == NULL))	// 没有收到过数据传输开始包
			{
				UnionUserErrLog("in UnionSendPackageToRemotePlatform:: no start sending pack received ever!\n");
				ret = errCodeEsscMDL_NoStartingSendDataCmdBefore;
				goto errorExit;
			}
			fprintf(fp,"%s",resStr);
			continue;
		}
		UnionUserErrLog("in UnionSendPackageToRemotePlatform:: wrong request package received! [%03d]\n",mngSvrResCmd);
		ret = errCodeEsscMDL_InvalidOperation;
		goto errorExit;
	}
	if (len >= sizeof(toldKmsRemoteReqPackage.package))
	{
		UnionUserErrLog("in UnionSendPackageToRemotePlatform::  retturn len[%d] >= sizeofResStr[%d]!\n",len,(int)sizeof(toldKmsRemoteReqPackage.package));
		ret = errCodeSmallBuffer;
		goto errorExit;
	}
	if ((ret = UnionUnpackMngSvrResponsePackage((char *)resStr,len,toldKmsRemoteReqPackage.tellerNo,toldKmsRemoteReqPackage.resID,toldKmsRemoteReqPackage.serviceID,toldKmsRemoteReqPackage.package,sizeof(toldKmsRemoteReqPackage.package))) < 0)
	{
		UnionUserErrLog("in UnionSendPackageToRemotePlatform:: UnionUnpackMngSvrResponsePackage [%04d][%s]!\n",len,resStr);
		goto errorExit;
	}
	toldKmsRemoteReqPackage.lenOfPackage = strlen(toldKmsRemoteReqPackage.package);
	UnionNullLog("Response Package:: [%d] resStr[%s]\n",len,resStr);
	
errorExit:
	if (fp != NULL)
		fclose(fp);
	if (socketHDL > 0)
		UnionCloseSocket(socketHDL);
	
	UnionNullLog("***** END 远程平台[%s:%d] *****\n",ipAddr,port);
	return(ret);
}

// 初始化远程操作请求报文
int UnionInitRemoteRequestPackage(PUnionRemoteKeyPlatform pkeyPlatform)
{
	int	ret = 0;

	gunionRemotePackageType = pkeyPlatform->packageType;
	memset(&toldKmsRemoteReqPackage,0,sizeof(TUnionOldKmsPackage));

	switch(pkeyPlatform->packageType)
	{
		case	5:
			if ((ret = UnionInitRemoteRequestPackageToRemoteKeyPlatform(pkeyPlatform)) < 0)
	                {
	                        UnionUserErrLog("in UnionInitRemoteRequestPackage:: UnionInitRemoteRequestPackageToRemoteKeyPlatform!\n");
	                        return(ret);
	                }
	                break;
		case	4:
			memcpy(&toldKmsRemoteReqPackage,&toldKmsReqPackage,sizeof(TUnionOldKmsPackage));
			break;
		case	3:
                default:
                	UnionUserErrLog("in UnionSetRequestRemotePackageFldValue:: packageType[%d] is invalid!\n",pkeyPlatform->packageType);
			return(errCodeParameter);
        }
        return 0;
}

// 发送报文到远程平台
int UnionTransferPackageToRemotePlatform(PUnionRemoteKeyPlatform pkeyPlatform)
{
	unsigned char	buf[81920];
	int		ret = 0;
	int		lenOfBuf = 0;

	switch(pkeyPlatform->packageType)
	{
		case	5:
			// 分发到总行平台
	                if ((ret = UnionTransferKeyOperateToRemoteKeyPlatform(pkeyPlatform)) < 0)
	                {
	                        UnionUserErrLog("in UnionTransferPackageToRemotePlatform:: UnionTransferKeyOperateToRemoteKeyPlatform!\n");
	                        return(ret);
	                }
	                break;
		case	4:
			lenOfBuf = 2;
			lenOfBuf += sprintf((char *)buf+lenOfBuf,"1%s%s%s",toldKmsRemoteReqPackage.tellerNo,toldKmsRemoteReqPackage.resID,toldKmsRemoteReqPackage.serviceID);
			lenOfBuf += sprintf((char *)buf+lenOfBuf,"%04d%s",toldKmsRemoteReqPackage.lenOfPackage,toldKmsRemoteReqPackage.package);
			buf[lenOfBuf] = 0;
			buf[0] = (lenOfBuf-2) / 256;
			buf[1] = (lenOfBuf-2) % 256;
			buf[2] = '1';
			
			if ((ret = UnionSendPackageToRemotePlatform(pkeyPlatform->ipAddr,pkeyPlatform->port,pkeyPlatform->timeout,buf,lenOfBuf)) < 0)
			{
				UnionUserErrLog("in UnionTransferPackageToRemotePlatform:: UnionSendPackageToRemotePlatform!\n");
	                        return(ret);	
			}
			break;
		case	3:
                default:
                	UnionUserErrLog("in UnionTransferPackageToRemotePlatform:: packageType[%d] is invalid!\n",pkeyPlatform->packageType);
			return(errCodeParameter);
        }
        return 0;
}

// 读取一个域值
int UnionReadRequestRemotePackageFldValue(char *fldName,char *value,int sizeofValue)
{
	int	ret = 0;
	char	tmpBuf[64];
	
	switch (gunionRemotePackageType)
	{
		case	5:
			snprintf(tmpBuf,sizeof(tmpBuf),"body/%s",fldName);
			if ((ret = UnionReadResponseRemoteXMLPackageValue(tmpBuf,value,sizeofValue)) < 0)
	                {
	                        UnionUserErrLog("in UnionReadRequestRemotePackageFldValue:: UnionReadResponseRemoteXMLPackageValue[%s]!\n",tmpBuf);
	                        return(ret);
	                }
	                break;
		case	4:
			if ((ret = UnionReadRecFldFromRecStr(toldKmsRemoteReqPackage.package,toldKmsRemoteReqPackage.lenOfPackage,fldName,value,sizeofValue)) < 0)
			{
				UnionUserErrLog("in UnionReadRequestRemotePackageFldValue:: UnionReadRecFldFromRecStr[%s] package[%s]!\n",fldName,toldKmsRemoteReqPackage.package);
				return(ret);
			}
			break;
		case	3:
		default:
			UnionUserErrLog("in UnionReadRequestRemotePackageFldValue:: packageType[%d] is invalid!\n",gunionRemotePackageType);
			return(errCodeParameter);
	}
	return ret;
}

// 设置一个域值
int UnionSetRequestRemotePackageFldValue(char *fldName,char *value)
{
	int	ret = 0;
	char	tmpBuf[48];
	char	*ptr = NULL;
	
	switch (gunionRemotePackageType)
	{
		case	5:
			snprintf(tmpBuf,sizeof(tmpBuf),"body/%s",fldName);
			if ((ret = UnionSetRequestRemoteXMLPackageValue(tmpBuf,value)) < 0)
	                {
	                        UnionUserErrLog("in UnionSetRequestRemotePackageFldValue:: UnionSetRequestRemoteXMLPackageValue[%s]!\n",tmpBuf);
	                        return(ret);
	                }
	                break;
		case	4:
			snprintf(tmpBuf,sizeof(tmpBuf),"%s=",fldName);
			if ((ptr = strstr(toldKmsRemoteReqPackage.package,tmpBuf)) != NULL)
			{
				if ((ret = UnionUpdateSpecFldValueOnUnionRec(toldKmsRemoteReqPackage.package,toldKmsRemoteReqPackage.lenOfPackage,fldName,value)) < 0)
				{
					UnionUserErrLog("in UnionSetRequestRemotePackageFldValue:: UnionUpdateSpecFldValueOnUnionRec[%s]!\n",fldName);
					return(ret);
				}
				toldKmsRemoteReqPackage.lenOfPackage = ret;
			}
			else
			{
				ret = snprintf(toldKmsRemoteReqPackage.package + toldKmsRemoteReqPackage.lenOfPackage,sizeof(toldKmsRemoteReqPackage.package)-toldKmsRemoteReqPackage.lenOfPackage,"%s=%s|",fldName,value);
				toldKmsRemoteReqPackage.lenOfPackage += ret;
			}
			break;
		case	3:
		default:
			UnionUserErrLog("in UnionSetRequestRemotePackageFldValue:: packageType[%d] is invalid!\n",gunionRemotePackageType);
			return(errCodeParameter);
	}
	return 0;
}

// 设置一个域值
int UnionSetRequestRemotePackageHead(char *fldName,char *value)
{
	int	ret = 0;
	char	tmpBuf[48];
	
	switch (gunionRemotePackageType)
	{
		case	5:
			snprintf(tmpBuf,sizeof(tmpBuf),"head/%s",fldName);
			if ((ret = UnionSetRequestRemoteXMLPackageValue(tmpBuf,value)) < 0)
	                {
	                        UnionUserErrLog("in UnionSetRequestRemotePackageFldValue:: UnionSetRequestRemoteXMLPackageValue[%s]!\n",tmpBuf);
	                        return(ret);
	                }
	                break;
		case	4:
			if (strcmp(fldName,"serviceCode") == 0)
				snprintf(toldKmsRemoteReqPackage.serviceID,sizeof(toldKmsRemoteReqPackage.serviceID),"%s",value);
			break;
		case	3:
		default:
			UnionUserErrLog("in UnionSetRequestRemotePackageFldValue:: packageType[%d] is invalid!\n",gunionRemotePackageType);
			return(errCodeParameter);
	}
	return 0;
}



// 生成一个临时文件
int UnionGenerateTempFile(char *fileFullName,int sizeofFileFullName,int tmpFileIndex)
{
	char	*ptr = NULL;
	int	ret = 0;
	char	fileDir[256];
	if ((ptr = UnionReadStringTypeRECVar("defaultDirOfFileReceiver")) == NULL)
	{
		UnionUserErrLog("in UnionGenerateTempFile:: UnionReadStringTypeRECVar [UnionReadStringTypeRECVar] not found!\n");
		return(errCodeRECMDL_VarNotExists);
	}
	UnionReadDirFromStr(ptr,-1,fileDir);
	ret = snprintf(fileFullName,sizeofFileFullName,"%s/%05d_%d.tmp",fileDir,getpid(),tmpFileIndex);
	return(ret);
}


void UnionDealRemotePlatformResponseTimeout()
{
	UnionUserErrLog("in UnionDealRemoteKeyPlatformResponseTimeout:: remote platform response time out!\n");
#ifdef _LINUX_
	siglongjmp(gunionRemotePlatformJmpEnv,10);
#else
	longjmp(gunionRemotePlatformJmpEnv,10);
#endif
}

// 传送文件到远程平台
int UnionTransferFileDataToRemote(int sckHDL,char *fileName,char *tellerNo,int resID)
{
	int		ret = 0;
	unsigned char	buf[2176];
	char		data[2048];
	int		MAX_TRANSFER_LEN = 2000;
	int		realSendLen = 0;
	char		resIDBuf[16];
	FILE		*fp = NULL;
	int		len = 0;
	int		block = 0;

	if ((sckHDL < 0) || (fileName == NULL) || (tellerNo == NULL))
	{
		UnionUserErrLog("in UnionTransferFileDataToRemote:: sckHDL = [%d]!\n",sckHDL);
		return(errCodeParameter);
	}

	snprintf(resIDBuf,sizeof(resIDBuf),"%03d",resID);
	if ((fp = fopen(fileName,"rb")) == NULL)
	{
		UnionSystemErrLog("in UnionTransferFileDataToRemote:: fopen fileName[%s]!\n",fileName);
		return(errCodeUseOSErrCode);
	}

	// 先传送一个开始包
	if ((ret = UnionPackMngSvrRequestPackage(tellerNo,resID,conResCmdStartSendingData,0,"",(char *)buf+2,sizeof(buf)-2)) < 0)
	{
		UnionUserErrLog("in UnionTransferFileDataToRemote:: UnionPackMngSvrRequestPackage begin ...!\n");
		goto errorExit;
	}

	buf[0] = ret / 256;
	buf[1] = ret % 256;
	ret += 2;

	UnionNullLog("Transfer [%s] beginning....\n",fileName);
	UnionNullLog("Request Package:: [%04d][%s]\n",ret,buf+2);

	if ((ret = UnionSendToSocket(sckHDL,buf,ret)) < 0)
	{
		UnionUserErrLog("in UnionTransferFileDataToRemote:: UnionSendToSocket buf[%d][%s]!\n",ret,buf);
		goto errorExit;
	}

	// 开始传送文件数据
	while(!feof(fp))
	{
		if ((len = fread(data,1,MAX_TRANSFER_LEN,fp)) <= 0)
			continue;

		if ((ret = UnionPackMngSvrRequestPackage(tellerNo,resID,conResCmdSendingData,len,data,(char *)buf+2,sizeof(buf)-2)) < 0)
		{
			UnionUserErrLog("in UnionTransferFileDataToRemote:: UnionPackMngSvrRequestPackage transfer ...!\n");
			goto errorExit;
		}

		buf[0] = ret / 256;
		buf[1] = ret % 256;
		ret += 2;

		if ((ret = UnionSendToSocket(sckHDL,buf,ret)) < 0)
		{
			UnionUserErrLog("in UnionTransferFileDataToRemote:: UnionSendToSocket buf[%d][%s]!\n",ret,buf+2);
			goto errorExit;
		}

		realSendLen += len;
		++block;	
	}
	UnionNullLog("Request Package:: totalNum block[%d]\n",block);

	// 发送一个结束包
	len = snprintf(data,sizeof(data),"%08d",realSendLen);
	if ((ret = UnionPackMngSvrRequestPackage(tellerNo,resID,conResCmdFinishSendingData,len,data,(char *)buf+2,sizeof(buf)-2)) < 0)
	{
		UnionUserErrLog("in UnionTransferFileDataToRemote:: UnionPackMngSvrRequestPackage transfer ...!\n");
		goto errorExit;
	}

	buf[0] = ret / 256;
	buf[1] = ret % 256;
	ret += 2;

	if ((ret = UnionSendToSocket(sckHDL,buf,ret)) < 0)
	{
		UnionUserErrLog("in UnionTransferFileDataToRemote:: UnionSendToSocket buf[%d][%s]!\n",ret,buf);
		goto errorExit;
	}

	UnionNullLog("Transfer [%s] OK! fileSize [%08d] bytes\n",fileName,realSendLen);
errorExit:
	if (fp != NULL)
		fclose(fp);
	return(ret);
}

// 打包一个请求包
int UnionPackMngSvrRequestPackage(char *tellerNo,int resID,int resCmd,int lenOfData,char *data,char *buf,int sizeofBuf)
{
	int	offset = 0;

	buf[0] = '1';
	offset = 1;

	offset += snprintf(buf+offset,sizeofBuf-offset,"%s",tellerNo);
	offset += snprintf(buf+offset,sizeofBuf-offset,"%03d",resID);
	offset += snprintf(buf+offset,sizeofBuf-offset,"%03d",resCmd);
	offset += snprintf(buf+offset,sizeofBuf-offset,"%04d",lenOfData);

	if ((sizeofBuf - offset) <= lenOfData)
	{
		UnionUserErrLog("in UnionPackMngSvrRequestPackage:: lenOfData[%d] - offset[%d] < sizeofBuf [%d] too small!\n",lenOfData,offset,sizeofBuf);
		return(errCodeParameter);
	}
	if (lenOfData > 0)
	{
		memcpy(buf+offset,data,lenOfData);
		offset += lenOfData;
	}
	buf[offset] = 0;
	return (offset);	
}

// 解析一个请求包
int UnionUnpackMngSvrRequestPackage(char *reqStr,int lenOfReqStr,char *tellerNo,char *resID,char *resCmd,char *resStr,int sizeofResStr)
{
	char	tmpBuf[128];
	int	offset = 0;
	int	lenOfPackage = 0;
	if ((offset = UnionIsCheckKMSPackage('1',reqStr,lenOfReqStr,tellerNo,tmpBuf,resID,resCmd)) < 0)
	{
		UnionUserErrLog("in UnionUnpackMngSvrRequestPackage:: UnionIsCheckKMSPackage buf[%s]! ret = [%d]\n",reqStr,offset);
		return(offset);
	}

	memcpy(tmpBuf,reqStr+offset,4);
	tmpBuf[4] = 0;
	lenOfPackage = atoi(tmpBuf);
	offset += 4;

	if (lenOfPackage >= sizeofResStr)
	{
		UnionUserErrLog("in UnionUnpackMngSvrRequestPackage:: package is too long, buf[%d] >=  sizeofResStr[%d]\n",offset,sizeofResStr);
		return(errCodeSmallBuffer);
	}
	memcpy(resStr,reqStr+offset,lenOfPackage);
	resStr[lenOfPackage] = 0;
	return 0;
}

// 解析一个响应包
int UnionUnpackMngSvrResponsePackage(char *reqStr,int lenOfReqStr,char *tellerNo,char *resID,char *resCmd,char *resStr,int sizeofResStr)
{
	char	tmpBuf[128];
	int	offset = 0;
	int	lenOfPackage = 0;
	char	responseCode[8];
	int	ret = 0;

	if ((offset = UnionIsCheckKMSPackage('0',reqStr,lenOfReqStr,tellerNo,responseCode,resID,resCmd)) < 0)
	{
		UnionUserErrLog("in UnionUnpackMngSvrResponsePackage:: UnionIsCheckKMSPackage buf[%s]! ret = [%d]\n",reqStr,offset);
		return(offset);
	}

	memcpy(tmpBuf,reqStr+offset,4);
	tmpBuf[4] = 0;
	lenOfPackage = atoi(tmpBuf);
	offset += 4;

	if (lenOfPackage >= sizeofResStr)
	{
		UnionUserErrLog("in UnionUnpackMngSvrResponsePackage:: package is too long, buf[%d] >=  sizeofResStr[%d]\n",offset,sizeofResStr);
		return(errCodeSmallBuffer);
	}
	memcpy(resStr,reqStr+offset,lenOfPackage);
	resStr[lenOfPackage] = 0;

	if (atoi(responseCode) < 0)
		ret = atoi(responseCode);

	return ret;
}

