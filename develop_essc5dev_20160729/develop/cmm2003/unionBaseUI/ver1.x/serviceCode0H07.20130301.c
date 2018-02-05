
#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_
#endif

#ifndef _UnionLogMDL_3_x_
#define _UnionLogMDL_3_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "UnionLog.h"
#include "UnionStr.h"
#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionRealBaseDB.h"
#include "errCodeTranslater.h"
#include "baseUIService.h"

/*********************************
服务代码:	0H07
服务名:		增加LMK保护方式
功能描述:	增加LMK保护方式
**********************************/

int UnionDealServiceCode0H07(PUnionHsmGroupRec phsmGroupRec)
{
	int		ret;
	int		i = 0;
	int		totalNum = 0;
	char		sql[1024+1];
	char		tmpBuf[128+1];
	char		modeID[16+1];
	char		modeName[64+1];
	char		desProtectKey[48+1];
	char		desProtectCV[16+1];
	char		sm4ProtectKey[32+1];
	char		sm4ProtectCV[16+1];
	char		remark[128+1];
	char		tmpModeID[16+1];
	
	// 保护方式ID
	memset(modeID,0,sizeof(modeID));
	if ((ret = UnionReadRequestXMLPackageValue("body/modeID",modeID,sizeof(modeID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H07:: UnionReadRequestXMLPackageValue[%s]!\n","body/modeID");
		return(ret);
	}

	// 保护方式名
	memset(modeName,0,sizeof(modeName));
	if ((ret = UnionReadRequestXMLPackageValue("body/modeName",modeName,sizeof(modeName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H07:: UnionReadRequestXMLPackageValue[%s]!\n","body/modeName");
		return(ret);
	}

	// DES保护密钥
	memset(desProtectKey,0,sizeof(desProtectKey));
	memset(desProtectCV,0,sizeof(desProtectCV));
	if ((ret = UnionReadRequestXMLPackageValue("body/desProtectKey",desProtectKey,sizeof(desProtectKey))) > 0)
	{
		// DES密钥检验值
		if ((ret = UnionReadRequestXMLPackageValue("body/desProtectCV",desProtectCV,sizeof(desProtectCV))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0H07:: UnionReadRequestXMLPackageValue[%s]!\n","body/desProtectCV");
			return(ret);
		}

		if (ret != 6 && ret != 16)
		{
			UnionUserErrLog("in UnionDealServiceCode0H07:: desProtectCV[%s] length error, must is 6 or 16!\n",desProtectCV);
			UnionSetResponseRemark("DES密钥校验值长度必须为6或16");
			return(errCodeEssc_CheckValue);
		}
	}


	// SM4保护密钥
	memset(sm4ProtectKey,0,sizeof(sm4ProtectKey));
	memset(sm4ProtectCV,0,sizeof(sm4ProtectCV));
	if ((ret = UnionReadRequestXMLPackageValue("body/sm4ProtectKey",sm4ProtectKey,sizeof(sm4ProtectKey))) > 0)
	{
		// SM4密钥校验值
		if ((ret = UnionReadRequestXMLPackageValue("body/sm4ProtectCV",sm4ProtectCV,sizeof(sm4ProtectCV))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0H07:: UnionReadRequestXMLPackageValue[%s]!\n","body/sm4ProtectCV");
			return(ret);
		}
		if (ret != 6 && ret != 16)
		{
			UnionUserErrLog("in UnionDealServiceCode0H07:: sm4ProtectCV[%s] length error, must is 6 or 16!\n",sm4ProtectCV);
			UnionSetResponseRemark("SM4密钥校验值长度必须为6或16");
			return(errCodeEssc_CheckValue);
		}
	}


	// 备注
	memset(remark,0,sizeof(remark));
	if ((ret = UnionReadRequestXMLPackageValue("body/remark",remark,sizeof(remark))) < 0)
	{
		//UnionUserErrLog("in UnionDealServiceCode0H07:: UnionReadRequestXMLPackageValue[%s]!\n","body/remark");
		//return(ret);
	}

	if (strlen(desProtectKey) >= 16 || strlen(sm4ProtectKey) >= 16)
	{
		memset(sql,0,sizeof(sql));	
		strcpy(sql,"select * from lmkProtectMode");
		if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0H07:: UnionSelectRealDBRecord[%s]!\n",sql);
			return(ret);
		}

		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadXMLPackageValue("totalNum",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0H07:: UnionReadXMLPackageValue[%s]!\n","totalNum");
			return(ret);
		}
		else
			totalNum = atoi(tmpBuf);

		for (i = 1; i <= totalNum; i++)
		{
			if ((ret = UnionLocateXMLPackage("detail",i)) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0H07:: UnionLocateXMLPackage[%s]!\n","totalNum");
				return(ret);
			}

			memset(tmpModeID,0,sizeof(tmpModeID));
			if ((ret = UnionReadXMLPackageValue("modeID",tmpModeID,sizeof(tmpModeID))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0H07:: UnionReadXMLPackageValue[%s][%d]!\n","modeID",i);
				return(ret);
			}

			memset(tmpBuf,0,sizeof(tmpBuf));
			if ((ret = UnionReadXMLPackageValue("desProtectCV",tmpBuf,sizeof(tmpBuf))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0H07:: UnionReadXMLPackageValue[%s][%d]!\n","desProtectCV",i);
				return(ret);
			}

			if (strlen(tmpBuf) > 0)
			{
				if (strcmp(tmpBuf,desProtectCV) != 0)
				{
					UnionUserErrLog("in UnionDealServiceCode0H07:: modeID[%s] desProtectCV[%s] != modeID[%s] desProtectCV[%s] [%d]!\n",tmpModeID,tmpBuf,modeID,desProtectCV,i);
					UnionSetResponseRemark("DES保护密钥校验值错误");
					return(errCodeEssc_CheckValue);
				}
			}

			memset(tmpBuf,0,sizeof(tmpBuf));
			if ((ret = UnionReadXMLPackageValue("sm4ProtectCV",tmpBuf,sizeof(tmpBuf))) < 0)
			{
				UnionUserErrLog("in UnionDealServiceCode0H07:: UnionReadXMLPackageValue[%s][%d]!\n","sm4ProtectCV",i);
				return(ret);
			}

			if (strlen(tmpBuf) > 0)
			{
				if (strcmp(tmpBuf,sm4ProtectCV) != 0)
				{
					UnionUserErrLog("in UnionDealServiceCode0H07:: modeID[%s] sm4ProtectCV[%s] != modeID[%s] sm4ProtectCV[%s] [%d]!\n",tmpModeID,tmpBuf,modeID,sm4ProtectCV,i);
					UnionSetResponseRemark("SM4保护密钥校验值错误");
					return(errCodeEssc_CheckValue);
				}
			}
		}
	}
	
	memset(sql,0,sizeof(sql));
	sprintf(sql,"insert into lmkProtectMode (modeID,modeName,desProtectKey,desProtectCV,sm4ProtectKey,sm4ProtectCV,remark) values ('%s','%s','%s','%s','%s','%s','%s')",modeID,modeName,desProtectKey,desProtectCV,sm4ProtectKey,sm4ProtectCV,remark);
	if ((ret = UnionExecRealDBSql(sql)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0H07:: UnionExecRealDBSql[%s]!\n",sql);
		UnionSetResponseRemark("增加LMK保护方式失败");
		return(ret);
	}

	return(0);
}
