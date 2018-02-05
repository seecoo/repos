
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

#include "unionREC.h"
#include "unionErrCode.h"
#include "unionXMLPackage.h"
#include "unionTableDef.h"
#include "unionRealBaseDB.h"
#include "unionVersion.h"
#include "UnionLog.h"
#include "unionLisence.h"
#include "UnionStr.h"
#include "unionRecFile.h"
#include "baseUIService.h"

/*********************************
�������:	0002
������:		��ȡϵͳ��Ϣ
��������:	��ȡϵͳ��Ϣ
**********************************/
int UnionDealServiceCode0002(PUnionHsmGroupRec phsmGroupRec)
{
	int	ret;
	int	timeout = 10;
	int	portOfFileReceiver = 0;
	char	nameOfSystem[40+1];
	char	nameOfMyself[40+1];
	char	serialNumber[40+1];
	char	version[40+1];
	char	clientVersion[40+1];
	char	industry[40+1];
	char	copyright[40+1];
	char	liscenceCode[40+1];
	char	maintainDate[8+1];
	char	workingDate[8+1];
	char	tmpBuf[16+1];
	char	sysID[128+1];
	char	sql[128+1];
	char	fileStoreDir[256+1];
	char	*ptr = NULL;
	int	complexityOfUserID = 0;
	int	complexityOfUserPassword = 0;
	char	fullFileName[512+1];
	DIR	*dir = NULL;
	struct dirent	*dirp = NULL;
	struct stat	fileInfo;
	char	fileDir[512+1];
	char	dateTime[14+1];
	char	abOfMyself[40+1];
	
	// ��ȡϵͳ����
	memset(nameOfSystem,0,sizeof(nameOfSystem));
	if ((ptr = UnionReadStringTypeRECVar("nameOfSystem")) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCode0002:: UnionReadStringTypeRECVar[%s]!\n","nameOfSystem");
		return(errCodeRECMDL_VarNotExists);
	}
	strcpy(nameOfSystem,ptr);

	// ��ȡ�ͻ�Ӣ�ļ�д
	memset(abOfMyself,0,sizeof(abOfMyself));
	if ((ptr = UnionReadStringTypeRECVar("abOfMyself")) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCode0002:: UnionReadStringTypeRECVar[%s]!\n","abOfMyself");
		return(errCodeRECMDL_VarNotExists);
	}
	strcpy(abOfMyself,ptr);

	// ��ȡ�汾
	memset(version,0,sizeof(version));
	if ((ptr = UnionReadStringTypeRECVar("version")) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCode0002:: UnionReadStringTypeRECVar[%s]!\n","version");
		return(errCodeRECMDL_VarNotExists);
	}
	strcpy(version,ptr);
	
        // ��ȡ�ͻ��˰汾 add by yangw 20160307
        memset(clientVersion,0,sizeof(clientVersion));
        if ((ptr = UnionReadStringTypeRECVar("clientVersion")) == NULL)
        {
                UnionLog("in UnionDealServiceCode0002:: UnionReadStringTypeRECVar[%s]!\n","clientVersion");
        }
        else
        {
        	strcpy(clientVersion,ptr);
        }

	// ��ҵ��Ϣ
	memset(industry,0,sizeof(industry));
	if ((ptr = UnionReadStringTypeRECVar("industry")) == NULL)
		strcpy(industry,"������ҵ-���ư�");
	else
		strcpy(industry,ptr);
	// ��Ȩ 
	memset(copyright,0,sizeof(copyright));
	sprintf(copyright,"���ݽ��Ͽ��ѿƼ��ɷ����޹�˾");

	// ��ȡ��Ȩ�û�
	memset(nameOfMyself,0,sizeof(nameOfMyself));
	if ((ptr = UnionReadStringTypeRECVar("nameOfMyself")) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCode0002:: UnionReadStringTypeRECVar[%s]!\n","nameOfMyself");
		return(errCodeRECMDL_VarNotExists);
	}
	strcpy(nameOfMyself,ptr);

	// ��ȡ���к�
	memset(serialNumber,0,sizeof(serialNumber));
	if ((ptr = UnionReadStringTypeRECVar("serialNumber")) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCode0002:: UnionReadStringTypeRECVar[%s]!\n","serialNumber");
		return(errCodeRECMDL_VarNotExists);
	}
	strcpy(serialNumber,ptr);
	
	// ��ȡ��Ȩ��
	memset(liscenceCode,0,sizeof(liscenceCode));
	if ((ptr = UnionReadStringTypeRECVar("liscenceCode")) == NULL)
	{
		UnionUserErrLog("in UnionDealServiceCode0002:: UnionReadStringTypeRECVar[%s]!\n","liscenceCode");
		return(errCodeRECMDL_VarNotExists);
	}
	strcpy(liscenceCode,ptr);

	memset(workingDate,0,sizeof(workingDate));
	memset(maintainDate,0,sizeof(maintainDate));
	// ��ȡά���������ں�ʹ�õ�������
	if ((ret = UnionVerifyFinalLisenceKeyUseSpecInputData(NULL,NULL,maintainDate,workingDate,NULL)) <= 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0002:: UnionVerifyFinalLisenceKeyUseSpecInputData!\n");
		return(errCodeLiscenceCodeInvalid);
	}
	
	// ������Ӧ
	// ϵͳ����
	if ((ret = UnionSetResponseXMLPackageValue("body/nameOfSystem",nameOfSystem)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0002:: UnionSetResponseXMLPackageValue[%s]\n","body/nameOfSystem");
		return(ret);
	}

	// �ͻ�Ӣ�ļ�д
	if ((ret = UnionSetResponseXMLPackageValue("body/abOfMyself",abOfMyself)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0002:: UnionSetResponseXMLPackageValue[%s]\n","body/abOfMyself");
		return(ret);
	}

	// �汾
	if ((ret = UnionSetResponseXMLPackageValue("body/version",version)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0002:: UnionSetResponseXMLPackageValue[%s]\n","body/version");
		return(ret);
	}

	//�ͻ��� �汾
        if ((ret = UnionSetResponseXMLPackageValue("body/clientVersion",clientVersion)) < 0)
        {
                UnionUserErrLog("in UnionDealServiceCode0002:: UnionSetResponseXMLPackageValue[%s]\n","body/clientVersion");
                return(ret);
        }
	
	// ��ҵ��Ϣ
	if ((ret = UnionSetResponseXMLPackageValue("body/industry",industry)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0002:: UnionSetResponseXMLPackageValue[%s]\n","body/industry");
		return(ret);
	}

	// ��Ȩ 
	if ((ret = UnionSetResponseXMLPackageValue("body/copyright",copyright)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0002:: UnionSetResponseXMLPackageValue[%s]!\n","body/copyright");
		return(ret);
	}

	// ��Ȩ�û�
	if ((ret = UnionSetResponseXMLPackageValue("body/authorizeUser",nameOfMyself)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0002:: UnionSetResponseXMLPackageValue[%s]!\n","body/authorizeUser");
		return(ret);
	}

	// ���к�
	if ((ret = UnionSetResponseXMLPackageValue("body/serialNumber",serialNumber)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0002:: UnionSetResponseXMLPackageValue[%s]!\n","body/serialNumber");
		return(ret);
	}

	// ��Ȩ��
	if ((ret = UnionSetResponseXMLPackageValue("body/liscenceCode",liscenceCode)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0002:: UnionSetResponseXMLPackageValue[%s]!\n","body/liscenceCode");
		return(ret);
	}

	// ά���������� 
	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"%.4s-%.2s-%.2s",maintainDate,maintainDate+4,maintainDate+6);
	if ((ret = UnionSetResponseXMLPackageValue("body/maintainDate",tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0002:: UnionSetResponseXMLPackageValue[%s]!\n","body/maintainDate");
		return(ret);
	}

	// ʹ�õ������� 
	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"%.4s-%.2s-%.2s",workingDate,workingDate+4,workingDate+6);
	if ((ret = UnionSetResponseXMLPackageValue("body/workingDate",tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0002:: UnionSetResponseXMLPackageValue[%s]!\n","body/workingDate");
		return(ret);
	}

	// ��ȡsysID
	memset(sysID,0,sizeof(sysID));
	if ((ret = UnionReadRequestXMLPackageValue("head/sysID",sysID,sizeof(sysID))) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0002:: UnionReadRequestXMLPackageValue[head/sysID]! ret = [%d]\n",ret);
		return(ret);
	}
	memset(tmpBuf,0,sizeof(tmpBuf));
	if (strcmp(sysID,"CONFUI") == 0)
		sprintf(tmpBuf,"timeoutOf%s","CUI");
	else
		sprintf(tmpBuf,"timeoutOf%s",sysID);
	if ((timeout = UnionReadIntTypeRECVar(tmpBuf)) <= 0)
		timeout = 10;
	
	// ���ó�ʱʱ��
	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"%d",timeout);
	if ((ret = UnionSetResponseXMLPackageValue("body/timeout",tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0002:: UnionSetResponseXMLPackageValue[%s]!\n","body/timeout");
		return(ret);
	}
	
	// ��ȡ�ļ����ն˿�
	if ((portOfFileReceiver = UnionReadIntTypeRECVar("portOfFileReceiver")) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0002:: UnionReadIntTypeRECVar[%s]!\n","portOfFileReceiver");
		return(portOfFileReceiver);
	}
	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"%d",portOfFileReceiver);
	if ((ret = UnionSetResponseXMLPackageValue("body/portOfFileReceiver",tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0002:: UnionSetResponseXMLPackageValue[%s]!\n","body/portOfFileReceiver");
		return(ret);
	}
	
	// �û��˺Ÿ��Ӷ�
	if ((complexityOfUserID = UnionReadIntTypeRECVar("complexityOfUserID")) <= 0)
		complexityOfUserID = 0;
	
	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"%d",complexityOfUserID);
	if ((ret = UnionSetResponseXMLPackageValue("body/complexityOfUserID",tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0002:: UnionSetResponseXMLPackageValue[%s]!\n","body/complexityOfUserID");
		return(ret);
	}

	// �û����븴�Ӷ�
	if ((complexityOfUserPassword = UnionReadIntTypeRECVar("complexityOfUserPassword")) <= 0)
		complexityOfUserPassword = 0;

	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"%d",complexityOfUserPassword);
	if ((ret = UnionSetResponseXMLPackageValue("body/complexityOfUserPassword",tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0002:: UnionSetResponseXMLPackageValue[%s]!\n","body/complexityOfUserPassword");
		return(ret);
	}

	//��ѯ�ļ��洢·��
	memset(sql,0,sizeof(sql));
	sprintf(sql,"select fileStoreDir from fileType where fileTypeID = 'LOGO'");
	if ((ret = UnionSelectRealDBRecord(sql,0,0)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0002:: UnionSelectRealDBRecord[%s]!\n",sql);
		return(ret);
	}
	else if (ret == 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0002:: fileTypeID[%s]!\n","LOGO");
		return(errCodeDatabaseMDL_RecordNotFound);
	}
	
	if ((ret = UnionLocateXMLPackage("detail",1)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0002:: UnionLocateXMLPackage[%s]!\n","detail");
		return(ret);
	}

	// ��ȡ�ļ��洢Ŀ¼
	memset(fileStoreDir,0,sizeof(fileStoreDir));
	if ((ret = UnionReadXMLPackageValue("fileStoreDir",fileStoreDir,sizeof(fileStoreDir))) <= 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0002:: UnionReadXMLPackageValue[%s]!\n","fileStoreDir");
		return(ret);
	}
	
	memset(fileDir,0,sizeof(fileDir));
	UnionReadDirFromStr(fileStoreDir,-1,fileDir);

	// ��Ŀ¼
	if ((dir = opendir(fileDir)) == NULL)
	{
		UnionSystemErrLog("in UnionDealServiceCode0002:: opendir [%s] return NULL!\n",fileDir);
		return(errCodeUseOSErrCode);
	}
	
	memset(tmpBuf,0,sizeof(tmpBuf));
	while((dirp = readdir(dir)) != NULL)
	{
		if (memcmp(dirp->d_name,".",1) == 0)
			continue;
			
		memset(fullFileName,0,sizeof(fullFileName));
		sprintf(fullFileName,"%s/%s",fileDir,dirp->d_name);
		
		memset(&fileInfo,0,sizeof(fileInfo));
		if ((ret = stat(fullFileName,&fileInfo)) < 0)
		{
			UnionSystemErrLog("in UnionDealServiceCode0002:: stat [%s] return error!\n",fullFileName);
			break;
		}
		
		// �����ļ���С
		memset(tmpBuf,0,sizeof(tmpBuf));
		sprintf(tmpBuf,"%ld",fileInfo.st_size);
	}
	
	if (strlen(tmpBuf) == 0)
		strcpy(tmpBuf,"-1");

	closedir(dir);

	if ((ret = UnionSetResponseXMLPackageValue("body/logoFileSize",tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0002:: UnionSetResponseXMLPackageValue[%s][%s]!\n","body/logoFileSize",tmpBuf);
		return(ret);	
	}

	UnionGetFullSystemDateTime(dateTime);
	if ((ret = UnionSetResponseXMLPackageValue("body/systemDateTime",dateTime)) < 0)
	{
		UnionUserErrLog("in UnionDealServiceCode0002:: UnionSetResponseXMLPackageValue[%s]!\n","body/systemDateTime");
		return(ret);
	}


	return 0;
}

