
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
�������:	0306
������:		�����ļ�
��������:	�����ļ�
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

	 // �ļ���
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
		UnionSetResponseRemark("�����ļ�������Ϊ��");
		return(errCodeFileTransSvrNoFileName);
	}
	// ��ȡ�ļ�����
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
		UnionSetResponseRemark("�����ļ����Ͳ���Ϊ��");
		return(errCodeFileTransSvrNoFileName);
	}

	// ��ȡ����ʽ Ĭ��Ϊ0 
	// 0:�����ļ�
	// 1:ɾ���ļ�
	if ((ret = UnionReadRequestXMLPackageValue("body/mode",tmpBuf,sizeof(tmpBuf))) > 0)
	{
		mode = atoi(tmpBuf);
	}
	
	//��ѯ�ļ��洢·��
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

	// ��ȡ�ļ��洢Ŀ¼
	memset(fileStoreDir,0,sizeof(fileStoreDir));
	if ((ret = UnionReadXMLPackageValue("fileStoreDir",fileStoreDir,sizeof(fileStoreDir))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0306:: UnionReadXMLPackageValue[%s]!\n","fileStoreDir");
		return(ret);
	}

	// ����ļ��Ƿ����
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
			UnionSetResponseRemark("ɾ���ļ�[%s]ʧ��",fileName);
			return(errCodeUseOSErrCode);
		}
	}
	else
	{
		// ������Ӧ�ļ�Ŀ¼
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
