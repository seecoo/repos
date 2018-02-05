
#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/dir.h>

#include "UnionLog.h"
#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionTableDef.h"
#include "unionRealBaseDB.h"
#include "baseUIService.h"

/*********************************
服务代码:	0410
服务名:		查询表清单
功能描述:	查询表清单
**********************************/

int UnionDealServiceCode0410(PUnionHsmGroupRec phsmGroupRec)
{
	int	i;
	int	ret;
	int	tableNum = 0;
	char	tmpBuf[512+1];
	char	tableType[128+1];
	char	fileName[1024+1];
	char	methodOfCached[10+1];
	char	remark[128+1];
	PUnionTableDefTBL	ptableDefTBL = NULL;

	// 读取字段类型
	memset(tableType,0,sizeof(tableType));
	if ((ret = UnionReadRequestXMLPackageValue("body/tableType",tableType,sizeof(tableType))) < 0)
	{
		//UnionUserErrLog("in UnionDealServiceCode0410:: UnionReadRequestXMLPackageValue[%s]!\n","body/tableType");
		//return(ret);
	}

	if ((ptableDefTBL = UnionGetTableDefTBL()) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCode0410:: UnionGetTableDefTBL!\n");
		return(errCodeParameter);
	}

	UnionSetResponseXMLPackageValue("body/totalNum","0");

	UnionLog("in UnionDealServiceCode0410:: ptableDefTBL->tableNum[%d]\n",ptableDefTBL->tableNum);
	
	for (i = 0; i < ptableDefTBL->tableNum; i++)
	{
		if ((strlen(tableType) > 0) && (strcmp(tableType,ptableDefTBL->rec[i].tableType) != 0))
			continue;

		if ((ret = UnionLocateResponseNewXMLPackage("body/detail", tableNum+1)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0410:: UnionLocateResponseNewXMLPackage[%d]!\n",tableNum+1);
			return(ret);
		}

		// 设置表名
		if ((ret = UnionSetResponseXMLPackageValue("tableName",ptableDefTBL->rec[i].tableName)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0410:: UnionSetResponseXMLPackageValue[%d][%s][%s]!\n",i,"tableName",ptableDefTBL->rec[i].tableName);
			return(ret);
		}

		// 设置表中文名
		if ((ret = UnionSetResponseXMLPackageValue("tableChnName",ptableDefTBL->rec[i].tableChnName)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0410:: UnionSetResponseXMLPackageValue[%d][%s][%s]!\n",i,"tableChnName",ptableDefTBL->rec[i].tableChnName);
			return(ret);
		}

		// 设置表类型
		if ((ret = UnionSetResponseXMLPackageValue("tableType",ptableDefTBL->rec[i].tableType)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0410:: UnionSetResponseXMLPackageValue[%d][%s][%s]!\n",i,"tableType",ptableDefTBL->rec[i].tableType);
			return(ret);
		}
		
		// 获取文件路径
		memset(fileName,0,sizeof(fileName));
		UnionGetFileNameOfTableDef(ptableDefTBL->rec[i].tableName,fileName);
		
		// 初始化文件
		if ((ret = UnionInitXMLPackage(fileName,NULL,0)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0410:: UnionInitXMLPackage[%s]!\n",fileName);
			return(ret);
		}
	
		// 读取高速缓存方法	
		memset(methodOfCached,0,sizeof(methodOfCached));
		if ((ret = UnionReadXMLPackageValue("methodOfCached",methodOfCached,sizeof(methodOfCached))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0410:: UnionReadXMLPackageValue[methodOfCached]!\n");
			strcpy(methodOfCached,"0");
			//return(ret);
		}
		
		// 设置高速缓存方法
		if ((ret = UnionSetResponseXMLPackageValue("methodOfCached",methodOfCached)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0410:: UnionSetResponseXMLPackageValue[%d][%s][%s]!\n",i,"methodOfCached",methodOfCached);
			return(ret);
		}

		// 读取备注
		memset(remark,0,sizeof(remark));
		if ((ret = UnionReadXMLPackageValue("remark",remark,sizeof(remark))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0410:: UnionReadXMLPackageValue[remark]!\n");
			return(ret);
		}
		
		// 设置备注
		if ((ret = UnionSetResponseXMLPackageValue("remark",remark)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0410:: UnionSetResponseXMLPackageValue[%d][%s][%s]!\n",i,"remark",remark);
			return(ret);
		}
		
		tableNum ++;
	}
	UnionLog("in UnionDealServiceCode0410:: tableNum[%d]\n",tableNum);


	UnionLocateResponseXMLPackage("",0);
	
	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"%d",tableNum);
	UnionSetResponseXMLPackageValue("body/totalNum",tmpBuf);

	return 0;
}
