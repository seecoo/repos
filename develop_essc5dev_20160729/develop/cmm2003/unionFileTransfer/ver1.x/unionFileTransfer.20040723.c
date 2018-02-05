#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef _WIN32
#include <io.h>
#include <process.h>
#include <winsock.h>
#include <windows.h>
#include "unionWorkingDir.h"
#define	WSA_MAKEWORD(x,y)	((y)*256+(x))
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

#include "UnionMD5.h"
#include "UnionIO.h"
#include "UnionStr.h"
#include "unionErrCode.h"
#include "UnionSocket.h"	// 使用3.x版本

#include "UnionLog.h"		// 使用3.x版本

#include "unionFileTransfer.h"
#include "unionRecFile.h"

int	gunionAsFileReceiver = 1;
// 每次传输包的大小
int	gunionSizePerTransfer = 8192;

void UnionSetSizePerTransfer(int sizePerTransfer)
{
	gunionSizePerTransfer = sizePerTransfer;
}


// 设置成文件接收者
void UnionSetAsFileReceiver()
{
	gunionAsFileReceiver = 1;
}

// 设置成非文件接收者
void UnionSetAsNoneFileReceiver()
{
	gunionAsFileReceiver = 0;
}

void UnionSetAsCmd()
{
	gunionAsFileReceiver = 2;
}

// 是文件接收者
int UnionIsFileReceiver()
{
	return(gunionAsFileReceiver);
}

/*
   功能
   将一个文件传输句柄写入到串中
   输入参数
   phdl		句柄
   sizeOfRecStr	串缓冲的大小
   输出参数
   recStr		串
   返回值
   >=0		成功，串长度
   <0		失败，错误码
 */
int UnionPutFileTransferHandleIntoRecStr(PUnionFileTransferHandle phdl,char *recStr,int sizeOfRecStr)
{
	int	offset = 0;
	int	ret;

	if ((ret = UnionPutRecFldIntoRecStr("ipAddrOfCaller",phdl->ipAddrOfCaller,strlen(phdl->ipAddrOfCaller),recStr+offset,sizeOfRecStr-offset)) < 0)
	{
		UnionUserErrLog("in UnionPutFileTransferHandleIntoRecStr:: UnionPutRecFldIntoRecStr [ipAddrOfCaller]!\n");
		return(ret);
	}
	offset += ret;

	if ((ret = UnionPutRecFldIntoRecStr("ipAddrOfListener",phdl->ipAddrOfListener,strlen(phdl->ipAddrOfListener),recStr+offset,sizeOfRecStr-offset)) < 0)
	{
		UnionUserErrLog("in UnionPutFileTransferHandleIntoRecStr:: UnionPutRecFldIntoRecStr [ipAddrOfListener]!\n");
		return(ret);
	}
	offset += ret;


	if ((ret = UnionPutIntTypeRecFldIntoRecStr("port",phdl->port,recStr+offset,sizeOfRecStr-offset)) < 0)
	{
		UnionUserErrLog("in UnionPutFileTransferHandleIntoRecStr:: UnionPutIntTypeRecFldIntoRecStr [port]!\n");
		return(ret);
	}
	offset += ret;

	if ((ret = UnionPutIntTypeRecFldIntoRecStr("socket",phdl->socket,recStr+offset,sizeOfRecStr-offset)) < 0)
	{
		UnionUserErrLog("in UnionPutFileTransferHandleIntoRecStr:: UnionPutIntTypeRecFldIntoRecStr [socket]!\n");
		return(ret);
	}
	offset += ret;

	if ((ret = UnionPutRecFldIntoRecStr("checkValue",(char*)phdl->checkValue,strlen((char*)phdl->checkValue),recStr+offset,sizeOfRecStr-offset)) < 0)
	{
		UnionUserErrLog("in UnionPutFileTransferHandleIntoRecStr:: UnionPutRecFldIntoRecStr [checkValue]!\n");
		return(ret);
	}
	offset += ret;

	if ((ret = UnionPutRecFldIntoRecStr("originFileName",phdl->originFileName,strlen(phdl->originFileName),recStr+offset,sizeOfRecStr-offset)) < 0)
	{
		UnionUserErrLog("in UnionPutFileTransferHandleIntoRecStr:: UnionPutRecFldIntoRecStr [originFileName]!\n");
		return(ret);
	}
	offset += ret;

	if ((ret = UnionPutRecFldIntoRecStr("originDir",phdl->originDir,strlen(phdl->originDir),recStr+offset,sizeOfRecStr-offset)) < 0)
	{
		UnionUserErrLog("in UnionPutFileTransferHandleIntoRecStr:: UnionPutRecFldIntoRecStr [originDir]!\n");
		return(ret);
	}
	offset += ret;

	if ((ret = UnionPutRecFldIntoRecStr("destinitionFileName",phdl->destinitionFileName,strlen(phdl->destinitionFileName),recStr+offset,sizeOfRecStr-offset)) < 0)
	{
		UnionUserErrLog("in UnionPutFileTransferHandleIntoRecStr:: UnionPutRecFldIntoRecStr [destinitionFileName]!\n");
		return(ret);
	}
	offset += ret;

	if ((ret = UnionPutRecFldIntoRecStr("destinitionDir",phdl->destinitionDir,strlen(phdl->destinitionDir),recStr+offset,sizeOfRecStr-offset)) < 0)
	{
		UnionUserErrLog("in UnionPutFileTransferHandleIntoRecStr:: UnionPutRecFldIntoRecStr [destinitionDir]!\n");
		return(ret);
	}
	offset += ret;

	if ((ret = UnionPutLongLongTypeRecFldIntoRecStr("totalFileLength",phdl->totalFileLength,recStr+offset,sizeOfRecStr-offset)) < 0)
	{
		UnionUserErrLog("in UnionPutFileTransferHandleIntoRecStr:: UnionPutLongTypeRecFldIntoRecStr [totalFileLength]!\n");
		return(ret);
	}
	offset += ret;

	if ((ret = UnionPutLongTypeRecFldIntoRecStr("totalNum",phdl->totalNum,recStr+offset,sizeOfRecStr-offset)) < 0)
	{
		UnionUserErrLog("in UnionPutFileTransferHandleIntoRecStr:: UnionPutLongTypeRecFldIntoRecStr [totalNum]!\n");
		return(ret);
	}
	offset += ret;

	if ((ret = UnionPutLongTypeRecFldIntoRecStr("sizePerTransfer",phdl->sizePerTransfer,recStr+offset,sizeOfRecStr-offset)) < 0)
	{
		UnionUserErrLog("in UnionPutFileTransferHandleIntoRecStr:: UnionPutLongTypeRecFldIntoRecStr [sizePerTransfer]!\n");
		return(ret);
	}
	offset += ret;

	if ((ret = UnionPutLongLongTypeRecFldIntoRecStr("lengthTransferred",phdl->lengthTransferred,recStr+offset,sizeOfRecStr-offset)) < 0)

	{
		UnionUserErrLog("in UnionPutFileTransferHandleIntoRecStr:: UnionPutLongLongTypeRecFldIntoRecStr [lengthTransferred]!\n");
		return(ret);
	}
	offset += ret;

	if ((ret = UnionPutLongTypeRecFldIntoRecStr("numTransferred",phdl->numTransferred,recStr+offset,sizeOfRecStr-offset)) < 0)
	{
		UnionUserErrLog("in UnionPutFileTransferHandleIntoRecStr:: UnionPutLongTypeRecFldIntoRecStr [numTransferred]!\n");
		return(ret);
	}
	offset += ret;

	if ((ret = UnionPutLongTypeRecFldIntoRecStr("startTime",phdl->startTime,recStr+offset,sizeOfRecStr-offset)) < 0)
	{
		UnionUserErrLog("in UnionPutFileTransferHandleIntoRecStr:: UnionPutLongTypeRecFldIntoRecStr [startTime]!\n");
		return(ret);
	}
	offset += ret;

	if ((ret = UnionPutLongTypeRecFldIntoRecStr("finishTime",phdl->finishTime,recStr+offset,sizeOfRecStr-offset)) < 0)
	{
		UnionUserErrLog("in UnionPutFileTransferHandleIntoRecStr:: UnionPutLongTypeRecFldIntoRecStr [finishTime]!\n");
		return(ret);
	}
	offset += ret;

	if ((ret = UnionPutLongTypeRecFldIntoRecStr("currentTime",phdl->currentTime,recStr+offset,sizeOfRecStr-offset)) < 0)
	{
		UnionUserErrLog("in UnionPutFileTransferHandleIntoRecStr:: UnionPutLongTypeRecFldIntoRecStr [currentTime]!\n");
		return(ret);
	}
	offset += ret;

	return(offset);
}	

