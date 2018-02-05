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
//#include "unionFileDoc.h"
#include "mngSvrFldTagDef.h"

char		gunionCurrentRecvFileName[256] = "";

int UnionGetCurrentRecvFileName(char *fileName)
{
	strcpy(fileName, gunionCurrentRecvFileName);
	return(0);
}
int UnionSetCurrentRecvFileName(char *fileName)
{
	memset(gunionCurrentRecvFileName, 0, sizeof(gunionCurrentRecvFileName));
	strncpy(gunionCurrentRecvFileName, fileName, sizeof(gunionCurrentRecvFileName) - 1);
	return(0);
}

/* 功能
	根据请求串，获取其它要更新的域信息
输入参数
	reqStr		请求串
	lenOfReqStr	请求串长度
	sizeOfBuf	关键字缓冲的大小
输出参数
	recStr		其它域拼成的串
返回值
	>=0	成功，返回其它域拼成的串的长度
	<0	失败，错误码
*/
int UnionFormOtherRecFldStrExceptFileSvrFld(char *reqStr,int lenOfReqStr,char *recStr,int sizeOfBuf)
{
	int		ret;
	int		offset = 0;
	TUnionRec	rec;
	int		fldNum;
	int		fldIndex;
	
	if ((ret = UnionReadRecFromRecStr(reqStr,lenOfReqStr,&rec)) < 0)
	{
		UnionUserErrLog("in UnionFormOtherRecFldStrExceptFileSvrFld:: UnionReadRecFromRecStr!\n");
		return(ret);
	}
	for (fldIndex = 0; fldIndex < rec.fldNum; fldIndex++)
	{
		if ((strcmp(rec.fldName[fldIndex],conFileManagerFldNameTableName) == 0) ||
			(strcmp(rec.fldName[fldIndex],conFileManagerFldNameFileType) == 0) ||
			(strcmp(rec.fldName[fldIndex],"fileName") == 0))
			continue;
		sprintf(recStr+offset,"%s=%s|",rec.fldName[fldIndex],rec.fldValue[fldIndex]);
		offset += (strlen(rec.fldName[fldIndex]) + strlen(rec.fldValue[fldIndex]) + 1 + 1);
	}
	UnionLog("in UnionFormOtherRecFldStrExceptFileSvrFld:: [%04d][%s]\n",offset,recStr);
	return(offset);
}
		
