// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2008/11/10
// Version:	1.0

// 文件服务器

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

/* 功能
	接收客户端上传的一个文件
输入参数
	handle		socket句柄
	reqStr		请求串
	lenOfReqStr	请求串长度
	sizeOfResStr	响应串缓冲的大小
输出参数
	resStr		响应串
	fileRecved	是否有文件返回客户端
返回值
	>=0	成功
	<0	失败，错误码
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
	//获取文件名
	memset(cliFileName,0,sizeof(cliFileName));
	if ((ret = UnionReadRecFldFromRecStr(reqStr,lenOfReqStr,"fileName",cliFileName,sizeof(cliFileName))) < 0)
	{
		UnionUserErrLog("in UnionRecvFileFromClient:: UnionReadRecFldFromRecStr [%s] form [%s]!\n","fileName",reqStr);
		return(ret);
	}
	//获取目录名
	memset(dirName, 0, sizeof dirName);
	if ((ret = UnionFileSvrGetFileDir(dirName,sizeof(dirName))) < 0)
	{
		UnionUserErrLog("in UnionRecvFileFromClient:: UnionFileSvrGetFileDir err!\n");
		return(ret);
	}
	
	// 目录转换
	memset(desDirName,0,sizeof(desDirName));
	if ((ret = UnionReadDirFromStr(dirName,-1,desDirName)) < 0)
	{
		UnionUserErrLog("in UnionRecvFileFromClient:: UnionReadDirFromStr form [%s]!\n",dirName);
		return(ret);
	}
	// 拼装文件全名
	memset(fullFileName, 0, sizeof(fullFileName));
	sprintf(fullFileName,"%s/%s",desDirName,cliFileName);
	// 接收上传文件
	if ( (ret = UnionMngSvrRecvFileFromClient(handle, fullFileName)) < 0)
	{
		UnionUserErrLog("in UnionRecvFileFromClient:: UnionMngSvrRecvFileFromClient [%s]!\n",fullFileName);
		return(ret);
	}

	return(0);
}

/* 功能
	向客户端传送一个文件
输入参数
	handle		socket句柄
	reqStr		请求串
	lenOfReqStr	请求串长度
	sizeOfResStr	响应串缓冲的大小
输出参数
	resStr		响应串
	fileRecved	是否有文件返回客户端
返回值
	>=0	成功
	<0	失败，错误码
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
	// 读客户端指定的下载后存储的文件名称
	memset(cliFileName,0,sizeof(cliFileName));
	if ((ret = UnionReadRecFldFromRecStr(reqStr,lenOfReqStr,"cliFileName",cliFileName,sizeof(cliFileName))) < 0)
	{
		UnionAuditLog("in UnionSendFileToClient:: UnionReadRecFldFromRecStr fld [cliFileName] from [%s]\n",reqStr);
		//return(ret);
	}	
	
	// 读服务器端的目录和文件名 
	memset(dirName,0,sizeof(dirName));
	memset(fileName,0,sizeof(fileName));
	//获取文件名
	if ((ret = UnionReadRecFldFromRecStr(reqStr,lenOfReqStr,"fileName",fileName,sizeof(fileName))) < 0)
	{
		UnionUserErrLog("in UnionSendFileToClient:: UnionReadRecFldFromRecStr fld [fileName] from [%s]\n",reqStr);
		return(ret);
	}
	//获取目录名
	memset(dirName, 0, sizeof dirName);
	if ((ret = UnionFileSvrGetFileDir(dirName,sizeof(dirName))) < 0)
	{
		UnionUserErrLog("in UnionRecvFileFromClient:: UnionFileSvrGetFileDir err!\n");
		return(ret);
	}
	// 目录转换
	memset(desDirName,0,sizeof(desDirName));
	if ((ret = UnionReadDirFromStr(dirName,-1,desDirName)) < 0)
	{
		UnionUserErrLog("in UnionRecvFileFromClient:: UnionReadDirFromStr form [%s]!\n",dirName);
		return(ret);
	}
	// 拼装文件全名
	memset(fullFileName, 0, sizeof(fullFileName));
	sprintf(fullFileName,"%s/%s",desDirName,fileName);

	// 向客户端发送文件
	if ((ret = UnionMngSvrTransferFile(handle,fullFileName)) < 0)
	{
		UnionUserErrLog("in UnionSendFileToClient:: UnionMngSvrTransferFile [%s]!!\n",fullFileName);
		return(ret);
	}
	
	// 设置文件名称
	if (strlen(cliFileName) != 0)	// 指定了客户端保存的文件名称
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

/* 功能
	创建文件目录
输入参数
	handle		socket句柄
	reqStr		请求串
	lenOfReqStr	请求串长度
	sizeOfResStr	响应串缓冲的大小
输出参数
	resStr		响应串
	fileRecved	是否有文件返回客户端
返回值
	>=0	成功
	<0	失败，错误码
*/
int UnionCreateFileDir(int handle,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr,int *fileRecved)
{
	int	ret;
	char	dirName[1024+1];
	char	desDirName[1024+1];
	
	*fileRecved = 0;
	//获取目录名
        memset(dirName, 0, sizeof dirName);
        if ((ret = UnionFileSvrGetFileDir(dirName,sizeof(dirName))) < 0)
        {
                UnionUserErrLog("in UnionRecvFileFromClient:: UnionFileSvrGetFileDir err!\n");
                return(ret);
        }
	// 目录转换
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

/* 功能
	文件服务器解释器
输入参数
	handle		socket句柄
	resID		资源标识
	serviceID	命令字
	reqStr	请求串
	lenOfReqStr	请求串长度
	sizeOfResStr	响应串缓冲的大小
输出参数
	resStr		响应串
	fileRecved	是否有文件接收到
返回值
	>=0	成功
	<0	失败，错误码
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
