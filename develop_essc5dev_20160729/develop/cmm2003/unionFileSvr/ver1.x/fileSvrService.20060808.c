// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2008/11/10
// Version:	1.0

// �ļ�������

#include <stdio.h>
#include <string.h>

#include "mngSvrServicePackage.h"
#include "unionErrCode.h"
#include "mngSvrCommProtocol.h"
#include "UnionLog.h"
#include "unionResID.h"
#include "mngSvrFileSvr.h"
#include "unionFileManager.h"
#include "unionTableField.h"
#include "unionRec0.h"
#include "unionREC.h"

int UnionFileSvrGetFileDir(char *dirName, int sizeOfBuf)
{
	char		*ptr;

	if ( (ptr = UnionReadStringTypeRECVar("dirOfFileSvr")) == NULL)
	{
		UnionUserErrLog("in UnionFileSvrGetFileDir:: [%s] is not define!\n","dirOfFileSvr");
		return(errCodeParameter);
	}
	strcpy(dirName, UnionReadStringTypeRECVar("dirOfFileSvr"));

	return 0;
}

/* ����
	���տͻ����ϴ���һ���ļ�
�������
	handle		socket���
	reqStr		����
	lenOfReqStr	���󴮳���
	sizeOfResStr	��Ӧ������Ĵ�С
�������
	resStr		��Ӧ��
	fileRecved	�Ƿ����ļ����ؿͻ���
����ֵ
	>=0	�ɹ�
	<0	ʧ�ܣ�������
*/
int UnionRecvFileFromClient(int handle,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr,int *fileRecved)
{
	int	ret;
	char	fullFileName[1024+1];
	char	dirName[1024+1];
	char	desDirName[1024+1];
	char	recStr[2048+1];
	int	fileType;
	char	cliFileName[128+1];
	
	*fileRecved = 0;
	//��ȡ�ļ���
	memset(cliFileName,0,sizeof(cliFileName));
	if ((ret = UnionReadRecFldFromRecStr(reqStr,lenOfReqStr,"fileName",cliFileName,sizeof(cliFileName))) < 0)
	{
		UnionUserErrLog("in UnionRecvFileFromClient:: UnionReadRecFldFromRecStr [%s] form [%s]!\n","fileName",reqStr);
		return(ret);
	}
	//��ȡĿ¼��
	memset(dirName, 0, sizeof dirName);
	if ((ret = UnionFileSvrGetFileDir(dirName,sizeof(dirName))) < 0)
	{
		UnionUserErrLog("in UnionRecvFileFromClient:: UnionFileSvrGetFileDir err!\n");
		return(ret);
	}
	
	// Ŀ¼ת��
	memset(desDirName,0,sizeof(desDirName));
	if ((ret = UnionReadDirFromStr(dirName,-1,desDirName)) < 0)
	{
		UnionUserErrLog("in UnionRecvFileFromClient:: UnionReadDirFromStr form [%s]!\n",dirName);
		return(ret);
	}
	// ƴװ�ļ�ȫ��
	memset(fullFileName, 0, sizeof(fullFileName));
	sprintf(fullFileName,"%s/%s",desDirName,cliFileName);
	// �����ϴ��ļ�
	if ( (ret = UnionMngSvrRecvFileFromClient(handle, fullFileName)) < 0)
	{
		UnionUserErrLog("in UnionRecvFileFromClient:: UnionMngSvrRecvFileFromClient [%s]!\n",fullFileName);
		return(ret);
	}

	return(0);
}

/* ����
	��ͻ��˴���һ���ļ�
�������
	handle		socket���
	reqStr		����
	lenOfReqStr	���󴮳���
	sizeOfResStr	��Ӧ������Ĵ�С
�������
	resStr		��Ӧ��
	fileRecved	�Ƿ����ļ����ؿͻ���
����ֵ
	>=0	�ɹ�
	<0	ʧ�ܣ�������
*/
int UnionSendFileToClient(int handle,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr,int *fileRecved)
{
	int	ret;
	char	fileName[128+1];
	char	fullFileName[1024+1];
	char	dirName[1024+1];
	char	desDirName[1024+1];
	char	tableName[128+1];
	char	fileNameFldName[128+1];
	char	primaryKey[1024+1];
	int	lenOfPrimaryKey;
	char	recStr[1024+1];
	int	clientDefDirAndFileName = 0;
	char	cliFileName[128+1];
	char	fileSuffix[128+1];
	
	*fileRecved = 0;
	// ���ͻ���ָ�������غ�洢���ļ�����
	memset(cliFileName,0,sizeof(cliFileName));
	if ((ret = UnionReadRecFldFromRecStr(reqStr,lenOfReqStr,"cliFileName",cliFileName,sizeof(cliFileName))) < 0)
	{
		UnionAuditLog("in UnionSendFileToClient:: UnionReadRecFldFromRecStr fld [cliFileName] from [%s]\n",reqStr);
		//return(ret);
	}	
	
	// ���������˵�Ŀ¼���ļ��� 
	memset(dirName,0,sizeof(dirName));
	memset(fileName,0,sizeof(fileName));
	//��ȡ�ļ���
	if ((ret = UnionReadRecFldFromRecStr(reqStr,lenOfReqStr,"fileName",fileName,sizeof(fileName))) < 0)
	{
		UnionUserErrLog("in UnionSendFileToClient:: UnionReadRecFldFromRecStr fld [fileName] from [%s]\n",reqStr);
		return(ret);
	}
	//��ȡĿ¼��
	memset(dirName, 0, sizeof dirName);
	if ((ret = UnionFileSvrGetFileDir(dirName,sizeof(dirName))) < 0)
	{
		UnionUserErrLog("in UnionRecvFileFromClient:: UnionFileSvrGetFileDir err!\n");
		return(ret);
	}
	// Ŀ¼ת��
	memset(desDirName,0,sizeof(desDirName));
	if ((ret = UnionReadDirFromStr(dirName,-1,desDirName)) < 0)
	{
		UnionUserErrLog("in UnionRecvFileFromClient:: UnionReadDirFromStr form [%s]!\n",dirName);
		return(ret);
	}
	// ƴװ�ļ�ȫ��
	memset(fullFileName, 0, sizeof(fullFileName));
	sprintf(fullFileName,"%s/%s",desDirName,fileName);

	// ��ͻ��˷����ļ�
	if ((ret = UnionMngSvrTransferFile(handle,fullFileName)) < 0)
	{
		UnionUserErrLog("in UnionSendFileToClient:: UnionMngSvrTransferFile [%s]!!\n",fullFileName);
		return(ret);
	}
	
	// �����ļ�����
	if (strlen(cliFileName) != 0)	// ָ���˿ͻ��˱�����ļ�����
	{
		memset(fileSuffix,0,sizeof(fileSuffix));
		UnionReadSuffixOfFileName(fileName,strlen(fileName),1,fileSuffix);
		sprintf(fileName,"%s%s",cliFileName,fileSuffix);
	}
	if ((ret = UnionPutRecFldIntoRecStr("fileName",fileName,strlen(fileName),resStr,sizeOfResStr)) < 0)
	{
		UnionUserErrLog("in UnionSendFileToClient:: UnionPutRecFldIntoRecStr fldName = [%s] fldValue = [%s]!\n","fileName",fileName);
		return(ret);
	}
	
	return(ret);
}