/* 功能
	根据请求串，获取文件配置的信息
输入参数
	reqStr		请求串
	lenOfReqStr	请求串长度
	sizeOfBuf	关键字缓冲的大小
输出参数
	dirName		文件存储目录,如果指针不为空
	tableName	表名,如果指针不为空
	fileNameFldName	文件名字段名,如果指针不为空
	primaryKey	关键字串,如果指针不为空
返回值
	>=0	成功，返回关键字串的长度
	<0	失败，错误码
*/
int UnionReadFileConfFromReqStr(char *reqStr,int lenOfReqStr,char *dirName,char *tableName,char *fileNameFldName,char *primaryKey,char *primaryKeyValueStr,int *fileType,int sizeOfBuf)
{
	TUnionFileManager	rec;
	int			ret;
	char			tmpBuf[512];
	char			fldGrp[20][128];
	int			fldNum;
	int			index;
	int			offset = 0;
	int			offset2 = 0;
	int			vLen;
	// 2010-8-19,王纯军增加
	int			cliSupplyPrimaryKeyFld = 0;
	int			keyFldNum;
	char			keyFld[20][128];
	char			seperator[2];
	// 2010-8-19,王纯军增加结束

	// 读关键字
	memset(&rec,0,sizeof(rec));
	if ((ret = UnionFormPrimaryKeyOfFileManagerRecFromRecStr(reqStr,lenOfReqStr,&rec)) < 0)
	{
		UnionUserErrLog("in UnionReadFileConfFromReqStr:: UnionFormPrimaryKeyOfFileManagerRecFromRecStr from [%04d][%s]\n",lenOfReqStr,reqStr);
		return(ret);
	}
	if (fileType != NULL)
		*fileType = rec.fileType;
	// 读记录
	if ((ret = UnionReadFileManagerRec(rec.tableName,rec.fileType,&rec)) < 0)
	{
		UnionUserErrLog("in UnionReadFileConfFromReqStr:: UnionReadFileManagerRec [%s][%d] from [%s]\n",rec.tableName,rec.fileType,reqStr);
		return(ret);
	}
	if (dirName != NULL)
		strcpy(dirName,rec.fileStoreDir);
	if (tableName != NULL)
		strcpy(tableName,rec.tableName);
	//if (fileNameFldName != NULL)
	//	strcpy(fileNameFldName,rec.fileNameFldName);
	if (fileNameFldName != NULL)
	{
		if ((ret = UnionReadTableFldNameByFldID(rec.fileNameFldName,fileNameFldName)) < 0)
		{
			UnionUserErrLog("in UnionReadFileConfFromReqStr:: UnionReadTableFldNameByFldID [%s]\n",rec.fileNameFldName);
			return(ret);
		}
	}
	
	if ( (primaryKey == NULL) || (UnionIsBackuperMngSvr()==1) )
		return(0);
	// 拼关键字
	memset(fldGrp,0,sizeof(fldGrp));
	if ((ret = UnionGetAllFldNameFromFldIDList(rec.primaryKeyList,strlen(rec.primaryKeyList),fldGrp,20)) < 0)
	{
		UnionUserErrLog("in UnionReadFileConfFromReqStr:: UnionGetAllFldNameFromFldIDList [%03d][%d]\n",strlen(rec.primaryKeyList),rec.primaryKeyList);
		return(ret);
	}
	fldNum = ret;
	
	// 2010-8-19,王纯军增加
	UnionReadIntTypeRecFldFromRecStr(reqStr,lenOfReqStr,conMngSvrFldCliSupplyPrimaryKeyFld,&cliSupplyPrimaryKeyFld);
	if (cliSupplyPrimaryKeyFld)	// 客户端提供了关键字
	{
		// 读客户端提供的关键字间的分隔符
		memset(seperator,0,sizeof(seperator));
		if ((ret = UnionReadRecFldFromRecStr(reqStr,lenOfReqStr,conMngSvrFldClientCmdFileFldSeperator,seperator,sizeof(seperator))) <= 0)
			seperator[0] = '~';
		// 读客户端提供的关键字
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadRecFldFromRecStr(reqStr,lenOfReqStr,conMngSvrFldCliSupplyPrimaryKeyValue,tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionReadFileConfFromReqStr:: UnionReadRecFldFromRecStr [%s] form [%03d][%s]\n",conMngSvrFldCliSupplyPrimaryKeyValue,lenOfReqStr,reqStr);
			return(ret);
		}
		memset(keyFld,0,sizeof(keyFld));
		// 折分客户端提供的关键字
		if ((keyFldNum = UnionSeprateVarStrIntoVarGrp(tmpBuf,ret,seperator[0],keyFld,20)) < 0)
		{
			UnionUserErrLog("in UnionReadFileConfFromReqStr:: UnionSeprateVarStrIntoVarGrp [%03d][%s]!\n",ret,tmpBuf);
			return(keyFldNum);
		}
	}			
	// 2010-8-19,王纯军增加结束
	for (index = 0; index < fldNum; index++)
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		if (!cliSupplyPrimaryKeyFld)	// 2010-8-19,王纯军增加
		{
			if ((vLen = UnionReadRecFldFromRecStr(reqStr,lenOfReqStr,fldGrp[index],tmpBuf,sizeof(tmpBuf))) < 0)
			{
				UnionUserErrLog("in UnionReadFileConfFromReqStr:: UnionReadRecFldFromRecStr [%s] from [%04d][%s]\n",fldGrp[index],lenOfReqStr,reqStr);
				return(vLen);
			}
		}
		// 2010-8-19,王纯军增加
		else
		{
			strcpy(tmpBuf,keyFld[index]);
			vLen = strlen(tmpBuf);
		}
		// 2010-8-19,王纯军增加结束
		if ((ret = UnionPutRecFldIntoRecStr(fldGrp[index],tmpBuf,vLen,primaryKey+offset,sizeOfBuf-offset)) < 0)
		{
			UnionUserErrLog("in UnionReadFileConfFromReqStr:: UnionPutRecFldIntoRecStr [%s]\n",fldGrp[index]);
			return(ret);
		}
		offset += ret;
		if (primaryKeyValueStr != NULL)
		{
			sprintf(primaryKeyValueStr+offset2,"%s~",tmpBuf);
			offset2 += (vLen+1);
		}
	}
	return(offset);			
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
	char	fileName[128+1];
	char	fullFileName[1024+1];
	char	dirName[1024+1];
	char	desDirName[1024+1];
	char	tableName[128+1];
	char	fileNameFldName[128+1];
	char	primaryKey[1024+1];
	int	lenOfPrimaryKey;
	char	recStr[2048+1];
	int	fileType;
	char	fileSuffix[100];
	char	primaryKeyValueStr[512];
	char	dateTime[14+1];
	//TUnionFileDoc	fileDoc;
	char	cliFileName[128+1];
	int	isUseCliFileName = 0;
	
	*fileRecved = 0;
	// 读出文件配置
	memset(dirName,0,sizeof(dirName));
	memset(tableName,0,sizeof(tableName));
	memset(fileNameFldName,0,sizeof(fileNameFldName));
	memset(primaryKey,0,sizeof(primaryKey));
	memset(primaryKeyValueStr,0,sizeof(primaryKeyValueStr));
	if ((lenOfPrimaryKey = UnionReadFileConfFromReqStr(reqStr,lenOfReqStr,dirName,tableName,fileNameFldName,primaryKey,primaryKeyValueStr,&fileType,sizeof(primaryKey))) < 0)
	{
		UnionUserErrLog("in UnionRecvFileFromClient:: UnionReadFileConfFromReqStr from [%04d][%s]!\n",lenOfReqStr,reqStr);
		return(lenOfPrimaryKey);
	}
	//获取文件名
	memset(cliFileName,0,sizeof(cliFileName));
	if ((ret = UnionReadRecFldFromRecStr(reqStr,lenOfReqStr,"fileName",cliFileName,sizeof(cliFileName))) < 0)
	{
		UnionUserErrLog("in UnionRecvFileFromClient:: UnionReadRecFldFromRecStr [%s] form [%s]!\n","fileName",reqStr);
		return(ret);
	}

	//获取是否使用客户端文件名称标志
	UnionReadIntTypeRecFldFromRecStr(reqStr,lenOfReqStr,"useClientFileName",&isUseCliFileName);

	memset(fileSuffix,0,sizeof(fileSuffix));
	UnionReadSuffixOfFileName(cliFileName,ret,1,fileSuffix);
	// 目录转换
	memset(desDirName,0,sizeof(desDirName));
	if ((ret = UnionReadDirFromStr(dirName,-1,desDirName)) < 0)
	{
		UnionUserErrLog("in UnionRecvFileFromClient:: UnionReadDirFromStr form [%s]!\n",dirName);
		return(ret);
	}

	// modify by xusj 2010-04-16 begin
        if(!UnionIsBackuperMngSvr() && !isUseCliFileName)
        {
		// 替换文件名
		memset(dateTime,0,sizeof(dateTime));
		UnionGetFullSystemDateTime(dateTime);
		sprintf(fileName,"%s%d-%s%s",primaryKeyValueStr,fileType,dateTime,fileSuffix);
	}
	else
	{
		strcpy(fileName,cliFileName);
	}
	// modify by xusj 2010-04-16 end
	// 拼装文件全名
	memset(fullFileName, 0, sizeof(fullFileName));
	sprintf(fullFileName,"%s/%s",desDirName,fileName);
	UnionSetCurrentRecvFileName(fullFileName); // added 2012-12-24
	// 接收上传文件
	if ( (ret = UnionMngSvrRecvFileFromClient(handle, fullFileName)) < 0)
	{
		UnionUserErrLog("in UnionRecvFileFromClient:: UnionMngSvrRecvFileFromClient [%s]!\n",fileName);
		return(ret);
	}

	//更新文件登记表,modify by xusj 2010-04-16 begin
	if (UnionIsBackuperMngSvr())
		return 0;
	if ((strlen(tableName) == 0) || (strlen(fileNameFldName) == 0))
		//return(0);
		goto regJnl;
	//modify by xusj 2010-04-16 end

	// 在文件管理档案中，增加登记
	// 在此处增加代码。
	/*
	memset(&fileDoc,0,sizeof(fileDoc));
	strcpy(fileDoc.tableName,tableName);
	fileDoc.fileType = fileType;
	strcpy(fileDoc.storeDir,desDirName);
	strcpy(fileDoc.fileName,fileName);
	strcpy(fileDoc.clientFileName,cliFileName);
	if ((ret = UnionInsertFileDocRec(&fileDoc)) < 0)
	{
		UnionUserErrLog("in UnionRecvFileFromClient:: UnionInsertFileDocRec [%s.%d.%s.%s]!\n",tableName,fileType,desDirName,fileName);
		//return(ret);
	}
	*/
	// 拼装其它域
	memset(recStr,0,sizeof(recStr));
	if ((ret = UnionFormOtherRecFldStrExceptFileSvrFld(reqStr,lenOfReqStr,recStr,sizeof(recStr))) < 0)
	{
		UnionUserErrLog("in UnionRecvFileFromClient:: UnionFormOtherRecFldStrExceptFileSvrFld! [%04d][%s]\n",lenOfReqStr,reqStr);
		return(ret);
	}
	sprintf(recStr+ret,"%s=%s|",fileNameFldName,fileName);	
	if ((ret = UnionUpdateUniqueObjectRecord(tableName,primaryKey,recStr,strlen(recStr))) < 0)
	{
		UnionUserErrLog("in UnionRecvFileFromClient:: UnionUpdateUniqueObjectRecord [%s] of [%s]\n",primaryKey,tableName);
		return(ret);
	}
	UnionLog("in UnionRecvFileFromClient:: UnionUpdateUniqueObjectRecord [%s] on [%s] OK! recStr = [%d][%s]\n",tableName,primaryKey,strlen(recStr),recStr);