/*
   功能
   从一个串中读取文件传输句柄
   输入参数
   recStr		串
   lenOfRecStr	串长度
   输出参数
   phdl		句柄
   返回值
   >=0		成功，串长度
   <0		失败，错误码
 */
int UnionReadFileTransferHandleFromRecStr(char *recStr,int lenOfRecStr,PUnionFileTransferHandle phdl)
{
	int	offset = 0;
	int	ret;

	if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"ipAddrOfCaller",phdl->ipAddrOfCaller,sizeof(phdl->ipAddrOfCaller))) < 0)
	{
		UnionUserErrLog("in UnionReadFileTransferHandleFromRecStr:: UnionReadRecFldFromRecStr [ipAddrOfCaller]!\n");
		return(ret);
	}
	offset += ret;

	if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"ipAddrOfListener",phdl->ipAddrOfListener,sizeof(phdl->ipAddrOfListener))) < 0)
	{
		UnionUserErrLog("in UnionReadFileTransferHandleFromRecStr:: UnionReadRecFldFromRecStr [ipAddrOfListener]!\n");
		return(ret);
	}
	offset += ret;

	if ((ret = UnionReadIntTypeRecFldFromRecStr(recStr,lenOfRecStr,"port",&(phdl->port))) < 0)
	{
		UnionUserErrLog("in UnionReadFileTransferHandleFromRecStr:: UnionReadIntTypeRecFldFromRecStr [port]!\n");
		return(ret);
	}
	offset += ret;


	if ((ret = UnionReadIntTypeRecFldFromRecStr(recStr,lenOfRecStr,"socket",&(phdl->socket))) < 0)
	{
		UnionUserErrLog("in UnionReadFileTransferHandleFromRecStr:: UnionReadIntTypeRecFldFromRecStr [socket]!\n");
		return(ret);
	}
	offset += ret;

	if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"originFileName",phdl->originFileName,sizeof(phdl->originFileName))) < 0)
	{
		UnionUserErrLog("in UnionReadFileTransferHandleFromRecStr:: UnionReadRecFldFromRecStr [originFileName]!\n");
		return(ret);
	}
	offset += ret;

	if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"originDir",phdl->originDir,sizeof(phdl->originDir))) < 0)
	{
		UnionUserErrLog("in UnionReadFileTransferHandleFromRecStr:: UnionReadRecFldFromRecStr [originDir]!\n");
		return(ret);
	}
	offset += ret;

	if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"destinitionFileName",phdl->destinitionFileName,sizeof(phdl->destinitionFileName))) < 0)
	{
		UnionUserErrLog("in UnionReadFileTransferHandleFromRecStr:: UnionReadRecFldFromRecStr [destinitionFileName]!\n");
		return(ret);
	}
	offset += ret;

	if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"destinitionDir",phdl->destinitionDir,sizeof(phdl->destinitionDir))) < 0)
	{
		UnionUserErrLog("in UnionReadFileTransferHandleFromRecStr:: UnionReadRecFldFromRecStr [destinitionDir]!\n");
		return(ret);
	}
	offset += ret;

	if ((ret = UnionReadRecFldFromRecStr(recStr,lenOfRecStr,"checkValue",(char*)phdl->checkValue,sizeof(phdl->checkValue))) < 0)
	{
		UnionUserErrLog("in UnionReadFileTransferHandleFromRecStr:: UnionReadRecFldFromRecStr [checkValue]!\n");
		return(ret);
	}
	offset += ret;

	if ((ret = UnionReadLongLongTypeRecFldFromRecStr(recStr,lenOfRecStr,"totalFileLength",(int64_t *)&(phdl->totalFileLength))) < 0)
	{
		UnionUserErrLog("in UnionReadFileTransferHandleFromRecStr:: UnionReadLongTypeRecFldFromRecStr [totalFileLength]!\n");
		return(ret);
	}
	offset += ret;

	if ((ret = UnionReadLongTypeRecFldFromRecStr(recStr,lenOfRecStr,"totalNum",&(phdl->totalNum))) < 0)
	{
		UnionUserErrLog("in UnionReadFileTransferHandleFromRecStr:: UnionReadLongTypeRecFldFromRecStr [totalNum]!\n");
		return(ret);
	}
	offset += ret;

	if ((ret = UnionReadLongTypeRecFldFromRecStr(recStr,lenOfRecStr,"sizePerTransfer",&(phdl->sizePerTransfer))) < 0)
	{
		UnionUserErrLog("in UnionReadFileTransferHandleFromRecStr:: UnionReadLongTypeRecFldFromRecStr [sizePerTransfer]!\n");
		return(ret);
	}
	offset += ret;

	if ((ret = UnionReadLongLongTypeRecFldFromRecStr(recStr,lenOfRecStr,"lengthTransferred",(int64_t *)&(phdl->lengthTransferred))) < 0)
	{
		UnionUserErrLog("in UnionReadFileTransferHandleFromRecStr:: UnionReadLongTypeRecFldFromRecStr [lengthTransferred]!\n");
		return(ret);
	}
	offset += ret;

	if ((ret = UnionReadLongTypeRecFldFromRecStr(recStr,lenOfRecStr,"numTransferred",&(phdl->numTransferred))) < 0)
	{
		UnionUserErrLog("in UnionReadFileTransferHandleFromRecStr:: UnionReadLongTypeRecFldFromRecStr [numTransferred]!\n");
		return(ret);
	}
	offset += ret;

	if ((ret = UnionReadLongTypeRecFldFromRecStr(recStr,lenOfRecStr,"startTime",(long *)&(phdl->startTime))) < 0)
	{
		UnionUserErrLog("in UnionReadFileTransferHandleFromRecStr:: UnionReadLongTypeRecFldFromRecStr [startTime]!\n");
		return(ret);
	}
	offset += ret;

	if ((ret = UnionReadLongTypeRecFldFromRecStr(recStr,lenOfRecStr,"finishTime",(long *)&(phdl->finishTime))) < 0)
	{
		UnionUserErrLog("in UnionReadFileTransferHandleFromRecStr:: UnionReadLongTypeRecFldFromRecStr [finishTime]!\n");
		return(ret);
	}
	offset += ret;

	if ((ret = UnionReadLongTypeRecFldFromRecStr(recStr,lenOfRecStr,"currentTime",(long *)&(phdl->currentTime))) < 0)
	{
		UnionUserErrLog("in UnionReadFileTransferHandleFromRecStr:: UnionReadLongTypeRecFldFromRecStr [currentTime]!\n");
		return(ret);
	}
	offset += ret;

	return(offset);
}	

