
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
#include <sys/stat.h>

#include "UnionLog.h"
#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionTableDef.h"
#include "unionRealBaseDB.h"
#include "unionRecFile.h"
#include "UnionStr.h"
#include "baseUIService.h"

/*********************************
服务代码:	0308
服务名:		文件打包
功能描述:	文件打包
**********************************/

int UnionDealServiceCode0308(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret;
	int	i;
	char	fileNameList[8192];
	int	maxFileNum = 500;
	char	fileNameGrp[maxFileNum][128];
	char	fileTypeID[32];
	char	fileStoreDir[512];
	int	downFlag = 0;	
	char	downFileFullName[512];
	char	downFileDir[512];
	int	fileNum = 0;
	char	sql[1024];
	char	fileDir[1024];	
	char	tmpBuf[1024];
	char	*tmpEnv;
	char	dateTime[32];
	int	len = 0;
	int	lenOfFileFullName = 0;
	char	path[512];

	 // 文件名
	memset(fileNameList,0,sizeof(fileNameList));
	if ((ret = UnionReadRequestXMLPackageValue("body/fileNameList",fileNameList,sizeof(fileNameList))) > 0)
	{
		UnionFilterHeadAndTailBlank(fileNameList);

		// 拼分域定义串
		memset(fileNameGrp,0,sizeof(fileNameGrp));
		if ((fileNum = UnionSeprateVarStrIntoVarGrp(fileNameList,strlen(fileNameList),',',fileNameGrp,maxFileNum)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0308:: UnionSeprateVarStrIntoVarGrp[%s]!\n",fileNameList);
			return(fileNum);
		}
	}

	// 读取文件类型
	memset(fileTypeID,0,sizeof(fileTypeID));
	if ((ret = UnionReadRequestXMLPackageValue("body/fileTypeID",fileTypeID,sizeof(fileTypeID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0308:: UnionReadRequestXMLPackageValue[%s]!\n","body/fileTypeID");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(fileTypeID);
	if (strlen(fileTypeID) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0308:: fileTypeID can not be null!\n");
		UnionSetResponseRemark("文件打包类型不能为空");
		return(errCodeFileTransSvrNoFileName);
	}

	// 打包方式
	// 0: 根据文件名打包
	// 1: 全部打包
	if (strlen(fileNameList) > 0)
		downFlag = 0;
	else
		downFlag = 1;
	
	//查询文件存储路径
	memset(sql,0,sizeof(sql));
	sprintf(sql,"select fileStoreDir from fileType where fileTypeID = '%s'",fileTypeID);
	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0308:: UnionSelectRealDBRecord[%s]!\n",sql);
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0308:: fileTypeID[%s] not found!\n",fileTypeID);
		UnionSetResponseRemark("文件类型[%s]不存在",fileTypeID);	
		return(errCodeParameter);
	}
	
	if ((ret = UnionLocateXMLPackage("detail",1)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0308:: UnionLocateXMLPackage[%s]!\n","detail");
		return(ret);
	}

	// 读取文件存储目录
	memset(fileStoreDir,0,sizeof(fileStoreDir));
	if ((ret = UnionReadXMLPackageValue("fileStoreDir",fileStoreDir,sizeof(fileStoreDir))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0308:: UnionReadXMLPackageValue[%s]!\n","fileStoreDir");
		return(ret);
	}
	
	if ((ret = UnionReadDirFromStr(fileStoreDir,-1,path)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0308:: UnionReadDirFromStr[%s]!\n","fileStoreDir");
		UnionSetResponseRemark("获取文件打包路径失败");
		return(ret);
	}

	memset(downFileFullName,0,sizeof(downFileFullName));
	memset(tmpBuf,0,sizeof(tmpBuf));
	tmpEnv = getenv("UNIONREC");
	memset(downFileDir,0,sizeof(downFileDir));
	sprintf(downFileDir,"%s/fileDir/download",tmpEnv);
	memset(dateTime,0,sizeof(dateTime));
	UnionGetFullSystemDateTime(dateTime);
	sprintf(downFileFullName,"%s/down%sFile.%s.tar",downFileDir,fileTypeID,dateTime);

	// 检测目录是否存在
	if (access(downFileDir,0) < 0)
	{
		if (mkdir(downFileDir,0755) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0308:: mkdir[%s]\n",downFileDir);
			return(-1);
		}
	}

	// 检测打包的文件是否存在
	if (access(downFileFullName,0) >= 0)
	{
		// 存在则删除
		if ((ret = unlink(downFileFullName)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0308:: delete fileName[%s] is failed!\n",downFileFullName);
			return(ret);
		}
	}
	

	if (downFlag == 0)
	{
		memset(fileNameList,0,sizeof(fileNameList));
		for (i = 0; i < fileNum; i++)
		{
			// 检查文件是否存在
			lenOfFileFullName = sprintf(fileDir,"%s/%s",path,fileNameGrp[i]);
			fileDir[lenOfFileFullName] = 0;
			if ((access(fileDir,0) < 0))
			{
				UnionUserErrLog("in UnionDealServiceCode0308:: filFulleName[%s] not found!\n",fileDir);
				return(errCodeMngSvrFileNotExists);
			}
			len += sprintf(fileNameList + len,"%s ",fileNameGrp[i]);			
		}
		
		chdir(path);
		len = sprintf(tmpBuf,"tar cf %s %s",downFileFullName,fileNameList);
		tmpBuf[len] = 0;
		system(tmpBuf);
	}
	else
	{
		len = sprintf(tmpBuf,"tar cf %s %s/*",downFileFullName,path);
		tmpBuf[len] = 0;
		system(tmpBuf);
	}

	// 设置响应文件目录
	if ((ret = UnionSetResponseXMLPackageValue("body/fileFullName",downFileFullName)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0308:: UnionSetResponseXMLPackageValue[%s]!\n","body/fileFullName");
		return(ret);
	}

	return 0;
}