/* ����
	�����ļ�Ŀ¼
�������
	handle		socket���
	reqStr		����
	lenOfReqStr	���󴮳���
	sizeOfResStr	��Ӧ������Ĵ�С
�������
	resStr		��Ӧ��
	fileRecved	�Ƿ����ļ����ؿͻ���
����ֵ
	>=0	�ɹ�
	<0	ʧ�ܣ�������
*/
int UnionCreateFileDir(int handle,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr,int *fileRecved)
{
	int	ret;
	char	dirName[1024+1];
	char	desDirName[1024+1];
	
	*fileRecved = 0;
	//��ȡĿ¼��
        memset(dirName, 0, sizeof dirName);
        if ((ret = UnionFileSvrGetFileDir(dirName,sizeof(dirName))) < 0)
        {
                UnionUserErrLog("in UnionRecvFileFromClient:: UnionFileSvrGetFileDir err!\n");
                return(ret);
        }
	// Ŀ¼ת��
	memset(desDirName,0,sizeof(desDirName));
	if ((ret = UnionReadDirFromStr(dirName,-1,desDirName)) < 0)
	{
		UnionUserErrLog("in UnionCreateFileDir:: UnionReadDirFromStr form [%s]!\n",dirName);
		return(ret);
	}
	if ((ret = UnionCreateDir(desDirName)) < 0)
	{
		UnionUserErrLog("in UnionRecvFileFromClient:: UnionCreateDir [%s]\n",desDirName);
		return(ret);
	}
	return(0);
}

/* ����
	�ļ�������������
�������
	handle		socket���
	resID		��Դ��ʶ
	serviceID	������
	reqStr	����
	lenOfReqStr	���󴮳���
	sizeOfResStr	��Ӧ������Ĵ�С
�������
	resStr		��Ӧ��
	fileRecved	�Ƿ����ļ����յ�
����ֵ
	>=0	�ɹ�
	<0	ʧ�ܣ�������
*/
int UnionExcuteMngSvrFileSvrService(int handle,int resID,int serviceID,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr,int *fileRecved)
{
	int			ret;
		
	switch (serviceID)
	{
		case	conResCmdDownloadFile:
			if ((ret = UnionSendFileToClient(handle,reqStr,lenOfReqStr,resStr,sizeOfResStr,fileRecved)) < 0)
			{
				UnionUserErrLog("in UnionExcuteMngSvrFileSvrService:: UnionSendFileToClient [%s]!\n",reqStr);
				return(ret);
			}
			break;
		case	conResCmdUploadFile:
			if ((ret = UnionRecvFileFromClient(handle,reqStr,lenOfReqStr,resStr,sizeOfResStr,fileRecved)) < 0)
			{
				UnionUserErrLog("in UnionExcuteMngSvrFileSvrService:: UnionRecvFileFromClient [%s]!\n",reqStr);
				return(ret);
			}
			break;
		case	conResCmdCreateFileDir:
			if ((ret = UnionCreateFileDir(handle,reqStr,lenOfReqStr,resStr,sizeOfResStr,fileRecved)) < 0)
			{
				UnionUserErrLog("in UnionExcuteMngSvrFileSvrService:: UnionCreateFileDir [%s]!\n",reqStr);
				return(ret);
			}
			break;
		default:
			UnionProgramerLog("in UnionExcuteMngSvrFileSvrService:: invalid fileSvr command [%d]\n",serviceID);
			return(errCodeFileSvrMDL_InvalidCmd);
	}
	UnionSetResMngResponsePackageFld(conMngSvrPackFldNameData,resStr,ret);
	return(ret);
}