void UnionLogFileTransferHandle(PUnionFileTransferHandle phdl,char *remark)
{
	if (phdl == NULL)
		return;
	if (remark != NULL)
		UnionNullLog("%s\n",remark);
	UnionNullLog("receiver::         [%s]\n",phdl->ipAddrOfListener);
	UnionNullLog("caller::           [%s][%d][%d]\n",phdl->ipAddrOfCaller,phdl->port,phdl->socket);
	UnionNullLog("originFile::       [%s][%s]\n",phdl->originDir,phdl->originFileName);
	UnionNullLog("destinitionFile::  [%s][%s]\n",phdl->destinitionDir,phdl->destinitionFileName);
	UnionNullLog("checkValue::       [%s]\n", phdl->checkValue);
#ifndef _WIN32
	UnionNullLog("totalFileLength::  [%lld]\n",phdl->totalFileLength);
#else
	UnionNullLog("totalFileLength::  [%I64d]\n",phdl->totalFileLength);
#endif
	UnionNullLog("totalNum::         [%ld]\n",phdl->totalNum);
	UnionNullLog("sizePerTransfer::  [%ld]\n",phdl->sizePerTransfer);
#ifndef _WIN32
	UnionNullLog("lengthTransferred::[%lld]\n",phdl->lengthTransferred);
#else
	UnionNullLog("lengthTransferred::[%I64d]\n",phdl->lengthTransferred);
#endif
	UnionNullLog("numTransferred::   [%ld]\n",phdl->numTransferred);
	UnionNullLog("startTime::        [%ld]\n",phdl->startTime);
	UnionNullLog("finishTime::       [%ld]\n",phdl->finishTime);
	UnionNullLog("currentTime::      [%ld]\n",phdl->currentTime);
	return;
}

/*
   功能
   设置文件传输属性
   输入参数
   phdl	传输引擎指针
   输出参数
   无
   返回值
   >=0	成功
   <0	错误码
 */
int UnionSetTransAttrForTransferHandle(PUnionFileTransferHandle phdl)
{
	char	fullFileName[512+1];
	int	ret;

	if (phdl->totalFileLength > 0)//recive file for Server
	{
		memset(fullFileName,0,sizeof(fullFileName));
		UnionFormFullFileName(phdl->destinitionDir ,phdl->destinitionFileName, fullFileName);//get destinitionFile info
#ifdef _SFT_
		if (access(fullFileName, 0))
			phdl->lengthTransferred = 0;
		else if ((phdl->lengthTransferred = UnionGetFileSize(fullFileName)) <0)
		{
			UnionUserErrLog("in UnionSetTransAttrForTransferHandle::UnionGetFileSize [%s]!\n", fullFileName);
			return(phdl->lengthTransferred);
		}
#else
		phdl->lengthTransferred = 0;
#endif
	}
	else//send file for Server
	{
		memset(fullFileName,0,sizeof(fullFileName));
                UnionFormFullFileName(phdl->originDir ,phdl->originFileName, fullFileName);
		if ((phdl->totalFileLength = UnionGetFileSize(fullFileName)) <0)
		{
			UnionUserErrLog("in UnionSetTransAttrForTransferHandle::UnionGetFileSize [%s]!\n", fullFileName);
			return(phdl->totalFileLength);
		}
		if ((ret = UnionMD5File(fullFileName, (unsigned char*)phdl->checkValue)) <0)
		{
			phdl->totalFileLength = ret;
			UnionUserErrLog("in UnionSetTransAttrForTransferHandle::UnionMD5File [%s]!\n", fullFileName);
			return(ret);
		}
		phdl->checkValue[32] = 0;
	}
	if (phdl->sizePerTransfer <= 0)
		phdl->sizePerTransfer = 8192;
	if (phdl->totalFileLength % phdl->sizePerTransfer == 0)
		phdl->totalNum = phdl->totalFileLength / phdl->sizePerTransfer;
	else
		phdl->totalNum = phdl->totalFileLength / phdl->sizePerTransfer + 1;

		phdl->numTransferred = phdl->lengthTransferred / phdl->sizePerTransfer;

	return(0);
}

/*
   功能
   执行一个文件
   输入参数
   phdl	传输引擎指针
   输出参数
   无
   返回值
   >=0	成功
   <0	错误码
 */
int UnionExecuteFileSpecBySpecTransferHandle(PUnionFileTransferHandle phdl)
{
	int				ret;
	char				outputFileName[256+1];

	// 获取执行结果的输出文件
	memset(outputFileName,0,sizeof(outputFileName));
#ifdef _WIN32
	char v_achWorkingDir[512];
	memset(v_achWorkingDir, 0, sizeof(v_achWorkingDir));
	UnionGetMainWorkingDir(v_achWorkingDir);
	sprintf(outputFileName,"%s/tmp/%d.tmp",v_achWorkingDir,getpid());
	return(0);
#else

	sprintf(outputFileName,"%s/%d",getenv("UNIONTEMP"),getpid());
	// 执行文件
	if ((ret = UnionExecuteSpecFile(phdl->originDir,phdl->originFileName,outputFileName,1)) < 0)
	{
		UnionUserErrLog("in UnionExecuteFileSpecBySpecTransferHandle:: UnionExecuteSpecFile!\n");
		return(ret);
	}
	strcpy(phdl->originDir,"");
	strcpy(phdl->originFileName,outputFileName);
	return(UnionSetTransAttrForTransferHandle(phdl));
#endif
}

#ifdef _unionFileTransfer_2_x_
// 呼叫创建一个文件传输连接
// ipAddr,port是输入参数，分别对应文件传输方的IP地址和传输使用的端口
// fileName是要传输的文件的名称，是输入参数; dir是要传输的文件的目录
// 返回值为文件传输句柄，NULL，表示创建失败
PUnionFileTransferHandle UnionCreateFileTransferHandle(char *ipAddr,int port,char *dir,char *fileName,char *desDir,char *desFileName)
#else
	// 呼叫创建一个文件传输连接
	// ipAddr,port是输入参数，分别对应文件传输方的IP地址和传输使用的端口
	// fileName是要传输的文件的名称，是输入参数; dir是要传输的文件的目录
	// 返回值为文件传输句柄，NULL，表示创建失败
