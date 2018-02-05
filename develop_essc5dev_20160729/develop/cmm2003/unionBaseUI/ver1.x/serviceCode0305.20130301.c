
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
#include "unionREC.h"
#include "unionRecFile.h"
#include "UnionStr.h"
#include "baseUIService.h"

/*********************************
服务代码:	0305
服务名:		上传文件
功能描述:	上传文件
**********************************/

int UnionDealServiceCode0305(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret;
	char	fileName[128+1];
	char	fileTypeID[4+1];
	char	fileStoreDir[256+1];
	char	sql[1024+1];
	char	oriFileName[512+1];
	char	newFileName[512+1];
	char	*ptr = NULL;
	
	 // 文件名
	memset(fileName,0,sizeof(fileName));
	if ((ret = UnionReadRequestXMLPackageValue("body/fileName",fileName,sizeof(fileName))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0305:: UnionReadRequestXMLPackageValue[%s]!\n","body/fileName");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(fileName);
	if (strlen(fileName) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0305:: fileName can not be null!\n");
		UnionSetResponseRemark("上传文件名不能为空");
		return(errCodeFileTransSvrNoFileName);
	}
	// 读取文件类型
	memset(fileTypeID,0,sizeof(fileTypeID));
	if ((ret = UnionReadRequestXMLPackageValue("body/fileTypeID",fileTypeID,sizeof(fileTypeID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0305:: UnionReadRequestXMLPackageValue[%s]!\n","body/fileTypeID");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(fileTypeID);
	if (strlen(fileTypeID) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0305:: fileTypeID can not be null!\n");
		UnionSetResponseRemark("上传文件类型不能为空");
		return(errCodeParameter);
	}
	
	
	//查询文件存储路径
	memset(sql,0,sizeof(sql));
	sprintf(sql,"select fileStoreDir from fileType where fileTypeID = '%s'",fileTypeID);
	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0305:: UnionSelectRealDBRecord[%s]!\n",sql);
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0305:: fileTypeID[%s]!\n",fileTypeID);
		return(errCodeDatabaseMDL_RecordNotFound);
	}
	
	if ((ret = UnionLocateXMLPackage("detail",1)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0305:: UnionLocateXMLPackage[%s]!\n","detail");
		return(ret);
	}

	// 读取文件存储目录
	memset(fileStoreDir,0,sizeof(fileStoreDir));
	if ((ret = UnionReadXMLPackageValue("fileStoreDir",fileStoreDir,sizeof(fileStoreDir))) <= 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0305:: UnionReadXMLPackageValue[%s]!\n","fileStoreDir");
		return(ret);
	}

	// 获取文件接收目录
	if ((ptr = UnionReadStringTypeRECVar("defaultDirOfFileReceiver")) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCode0305:: UnionReadStringTypeRECVar[%s]!\n","defaultDirOfFileReceiver");
		return(errCodeRECMDL_VarNotExists);
	}

	memset(oriFileName,0,sizeof(oriFileName));
	UnionFormFullFileName(ptr,fileName,oriFileName);
	memset(newFileName,0,sizeof(newFileName));
	UnionFormFullFileName(fileStoreDir,fileName,newFileName);
	
	UnionLog("in UnionDealServiceCode0305:: oriFileName[%s] to newFileName[%s]!\n",oriFileName,newFileName);

	// 检查要移动的文件是否存在
	if (access(oriFileName,0) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0305:: oriFileName[%s] not found!\n",oriFileName);
		return(errCodeMngSvrFileNotExists);
	}	

	// 移动文件到指定目录
	if ((ret = rename(oriFileName,newFileName)) < 0)		
	{
		UnionUserErrLog("in UnionDealServiceCode0305:: rename[%s] to [%s] is failed!\n",oriFileName,newFileName);
		return(ret);
	}

	return 0;
}
