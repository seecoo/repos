
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
�������:	0308
������:		�ļ����
��������:	�ļ����
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

	 // �ļ���
	memset(fileNameList,0,sizeof(fileNameList));
	if ((ret = UnionReadRequestXMLPackageValue("body/fileNameList",fileNameList,sizeof(fileNameList))) > 0)
	{
		UnionFilterHeadAndTailBlank(fileNameList);

		// ƴ�����崮
		memset(fileNameGrp,0,sizeof(fileNameGrp));
		if ((fileNum = UnionSeprateVarStrIntoVarGrp(fileNameList,strlen(fileNameList),',',fileNameGrp,maxFileNum)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0308:: UnionSeprateVarStrIntoVarGrp[%s]!\n",fileNameList);
			return(fileNum);
		}
	}

	// ��ȡ�ļ�����
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
		UnionSetResponseRemark("�ļ�������Ͳ���Ϊ��");
		return(errCodeFileTransSvrNoFileName);
	}

	// �����ʽ
	// 0: �����ļ������
	// 1: ȫ�����
	if (strlen(fileNameList) > 0)
		downFlag = 0;
	else
		downFlag = 1;
	
	//��ѯ�ļ��洢·��
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
		UnionSetResponseRemark("�ļ�����[%s]������",fileTypeID);	
		return(errCodeParameter);
	}
	
	if ((ret = UnionLocateXMLPackage("detail",1)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0308:: UnionLocateXMLPackage[%s]!\n","detail");
		return(ret);
	}

	// ��ȡ�ļ��洢Ŀ¼
	memset(fileStoreDir,0,sizeof(fileStoreDir));
	if ((ret = UnionReadXMLPackageValue("fileStoreDir",fileStoreDir,sizeof(fileStoreDir))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0308:: UnionReadXMLPackageValue[%s]!\n","fileStoreDir");
		return(ret);
	}
	
	if ((ret = UnionReadDirFromStr(fileStoreDir,-1,path)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0308:: UnionReadDirFromStr[%s]!\n","fileStoreDir");
		UnionSetResponseRemark("��ȡ�ļ����·��ʧ��");
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

	// ���Ŀ¼�Ƿ����
	if (access(downFileDir,0) < 0)
	{
		if (mkdir(downFileDir,0755) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0308:: mkdir[%s]\n",downFileDir);
			return(-1);
		}
	}

	// ��������ļ��Ƿ����
	if (access(downFileFullName,0) >= 0)
	{
		// ������ɾ��
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
			// ����ļ��Ƿ����
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

	// ������Ӧ�ļ�Ŀ¼
	if ((ret = UnionSetResponseXMLPackageValue("body/fileFullName",downFileFullName)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0308:: UnionSetResponseXMLPackageValue[%s]!\n","body/fileFullName");
		return(ret);
	}

	return 0;
}