regJnl:
	// add by xusj 2010-04-16 begin
	// 登记文件同步流水
	if ((ret = UnionRegFileOperationToFileDataSynJnl(tableName, desDirName, fileName, fileType)) < 0)
	{
		UnionUserErrLog("in UnionRecvFileFromClient:: UnionRegFileOperationToFileDataSynJnl [%s] of [%s]\n",primaryKey,tableName);
		//return(ret);
	}
        // add by xusj 2010-04-16 end

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
	if (((ret = UnionReadIntTypeRecFldFromRecStr(reqStr,lenOfReqStr,"clientDefDirAndFileName",&clientDefDirAndFileName)) < 0) ||
		(clientDefDirAndFileName == 0)) // 客户端未指定下载目录和文件名
	{
		// 读出文件配置
		memset(tableName,0,sizeof(tableName));
		memset(fileNameFldName,0,sizeof(fileNameFldName));
		memset(primaryKey,0,sizeof(primaryKey));
		if ((lenOfPrimaryKey = UnionReadFileConfFromReqStr(reqStr,lenOfReqStr,dirName,tableName,fileNameFldName,primaryKey,NULL,NULL,sizeof(primaryKey))) < 0)
		{
			UnionUserErrLog("in UnionSendFileToClient:: UnionReadFileConfFromReqStr from [%04d][%s]!\n",lenOfReqStr,reqStr);
			return(lenOfPrimaryKey);
		}
		// 目录转换
		memset(desDirName,0,sizeof(desDirName));
		if ((ret = UnionReadDirFromStr(dirName,-1,desDirName)) < 0)
		{
			UnionUserErrLog("in UnionSendFileToClient:: UnionReadDirFromStr form [%s]!\n",dirName);
			return(ret);
		}
		// 从表中读取文件名
		if ((ret = UnionSelectSpecFldOfObjectByPrimaryKey(tableName,primaryKey,fileNameFldName,fileName,sizeof(fileName))) < 0)
		{
			UnionUserErrLog("in UnionSendFileToClient:: UnionSelectSpecFldOfObjectByPrimaryKey fld [%s] from table [%s]\n",fileNameFldName,tableName);
			return(ret);
		}
		/**add by xusj begin 20100526***/
		if (strlen(fileName) == 0)
		{
			UnionUserErrLog("in UnionSendFileToClient:: fileName is null for [%s][%s]\n",tableName,primaryKey);
			return(errCodeMngSvrFileNotExists);
		}
		/**add by xusj end***/
	}
	else	// 客户端指定了下载目录和文件名
	{
		if ((ret = UnionReadRecFldFromRecStr(reqStr,lenOfReqStr,"storeDir",desDirName,sizeof(desDirName))) < 0)
		{
			UnionUserErrLog("in UnionSendFileToClient:: UnionReadRecFldFromRecStr fld [storeDir] from [%s]\n",reqStr);
			return(ret);
		}
		if ((ret = UnionReadRecFldFromRecStr(reqStr,lenOfReqStr,"fileName",fileName,sizeof(fileName))) < 0)
		{
			UnionUserErrLog("in UnionSendFileToClient:: UnionReadRecFldFromRecStr fld [fileName] from [%s]\n",reqStr);
			return(ret);
		}
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
	// 读出文件配置
	memset(dirName,0,sizeof(dirName));
	if ((ret = UnionReadFileConfFromReqStr(reqStr,lenOfReqStr,dirName,NULL,NULL,NULL,NULL,NULL,0)) < 0)
	{
		UnionUserErrLog("in UnionCreateFileDir:: UnionReadFileConfFromReqStr from [%04d][%s]!\n",lenOfReqStr,reqStr);
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
	char			fileName[128];
	char			Buf[4096+1];
	char			Buf1[4096*2+1];
	FILE			*fp;
		
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
			UnionGetCurrentRecvFileName(fileName);

                        if((fp = fopen(fileName,"r")) == NULL)
                        {
                                UnionUserErrLog("in UnionExcuteMngSvrFileSvrService:: open file error!!\n");
                                return(-1);
                        }
                        while(!feof(fp))
                        {
				ret = fread(Buf, 1, sizeof(Buf), fp);
                                bcdhex_to_aschex(Buf, ret, Buf1);
                        }
			// read cert 
			// printf
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
