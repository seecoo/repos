
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
服务代码:	0404
服务名:		查询表
功能描述:	查询表
**********************************/

int UnionDealServiceCode0404(PUnionHsmGroupRec phsmGroupRec)
{
	int	j;
	int	ret;
	int	uniqueTotalNum = 0;
	int	indexTotalNum = 0;
	int	foreignkeyTotalNum = 0;
	int	isUpdateCascade = 2;
	int	isDeleteCascade = 2;
	char	tableName[40+1];
	char	tableChnName[40+1];
	char	tableType[128+1];
	char	methodOfCached[10+1];
	char	remark[128+1];
	char	fileName[128+1];
	char	name[128+1];
	char	tmpBuf[512+1];

	// 读取表名
	memset(tableName,0,sizeof(tableName));
	if ((ret = UnionReadRequestXMLPackageValue("body/tableName",tableName,sizeof(tableName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0404:: UnionReadRequestXMLPackageValue[%s]!\n","body/tableName");
		return(ret);
	}

	// 检查表是否已经存在
	memset(fileName,0,sizeof(fileName));
	UnionGetFileNameOfTableDef(tableName,fileName);

	// 文件不存在
	if ((access(fileName,0) < 0) || strlen(tableName) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0404:: fileName[%s] not found!\n",fileName);
		return(errCodeDatabaseMDL_TableNotFound);
	}

	if ((ret = UnionInitXMLPackage(fileName,NULL,0)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0404:: UnionInitXMLPackage[%s]!\n",fileName);
		return(ret);
	}
		
	// 表中文名
	memset(tableChnName,0,sizeof(tableChnName));
	if ((ret = UnionReadXMLPackageValue("tableChnName", tableChnName, sizeof(tableChnName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0404:: UnionReadXMLPackageValue[tableChnName]!\n");
		return(ret);
	}

	if ((ret = UnionSetResponseXMLPackageValue("body/tableChnName",tableChnName)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0404:: UnionSetResponseXMLPackageValue[%s][%s]!\n","body/tableChnName",tableChnName);
		return(ret);
	}

	// 表类型
	memset(tableType,0,sizeof(tableType));
	if ((ret = UnionReadXMLPackageValue("tableType", tableType, sizeof(tableType))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0404:: UnionReadXMLPackageValue[tableType]!\n");
		return(ret);
	}

	if ((ret = UnionSetResponseXMLPackageValue("body/tableType",tableType)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0404:: UnionSetResponseXMLPackageValue[%s][%s]!\n","body/tableType",tableType);
		return(ret);
	}

	// 缓存方法
	memset(methodOfCached,0,sizeof(methodOfCached));
	if ((ret = UnionReadXMLPackageValue("methodOfCached", methodOfCached, sizeof(methodOfCached))) < 0)
	{
		//UnionUserErrLog("in UnionDealServiceCode0404:: UnionReadXMLPackageValue[methodOfCached]!\n");
		//return(ret);
		strcpy(methodOfCached,"0");
	}

	if ((ret = UnionSetResponseXMLPackageValue("body/methodOfCached",methodOfCached)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0404:: UnionSetResponseXMLPackageValue[%s][%s]!\n","body/methodOfCached",methodOfCached);
		return(ret);
	}

	// 备注
	memset(remark,0,sizeof(remark));
	if ((ret = UnionReadXMLPackageValue("remark", remark, sizeof(remark))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0404:: UnionReadXMLPackageValue[remark]!\n");
		return(ret);
	}

	if ((ret = UnionSetResponseXMLPackageValue("body/remark",remark)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0404:: UnionSetResponseXMLPackageValue[%s][%s]!\n","remark",remark);
		return(ret);
	}

	// 设置响应唯一值
	if ((ret = UnionSetResponseXMLPackageValue("body/uniqueTotalNum","0")) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0404:: UnionSetResponseXMLPackageValue[%s]!\n","body/uniqueTotalNum");
		return(ret);	
	}

	// 唯一值字段
	for (j = 0;;j++)
	{
		// 读取的唯一值字段大于预计的
		if (j >= 16)
		{
			UnionUserErrLog("in UnionDealServiceCode0404:: uniqueNum [%d]  > 16!\n",j+1);
			return(errCodeObjectMDL_FieldNumberTooMuch);
		}
	
		if ((ret = UnionLocateXMLPackage("unique", j+1)) < 0)
		{
			UnionLog("in UnionDealServiceCode0404:: UnionLocateXMLPackage unique[%d] not found!\n",j+1);
			break;
		}
	
		// 读取唯一值名称
		memset(name,0,sizeof(name));
		if ((ret = UnionReadXMLPackageValue("name",name,sizeof(name))) <= 0)
		{
			strcpy(name,"");
		}
		// 读取fields
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadXMLPackageValue("fields",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0404::  UnionReadXMLPackageValue[%d][%s] unique\n",j+1,"fields");
			return(ret);
		}

		UnionLocateResponseNewXMLPackage("body/unique",j+1);
		
		// 设置响应唯一值名称
		if ((ret = UnionSetResponseXMLPackageValue("name",name)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0404:: UnionSetResponseXMLPackageValue[%s]!\n","unique/name");
			return(ret);	
		}
		// 设置响应唯一值
		if ((ret = UnionSetResponseXMLPackageValue("fields",tmpBuf)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0404:: UnionSetResponseXMLPackageValue[%s]!\n","fields");
			return(ret);	
		}

		uniqueTotalNum++;
	}

	UnionLocateResponseXMLPackage("",0);
	// 设置响应索引
	if ((ret = UnionSetResponseXMLPackageValue("body/indexTotalNum","0")) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0404:: UnionSetResponseXMLPackageValue[%s]!\n","body/indexTotalNum");
		return(ret);	
	}
		
	// 索引字段
	for (j = 0;;j++)
	{
		// 读取的唯一值字段大于预计的
		if (j >= 16)
		{
			UnionUserErrLog("in UnionDealServiceCode0404:: indexNum [%d]  > 16!\n",j+1);
			return(errCodeObjectMDL_FieldNumberTooMuch);
		}
	
		if ((ret = UnionLocateXMLPackage("index", j+1)) < 0)
		{
			UnionLog("in UnionDealServiceCode0404:: UnionLocateXMLPackage index[%d] not found!\n",j+1);
			break;
		}

		// 读取索引名称
		memset(name,0,sizeof(name));
		if ((ret = UnionReadXMLPackageValue("name",name,sizeof(name))) <= 0)
		{
			strcpy(name,"");
		}
		// 读取fields
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadXMLPackageValue("fields",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0404::  UnionReadXMLPackageValue[%d][%s] index\n",j+1,"fields");
			return(ret);
		}
		
		UnionLocateResponseXMLPackage("body/index",j+1);

		// 设置响应索引名称
		if ((ret = UnionSetResponseXMLPackageValue("name",name)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0404:: UnionSetResponseXMLPackageValue[%s]!\n","index/name");
			return(ret);	
		}
		
		// 设置响应索引字段
		if ((ret = UnionSetResponseXMLPackageValue("fields",tmpBuf)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0404:: UnionSetResponseXMLPackageValue[%s]!\n","fields");
			return(ret);	
		}

		indexTotalNum++;
	}
		
	UnionLocateResponseXMLPackage("",0);
	// 设置响应外键
	if ((ret = UnionSetResponseXMLPackageValue("body/foreignkeyTotalNum","0")) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0404:: UnionSetResponseXMLPackageValue[%s]!\n","body/foreignkeyTotalNum");
		return(ret);	
	}

	// 索引外键
	for (j = 0;;j++)
	{
		// 读取的外键字段大于预计的
		if (j >= 16)
		{
			UnionUserErrLog("in UnionDealServiceCode0404::foreignkeyNum [%d]  > 16!\n",j+1);
			return(errCodeObjectMDL_FieldNumberTooMuch);
		}
	
		if ((ret = UnionLocateXMLPackage("foreignkey", j+1)) < 0)
		{
			UnionLog("in UnionDealServiceCode0404:: UnionLocateXMLPackage foreignkey[%d] not found!\n",j+1);
			break;
		}
	
		UnionLocateResponseNewXMLPackage("body/foreignkey",j+1);

		// 读取外键名称
		memset(name,0,sizeof(name));
		if ((ret = UnionReadXMLPackageValue("name",name,sizeof(name))) <= 0)
		{
			strcpy(name,"");
		}

		if ((ret = UnionSetResponseXMLPackageValue("name", name)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0404:: UnionSetResponseXMLPackageValue[%s]!\n","foreignkey/name");
			return(ret);	
		}

		// 读取外键中的参照表名
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadXMLPackageValue("references", tmpBuf, sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0404::  UnionReadXMLPackageValue[%d][%s] foreignkey\n",j+1,"references");
			return(ret);
		}
			
		// 设置响应外键中的参照表名
		if ((ret = UnionSetResponseXMLPackageValue("references", tmpBuf)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0404:: UnionSetResponseXMLPackageValue[%s]!\n","references");
			return(ret);	
		}

		// 读取外键中本地表的字段名
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadXMLPackageValue("localfields", tmpBuf, sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0404::  UnionReadXMLPackageValue[%d][%s] foreignkey\n",j+1,"localfields");
			return(ret);
		}
			
		// 设置响应外键中本地表的字段名
		if ((ret = UnionSetResponseXMLPackageValue("localfields",tmpBuf)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0404:: UnionSetResponseXMLPackageValue[%s]!\n","localfields");
			return(ret);	
		}

		// 读取外键中参照表的字段名
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadXMLPackageValue("foreignfields",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0404::  UnionReadXMLPackageValue[%d][%s] foreignkey\n",j+1,"foreignfields");
			return(ret);
		}
			
		// 设置响应外键中参照表的字段名
		if ((ret = UnionSetResponseXMLPackageValue("foreignfields",tmpBuf)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0404:: UnionSetResponseXMLPackageValue[%s]!\n","foreignfields");
			return(ret);	
		}
		// 读取外键级联更新
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadXMLPackageValue("isUpdateCascade",tmpBuf,sizeof(tmpBuf))) <= 0)
		{
			strcpy(tmpBuf,"");
		}
		else if (atoi(tmpBuf) == 1 || atoi(tmpBuf) == 0)
			isUpdateCascade = atoi(tmpBuf);
		else
		{
			UnionUserErrLog("in UnionDealServiceCode0404:: isUpdateCascade[%s] error!\n",tmpBuf);
			return(errCodeParameter);
		}

		// 读取外键级联删除
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadXMLPackageValue("isDeleteCascade",tmpBuf,sizeof(tmpBuf))) <= 0)
		{
			strcpy(tmpBuf,"");
		}
		else if (atoi(tmpBuf) == 1 || atoi(tmpBuf) == 0)
			isDeleteCascade = atoi(tmpBuf);
		else
		{
			UnionUserErrLog("in UnionDealServiceCode0404:: isDeleteCascade[%s] error!\n",tmpBuf);
			return(errCodeParameter);
		}
	
		// 设置响应外键中的级联更新	
		memset(tmpBuf,0,sizeof(tmpBuf));
		sprintf(tmpBuf,"%d",isUpdateCascade);
		if ((ret = UnionSetResponseXMLPackageValue("isUpdateCascade",tmpBuf)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0404:: UnionSetResponseXMLPackageValue[%s]!\n","isUpdateCascade");
			return(ret);	
		}

		// 设置响应外键中的级联删除
		memset(tmpBuf,0,sizeof(tmpBuf));
		sprintf(tmpBuf,"%d",isDeleteCascade);
		if ((ret = UnionSetResponseXMLPackageValue("isDeleteCascade",tmpBuf)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0404:: UnionSetResponseXMLPackageValue[%s]!\n","isDeleteCascade");
			return(ret);	
		}
		
		foreignkeyTotalNum++;
	}
	
	UnionLocateResponseXMLPackage("",0);
	//
	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"%d",uniqueTotalNum);
	if ((ret = UnionSetResponseXMLPackageValue("body/uniqueTotalNum",tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0404:: UnionSetResponseXMLPackageValue[%s]!\n","body/uniqueTotalNum");
		return(ret);	
	}

	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"%d",indexTotalNum);
	if ((ret = UnionSetResponseXMLPackageValue("body/indexTotalNum",tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0404:: UnionSetResponseXMLPackageValue[%s]!\n","body/uniqueTotalNum");
		return(ret);	
	}

	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"%d",foreignkeyTotalNum);
	if ((ret = UnionSetResponseXMLPackageValue("body/foreignkeyTotalNum",tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0404:: UnionSetResponseXMLPackageValue[%s]!\n","body/foreignkeyTotalNum");
		return(ret);	
	}

	return 0;
}
