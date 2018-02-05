//	Author:		张永定
//	Copyright:	Union Tech. Guangzhou
//	Date:		2013-01-10

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionTableDef.h"
#include "unionRealBaseDB.h"
#include "UnionLog.h"
#include "baseUIService.h"
#include "UnionStr.h"
#include "unionRSA.h"
#include "UnionXOR.h"

/***************************************
服务代码:	0001
服务名:		获取验证码
功能描述:	获取验证码
***************************************/
int UnionDealServiceCode0001(PUnionHsmGroupRec phsmGroupRec)
{
	int	i;
	int	ret;
	int	random = 0;
	time_t	now = 0;
	int	length = 0;
	char	userID[64];
	char	tmpBuf[1024];
	char	useFlag[32];
	char	type[32];
	char	identifyCode[1024];
	char	sql[512];
	char	loginMode[32];
	char	userPK[512];
	char	derPK[512];
	char	ascDerPK[1024];
//	int	lenOfDerPK = 0;	
	int	len = 0;
	char	randData[32];

	// 读取用户ID
	memset(userID,0,sizeof(userID));
	if ((ret = UnionReadRequestXMLPackageValue("head/userID",userID,sizeof(userID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0001:: UnionReadRequestXMLPackageValue[%s]!\n","head/userID");
		return(ret);
	}

	// 读取用途
	memset(useFlag,0,sizeof(useFlag));
	if ((ret = UnionReadRequestXMLPackageValue("body/useFlag",useFlag,sizeof(useFlag))) < 0)
	{
		UnionLog("in UnionDealServiceCode0001:: UnionReadRequestXMLPackageValue[%s]!\n","body/useFlag");
		useFlag[0] = '1';
	}

	// 读取长度
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRequestXMLPackageValue("body/length",tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionLog("in UnionDealServiceCode0001:: UnionReadRequestXMLPackageValue[%s]!\n","body/length");
		length = 10;
	}
	else
	{
		if ((useFlag[0] == '2') || ((length = atoi(tmpBuf)) <= 0))
			length = 10;
	}
	
	// 读取样式
	memset(type,0,sizeof(type));
	if ((ret = UnionReadRequestXMLPackageValue("body/type",type,sizeof(type))) < 0)
	{
		UnionLog("in UnionDealServiceCode0001:: UnionReadRequestXMLPackageValue[%s]!\n","body/type");
		type[0] = 'A';
	}

	srand((unsigned)time(NULL));
	
	for (i = 0; i < length; i++)
	{
		random = rand();

		if (type[0] == 'N')
			identifyCode[i] = '0' + (int)(10.0 * rand()/(RAND_MAX));
		else if (type[0] == 'C')
			identifyCode[i] = 'A' + (int)(26.0 * rand()/(RAND_MAX));
		else
		{
			if (random % 2)
				identifyCode[i] = '0' + (int)(10.0 * rand()/(RAND_MAX));
			else
				identifyCode[i] = 'A' + (int)(26.0 * rand()/(RAND_MAX));
		}
	}
	identifyCode[length] = 0;

	// 用于用户登录、重启服务
	if ((useFlag[0] == '1') || (useFlag[0] == '2'))
	{
		if (useFlag[0] == '2')
		{
			time(&now);
			sprintf(identifyCode + length,"%ld",now);			
			UnionLog("in UnionDealServiceCode0001:: identifyCode[%s]\n",identifyCode);	
		}
		
		if (length > 20)
			identifyCode[20] = 0;
	
		memset(sql,0,sizeof(sql));
		sprintf(sql,"update sysUser set identifyCode = '%s' where userID = '%s'",identifyCode,userID);
	
		if ((ret = UnionExecRealDBSql(sql)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0001:: UnionExecRealDBSql[%s]!\n",sql);
			return(ret);
		}
		else if (ret == 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0001:: UnionExecRealDBSql[%s]!\n",sql);
			return(errCodeOperatorMDL_OperatorNotExists);
		}

	}
	
	// 查找用户表
	memset(sql,0,sizeof(sql));
	sprintf(sql,"select loginMode,userPK from sysUser where userID = '%s'",userID);
	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0001:: UnionSelectRealDBRecord[%s]!\n",sql);
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0001:: 用户ID[%s]不存在!\n",sql);
		return(errCodeOperatorMDL_OperatorNotExists);		
	}
	
	UnionLocateXMLPackage("detail", 1);

	// 登录方式
	memset(loginMode,0,sizeof(loginMode));
	if ((ret = UnionReadXMLPackageValue("loginMode", loginMode, sizeof(loginMode))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0001:: UnionReadXMLPackageValue[%s]!\n","loginMode");
		return(ret);
	}
	
	// 设置响应数据
	if (loginMode[0] == '2')		// USBKey
	{
		memset(userPK,0,sizeof(userPK));
		if ((ret = UnionReadXMLPackageValue("userPK", userPK, sizeof(userPK))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0001:: UnionReadXMLPackageValue[%s]!\n","userPK");
			return(ret);
		}
		if(memcmp(userPK,"SM2",3) == 0)//edit by yang 20150105 add the branch 
		{
			memset(randData,0,sizeof(randData));
			if ((ret = UnionReadRequestXMLPackageValue("body/randData",randData,sizeof(randData))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0001:: UnionReadRequestXMLPackageValue[%s]!\n","body/randData");
				return(ret);
			}
			if (strlen(randData)!=strlen(identifyCode))
			{
				UnionUserErrLog("in UnionDealServiceCode0001:: strlen(randData)!=strlen(identifyCode)");
				return -1;
			}
			memset(tmpBuf,0,sizeof(tmpBuf));
			if ((ret = UnionXOR(randData,identifyCode,strlen(randData),tmpBuf))<0)
			{
				UnionUserErrLog("in UnionDealServiceCode0001:: UnionXOR(randData,identifyCode)");
				return ret;
			}
			memset(identifyCode,0,sizeof(identifyCode));
			strcpy(identifyCode,tmpBuf);
			if ((ret = UnionSetResponseXMLPackageValue("body/identifyCode",identifyCode)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0001:: UnionSetResponseXMLPackageValue[%s]!\n","body/identifyCode");
				return(ret);
			}
		}
		else
		{
			if (memcmp(userPK,"RSA",3) == 0 || memcmp(userPK,"SM2",3) == 0)
			{
				ret -= 3;
				memmove(userPK,userPK+3,ret);
				userPK[ret] = 0;
			}
	
			// 使用公钥加密identifyCode
			if ((ret = UnionFormANSIDERRSAPK(userPK,ret,derPK,sizeof(derPK))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0001:: UnionFormANSIDERRSAPK[%s]!\n","userPK");
				return(ret);
			}
			derPK[ret] = 0;	
			
			len = bcdhex_to_aschex(derPK,ret,ascDerPK);
			ascDerPK[len] = 0;
			if ((ret = UnionPKEncByPKCS1((unsigned char *)identifyCode,length,ascDerPK,(unsigned char*)identifyCode)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0001:: UnionPKEncByPKCS1!\n");
				return(ret);
			}
			
			len = bcdhex_to_aschex(identifyCode,ret,ascDerPK);
			ascDerPK[len] = 0;
			
			if ((ret = UnionSetResponseXMLPackageValue("body/identifyCode",ascDerPK)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0001:: UnionSetResponseXMLPackageValue[%s]!\n","body/identifyCode");
				return(ret);
			}
		}
	}
	else
	{
		if ((ret = UnionSetResponseXMLPackageValue("body/identifyCode",identifyCode)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0001:: UnionSetResponseXMLPackageValue[%s]!\n","body/identifyCode");
			return(ret);
		}
	}
	
	return(0);
}
