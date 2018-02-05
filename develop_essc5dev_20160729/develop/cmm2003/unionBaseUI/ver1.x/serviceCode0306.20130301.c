
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
#include "unionRecFile.h"
#include "UnionStr.h"
#include "baseUIService.h"

/*********************************
服务代码:	0306
服务名:		下载文件
功能描述:	下载文件
**********************************/

int UnionDealServiceCode0306(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret;
	char	fileName[128+1];
	char	fileTypeID[4+1];
	char	fileStoreDir[256+1];
	char	sql[1024+1];
	char	fileDir[512+1];	
	char	tmpBuf[8];
	int	mode = 0;

	 // 文件名
	memset(fileName,0,sizeof(fileName));
	if ((ret = UnionReadRequestXMLPackageValue("body/fileName",fileName,sizeof(fileName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0306:: UnionReadRequestXMLPackageValue[%s]!\n","body/fileName");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(fileName);
	if (strlen(fileName) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0306:: fileName can not be null!\n");
		UnionSetResponseRemark("下载文件名不能为空");
		return(errCodeFileTransSvrNoFileName);
	}
	// 读取文件类型
	memset(fileTypeID,0,sizeof(fileTypeID));
	if ((ret = UnionReadRequestXMLPackageValue("body/fileTypeID",fileTypeID,sizeof(fileTypeID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0306:: UnionReadRequestXMLPackageValue[%s]!\n","body/fileTypeID");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(fileTypeID);
	if (strlen(fileTypeID) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0306:: fileTypeID can not be null!\n");
		UnionSetResponseRemark("下载文件类型不能为空");
		return(errCodeFileTransSvrNoFileName);
	}

	// 读取处理方式 默认为0 
	// 0:下载文件
	// 1:删除文件
	if ((ret = UnionReadRequestXMLPackageValue("body/mode",tmpBuf,sizeof(tmpBuf))) > 0)
	{
		mode = atoi(tmpBuf);
	}
	
	//查询文件存储路径
	memset(sql,0,sizeof(sql));
	sprintf(sql,"select fileStoreDir from fileType where fileTypeID = '%s'",fileTypeID);
	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0306:: UnionSelectRealDBRecord[%s]!\n",sql);
		return(ret);
	}
	
	if ((ret = UnionLocateXMLPackage("detail",1)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0306:: UnionLocateXMLPackage[%s]!\n","detail");
		return(ret);
	}

	// 读取文件存储目录
	memset(fileStoreDir,0,sizeof(fileStoreDir));
	if ((ret = UnionReadXMLPackageValue("fileStoreDir",fileStoreDir,sizeof(fileStoreDir))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0306:: UnionReadXMLPackageValue[%s]!\n","fileStoreDir");
		return(ret);
	}

	// 检查文件是否存在
	memset(fileDir,0,sizeof(fileDir));
	UnionFormFullFileName(fileStoreDir,fileName,fileDir);
	if ((access(fileDir,0) < 0) || strlen(fileName) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0306:: fileName[%s] not found!\n",fileName);
		return(errCodeMngSvrFileNotExists);
	}

	if (mode == 1)
	{
		if (unlink(fileDir) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0306:: unlink[%s] is failed!\n", fileDir);
			UnionSetResponseRemark("删除文件[%s]失败",fileName);
			return(errCodeUseOSErrCode);
		}
	}
	else
	{
		// 设置响应文件目录
		memset(fileDir,0,sizeof(fileDir));
		UnionReadDirFromStr(fileStoreDir,-1,fileDir);
		if ((ret = UnionSetResponseXMLPackageValue("body/fileDir",fileDir)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0306:: UnionSetResponseXMLPackageValue[%s]!\n","body/fileDir");
			return(ret);
		}
	}

	return 0;
}
