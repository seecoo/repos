
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
�������:	0307
������:		��ȡ�ļ��嵥
��������:	��ȡ�ļ��嵥
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

	// ��ȡ�ļ�����
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
		UnionSetResponseRemark("�ļ����Ͳ���Ϊ��");
		return(errCodeParameter);
	}

	// ��ȡ��ǰҳ��
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRequestXMLPackageValue("body/currentPage",tmpBuf,sizeof(tmpBuf))) <= 0)
	{
		//UnionUserErrLog("in UnionDealServiceCode0307:: UnionReadRequestXMLPackageValue[%s]!\n","body/fileTypeID");
		//return(ret);
	}
	else
		currentPage = atoi(tmpBuf);

	// ��ȡÿҳ������
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReadRequestXMLPackageValue("body/numOfPerPage",tmpBuf,sizeof(tmpBuf))) <= 0)
	{
		//UnionUserErrLog("in UnionDealServiceCode0307:: UnionReadRequestXMLPackageValue[%s]!\n","body/fileTypeID");
		//return(ret);
	}
	else
		numOfPerPage = atoi(tmpBuf);

	// ��ȡ�ļ�����
	memset(order_by,0,sizeof(order_by));
	if ((ret = UnionReadRequestXMLPackageValue("body/order_by",order_by,sizeof(order_by))) <= 0)
	{
		strcpy(order_by,"fileName asc");	
	}
	UnionFilterHeadAndTailBlank(order_by);

	// �����������
	if (strncasecmp(order_by,"fileName",8) == 0)
		sortType = 1;

	if (strncasecmp(order_by,"fileTime",8) == 0)
		sortType = 2;

	if (strncasecmp(order_by,"fileSize",8) == 0)
		sortType = 3;

	// �������ʽ
	memset(tmpBuf,0,sizeof(tmpBuf));
	memcpy(tmpBuf,order_by + 8,strlen(order_by) - 8);
	UnionFilterHeadAndTailBlank(tmpBuf);

	if (strcasecmp(tmpBuf,"asc") == 0)
		sortFlag = 1;
	else if (strcasecmp(tmpBuf,"desc") == 0)
		sortFlag = 2;

	//��ѯ�ļ��洢·��
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

	// ��ȡ�ļ��洢Ŀ¼
	memset(fileStoreDir,0,sizeof(fileStoreDir));
	if ((ret = UnionReadXMLPackageValue("fileStoreDir",fileStoreDir,sizeof(fileStoreDir))) <= 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0307:: UnionReadXMLPackageValue[%s]!\n","fileStoreDir");
		return(ret);
	}
	
	memset(fileDir,0,sizeof(fileDir));
	UnionReadDirFromStr(fileStoreDir,-1,fileDir);
	

	// �����ֶ��嵥
	memset(tmpBuf,0,sizeof(tmpBuf));
	strcpy(tmpBuf,"fileName,fileTime,fileSize");
	if ((ret = UnionSetResponseXMLPackageValue("body/fieldList",tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0307:: UnionSetResponseXMLPackageValue[%s]!\n","body/fieldList");
		return(ret);
	}

	// �����ֶ�������
	memset(tmpBuf,0,sizeof(tmpBuf));
	strcpy(tmpBuf,"�ļ���,�ļ��޸�ʱ��,�ļ���С");
	if ((ret = UnionSetResponseXMLPackageValue("body/fieldListChnName",tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0307:: UnionSetResponseXMLPackageValue[%s]!\n","body/fieldListChnName");
		return(ret);
	}
	
	// ��Ŀ¼
	if ((fileTotalNum = scandir(fileDir,&nameList,0,alphasort)) < 0)
	{
		UnionSystemErrLog("in UnionDealServiceCode0307:: scandir [%s] error!\n",fileDir);
		free(nameList);
		UnionSetResponseRemark("���ļ�Ŀ¼ʧ��");
		return(fileTotalNum);
	}

	// ��ȡ������
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
		
		// ���ü�¼��
		UnionSetResponseXMLPackageValue("body/totalNum","0");
		
		tmpNum = numOfPerPage;
	}
	else
	{
		// ���ü�¼��
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

	//���붯̬�ռ�
	if ((pfileInfo = (PUnionFileInfo)malloc(sizeof(TUnionFileInfo) * tmpNum)) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCode0307:: malloc failed!\n");
		UnionSetResponseRemark("Ϊ�洢�ļ���Ϣ�����붯̬�ռ�ʧ��");
		return(errCodeParameter);
	}

	memset(pfileInfo,0,sizeof(TUnionFileInfo) * tmpNum);
	
	// ��ȡ�ļ���Ϣ
	for(j = 0; j < fileTotalNum; j++)
	{
		// �������
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
		
		// ��ȡ�ļ���
		sprintf((pfileInfo + totalNum)->fileName,"%s",nameList[i]->d_name);	

		// ��ȡ�ļ�ʱ��
		fileTM = localtime(&fileInfo.st_mtime);
		memset(fileTime,0,sizeof(fileTime));
		strftime(fileTime,sizeof(fileTime),"%F %T",fileTM);
		sprintf((pfileInfo + totalNum)->fileTime,"%s",fileTime);
		
		// ��ȡ�ļ���С
		(pfileInfo + totalNum)->fileSize = fileInfo.st_size;

		totalNum++;
	}
	free(nameList);


	// ��Ҫ������
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
		// ����ID
		if ((ret = UnionLocateResponseNewXMLPackage("body/detail", i+1)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0307:: UnionLocateResponseNewXMLPackage!\n");
			free(pfileInfo);
			return(ret);
		}

		// �����ļ���
		if ((ret = UnionSetResponseXMLPackageValue("fileName",(pfileInfo + i)->fileName)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0307:: UnionSetResponseXMLPackageValue[%s][%s]!\n","fileName",(pfileInfo + i)->fileName);
			free(pfileInfo);
			return(ret);
		}
	
		// �����ļ�ʱ��
		if ((ret = UnionSetResponseXMLPackageValue("fileTime",(pfileInfo + i)->fileTime)) < 0)
		{
			UnionUserErrLog("in UnionDealServiceCode0307:: UnionSetResponseXMLPackageValue[%s][%s]!\n","fileTime",(pfileInfo + i)->fileTime);
			free(pfileInfo);
			return(ret);
		}
		
		// �����ļ���С
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
			UnionSetResponseRemark("��ǰҳ������");
			return(errCodeParameter);
		}
	}

	UnionLocateResponseXMLPackage("",0);
	
	// ��ֵ��¼����
	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"%d",totalNum);
	if ((ret = UnionSetResponseXMLPackageValue("body/totalNum",tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0307:: UnionSetResponseXMLPackageValue[%s]!\n","body/totalNum");
		return(ret);
	}

	return 0;
}