PUnionFileTransferHandle UnionCreateFileTransferHandle(char *ipAddr,int port,char *dir,char *fileName)
#endif
{
	PUnionFileTransferHandle	phdl = NULL,ptmpHDL;
	TUnionFileTransferHandle	tmpHDL;
	int				ret,len;
	unsigned char			tmpBuf[4000+1];

	if (!UnionIsValidIPAddrStr(ipAddr) || (port <= 0))	
	{
		UnionUserErrLog("in UnionCreateFileTransferHandle:: ipAddr [%s] wrong or port [%d] wrong!\n",ipAddr,port);
		goto failCreate;
	}

	if ((phdl = (PUnionFileTransferHandle)malloc(sizeof(*phdl))) == NULL)
	{
		UnionSystemErrLog("in UnionCreateFileTransferHandle:: malloc!\n");
		goto failCreate;
	}

	memset(phdl,0,sizeof(*phdl));
	phdl->socket = -1;

	strcpy(phdl->ipAddrOfListener,ipAddr);
	phdl->port = port;
	if (strlen(fileName) > sizeof(phdl->originFileName) - 1)
	{
		UnionUserErrLog("in UnionCreateFileTransferHandle:: fileName [%s] too long!\n",fileName);
		goto failCreate;
	}
	strcpy(phdl->originFileName,fileName);

	if ((dir != NULL) && (strlen(dir) != 0))
		//UnionReadDirFromStr(dir,-1,phdl->originDir);
		strcpy(phdl->originDir,dir);
	
#ifdef _unionFileTransfer_2_x_
	if ((desFileName != NULL) && (strlen(desFileName) != 0))
		strcpy(phdl->destinitionFileName,desFileName);
	if ((desDir != NULL) && (strlen(desDir) != 0))
		//UnionReadDirFromStr(desDir,-1,phdl->destinitionDir);
		strcpy(phdl->destinitionDir,desDir);
	//UnionLog("**** desDir = [%s][%s] desFileName = [%s][%s]!\n",desDir,phdl->destinitionDir,desFileName,phdl->destinitionFileName);
#endif

	/*
	   if (strlen(dir) > sizeof(phdl->originDir) - 1)
	   {
	   UnionUserErrLog("in UnionCreateFileTransferHandle:: dir [%s] too long!\n",dir);
	   goto failCreate;
	   }
	   strcpy(phdl->originDir,dir);
	 */
	// 设置每次包传输的大小
	phdl->sizePerTransfer = gunionSizePerTransfer;
	phdl->totalFileLength = -1;
	// 设置文件传输属性
	if ((ret = UnionSetTransAttrForTransferHandle(phdl)) < 0)	
	{
		UnionUserErrLog("in UnionCreateFileTransferHandle:: UnionSetTransAttrForTransferHandle!\n");
		goto failCreate;
	}

	UnionLogFileTransferHandle(phdl,"Before start sending");

	if ((phdl->socket = UnionCreateSocketClient(phdl->ipAddrOfListener,phdl->port)) < 0)
	{
		UnionUserErrLog("in UnionCreateFileTransferHandle:: UnionCreateSocketClient [%s] [%d]!\n",phdl->ipAddrOfListener,phdl->port);
		goto failCreate;
	}

	memset(tmpBuf,0,sizeof(tmpBuf));
	// 将句柄打包
	if ((ret = UnionPutFileTransferHandleIntoRecStr(phdl,(char *)tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionCreateFileTransferHandle:: UnionPutFileTransferHandleIntoRecStr!\n");
		goto failCreate;
	}
	len = ret;
	// 与接收服务器通讯
	if ((ret = UnionCommWithSpecSckHDLWith2BytsLen(phdl->socket,(unsigned char *)tmpBuf,len,tmpBuf,sizeof(tmpBuf),1)) < 0)
	{
		UnionUserErrLog("in UnionCreateFileTransferHandle:: UnionCommWithSpecSckHDLWith2BytsLen!\n");
		goto failCreate;
	}
	len = ret;
	tmpBuf[len] = 0;
	//ptmpHDL = (PUnionFileTransferHandle)tmpBuf;
	// 读取句柄
	ptmpHDL = &tmpHDL;
	memset(ptmpHDL,0,sizeof(*ptmpHDL));
	if ((ret = UnionReadFileTransferHandleFromRecStr((char *)tmpBuf,len,ptmpHDL)) < 0)
	{
		UnionUserErrLog("in UnionCreateFileTransferHandle:: UnionReadFileTransferHandleFromRecStr [%04d][%s]!\n",len,tmpBuf);
		goto failCreate;
	}

	if (ptmpHDL->totalFileLength <0)
	{
		UnionUserErrLog("in UnionCreateFileTransferHandle::server requst error [%lld]\n", ptmpHDL->totalFileLength);
		goto failCreate;
	}
	//if ((strlen(ptmpHDL->destinitionFileName) == 0) || (strlen(ptmpHDL->destinitionDir) == 0))
	if ((strlen(ptmpHDL->destinitionFileName) == 0))
	{
		UnionUserErrLog("in UnionCreateFileTransferHandle:: destinitionFileName or destinitionDir is null!\n");
		goto failCreate;
	}


	strcpy(phdl->destinitionFileName,ptmpHDL->destinitionFileName);
	strcpy(phdl->destinitionDir,ptmpHDL->destinitionDir);
	strcpy(phdl->ipAddrOfCaller,ptmpHDL->ipAddrOfCaller);
	phdl->lengthTransferred = ptmpHDL->lengthTransferred;
	phdl->numTransferred = ptmpHDL->numTransferred;
	UnionLogFileTransferHandle(phdl,"After receiving receiver's answering");

	return(phdl);

failCreate:
	UnionReleaseFileTransferHandle(phdl);
	return(NULL);
}

// 呼叫创建一个文件接收连接
// ipAddr,port是输入参数，分别对应文件传输方的IP地址和传输使用的端口
// oriFileName是要发送端的文件的名称，是输入参数; oriDir是发送端的文件的目录
// desDir是接收端的文件目录，desFileName是接收端的文件名称
// 返回值为文件传输句柄，NULL，表示创建失败
PUnionFileTransferHandle UnionCreateFileReceiverHandleForSpecCmd(char *ipAddr,int port,char *oriDir,char *oriFileName,char *desDir,char *desFileName,int receiverCmd)
{
	PUnionFileTransferHandle	phdl = NULL,ptmpHDL;
	TUnionFileTransferHandle	tmpHDL;
	int				ret,len;
	unsigned char			tmpBuf[4000+1];

	if (!UnionIsValidIPAddrStr(ipAddr) || (port <= 0))	
	{
		UnionUserErrLog("in UnionCreateFileReceiverHandleForSpecCmd:: ipAddr [%s] wrong or port [%d] wrong!\n",ipAddr,port);
		goto failCreate;
	}

	if ((phdl = (PUnionFileTransferHandle)malloc(sizeof(*phdl))) == NULL)
	{
		UnionSystemErrLog("in UnionCreateFileReceiverHandleForSpecCmd:: malloc!\n");
		goto failCreate;
	}

	memset(phdl,0,sizeof(*phdl));
	phdl->socket = -1;

	strcpy(phdl->ipAddrOfListener,ipAddr);
	phdl->port = port;

	if (strlen(oriFileName) > sizeof(phdl->originFileName) - 1)
	{
		UnionUserErrLog("in UnionCreateFileReceiverHandleForSpecCmd:: fileName [%s] too long!\n",oriFileName);
		goto failCreate;
	}
	strcpy(phdl->originFileName,oriFileName);

	if ((oriDir != NULL) && (strlen(oriDir) != 0))
		//UnionReadDirFromStr(oriDir,-1,phdl->originDir);
		strcpy(phdl->originDir,oriDir);

	if ((desFileName != NULL) && (strlen(desFileName) != 0))
		strcpy(phdl->destinitionFileName,desFileName);
	if ((desDir != NULL) && (strlen(desDir) != 0))
		//UnionReadDirFromStr(desDir,-1,phdl->destinitionDir);
		strcpy(phdl->destinitionDir,desDir);

	// 设置每次包传输的大小
	phdl->sizePerTransfer = gunionSizePerTransfer;
	phdl->totalFileLength = 1;

	// 设置文件传输属性
	if ((ret = UnionSetTransAttrForTransferHandle(phdl)) < 0)	
	{
		UnionUserErrLog("in UnionCreateFileReceiverHandleForSpecCmd:: UnionSetTransAttrForTransferHandle!\n");
		goto failCreate;
	}

	phdl->totalFileLength = 0 - abs(receiverCmd);
	UnionLogFileTransferHandle(phdl,"Before start receiving");

	if ((phdl->socket = UnionCreateSocketClient(phdl->ipAddrOfListener,phdl->port)) < 0)
	{
		UnionUserErrLog("in UnionCreateFileReceiverHandleForSpecCmd:: UnionCreateSocketClient [%s] [%d]!\n",phdl->ipAddrOfListener,phdl->port);
		goto failCreate;
	}

	memset(tmpBuf,0,sizeof(tmpBuf));
	// 将句柄打包
	if ((ret = UnionPutFileTransferHandleIntoRecStr(phdl,(char *)tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionCreateFileReceiverHandleForSpecCmd:: UnionPutFileTransferHandleIntoRecStr!\n");
		goto failCreate;
	}
	len = ret;
#ifndef _WIN32
	UnionSetSockSendTimeout(phdl->socket, 20);
#endif
	// 与接收服务器通讯
	if ((ret = UnionCommWithSpecSckHDLWith2BytsLen(phdl->socket,(unsigned char *)tmpBuf,len,tmpBuf,sizeof(tmpBuf),1)) < 0)
	{
		UnionUserErrLog("in UnionCreateFileReceiverHandleForSpecCmd:: UnionCommWithSpecSckHDLWith2BytsLen!\n");
		goto failCreate;
	}
	len = ret;
	tmpBuf[len] = 0;
	//ptmpHDL = (PUnionFileTransferHandle)tmpBuf;
	// 读取句柄
	ptmpHDL = &tmpHDL;
	memset(ptmpHDL,0,sizeof(*ptmpHDL));
	if ((ret = UnionReadFileTransferHandleFromRecStr((char *)tmpBuf,len,ptmpHDL)) < 0)
	{
		UnionUserErrLog("in UnionCreateFileReceiverHandleForSpecCmd:: UnionReadFileTransferHandleFromRecStr!\n");
		goto failCreate;
	}

	if (ptmpHDL->totalFileLength < 0)
	{
		UnionUserErrLog("in UnionCreateFileReceiverHandleForSpecCmd::server requst error [%lld]!\n", ptmpHDL->totalFileLength);
		goto failCreate;
	}

	//if ((strlen(ptmpHDL->destinitionFileName) == 0) || (strlen(ptmpHDL->destinitionDir) == 0))
	if ((strlen(ptmpHDL->destinitionFileName) == 0))
	{
		UnionUserErrLog("in UnionCreateFileTransferHandle:: destinitionFileName or destinitionDir is null!\n");
		goto failCreate;
	}	
	/*
	   if ((strlen(ptmpHDL->destinitionFileName) == 0) || (strlen(ptmpHDL->destinitionDir) == 0))
	   {
	   UnionUserErrLog("in UnionCreateFileReceiverHandleForSpecCmd:: destinitionFileName or destinitionDir is null!\n");
	   goto failCreate;
	   }
	 */

	strcpy(phdl->destinitionFileName,ptmpHDL->destinitionFileName);
	strcpy(phdl->destinitionDir,ptmpHDL->destinitionDir);
	strcpy(phdl->ipAddrOfCaller,ptmpHDL->ipAddrOfCaller);
	strcpy((char*)phdl->checkValue, (char*)ptmpHDL->checkValue);
	phdl->totalFileLength = ptmpHDL->totalFileLength;
	phdl->totalNum = ptmpHDL->totalNum;
	phdl->sizePerTransfer = ptmpHDL->sizePerTransfer;
	phdl->numTransferred = ptmpHDL->numTransferred;
	UnionLogFileTransferHandle(phdl,"in UnionCreateFileReceiverHandleForSpecCmd:: After receiving receiver's answering");

	return(phdl);

failCreate:
	UnionReleaseFileTransferHandle(phdl);
	return(NULL);
}

// 呼叫创建一个文件接收连接
// ipAddr,port是输入参数，分别对应文件传输方的IP地址和传输使用的端口
// oriFileName是要发送端的文件的名称，是输入参数; oriDir是发送端的文件的目录
// desDir是接收端的文件目录，desFileName是接收端的文件名称
// 返回值为文件传输句柄，NULL，表示创建失败
PUnionFileTransferHandle UnionCreateFileReceiverHandle(char *ipAddr,int port,char *oriDir,char *oriFileName,char *desDir,char *desFileName)
{
	return(UnionCreateFileReceiverHandleForSpecCmd(ipAddr,port,oriDir,oriFileName,desDir,desFileName,conFileReceiverCmdTagDownloadFile));
}

// 初始化一个文件接收服务器名柄
PUnionFileReceiveServer UnionInitFileReceiveServer(int port)
{
	PUnionFileReceiveServer	preceiveServer = NULL;

	if ((preceiveServer = (PUnionFileReceiveServer)malloc(sizeof(*preceiveServer))) == NULL)
	{
		UnionSystemErrLog("in UnionInitFileReceiveServer:: malloc()!\n");
		goto initFail;
	}
	preceiveServer->port = port;
	if ((preceiveServer->socket = UnionInitializeTCPIPServer(preceiveServer->port)) < 0)
	{
		UnionUserErrLog("in UnionInitFileReceiveServer:: UnionInitializeTCPIPServer()!\n");
		goto initFail;
	}

	UnionAuditLog("in UnionInitFileReceiveServer:: a servier bound to [%d] started OK!\n",port);

	return(preceiveServer);
initFail:
	UnionReleaseFileReceiveServer(preceiveServer);
	return(NULL);	
}

// 接收呼叫创建一个文件传输连接
// 返回值为文件传输句柄，NULL，表示创建失败
PUnionFileTransferHandle UnionAcceptFileTransferCall(int socket_fd,char *ipAddr,PUnionFileReceiveServer preceiveServer)
{
	struct sockaddr_in	cli_addr;
	int			clilen;
	struct linger		Linger;
	int			ret = 0;
	PUnionFileTransferHandle	phdl,ptmpHDL;
	TUnionFileTransferHandle	tmpHDL;
	unsigned char		tmpBuf[4000+1];
	int			len;

	if (preceiveServer == NULL)
	{
		UnionUserErrLog("in UnionAcceptFileTransferCall:: preceiveServer is null!\n");
		return(NULL);
	}

	if ((phdl = (PUnionFileTransferHandle)malloc(sizeof(*phdl))) == NULL)
	{
		UnionSystemErrLog("in UnionAcceptFileTransferCall:: malloc!\n");
		return(NULL);
	}
	memset(phdl,0,sizeof(*phdl));
	phdl->socket = -1;
	phdl->port = preceiveServer->port;

	if (socket_fd <= 0)
	{
		// Accept a Client's Connecting reqeust.
		clilen = sizeof(cli_addr);
		//phdl->socket = accept(preceiveServer->socket, (struct sockaddr *)&cli_addr,(unsigned long*)&clilen);
		phdl->socket = accept(preceiveServer->socket, (struct sockaddr *)&cli_addr,(socklen_t *)&clilen);
		if ( phdl->socket < 0 )
		{
			UnionSystemErrLog("in UnionAcceptFileTransferCall:: accept()!\n");
			goto failAccept;
		}
		memset(phdl->ipAddrOfCaller,0,sizeof(phdl->ipAddrOfCaller));
		inet_ntop(AF_INET, (void *)&cli_addr.sin_addr, phdl->ipAddrOfCaller, sizeof(phdl->ipAddrOfCaller));
	}
	else
	{
		phdl->socket = socket_fd;
		if (ipAddr == NULL)
		{
			UnionUserErrLog("in UnionAcceptFileTransferCall:: ipAddr is null!\n");
			return(NULL);
		}
		strcpy(phdl->ipAddrOfCaller, ipAddr);	
	}

	Linger.l_onoff = 1;
	Linger.l_linger = 5;
	if (setsockopt(phdl->socket,SOL_SOCKET,SO_LINGER,(char *)&Linger,sizeof(Linger)) != 0)
	{
		UnionSystemErrLog("in UnionAcceptFileTransferCall:: setsockopt linger!");
		goto failAccept;
	}

	UnionLog("in UnionAcceptFileTransferCall:: one client connectted now!\n");

	if ((ret = UnionReceiveFromSocketUntilLen(phdl->socket,tmpBuf,2)) < 0)
	{
		UnionUserErrLog("in UnionAcceptFileTransferCall:: UnionReceiveFromSocketUntilLen!\n");
		goto failAccept;
	}
	len = tmpBuf[0] * 256 + tmpBuf[1];
	if ((ret = UnionReceiveFromSocketUntilLen(phdl->socket,tmpBuf,len)) < 0)
	{
		UnionUserErrLog("in UnionAcceptFileTransferCall:: UnionReceiveFromSocketUntilLen!\n");
		goto failAccept;
	}
	if (ret != len)
	{
		UnionUserErrLog("in UnionAcceptFileTransferCall:: data len = [%d] != expected [%d]\n",ret,len);
		goto failAccept;
	}
	tmpBuf[len] = 0;
	UnionLog("in UnionAcceptFileTransferCall::recv [%s]\n", tmpBuf);
	memset(&tmpHDL,0,sizeof(tmpHDL));
	ptmpHDL = &tmpHDL;
	//memcpy(ptmpHDL,tmpBuf,sizeof(*ptmpHDL));
	if ((ret = UnionReadFileTransferHandleFromRecStr((char *)tmpBuf,len,ptmpHDL)) < 0)
	{
		UnionUserErrLog("in UnionAcceptFileTransferCall:: UnionReadFileTransferHandleFromRecStr! [%04d][%s]\n",len,tmpBuf);
		goto failAccept;
	}
	UnionLogFileTransferHandle(ptmpHDL,"client request::");
	strcpy(phdl->originFileName,ptmpHDL->originFileName);
	strcpy(phdl->originDir,ptmpHDL->originDir);
	strcpy(phdl->destinitionDir,ptmpHDL->destinitionDir);
	strcpy(phdl->destinitionFileName,ptmpHDL->destinitionFileName);
	strcpy((char*)phdl->checkValue, (char*)ptmpHDL->checkValue);
#ifdef _unionFileTransfer_2_x_
	if (ptmpHDL->totalFileLength > 0)//receive file
	{
		phdl->totalFileLength = ptmpHDL->totalFileLength;
		phdl->sizePerTransfer = ptmpHDL->sizePerTransfer;
		phdl->totalNum = ptmpHDL->totalNum;
		UnionSetAsFileReceiver();
		if ((ret = UnionAgreeToAccepteFileTransferred(phdl)) < 0)
		{
			UnionUserErrLog("in UnionAcceptFileTransferCall:: UnionAgreeToAccepteFileTransferred!\n");
			goto failAccept;
		}
		if ((ret = UnionSetTransAttrForTransferHandle(phdl)) < 0)
		{
			UnionUserErrLog("in UnionAcceptFileTransferCall:: UnionSetTransAttrForTransferHandle!\n");
			goto failAccept;
		}
	}
	else
	{
		switch (abs(ptmpHDL->totalFileLength))
		{
			case    conFileReceiverCmdTagExcuteFile:
				if ((ret = UnionExecuteFileSpecBySpecTransferHandle(phdl)) < 0)
				{
					UnionUserErrLog("in UnionAcceptFileTransferCall:: UnionExecuteFileSpecBySpecTransferHandle!\n");
					goto failAccept;
				}
				break;
			case	conFileReceiverCmdTagCheck://test 
				phdl->totalFileLength = 1;
				UnionSetAsCmd();
				break;
			default://send file
				phdl->sizePerTransfer = ptmpHDL->sizePerTransfer;
				phdl->lengthTransferred = ptmpHDL->lengthTransferred;
				UnionSetAsNoneFileReceiver();
				if ((ret = UnionAgreeToAccepteFileTransferred(phdl)) < 0)
				{
					UnionUserErrLog("in UnionAcceptFileTransferCall:: UnionAgreeToAccepteFileTransferred!\n");
					goto failAccept;
				}
				if ((ret = UnionSetTransAttrForTransferHandle(phdl)) < 0)
				{
					UnionUserErrLog("in UnionAcceptFileTransferCall:: UnionSetTransAttrForTransferHandle!\n");
					goto failAccept;
				}
				break;
		}
	}
#else
	UnionSetAsFileReceiver();
	phdl->totalFileLength = ptmpHDL->totalFileLength;
	phdl->sizePerTransfer = ptmpHDL->sizePerTransfer;
	phdl->totalNum = ptmpHDL->totalNum;
#endif
	strcpy(phdl->ipAddrOfListener,ptmpHDL->ipAddrOfListener);

	if ((ret = UnionPutFileTransferHandleIntoRecStr(phdl,(char *)tmpBuf+2,sizeof(tmpBuf)-2)) <0)
	{
		UnionUserErrLog("in UnionAcceptFileTransferCall:: UnionPutFileTransferHandleIntoRecStr!\n");
		goto failAccept;
	}
	len = ret;
	/*
	   memcpy(tmpBuf+2,phdl,sizeof(*phdl));
	   tmpBuf[0] = sizeof(*phdl) / 256;
	   tmpBuf[1] = sizeof(*phdl) % 256;
	 */
	tmpBuf[0] = len / 256;
	tmpBuf[1] = len % 256;

	UnionLogFileTransferHandle(phdl,"Before start receiving");

	if ((ret = UnionSendToSocket(phdl->socket,tmpBuf,len + 2)) < 0)
	{
		UnionUserErrLog("in UnionAcceptFileTransferCall:: UnionSendToSocket!\n");
		goto failAccept;
	}
	UnionLog("in UnionAcceptFileTransferCall:: now step 7...\n");

	return(phdl);		

failAccept:
	phdl->totalFileLength = ret;
	if ((ret = UnionPutFileTransferHandleIntoRecStr(phdl,(char *)tmpBuf+2,sizeof(tmpBuf)-2)) <0)
	{
		UnionUserErrLog("in UnionAcceptFileTransferCall:: UnionPutFileTransferHandleIntoRecStr!\n");
		UnionReleaseFileTransferHandle(phdl);
		return(NULL);
	}
	len = ret;
	tmpBuf[0] = len / 256;
	tmpBuf[1] = len % 256;

	if ((ret = UnionSendToSocket(phdl->socket,tmpBuf,len + 2)) < 0)
	{
		UnionUserErrLog("in UnionAcceptFileTransferCall:: UnionSendToSocket!\n");
		UnionReleaseFileTransferHandle(phdl);
		return(NULL);
	}

	UnionReleaseFileTransferHandle(phdl);
	return(NULL);
}


// 释放文件接收服务器
int UnionReleaseFileReceiveServer(PUnionFileReceiveServer preceiveServer)
{
	if (preceiveServer == NULL)
		return(0);
	if (preceiveServer->socket >= 0)
		UnionCloseSocket(preceiveServer->socket);
	preceiveServer = NULL;
	return(0);
}

// 释放文件传输句柄
// 失败返回负值
int UnionReleaseFileTransferHandle(PUnionFileTransferHandle phdl)
{
	if (phdl == NULL)
		return(0);
	if (phdl->socket >= 0)
		UnionCloseSocket(phdl->socket);
	phdl = NULL;
	return(0);
}

// 传输一个文件，phdl是使用的传输句柄
int UnionTransferFile(PUnionFileTransferHandle phdl)
{
	int		lenOfBuf = 0;
	File		fp;
	unsigned char	*tmpBuf = NULL;
	//unsigned char	tmpBuf[4096+2+1];
	int		ret;
	ReadFileLen	sendLen,recvLen, rLen;
	int		isRBT = 1;
	char		fileName[512+1];

	if (phdl->totalFileLength == 0)
		return(0);
	if (phdl == NULL)
	{
		UnionUserErrLog("in UnionTransferFile:: phdl is NULL!\n");
		return(-1);
	}

	memset(fileName,0,sizeof(fileName));
	UnionFormFullFileName(phdl->originDir,phdl->originFileName,fileName);
	if ((fp = Ufopen(fileName, URead)) == OpenFail)
	{
		UnionUserErrLog("in UnionTransferFile:: Ufopen [%s]\n",fileName);
		return(-1);
	}

	if (isRBT)
	{
		Ufseek(fp, phdl->lengthTransferred, USEEK_SET);
	}
	else
	{
		phdl->numTransferred = 0;
		phdl->lengthTransferred = 0;
	}

	lenOfBuf = (phdl->sizePerTransfer + 3) * sizeof(unsigned char);
	tmpBuf = (unsigned char *)malloc(lenOfBuf);
	phdl->numTransferred++;
	rLen = phdl->sizePerTransfer;

#ifndef _WIN32
	UnionSetSockSendTimeout(phdl->socket, 10);
#endif
	time(&(phdl->startTime));
	while (!UEof(fp))
	{
		sendLen = -1;
		//当传输最后一段时，要注意长度。
		if (phdl->totalNum ==  phdl->numTransferred)
			rLen = phdl->totalFileLength - phdl->lengthTransferred;
		Ufread(fp, tmpBuf+2, rLen, sendLen);
		if (sendLen < 0 || sendLen > rLen)
			continue;
		tmpBuf[0] = sendLen / 256;
		tmpBuf[1] = sendLen % 256;
		UnionLog("in UnionTransferFile:: the %ldth transferring...\n",phdl->numTransferred);
		if ((ret = UnionSendToSocket(phdl->socket,tmpBuf,sendLen+2)) <= 0)
		{
			UnionUserErrLog("in UnionTransferFile:: UnionSendToSocket!\n");
			goto failSend;
		}
		if ((ret = UnionReceiveFromSocketUntilLen(phdl->socket,tmpBuf,2)) <= 0)
		{
			UnionUserErrLog("in UnionTransferFile:: UnionReceiveFromSocketUntilLen!\n");
			goto failSend;
		}
		if (((recvLen = tmpBuf[0] * 256 + tmpBuf[1]) < 2) || (recvLen >= lenOfBuf))
		{
			UnionUserErrLog("in UnionTransferFile:: len [%zu] <= lenOfBuf[%d]!\n",recvLen,lenOfBuf);
			goto failSend;
		}
		if ((ret = UnionReceiveFromSocketUntilLen(phdl->socket,tmpBuf,recvLen)) <= 0)
		{
			UnionUserErrLog("in UnionTransferFile:: UnionReceiveFromSocketUntilLen!\n");
			goto failSend;
		}
		if (strncmp((char *)tmpBuf,"00",2) != 0)
		{
			tmpBuf[2] = 0;
			UnionUserErrLog("in UnionTransferFile:: receiver response = [%s]!\n",tmpBuf);
			goto failSend;
		}
		UnionLog("in UnionTransferFile:: the %ldth finished transfer\n",phdl->numTransferred);
		time(&(phdl->currentTime));
		phdl->lengthTransferred += sendLen;
		if (phdl->numTransferred++ == phdl->totalNum)
			break; 
	}//remote md5 check response
	UnionLog("in UnionTransferFile::remote md5 check response!\n");
#ifndef _WIN32
	UnionSetSockSendTimeout(phdl->socket, 20);
#endif
	if ((ret = UnionReceiveFromSocketUntilLen(phdl->socket,tmpBuf,2)) <= 0)
	{
		UnionUserErrLog("in UnionTransferFile:: UnionReceiveFromSocketUntilLen!\n");
		goto failSend;
	}
	if (((recvLen = tmpBuf[0] * 256 + tmpBuf[1]) < 2) || (recvLen >= lenOfBuf))
	{
		UnionUserErrLog("in UnionTransferFile:: len [%zu] <= lenOfBuf[%d]!\n",recvLen,lenOfBuf);
		goto failSend;
	}
	if ((ret = UnionReceiveFromSocketUntilLen(phdl->socket,tmpBuf,recvLen)) <= 0)
	{
		UnionUserErrLog("in UnionTransferFile:: UnionReceiveFromSocketUntilLen!\n");
		goto failSend;
	}
	if (strncmp((char *)tmpBuf,"00",2) != 0)
	{
		tmpBuf[2] = 0;
		UnionUserErrLog("in UnionTransferFile:: receiver response = [%s]!\n",tmpBuf);
		goto failSend;
	}
	free(tmpBuf);
	Ufclose(fp);
	fp = OpenFail;
	time(&(phdl->finishTime));
	return(0);

failSend:
	if (fp != OpenFail)
		Ufclose(fp);
	free(tmpBuf);
	return(-1);
}

// 接收一个文件，phdl是使用的传输句柄
int UnionReceiveFile(PUnionFileTransferHandle phdl)
{
	int		lenOfBuf = 0;
	File		fp;
	unsigned char	*tmpBuf = NULL;
	//unsigned char	tmpBuf[4096+2+1];
	int		ret;
	ReadFileLen	sendLen,recvLen;
	char		tmpBuf1[512+1];
	int		isRBT = 1;
	char		fileName[512+1];
	unsigned char	fileMD5[32+1];
	

	if (phdl == NULL)
	{
		UnionUserErrLog("in UnionReceiveFile:: phdl is NULL!\n");
		return(-1);
	}

	if (phdl->totalFileLength == 0)
                return(0);

	lenOfBuf = (phdl->sizePerTransfer + 3) * sizeof(unsigned char);
	tmpBuf = (unsigned char *)malloc(lenOfBuf);

	memset(fileName,0,sizeof(fileName));
	UnionReadDirFromStr(phdl->destinitionDir,-1,fileName);
	memset(tmpBuf1,0,sizeof(tmpBuf1));
	UnionReadDirFromStr(phdl->destinitionFileName,-1,tmpBuf1);

	UnionAppendFirstDirToSecondDir(tmpBuf1,fileName);
	UnionLog("in UnionReceiveFile:: begin to receive file [%s] [%s] [%s]\n",phdl->originDir, phdl->originFileName,fileName);
	UnionCreateDirFromFullFileName(fileName);	// 创建路径

#ifndef _SFT_
	isRBT = 0;
#endif

	if (isRBT)
	{	
		fp = Ufopen(fileName, UAppendWrite);
		if (fp == OpenFail)
		{
			UnionUserErrLog("in UnionReceiveFile:: Ufopen [%s]\n",fileName);
			goto failReceive;
		}
		Ufseek(fp, phdl->lengthTransferred, USEEK_SET);
	}
	else
	{
		fp = Ufopen(fileName, UWrite);
		if (fp == OpenFail)
		{
			UnionUserErrLog("in UnionReceiveFile:: Ufopen [%s]\n",fileName);
			goto failReceive;
		}
		phdl->numTransferred = 0;
		phdl->lengthTransferred = 0;
	}
	phdl->numTransferred++;

#ifndef _WIN32
	UnionSetSockSendTimeout(phdl->socket, 10);
#endif
	time(&(phdl->startTime));
	for (;;)
	{
		if (phdl->lengthTransferred >= phdl->totalFileLength)
			break;
		UnionLog("in UnionTransferFile:: the %ldth receiving...\n",phdl->numTransferred);
		if ((ret = UnionReceiveFromSocketUntilLen(phdl->socket,tmpBuf,2)) != 2)
		{
			UnionUserErrLog("in UnionReceiveFile:: UnionReceiveFromSocketUntilLen!\n");
			goto failReceive;
		}
		if (((recvLen = tmpBuf[0] * 256 + tmpBuf[1]) < 2) || (recvLen >= lenOfBuf))
		{
			UnionUserErrLog("in UnionReceiveFile:: recvLen [%zu] >= lenOfBuf[%d]!\n",recvLen,lenOfBuf);
			goto failReceive;
		}
		if ((ret = UnionReceiveFromSocketUntilLen(phdl->socket,tmpBuf,recvLen)) < 0)
		{
			UnionUserErrLog("in UnionReceiveFile:: UnionReceiveFromSocketUntilLen!\n");
			goto failReceive;
		}
		if (ret != recvLen)
		{
			UnionUserErrLog("in UnionReceiveFile:: real received len [%d] != expected len [%zu]!\n",ret,recvLen);
			goto failReceive;
		}
		//fwrite(tmpBuf,recvLen,1,fp);
		Ufwrite(fp, tmpBuf, recvLen, sendLen); 
		time(&(phdl->currentTime));
		phdl->lengthTransferred += recvLen;
		UnionLog("in UnionReceiveFile:: the %ldth finished receive\n",phdl->numTransferred);
		++phdl->numTransferred;
		tmpBuf[0] = 0;
		tmpBuf[1] = 0x02;
		tmpBuf[2] = '0';
		tmpBuf[3] = '0';
		if ((ret = UnionSendToSocket(phdl->socket,tmpBuf,4)) < 0)
		{
			UnionUserErrLog("in UnionReceiveFile:: UnionSendToSocket!\n");
			goto failReceive;
		}

	}
	//md5 check
	UnionLog("in UnionTransferFile::local md5 check response!\n");
	Ufflush(fp);
	Ufclose(fp);
	fp = OpenFail;

	memset(fileMD5,0,sizeof(fileMD5));
	UnionMD5File(fileName,fileMD5);
	tmpBuf[0] = 0;
	tmpBuf[1] = 0x02;
	if (strncmp((char *)fileMD5,(char *)phdl->checkValue, 32) != 0)
	{
		UnionUserErrLog("in UnionReceiveFile:: fileMD5[%s] phdl->checkValue[%s]!\n",fileMD5, phdl->checkValue);
		memcpy(tmpBuf+2,"02",2);
		lenOfBuf = -1;//set the fail flag
	}
	else
		memcpy(tmpBuf+2,"00",2);
	if ((ret = UnionSendToSocket(phdl->socket,tmpBuf,4)) < 0)
	{
		UnionUserErrLog("in UnionReceiveFile:: UnionSendToSocket!\n");
		goto failReceive;
	}
	sleep(3);
	if (lenOfBuf < 0)
		goto failReceive;
	UnionLog("in UnionReceiveFile:: receive file [%s] [%s] ok!\n",phdl->destinitionDir,phdl->destinitionFileName);
	time(&(phdl->finishTime));
	free(tmpBuf);
	return(0);
failReceive:
	free(tmpBuf);
	if (fp != OpenFail)
		Ufclose(fp);
	return(-1);
}

/*
   功能
   使用引擎发送一个文件
   输入参数
   desIPAddr	接收者的IP地址
   desPort		接收者的端口号
   oriDir		源文件目录
   oriFile		源文件名
   desDir		目标文件目录，如果为空，则目标文件目录由对端指定
   desFile		目标文件名，如果为空，则目标文件名与源文件名相同
   输出参数
   无
   返回值
   >=0		成功
   <0		失败，错误码
 */
int UnionSendFileByEngine(char *desIPAddr,int desPort,char *oriDir,char *oriFile,char *desDir,char *desFile)

{
	int				ret;
	PUnionFileTransferHandle	pgfileTransferHandle;

	UnionSetAsNoneFileReceiver();	// 设置成文件发送者
#ifdef _unionFileTransfer_2_x_
	if ((pgfileTransferHandle = UnionCreateFileTransferHandle(desIPAddr,desPort,oriDir,oriFile,desDir,desFile)) == NULL)
	{
		UnionUserErrLog("in UnionSendFileByEngine:: UnionCreateFileTransferHandle Error!\n");
		return(errCodeNullPointer);
	}
#else
	if ((pgfileTransferHandle = UnionCreateFileTransferHandle(desIPAddr,desPort,oriDir,oriFile)) == NULL)
	{
		UnionUserErrLog("in UnionSendFileByEngine:: UnionCreateFileTransferHandle Error!\n");
		return(errCodeNullPointer);
	}
#endif	
	if ((ret = UnionTransferFile(pgfileTransferHandle)) < 0)
		UnionUserErrLog("in UnionSendFileByEngine::UnionTransferFile failure! [%s-%d-%s-%s]\n",desIPAddr,desPort,oriDir,oriFile);
	else
		UnionLog("in UnionSendFileByEngine::UnionTransferFile OK! [%s-%d-%s-%s]\n",desIPAddr,desPort,oriDir,oriFile);

	UnionLog("time used = [%ld]\n",pgfileTransferHandle->finishTime - pgfileTransferHandle->startTime);

	UnionReleaseFileTransferHandle(pgfileTransferHandle);

	return(ret);
}	

/*
   功能
   使用引擎发送一个文件
   输入参数
   desIPAddr	接收者的IP地址
   desPort		接收者的端口号
   oriDir		源文件目录
   oriFile		源文件名
   desDir		目标文件目录，如果为空，则目标文件目录由对端指定
   desFile		目标文件名，如果为空，则目标文件名与源文件名相同
   输出参数
   无
   返回值
   >=0		成功
   <0		失败，错误码
 */
int UnionReceiveFileByEngine(char *desIPAddr,int desPort,char *oriDir,char *oriFile,char *desDir,char *desFile)

{
	int				ret;
	PUnionFileTransferHandle	pgfileTransferHandle;

	UnionSetAsFileReceiver();	// 设置成文件接收者
	if ((pgfileTransferHandle = UnionCreateFileReceiverHandle(desIPAddr,desPort,oriDir,oriFile,desDir,desFile)) == NULL)
	{
		UnionUserErrLog("in UnionReceiveFileByEngine:: UnionCreateFileReceiverHandle Error!\n");
		return(errCodeNullPointer);
	}

	if ((ret = UnionReceiveFile(pgfileTransferHandle)) < 0)
		UnionUserErrLog("in UnionReceiveFileByEngine::UnionReceiveFile failure! [%s-%d-%s-%s]\n",desIPAddr,desPort,desDir,desFile);
	else
		UnionLog("in UnionReceiveFileByEngine::UnionReceiveFile OK! [%s-%d-%s-%s]\n",desIPAddr,desPort,desFile,desFile);

	UnionLog("time used = [%ld]\n",pgfileTransferHandle->finishTime - pgfileTransferHandle->startTime);

	UnionReleaseFileTransferHandle(pgfileTransferHandle);

	return(ret);
}

/*
   功能
   使用引擎发送一个文件
   输入参数
   desIPAddr	接收者的IP地址
   desPort		接收者的端口号
   oriDir		源文件目录
   oriFile		源文件名
   desDir		目标文件目录，如果为空，则目标文件目录由对端指定
   desFile		目标文件名，如果为空，则目标文件名与源文件名相同
   输出参数
   无
   返回值
   >=0		成功
   <0		失败，错误码
 */
int UnionExcuteFileAtFileReceiverByEngine(char *desIPAddr,int desPort,char *oriDir,char *oriFile,char *desDir,char *desFile)
{
	int				ret;
	PUnionFileTransferHandle	pgfileTransferHandle;
	char				tmpBuf[1024];

	if ((pgfileTransferHandle = UnionCreateFileReceiverHandleForSpecCmd(desIPAddr,desPort,oriDir,oriFile,desDir,desFile,conFileReceiverCmdTagExcuteFile)) == NULL)
	{
		UnionUserErrLog("in UnionExcuteFileAtFileReceiverByEngine:: UnionCreateFileReceiverHandle Error!\n");
		return(errCodeNullPointer);
	}

	if ((ret = UnionReceiveFile(pgfileTransferHandle)) < 0)
		UnionUserErrLog("in UnionExcuteFileAtFileReceiverByEngine::UnionReceiveFile failure! [%s-%d-%s-%s]\n",desIPAddr,desPort,desDir,desFile);
	else
		UnionLog("in UnionExcuteFileAtFileReceiverByEngine::UnionReceiveFile OK! [%s-%d-%s-%s]\n",desIPAddr,desPort,desFile,desFile);

	UnionLog("time used = [%ld]\n",pgfileTransferHandle->finishTime - pgfileTransferHandle->startTime);
	sprintf(tmpBuf,"chmod a+x %s/%s",desDir,desFile);
	system(tmpBuf);
	UnionReleaseFileTransferHandle(pgfileTransferHandle);

	return(ret);
}
