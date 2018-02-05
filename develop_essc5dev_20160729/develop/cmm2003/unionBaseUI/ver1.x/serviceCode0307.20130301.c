
#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/dir.h>
#include <time.h>

#include "UnionLog.h"
#include "UnionStr.h"
#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionRealBaseDB.h"
#include "unionREC.h"
#include "unionRecFile.h"
#include "baseUIService.h"

#define FILE_NAME_MAX 256

/*********************************
服务代码:	0307
服务名:		获取文件清单
功能描述:	获取文件清单
**********************************/

int UnionDealServiceCode0307(PUnionHsmGroupRec phsmGroupRec)
{
	int		ret;
	int		i = 0;
	int		j = 0;
	int		fileNum = 0;
	int		fileTotalNum = 0;
	int		totalNum = 0;
	char		fullFileName[1024+1];
	char		fileTypeID[4+1];
	char		fileStoreDir[512+1];
	char		fileDir[512+1];
	char		fileTime[20+1];
	char		tmpBuf[512+1];
	int		tmpNum = 0;
	char		sql[1024+1];
	char		order_by[20+1];
	int		sortFlag = 1;
	int		sortType = 1;
	int		currentPage = 0;
	int		numOfPerPage = 1;	
	int		allTotalNum = 0;
	struct dirent	**nameList;
	struct stat	fileInfo;
	struct tm	*fileTM;

	typedef struct
	{
		char	fileName[FILE_NAME_MAX+1];
		char	fileTime[19+1];
		long	fileSize;
	}TUnionFileInfo;
	
	typedef TUnionFileInfo		*PUnionFileInfo;
	
	PUnionFileInfo			pfileInfo = NULL;
	TUnionFileInfo			tfileInfo;

	memset(&tfileInfo,0,sizeof(TUnionFileInfo));

	// 读取文件类型
	memset(fileTypeID,0,sizeof(fileTypeID));
	if ((ret = UnionReadRequestXMLPackageValue("body/fileTypeID",fileTypeID,sizeof(fileTypeID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0307:: UnionReadRequestXMLPackageValue[%s]!\n","body/fileTypeID");
		return(ret);
	}
	UnionFilterHeadAndTailBlank(fileTypeID);
	if (strlen(fileTypeID) == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0307:: fileTypeID can not be null!\n");
		UnionSetResponseRemark("文件类型不能为空");
		return(errCodeParameter);
	}

	// 获取当前页码
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRequestXMLPackageValue("body/currentPage",tmpBuf,sizeof(tmpBuf))) <= 0)
	{
		//UnionUserErrLog("in UnionDealServiceCode0307:: UnionReadRequestXMLPackageValue[%s]!\n","body/fileTypeID");
		//return(ret);
	}
	else
		currentPage = atoi(tmpBuf);

	// 获取每页的数量
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRequestXMLPackageValue("body/numOfPerPage",tmpBuf,sizeof(tmpBuf))) <= 0)
	{
		//UnionUserErrLog("in UnionDealServiceCode0307:: UnionReadRequestXMLPackageValue[%s]!\n","body/fileTypeID");
		//return(ret);
	}
	else
		numOfPerPage = atoi(tmpBuf);

	// 获取文件排序
	memset(order_by,0,sizeof(order_by));
	if ((ret = UnionReadRequestXMLPackageValue("body/order_by",order_by,sizeof(order_by))) <= 0)
	{
		strcpy(order_by,"fileName asc");	
	}
	UnionFilterHeadAndTailBlank(order_by);

	// 检测排序类型
	if (strncasecmp(order_by,"fileName",8) == 0)
		sortType = 1;

	if (strncasecmp(order_by,"fileTime",8) == 0)
		sortType = 2;

	if (strncasecmp(order_by,"fileSize",8) == 0)
		sortType = 3;

	// 检测排序方式
	memset(tmpBuf,0,sizeof(tmpBuf));
	memcpy(tmpBuf,order_by + 8,strlen(order_by) - 8);
	UnionFilterHeadAndTailBlank(tmpBuf);

	if (strcasecmp(tmpBuf,"asc") == 0)
		sortFlag = 1;
	else if (strcasecmp(tmpBuf,"desc") == 0)
		sortFlag = 2;

	//查询文件存储路径
	memset(sql,0,sizeof(sql));
	sprintf(sql,"select fileStoreDir from fileType where fileTypeID = '%s'",fileTypeID);
	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0307:: UnionSelectRealDBRecord[%s]!\n",sql);
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0307:: fileTypeID[%s]!\n",fileTypeID);
		return(errCodeDatabaseMDL_RecordNotFound);
	}
	
	if ((ret = UnionLocateXMLPackage("detail",1)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0307:: UnionLocateXMLPackage[%s]!\n","detail");
		return(ret);
	}

	// 读取文件存储目录
	memset(fileStoreDir,0,sizeof(fileStoreDir));
	if ((ret = UnionReadXMLPackageValue("fileStoreDir",fileStoreDir,sizeof(fileStoreDir))) <= 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0307:: UnionReadXMLPackageValue[%s]!\n","fileStoreDir");
		return(ret);
	}
	
	memset(fileDir,0,sizeof(fileDir));
	UnionReadDirFromStr(fileStoreDir,-1,fileDir);
	

	// 设置字段清单
	memset(tmpBuf,0,sizeof(tmpBuf));
	strcpy(tmpBuf,"fileName,fileTime,fileSize");
	if ((ret = UnionSetResponseXMLPackageValue("body/fieldList",tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0307:: UnionSetResponseXMLPackageValue[%s]!\n","body/fieldList");
		return(ret);
	}

	// 设置字段中文名
	memset(tmpBuf,0,sizeof(tmpBuf));
	strcpy(tmpBuf,"文件名,文件修改时间,文件大小");
	if ((ret = UnionSetResponseXMLPackageValue("body/fieldListChnName",tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0307:: UnionSetResponseXMLPackageValue[%s]!\n","body/fieldListChnName");
		return(ret);
	}
	
	// 打开目录
	if ((fileTotalNum = scandir(fileDir,&nameList,0,alphasort)) < 0)
	{
		UnionSystemErrLog("in UnionDealServiceCode0307:: scandir [%s] error!\n",fileDir);
		free(nameList);
		UnionSetResponseRemark("打开文件目录失败");
		return(fileTotalNum);
	}

	// 读取总数量
	for(i = 0; i < fileTotalNum; i++)
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		sprintf(tmpBuf,"%s",nameList[i]->d_name);
		if (tmpBuf[0] == '.')
			continue;
		allTotalNum++;
	}
	
	if (currentPage > 0)
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		sprintf(tmpBuf,"%d",allTotalNum);
		if ((ret = UnionSetResponseXMLPackageValue("body/allTotalNum",tmpBuf)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0307:: UnionSetResponseXMLPackageValue[%s]!\n","body/allTotalNum");
			free(nameList);
			return(ret);
		}
		
		// 设置记录数
		UnionSetResponseXMLPackageValue("body/totalNum","0");
		
		tmpNum = numOfPerPage;
	}
	else
	{
		// 设置记录数
		memset(tmpBuf,0,sizeof(tmpBuf));
		sprintf(tmpBuf,"%d",allTotalNum);
		if ((ret = UnionSetResponseXMLPackageValue("body/totalNum",tmpBuf)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0307:: UnionSetResponseXMLPackageValue[%s]!\n","body/totalNum");
			free(nameList);
			return(ret);
		}

		tmpNum = allTotalNum;
	}

	//申请动态空间
	if ((pfileInfo = (PUnionFileInfo)malloc(sizeof(TUnionFileInfo) * tmpNum)) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCode0307:: malloc failed!\n");
		UnionSetResponseRemark("为存储文件信息，申请动态空间失败");
		return(errCodeParameter);
	}

	memset(pfileInfo,0,sizeof(TUnionFileInfo) * tmpNum);
	
	// 获取文件信息
	for(j = 0; j < fileTotalNum; j++)
	{
		// 倒叙输出
		if (sortFlag == 2)
			i = fileTotalNum - j - 1;
		else
			i = j;

		memset(tmpBuf,0,sizeof(tmpBuf));
		sprintf(tmpBuf,"%s",nameList[i]->d_name);
		if (tmpBuf[0] == '.')
			continue;

		if (currentPage > 0)
		{
			fileNum++;
			if (fileNum <= (currentPage - 1) * numOfPerPage)
				continue;
			else if (totalNum >= numOfPerPage)
				break;
		}
	
		memset(fullFileName,0,sizeof(fullFileName));
		sprintf(fullFileName,"%s/%s",fileDir,nameList[i]->d_name);
		
		memset(&fileInfo,0,sizeof(fileInfo));
		if ((ret = stat(fullFileName,&fileInfo)) < 0)
		{
			UnionSystemErrLog("in UnionDealServiceCode0307:: stat [%s] error!\n",fullFileName);
			break;
		}
		
		// 获取文件名
		sprintf((pfileInfo + totalNum)->fileName,"%s",nameList[i]->d_name);	

		// 获取文件时间
		fileTM = localtime(&fileInfo.st_mtime);
		memset(fileTime,0,sizeof(fileTime));
		strftime(fileTime,sizeof(fileTime),"%F %T",fileTM);
		sprintf((pfileInfo + totalNum)->fileTime,"%s",fileTime);
		
		// 获取文件大小
		(pfileInfo + totalNum)->fileSize = fileInfo.st_size;

		totalNum++;
	}
	free(nameList);


	// 按要求排序
	if (sortType == 2 || sortType == 3)
	{
		for (i = 0; i < totalNum; i++)
			for (j = totalNum - 1; j > i; j--)
			{
				if (sortType == 2)
				{
					if (sortFlag == 1)
					{
						if (strcmp((pfileInfo + j)->fileTime,(pfileInfo + j -1)->fileTime) < 0)
						{
							tfileInfo = *(pfileInfo + j);
							*(pfileInfo + j) = *(pfileInfo + j - 1);
							*(pfileInfo + j - 1) = tfileInfo;
						}
					}
					else
					{
						if (strcmp((pfileInfo + j)->fileTime,(pfileInfo + j -1)->fileTime) > 0)
						{
							tfileInfo = *(pfileInfo + j);
							*(pfileInfo + j) = *(pfileInfo + j - 1);
							*(pfileInfo + j - 1) = tfileInfo;
						}
					}
				}
				else
				{
					if (sortFlag == 1)
					{
						if ((pfileInfo + j)->fileSize - (pfileInfo + j - 1)->fileSize < 0)
						{
							tfileInfo = *(pfileInfo + j);
							*(pfileInfo + j) = *(pfileInfo + j - 1);
							*(pfileInfo + j - 1) = tfileInfo;
						}
					}
					else
					{
						if ((pfileInfo + j)->fileSize - (pfileInfo + j - 1)->fileSize > 0)
						{
							tfileInfo = *(pfileInfo + j);
							*(pfileInfo + j) = *(pfileInfo + j - 1);
							*(pfileInfo + j - 1) = tfileInfo;
						}
					}
				}
			}
	}
	
	for (i = 0; i < totalNum; i++)
	{
		// 设置ID
		if ((ret = UnionLocateResponseNewXMLPackage("body/detail", i+1)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0307:: UnionLocateResponseNewXMLPackage!\n");
			free(pfileInfo);
			return(ret);
		}

		// 设置文件名
		if ((ret = UnionSetResponseXMLPackageValue("fileName",(pfileInfo + i)->fileName)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0307:: UnionSetResponseXMLPackageValue[%s][%s]!\n","fileName",(pfileInfo + i)->fileName);
			free(pfileInfo);
			return(ret);
		}
	
		// 设置文件时间
		if ((ret = UnionSetResponseXMLPackageValue("fileTime",(pfileInfo + i)->fileTime)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0307:: UnionSetResponseXMLPackageValue[%s][%s]!\n","fileTime",(pfileInfo + i)->fileTime);
			free(pfileInfo);
			return(ret);
		}
		
		// 设置文件大小
		memset(tmpBuf,0,sizeof(tmpBuf));
		sprintf(tmpBuf,"%ld",(pfileInfo + i)->fileSize);
		if ((ret = UnionSetResponseXMLPackageValue("fileSize",tmpBuf)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0307:: UnionSetResponseXMLPackageValue[%s][%s]!\n","fileSize",tmpBuf);
			free(pfileInfo);
			return(ret);
		}
	}
	
	free(pfileInfo);

	if (currentPage > 0 && allTotalNum > 0)
	{
		if (totalNum == 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0307:: currentPage[%d] error!\n",currentPage);
			UnionSetResponseRemark("当前页不存在");
			return(errCodeParameter);
		}
	}

	UnionLocateResponseXMLPackage("",0);
	
	// 赋值记录数量
	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"%d",totalNum);
	if ((ret = UnionSetResponseXMLPackageValue("body/totalNum",tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0307:: UnionSetResponseXMLPackageValue[%s]!\n","body/totalNum");
		return(ret);
	}

	return 0;
}
