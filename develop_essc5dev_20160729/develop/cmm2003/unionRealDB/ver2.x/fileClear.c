// Copyright:	Union Tech.
// Author:	lics
// Date:	2014-04-03
// Version:	1.0

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>

#include "unionREC.h"
#include "UnionLog.h"
#include "UnionStr.h"
#include "unionRealBaseDB.h"
#include "unionXMLPackage.h"

int UnionDeleteFileByDays(char *fileDir,int days,int clearMode)
{
	int			ret = 0;
	int 			fileTotalNum = 0;
	int 			j = 0,len = 0,i = 0;
	char			fullFileName[512+1];
	char			fileName[256+1];
	char 			fileDate[14+1];
	char			oldDate[14+1];	
	
	struct dirent		**nameList;
	struct stat		fileInfo;
	struct tm		*fileTM;

	// ��ȡǰN�������
	memset(oldDate,0,sizeof(oldDate));
	UnionGetDateBeforSpecDays(days,oldDate);
	
	// ��Ŀ¼
	if ((fileTotalNum = scandir(fileDir,&nameList,0,alphasort)) < 0)
	{
		UnionUserErrLog("in UnionDeleteFileByDays:: scandir [%s] error!\n",fileDir);
		//free(nameList);
		UnionSetResponseRemark("���ļ�Ŀ¼ʧ��");
		return(fileTotalNum);
	}
	
	for(j = 0; j < fileTotalNum; j++)
	{
		len = sprintf(fileName,"%s",nameList[j]->d_name);			
		if (fileName[0] == '.')
			continue;
		fileName[len] = 0;

		len = sprintf(fullFileName,"%s/%s",fileDir,fileName);
		fullFileName[len] = 0;
		if (clearMode == 1)	// �����ļ�������
		{
			len = 0;
			for (i = 0; i < strlen(fileName); i++)
			{
				if (UnionIsDigitChar(fileName[i]))
					len += sprintf(fileDate + len,"%c",fileName[i]);
				else
					len = 0;

				if (len == 8)
					break;
			}
			fileDate[len] = 0;
			if (!UnionIsValidFullDateStr(fileDate))
				continue;
		}
		else if (clearMode == 2)	// �����ļ�����
		{
			memset(&fileInfo,0,sizeof(fileInfo));
			if ((ret = stat(fullFileName,&fileInfo)) < 0)
			{
				UnionUserErrLog("in UnionDeleteFileByDays:: stat [%s] error!\n",fullFileName);
				continue;
			}
			
			// ��ȡ�ļ�ʱ��
			fileTM = localtime(&fileInfo.st_mtime);
			memset(fileDate,0,sizeof(fileDate));
			strftime(fileDate,sizeof(fileDate),"%Y%m%d",fileTM);
		}

		// ����Ƿ���Ҫɾ���ļ�
		if (strcmp(fileDate,oldDate) <= 0)
		{
			if ((ret = unlink(fullFileName)) < 0)
			{
				UnionUserErrLog("in UnionDeleteFileByDays:: unlink[%s] error!\n",fullFileName);
				continue;	
			}
			else
				UnionLog("in UnionDeleteFileByDays:: delete[%s]!\n",fullFileName);
		}
	}
	free(nameList);
	return 0;
}

int UnionDealDaemonTask(char *parameter)
{
	int		ret;
	int		i = 0;
	int		totalNum = 0;
	int		len = 0;
	char		sql[1024+1];
	char		fileDir[512+1];
	int		days = 0;
	char		tmpNum[10+1];
	char		fileStoreDir[1024+1];
	int		clearMode = 0;

	// ��ѯҪɾ����Ŀ¼�ļ�
	len = sprintf(sql,"select * from fileClear where enabled = 1 and days > 0");
	sql[len] = 0;
	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in UnionDealDaemonTask:: UnionSelectRealDBRecord sql[%s]!\n",sql);
		return(ret);
	}
	else if (ret == 0)
	{
		UnionLog("in UnionDealDaemonTask:: UnionSelectRealDBRecord count=[0], sql[%s]!\n",sql);
		return(ret);
	}

	memset(tmpNum,0,sizeof(tmpNum));
	if ((ret = UnionReadXMLPackageValue("totalNum",tmpNum,sizeof(tmpNum))) < 0)
	{
		UnionUserErrLog("in UnionDealDaemonTask:: UnionReadXMLPackageValue [%s]!\n","totalNum");
		return(ret);
	}
	totalNum = atoi(tmpNum);
		
	for (i = 1; i <= totalNum; i++)
	{
		if ((ret = UnionLocateXMLPackage("detail",i)) < 0)
		{
			UnionUserErrLog("in UnionDealDaemonTask:: UnionLocateXMLPackage [%s][%d]!\n","detail",i);
			continue;
		}

		// �ļ�Ŀ¼
		if ((ret = UnionReadXMLPackageValue("fileDir",fileDir,sizeof(fileDir))) < 0)
		{
			UnionUserErrLog("in UnionDealDaemonTask:: UnionReadXMLPackageValue [%s][%d]!\n","fileDir",i);
			continue;
		}
		fileDir[ret] = 0;

		memset(fileStoreDir,0,sizeof(fileStoreDir));
		UnionReadDirFromStr(fileDir,-1,fileStoreDir);			
		// ����ģʽ
		memset(tmpNum,0,sizeof(tmpNum));
		if ((ret = UnionReadXMLPackageValue("clearMode",tmpNum,sizeof(tmpNum))) < 0)
		{
			UnionUserErrLog("in UnionDealDaemonTask:: UnionReadXMLPackageValue [%s][%d]!\n","clearMode",i);
			continue;
		}
		else
			clearMode = atoi(tmpNum);
		// ��������
		memset(tmpNum,0,sizeof(tmpNum));
		if ((ret = UnionReadXMLPackageValue("days",tmpNum,sizeof(tmpNum))) < 0)
		{
			UnionUserErrLog("in UnionDealDaemonTask:: UnionReadXMLPackageValue [%s][%d]!\n","days",i);
			continue;
		}
		else
			days = atoi(tmpNum);
		
		// ����Ŀ¼�µ��ļ�
		if ((ret = UnionDeleteFileByDays(fileStoreDir,days,clearMode)) < 0) 
		{
			UnionUserErrLog("in UnionDealDaemonTask:: UnionDeleteFileByDays fileDir[%s] days[%d] clearMode[%d]  [%d]!\n",fileStoreDir,days,clearMode,i);
			continue;
		}
		
	}

	return 0;
}
