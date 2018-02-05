//	Wolfgang Wang, 2006/08/08

/* 2007/12/22,增加函数
 	UnionCommunicationWithSpecMngSvr
	UnionIsMngSvrReturnedError
	UnionGetMngSvrErrorInfo
	增加了变量：
		gunionIsMngSvrReturnedError
		gunionMsgSvrReturnedErrorInfo
	以控制获取mngSvr返回的错误
	修改
	UnionCommunicationWithMngSvr,将这个函数中的代码移到了UnionCommunicationWithSpecMngSvr，然后这个函数调用
	UnionCommunicationWithSpecMngSvr
*/

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#ifndef _WIN32
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#else
#include <process.h>
#include <windows.h>
#endif

#include "UnionSocket.h"		// 使用3.x版本
#include "unionResID.h"
#include "UnionLog.h"			// 使用3.x版本
#include "unionRecFile.h"		
#include "mngSvrCommProtocol.h"
#include "mngSvrServicePackage.h"
#include "UnionStr.h"
#include "simuMngSvrLocally.h"
#include "unionErrCode.h"
#include "UnionLog.h"
#ifndef _UnionEnv_3_x_
#define _UnionEnv_3_x_
#endif
#include "UnionEnv.h"

// Add by HuangBaoxin 2008/11/12
#ifdef _WIN32
#include "simuMngSvrLocally.h"
#include "errCodeTranslater.h"
#endif
// End Add

#ifndef _WIN32
char	pgMngSvrTmpFileName[256+1];
unsigned long	gunionTmpFileIndex = 0;
int	gunionIsUserSetTmpFileName = 0;		// 王纯军，2010/5/18,增加
#endif

char	gunionRemoteMngSvrName[128+1] = "";	// 王纯军，2009/3/18,增加
int	gunionMngSvrClientSckHDL = -1;		// 王纯军，2009/3/18,增加
int	gunionIsMngSvrClientUseLongConn = 0;	// 王纯军，2009/3/18,增加

// 2007/12/22 增加
int 	gunionIsMngSvrReturnedError = 0;
char 	gunionMsgSvrReturnedErrorInfo[128+1];
// 2007/12/22 增加结束

// 王纯军，2009/3/18,增加
// 建立mngSvrClient与后台的通讯连接
int UnionCreateMngSvrClientSckHDL(char *ipAddr,int port)
{
	int	sckHDL;
	
	if (gunionIsMngSvrClientUseLongConn)	// 使用长连接
	{
		if (gunionMngSvrClientSckHDL >= 0)
			return(gunionMngSvrClientSckHDL);
	}
	
	// 使用短连接或使用长连接但连接未建
	if ((sckHDL = UnionCreateSocketClient(ipAddr,port)) < 0)
	{
		UnionUserErrLog("in UnionCreateMngSvrClientSckHDL:: UnionCreateSocketClient [%s] [%d]!\n",ipAddr,port);
		return(sckHDL);
	}
	return(gunionMngSvrClientSckHDL = sckHDL);
}

// 王纯军，2009/3/18,增加
// 关闭mngSvrClient与后台的通讯连接
int UnionCloseMngSvrClientSckHDL(int sckHDL)
{
	// if (gunionIsMngSvrClientUseLongConn)	// 使用长连接，不关闭
	//	return(0);
	UnionCloseSocket(sckHDL);
	gunionMngSvrClientSckHDL = -1;
	return(0);
}

// 王纯军，2009/3/18,增加
// 设置mngSvrClient与后台使用长连接通讯
void UnionSetMngSvrClientUseLongConn()
{
	gunionIsMngSvrClientUseLongConn = 1;
	return;
}

// 王纯军，2009/3/18,增加
// 设置mngSvrClient与后台使用短连接通讯
void UnionSetMngSvrClientUseShortConn()
{
	gunionIsMngSvrClientUseLongConn = 0;
	return;
}

// 设置暂时文件	2010-5-18,王纯军
void UnionSetUserSpecMngSvrTempFileName(char *fileName)
{
	strcpy(pgMngSvrTmpFileName,fileName);
	gunionIsUserSetTmpFileName = 1;
}

// 重置暂时文件	2010-5-18,王纯军
void UnionResetUserSpecMngSvrTempFileName()
{
	memset(pgMngSvrTmpFileName,0,sizeof(pgMngSvrTmpFileName));
	gunionIsUserSetTmpFileName = 0;
}

int UnionSetRemoteMngSvrName(char *name)
{
	memset(gunionRemoteMngSvrName,0,sizeof(gunionRemoteMngSvrName));
	strcpy(gunionRemoteMngSvrName,name);
}

// 2009/3/19增加
char *UnionGetCurrentRemoteMngSvrName()
{
	return(gunionRemoteMngSvrName);
}

// 2007/12/22 增加
// 判断是否是ESSC返回失败
int UnionIsMngSvrReturnedError()
{
	return(gunionIsMngSvrReturnedError);
}

// 2007/12/22 增加
// 获取mngSvr返回的错误码
char *UnionGetMngSvrErrorInfo()
{
	if (!gunionIsMngSvrReturnedError)
	{
		memset(gunionMsgSvrReturnedErrorInfo,0,sizeof(gunionMsgSvrReturnedErrorInfo));
	}
	gunionMsgSvrReturnedErrorInfo[sizeof(gunionMsgSvrReturnedErrorInfo)-1] = 0;
	return(gunionMsgSvrReturnedErrorInfo);
}
		
int UnionConvertDosStrIntoUnixStr(char *str,int lenOfStr)
{
	char	tmpBuf[2048+1];
	int	index1,index2 = 0;
	int	isReturnChar;
	
	for (index1 = 0; (index1 < lenOfStr) && (index2 < sizeof(tmpBuf)); index1++)
	{
		if (str[index1] != 0x0d)
		{
			tmpBuf[index2] = str[index1];
			index2++;
			continue;
		}
		index1++;
		if (index1 >= lenOfStr)
		{
			tmpBuf[index2] = str[index1];
			index2++;
			break;
		}
		if (str[index1] == 0x0a)	// 是回车换行
		{
			tmpBuf[index2] = 0x0a;
			index2++;
			continue;
		}
		else
		{
			tmpBuf[index2] = 0x0d;
			index2++;
			if (index2 >= sizeof(tmpBuf))
				break;
			tmpBuf[index2] = str[index1];
			index2++;
			continue;
		}
	}
	if (index2 >= sizeof(tmpBuf))	// 不转换
		return(lenOfStr);		
	memcpy(str,tmpBuf,index2);
	return(index2);
}

// 获取一个暂时文件名称
#ifndef _WIN32
void UnionFormSelfTempFileName(char *fileName)
{
	sprintf(fileName,"%s/%d.self.tmp",getenv("UNIONTEMP"),getpid());
	return;
}

// 删除暂时文件
int UnionDeleteSelfTempFile()
{
	char	tmpBuf[256];
	char	fileName[256];
	
	UnionFormSelfTempFileName(fileName);
	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"rm %s",fileName);
	system(tmpBuf);
	return(0);
}

char *UnionGenerateMngSvrTempFile()
{
	// 2010-5-18,王纯军增加
	if (gunionIsUserSetTmpFileName)		
		return(pgMngSvrTmpFileName);
	// 2010-5-18,王纯军增加结束
	memset(pgMngSvrTmpFileName,0,sizeof(pgMngSvrTmpFileName));
	sprintf(pgMngSvrTmpFileName,"%s/%05d.%ld.tmp",getenv("UNIONTEMP"),getpid(),++gunionTmpFileIndex);
	return(pgMngSvrTmpFileName);
}

int UnionDeleteAllMngSvrTempFile()
{
	int	i;
	char	tmpBuf[512];

	sprintf(tmpBuf,"rm %s/*.tmp",getenv("UNIONTEMP"));
	system(tmpBuf);
	return(0);
}

// 获取当前暂时文件的名称
char *UnionGetCurrentMngSvrTempFileName()
{
	return(pgMngSvrTmpFileName);
}

// 删除暂时文件
int UnionDeleteMngSvrTempFile()
{
	char	tmpBuf[512];
	
	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"rm %s",pgMngSvrTmpFileName);
	system(tmpBuf);
	return(0);
}

#endif

// 在服务器和客户端之间，发送一个数据文件
int UnionTransferMngDataFile(int sckHDL,char *fileName,char *tellerNo,int resID)
{
	FILE		*fp = NULL;
	unsigned char	tmpBuf[2048+1+2];
	char		data[2000+1];
	int		ret,sendLen=0;
	long		realSendLen = 0;
	char		resIDBuf[10+1];
	
	if ((sckHDL < 0) || (fileName == NULL) || (tellerNo == NULL))
	{
		UnionUserErrLog("in UnionTransferMngDataFile:: sckHDL = [%d]!\n",sckHDL);
		return(errCodeParameter);
	}
	sprintf(resIDBuf,"%03d",resID);
	//UnionDebugNullLog("\ntransfer [%s] beginning ...\n",fileName);
	if ((fp = fopen(fileName,"rb")) == NULL)
	{
		UnionSystemErrLog("in UnionTransferMngDataFile:: fopen [%s]\n",fileName);
		return(errCodeUseOSErrCode);
	}
	// 先发送一个传输开始包
	if ((ret = UnionPackMngSvrRequestPackage(" ",0,tellerNo,atoi(resIDBuf),conResCmdStartSendingData,(char *)tmpBuf+2,sizeof(tmpBuf)-2)) < 0)
	{
		UnionSystemErrLog("in UnionTransferMngDataFile:: UnionPackMngSvrRequestPackage!\n");
		goto errExit;
	}
	tmpBuf[2+ret] = 0;
	//UnionDebugNullLog("[%04d][%s]\n",ret,tmpBuf+2);
	tmpBuf[0] = ret / 256;
	tmpBuf[1] = ret % 256;
	if ((ret = UnionSendToSocket(sckHDL,tmpBuf,ret+2)) < 0)
	{
		UnionSystemErrLog("in UnionTransferMngDataFile:: UnionSendToSocket!\n");
		goto errExit;
	}
	// 开始发送文件	
	while (!feof(fp))
	{
		memset(data,0,sizeof(data));
		if ((sendLen = fread(data,1,sizeof(data)-1,fp)) <= 0)
		{
			//UnionUserErrLog("in UnionTransferMngDataFile:: sendLen = [%d] [%d] data = [%s]\n",sizeof(data),sendLen,data);
			continue;
		}
#ifdef _WIN32
		sendLen = UnionConvertDosStrIntoUnixStr(data,sendLen);
#endif
		if ((ret = UnionPackMngSvrRequestPackage(data,sendLen,tellerNo,atoi(resIDBuf),conResCmdSendingData,
			(char *)tmpBuf+2,sizeof(tmpBuf)-2)) < 0)
		{
			UnionSystemErrLog("in UnionTransferMngDataFile:: UnionPackMngSvrRequestPackage!\n");
			goto errExit;
		}
		tmpBuf[2+ret] = 0;
		//UnionDebugNullLog("[%04d][%s]\n",sendLen,tmpBuf+2);
		tmpBuf[0] = ret / 256;
		tmpBuf[1] = ret % 256;
		if ((ret = UnionSendToSocket(sckHDL,tmpBuf,ret+2)) < 0)
		{
			UnionUserErrLog("in UnionTransferMngDataFile:: UnionSendToSocket!\n");
			goto errExit;
		}
		realSendLen += sendLen;
	}
	// 再发送一个传输结束包
	sprintf(data,"%08ld",realSendLen);
	if ((ret = UnionPackMngPackage(data,strlen(data),tellerNo,atoi(resIDBuf),conResCmdFinishSendingData,(char *)tmpBuf+2,sizeof(tmpBuf)-2)) < 0)
	{
		UnionSystemErrLog("in UnionTransferMngDataFile:: UnionPackMngPackage!\n");
		goto errExit;
	}
	tmpBuf[2+ret] = 0;
	//UnionDebugNullLog("[%04d][%s]\n",ret,tmpBuf+2);
	tmpBuf[0] = ret / 256;
	tmpBuf[1] = ret % 256;
	if ((ret = UnionSendToSocket(sckHDL,tmpBuf,ret+2)) < 0)
	{
		UnionSystemErrLog("in UnionTransferMngDataFile:: UnionSendToSocket!\n");
		goto errExit;
	}
	//UnionDebugNullLog("transfer [%s] OK! [%08ld] characters send!\n\n",fileName,realSendLen);
errExit:
	if (fp != NULL)
		fclose(fp);
	return(ret);
}

// mngSvr服务器端向客户端发送一个文件
int UnionMngSvrTransferFile(int sckHDL,char *fileName)
{
	char		resID[10+1];
	char		tellerNo[40+1];
	int		ret;
		
	if ((sckHDL < 0) || (fileName == NULL))
	{
		UnionUserErrLog("in UnionMngSvrTransferFile:: sckHDL = [%d]!\n",sckHDL);
		return(errCodeParameter);
	}
	
	// 读资源号
	memset(resID,0,sizeof(resID));
	if ((ret = UnionReadResMngRequestPackageFld(conMngSvrPackFldNameResID,resID,sizeof(resID))) < 0)
	{
		UnionUserErrLog("in UnionMngSvrTransferFile:: UnionReadResMngRequestPackageFld = [%s]!\n",conMngSvrPackFldNameResID);
		return(ret);
	}
	// 读柜员号
	memset(tellerNo,0,sizeof(tellerNo));
	if ((ret = UnionReadResMngRequestPackageFld(conMngSvrPackFldNameTellerNo,tellerNo,sizeof(tellerNo))) < 0)
	{
		UnionUserErrLog("in UnionMngSvrTransferFile:: UnionReadResMngRequestPackageFld = [%s]!\n",conMngSvrPackFldNameTellerNo);
		return(ret);
	}
	return(UnionTransferMngDataFile(sckHDL,fileName,tellerNo,atoi(resID)));
}

// 2009/3/19，王纯军增加
int UnionGetConfOfSpecMngSvr(char *mngSvrName,char *ipAddr,int *port)
{
	int		ret;
	char		fileName[512];
	char		*p;
	int		i;
	
#ifdef _WIN32
	sprintf(fileName,"./esscMngSvr.conf");
#else
	sprintf(fileName,"%s/esscMngSvr.conf",getenv("UNIONETC"));
#endif
	if ((ret = UnionInitEnvi(fileName)) < 0)
	{
		UnionUserErrLog("in UnionGetConfOfSpecMngSvr:: UnionInitEnvi [%s]!\n",fileName);
		return(ret);
	}

	for (i = 0; i < UnionGetEnviVarNum(); i++)
	{
		// 读取名称
		if ((p = UnionGetEnviVarOfTheIndexByIndex(i,0)) == NULL)
		{
			UnionUserErrLog("in UnionGetConfOfSpecMngSvr:: UnionGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,0);
			continue;
		}
		if (strcmp(mngSvrName,p) != 0)	// 未找到
			continue;
		// 读取ip地址
		if ((p = UnionGetEnviVarOfTheIndexByIndex(i,1)) == NULL)
		{
			UnionUserErrLog("in UnionGetConfOfSpecMngSvr:: UnionGetEnviVarOfTheIndexByIndex [%d] [%d]\n",i,1);
			continue;
		}
		strcpy(ipAddr,p);
			
		// 读取端口号
		if ((p = UnionGetEnviVarOfTheIndexByIndex(i,2)) != NULL)
		{
			*port = atoi(p);
		}
		UnionClearEnvi();
		return(0);
	}
	UnionUserErrLog("in UnionGetConfOfSpecMngSvr:: mngSvr [%s] not defined!\n",mngSvrName);
	UnionClearEnvi();
		
	return(errCodeEsscMDL_MngSvrNotDefined);
}

int UnionGetConfOfMngSvr(char *ipAddr,int *port)
{
	int	ret;
	char	tmpBuf[2048+1];
	FILE	*fp;
	char	*ptr;
	char	tmpBuf2[100];
	char	*mngSvrName;
	
	memset(tmpBuf,0,sizeof(tmpBuf));
#ifdef _WIN32
	sprintf(tmpBuf,"./esscMngSvr.conf");
#else
	sprintf(tmpBuf,"%s/esscMngSvr.conf",getenv("UNIONETC"));
#endif
#ifdef _WIN32
	if ((fp = fopen(tmpBuf,"rb")) == NULL)
	{
		UnionSystemErrLog("in UnionGetConfOfMngSvr:: fopen [%s]\n",tmpBuf);
		return(errCodeUseOSErrCode);
	}
	fread(tmpBuf,sizeof(tmpBuf)-1,1,fp);
	fclose(fp);
	if ((ptr = strstr(tmpBuf,"ipAddr=")) == NULL)
	{
		UnionUserErrLog("in UnionGetConfOfMngSvr:: no ipAddr defined!\n");
		return(errCodeEsscMDL_NoIPAddrDefinedInMngSvrClientDefFile);
	}
	sscanf(ptr+7,"%s",tmpBuf2);
	if (!UnionIsValidIPAddrStr(tmpBuf2))
	{
		UnionUserErrLog("in UnionGetConfOfMngSvr:: %s is not valid ipAddr!\n",tmpBuf2);
		return(errCodeEsscMDL_NoIPAddrDefinedInMngSvrClientDefFile);
	}
	strcpy(ipAddr,tmpBuf2);
	if ((ptr = strstr(tmpBuf,"port=")) == NULL)
	{
		UnionUserErrLog("in UnionGetConfOfMngSvr:: no port defined!\n");
		return(errCodeEsscMDL_NoPortDefinedInMngSvrClientDefFile);
	}
	sscanf(ptr+5,"%d",port);
	if (*port <= 0)
	{
		UnionUserErrLog("in UnionGetConfOfMngSvr:: port [%d] error!\n",*port);
		return(errCodeEsscMDL_NoPortDefinedInMngSvrClientDefFile);
	}
#else
	if (((mngSvrName = UnionGetCurrentRemoteMngSvrName()) != NULL) && (strlen(mngSvrName) != 0))
	{
		if ((ret = UnionGetConfOfSpecMngSvr(mngSvrName,ipAddr,port)) >= 0)
			return(ret);
	}
	if ((ret = UnionInitEnvi(tmpBuf)) < 0)
	{	
		// printf("Fail to UnionInitEnvi [%s]\n",tmpBuf);
		UnionUserErrLog("Fail to UnionInitEnvi [%s]\n",tmpBuf);
		return(ret);
	}
	
	if ((ptr = UnionGetEnviVarByName("ipAddr")) == NULL)
	{
		// printf("Fail to get IPAddr!\n");
		UnionUserErrLog("Fail to get IPAddr!\n");
		UnionClearEnvi();
		return(UnionGetConfOfSpecMngSvr(mngSvrName,ipAddr,port));
	}
	else
	{
		memcpy(ipAddr,ptr,strlen(ptr));
		// printf("HsmSvr IP = [%s]\n",ptr);
		UnionNullLog("HsmSvr IP = [%s]\n",ptr);
	}

	if ((ptr = UnionGetEnviVarByName("port")) == NULL)
	{
		// printf("Fail to get Port!\n");
		UnionUserErrLog("Fail to get Port!\n");
		UnionClearEnvi();
		return(UnionGetConfOfSpecMngSvr(mngSvrName,ipAddr,port));
	}
	else
	{
		*port = atoi(ptr);
		// printf("Port = [%d]\n",*port);
		UnionNullLog("Port = [%d]\n",*port);
	}
	UnionClearEnvi();
#endif
	return(0);
}


// 与MngSvr通讯
// tellerNo,resID,serviceID,reqStr,lenOfReqStr是输入参数，分别对应操作员标识、资源、命令标识，请求数据，请求数据长度
// sizeOfResStr，是输入参数，指示了接收数据的缓冲区的大小
// resStr,是输出参数，是响应数据
// recvFileName,是输出参数，如果不等于NULL，表示服务器端发送来一个文件，是文件在本地的存储名称，如果是NULL，表示没有文件发送来
// 返回值是resStr中数据的长度
int UnionCommunicationWithMngSvr(char *tellerNo,int resID,int serviceID,
		char *reqStr,int lenOfReqStr,
		char *resStr,int sizeOfResStr,
		int *fileRecved)

{
	char		ipAddr[100];
	int		port;
	int		ret;

	gunionIsMngSvrReturnedError = 0;	// 2007/11/22增加

	// 获取ESSC的管理地址和端口
	memset(ipAddr,0,sizeof(ipAddr));
	if ((ret = UnionGetConfOfMngSvr(ipAddr,&port)) < 0)
	{
		UnionUserErrLog("in UnionCommunicationWithMngSvr:: UnionGetConfOfMngSvr!\n");
		return(ret);
	}
	return(UnionCommunicationWithSpecMngSvr(ipAddr,port,tellerNo,resID,serviceID,
		reqStr,lenOfReqStr,resStr,sizeOfResStr,fileRecved));
}


// 2009/5/29,王纯军重写了这个函数
// 与MngSvr通讯
// tellerNo,resID,serviceID,reqStr,lenOfReqStr是输入参数，分别对应操作员标识、资源、命令标识，请求数据，请求数据长度
// sizeOfResStr，是输入参数，指示了接收数据的缓冲区的大小
// resStr,是输出参数，是响应数据
// recvFileName,是输出参数，如果不等于NULL，表示服务器端发送来一个文件，是文件在本地的存储名称，如果是NULL，表示没有文件发送来
// 返回值是resStr中数据的长度
int UnionCommunicationWithSpecMngSvr(char *ipAddr,int port,char *tellerNo,int resID,int serviceID,
		char *reqStr,int lenOfReqStr,
		char *resStr,int sizeOfResStr,
		int *fileRecved)

{
	unsigned char 	tmpBuf[8192*2+1];
	int		len;
	int		ret;
	int		responseCode;

// Add by HuangBaoxin 2008/11/12
#ifdef _WIN32
        if (UnionIsReadMngSvrDataLocally())
        {
                if ((ret = UnionReadMngSvrDataLocally(-1, tellerNo,resID,serviceID,reqStr,lenOfReqStr,resStr,sizeOfResStr,fileRecved)) < 0)
                {
                        UnionUserErrLog("in UnionCommunicationWithSpecMngSvr:: UnionReadMngSvrDataLocally!\n");
                        strcpy(resStr,UnionFindRemarkOfErrCode(ret));
                }
		return(ret);
        }
#endif
// End Add

	gunionIsMngSvrReturnedError = 0;	// 2007/11/22增加
	
	if ((reqStr == NULL) || (resStr == NULL) || (lenOfReqStr < 0) || (sizeOfResStr < 0) ||
		(tellerNo == NULL) || (tellerNo == NULL))
	{
		UnionUserErrLog("in UnionCommunicationWithSpecMngSvr:: parameter error!\n");
		return(errCodeParameter);
	}
	// 打请求包
	if ((ret = UnionPackMngSvrRequestPackage(reqStr,lenOfReqStr,tellerNo,resID,serviceID,tmpBuf,sizeof(tmpBuf))) < 0)
	{
		UnionUserErrLog("in UnionCommunicationWithSpecMngSvr:: UnionPackMngSvrRequestPackage!\n");
		return(ret);
	}
	// 与服务器通讯
	if ((ret = UnionExchangeInfoWithSpecMngSvr(ipAddr,port,tmpBuf,ret,tmpBuf,sizeof(tmpBuf),fileRecved)) < 0)
	{
		UnionUserErrLog("in UnionCommunicationWithSpecMngSvr:: UnionPackMngSvrRequestPackage!\n");
		return(ret);
	}
	// 解响应包
	len = ret;
	if ((ret = UnionUnpackMngSvrResponsePackage((char *)tmpBuf,len,NULL,NULL,NULL,&responseCode,(char *)resStr,sizeOfResStr)) < 0)
	{
		UnionUserErrLog("in UnionCommunicationWithSpecMngSvr:: UnionUnpackMngSvrResponsePackage!\n");
		return(ret);
	}
	resStr[ret] = 0;
	if (responseCode < 0)		// 服务器返回的值不为0
	{
		ret = responseCode;
		// 2007/11/22增加
		gunionIsMngSvrReturnedError = 1;
		memset(gunionMsgSvrReturnedErrorInfo,0,sizeof(gunionMsgSvrReturnedErrorInfo));
		if (ret >= sizeof(gunionMsgSvrReturnedErrorInfo))
			memcpy(gunionMsgSvrReturnedErrorInfo,resStr,sizeof(gunionMsgSvrReturnedErrorInfo)-1);
		else
			memcpy(gunionMsgSvrReturnedErrorInfo,resStr,ret);
		// 2007/11/22增加结束
	}
	return(ret);
}

// 2009/5/29，王纯军增加，从UnionCommunicationWithSpecMngSvr中折分出这个函数
// 与MngSvr通讯
// reqStr,lenOfReqStr是输入参数请求数据，请求数据长度
// sizeOfResStr，是输入参数，指示了接收数据的缓冲区的大小
// resStr,是输出参数，是响应数据
// recvFileName,是输出参数，如果不等于NULL，表示服务器端发送来一个文件，是文件在本地的存储名称，如果是NULL，表示没有文件发送来
// 返回值是resStr中数据的长度
int UnionExchangeInfoWithSpecMngSvr(char *ipAddr,int port,char *reqStr,int lenOfReqStr,char *resStr,int sizeOfResStr,int *fileRecved)

{
	unsigned char 	tmpBuf[8192*2+1];
	int		len;
	int		ret;
	int		sckHDL = -1;
	FILE		*fp = NULL;
	int		mngSvrResCmd;
	int		dataRecvFinished = 0;
	char		*recvFileName;
	int		responseCode;
	int		createSckHDLRetryTimes = 0;	// 2010-5-18,王纯军增加
	
	if ((reqStr == NULL) || (resStr == NULL) || (lenOfReqStr < 0) || (sizeOfResStr < 0))
	{
		UnionUserErrLog("in UnionExchangeInfoWithSpecMngSvr:: parameter error!\n");
		return(errCodeParameter);
	}
	// 建立通讯连接
	// 2010-5-18,王纯军增加以下这段
recreateSckHDL:
	++createSckHDLRetryTimes;
	if (createSckHDLRetryTimes >= 2)
	{
		UnionUserErrLog("in UnionExchangeInfoWithSpecMngSvr:: createSckHDLRetryTimes = [%d]\n",createSckHDLRetryTimes);
		return(errCodeTooManyRetryTimes);
	}
	// 2010-5-18,王纯军增加段结束
	//if ((sckHDL = UnionCreateSocketClient(ipAddr,port)) < 0)	// 2010-5-18,王纯军删除
	if ((sckHDL = UnionCreateMngSvrClientSckHDL(ipAddr,port)) < 0)	// 2010-5-18,王纯军增加
	{
		UnionUserErrLog("in UnionExchangeInfoWithSpecMngSvr:: UnionCreateMngSvrClientSckHDL! [%s] [%d]\n",ipAddr,port);
		return(sckHDL);
	}
	tmpBuf[0] = lenOfReqStr / 256;
	tmpBuf[1] = lenOfReqStr % 256;
	if ((ret = UnionSendToSocket(sckHDL,tmpBuf,2)) < 0)
	{
		UnionUserErrLog("in UnionExchangeInfoWithSpecMngSvr:: UnionSendToSocket!\n");
		if (createSckHDLRetryTimes < 2)	// 2010-05-18,王纯军增加
			goto recreateSckHDL;	// 2010-05-18,王纯军增加	
		goto errorExit;
	}
	if ((ret = UnionSendToSocket(sckHDL,reqStr,lenOfReqStr)) < 0)
	{
		UnionUserErrLog("in UnionExchangeInfoWithSpecMngSvr:: UnionSendToSocket!\n");
		goto errorExit;
	}

	// 接收数据
	recvFileName = NULL;
	*fileRecved = 0;
	for (;;)
	{
		// 接收一个数据包
		memset(tmpBuf,0,2);
		if ((ret = UnionReceiveFromSocketUntilLen(sckHDL,tmpBuf,2)) < 0)
		{
			UnionUserErrLog("in UnionExchangeInfoWithSpecMngSvr:: UnionReceiveFromSocketUntilLen!\n");
			goto errorExit;
		}
		if (ret == 0)
		{
			UnionDebugLog("in UnionExchangeInfoWithSpecMngSvr:: svr close socket!\n");
			ret = errCodeEsscMDL_NoResponsePackFromMngSvr;
			goto errorExit;
		}
		if (((len = tmpBuf[0] * 256 + tmpBuf[1]) < 0) || (len > sizeof(tmpBuf)))
		{
			UnionUserErrLog("in UnionExchangeInfoWithSpecMngSvr:: len [%d] too large or too small!\n",len);
			ret = errCodeEsscMDL_DataLenFromMngSvr;
			goto errorExit;
		}
		if (len == 0)
			continue;
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReceiveFromSocketUntilLen(sckHDL,tmpBuf,len)) < 0)
		{
			UnionUserErrLog("in UnionExchangeInfoWithSpecMngSvr:: UnionReceiveFromSocketUntilLen!\n");
			goto errorExit;
		}
		if (ret == 0)
		{
			UnionDebugLog("in UnionExchangeInfoWithSpecMngSvr:: svr close socket!\n");
			ret = errCodeEsscMDL_NoResponsePackFromMngSvr;
			goto errorExit;
		}
		// 判断是不是响应包
		if (UnionIsMngSvrResponsePackage((char *)tmpBuf,len))		// 是响应包
		{
			if (recvFileName != NULL)		// 是否有数据传输
			{
				if (!dataRecvFinished)		// 有数据传，是否传输结束
				{
					UnionUserErrLog("in UnionExchangeInfoWithSpecMngSvr:: data send not finished yet!\n");
					ret = errCodeEsscMDL_NotFinishSendingData;
					goto errorExit;
				}
			}
			break;
		}
		// 是请求包，解这个请求包
		if ((len = UnionUnpackMngSvrRequestPackage((char *)tmpBuf,len,NULL,NULL,&mngSvrResCmd,(char *)tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionExchangeInfoWithSpecMngSvr:: UnionUnpackMngSvrRequestPackage!\n");
			ret = len;
			goto errorExit;
		}
		// 判断是不是数据开始传送包
		if (mngSvrResCmd == conResCmdStartSendingData) 
		{
			if (recvFileName != NULL)	// 已经收到过一个数据传输开始包
			{
				UnionUserErrLog("in UnionExchangeInfoWithSpecMngSvr:: another data start sending pack received again!\n");
				ret = errCodeEsscMDL_AnotherStartingSendDataCmd;
				goto errorExit;
			}
			if ((recvFileName = UnionGenerateMngSvrTempFile()) == NULL)	// 不能获得一个暂时存储文件名
			{
				UnionUserErrLog("in UnionExchangeInfoWithSpecMngSvr:: UnionGenerateMngSvrTempFile!\n");
				ret = errCodeEsscMDL_FailToGetTempFile;
				goto errorExit;
			}
			if ((fp = fopen(recvFileName,"wb")) == NULL)	// 打不开暂时文件
			{
				UnionSystemErrLog("in UnionExchangeInfoWithSpecMngSvr:: fopen [%s]!\n",recvFileName);
				ret = errCodeUseOSErrCode;
				goto errorExit;
			}
			continue;
		}
		// 判断是不是数据结束传送包
		if (mngSvrResCmd == conResCmdFinishSendingData) 
		{
			if (recvFileName == NULL)	// 没有收到过数据传输开始包
			{
				UnionUserErrLog("in UnionExchangeInfoWithSpecMngSvr:: no start sending pack received ever!\n");
				ret = errCodeEsscMDL_NoStartingSendDataCmdBefore;
				goto errorExit;
			}
			dataRecvFinished = 1;
			continue;
		}	
		// 判断是不是数据包
		if (mngSvrResCmd == conResCmdSendingData)
		{
			if ((recvFileName == NULL) || (fp == NULL))	// 没有收到过数据传输开始包
			{
				UnionUserErrLog("in UnionExchangeInfoWithSpecMngSvr:: no start sending pack received ever!\n");
				ret = errCodeEsscMDL_NoStartingSendDataCmdBefore;
				goto errorExit;
			}
			fwrite(tmpBuf,len,1,fp);
			continue;
		}
		UnionUserErrLog("in UnionExchangeInfoWithSpecMngSvr:: wrong request package received! [%03d]\n",mngSvrResCmd);
		ret = errCodeEsscMDL_InvalidOperation;
		goto errorExit;
	}
	if (len > sizeOfResStr)
	{
		UnionUserErrLog("in UnionExchangeInfoWithSpecMngSvr:: UnionUnpackMngSvrResponsePackage!\n");
		ret = errCodeSmallBuffer;
		goto errorExit;
	}
	memcpy(resStr,tmpBuf,len);		
	ret = len;
	resStr[len] = 0;
	if (recvFileName != NULL)
		*fileRecved = 1;
	// 2010-5-18,王纯军增加以下这段
	if (fp != NULL)
		fclose(fp);
	if ((sckHDL >= 0) && (!gunionIsMngSvrClientUseLongConn))
		UnionCloseMngSvrClientSckHDL(sckHDL);	// 2010-05-18,王纯军增加
	return(ret);
	// 2010-5-18,王纯军增加结束	
errorExit:
	if (fp != NULL)
		fclose(fp);
	if (sckHDL >= 0)
		// UnionCloseSocket(sckHDL);		// 2010-05-18,王纯军修改
		UnionCloseMngSvrClientSckHDL(sckHDL);	// 2010-05-18,王纯军增加
	return(ret);
}

// 客户端从服务器申请下载一个资源文件
// tellerNo,resID,是输入参数，分别对应操作员标识、资源
// sizeOfResStr，是输入参数，指示了接收数据的缓冲区的大小
// resStr,是输出参数，是响应数据
// fileRecved,是输出参数，是1，表示收到了资源文件，资源文件存储在一个暂时文件中
//	通过调用UnionGetCurrentMngSvrTempFileName获得这个暂时文件名
// 返回值是resStr中数据的长度
int UnionMngClientDownloadFile(char *tellerNo,int resID,
		char *resStr,int sizeOfResStr,
		int *fileRecved)
{
	//return(UnionCommunicationWithMngSvr(tellerNo,resID,conResCmdDownloadFile," ",0,resStr,sizeOfResStr,fileRecved));
	return(UnionMngClientDownloadSpecFile(tellerNo,resID,NULL,resStr,sizeOfResStr,fileRecved));
}

// 客户端从服务器申请下载一个指定名称的文件
// tellerNo,resID,是输入参数，分别对应操作员标识、资源
// sizeOfResStr，是输入参数，指示了接收数据的缓冲区的大小
// fileName，要下载的文件名称
// resStr,是输出参数，是响应数据
// fileRecved,是输出参数，是1，表示收到了资源文件，资源文件存储在一个暂时文件中
//	通过调用UnionGetCurrentMngSvrTempFileName获得这个暂时文件名
// 返回值是resStr中数据的长度
int UnionMngClientDownloadSpecFile(char *tellerNo,int resID,char *fileName,
		char *resStr,int sizeOfResStr,
		int *fileRecved)
{
	if (fileName == NULL)
		return(UnionCommunicationWithMngSvr(tellerNo,resID,conResCmdDownloadFile," ",0,resStr,sizeOfResStr,fileRecved));
	else
		return(UnionCommunicationWithMngSvr(tellerNo,resID,conResCmdDownloadFile,fileName,strlen(fileName),resStr,sizeOfResStr,fileRecved));
}

// 客户端从服务器申请下载一个指定名称的文件
// tellerNo,resID,是输入参数，分别对应操作员标识、资源
// sizeOfResStr，是输入参数，指示了接收数据的缓冲区的大小
// svrFileName，要下载的文件名称
// localFileName，下载的文件在本地的存储名称
// resStr,是输出参数，是响应数据
// fileRecved,是输出参数，是1，表示收到了资源文件，资源文件存储在一个暂时文件中
//	通过调用UnionGetCurrentMngSvrTempFileName获得这个暂时文件名
// 返回值是resStr中数据的长度
int UnionMngClientDownloadSpecFileToLocalFile(char *tellerNo,int resID,char *svrFileName,char *localFileName,
		char *resStr,int sizeOfResStr,
		int *fileRecved)
{
	int	ret;
	
	if ((ret = UnionMngClientDownloadSpecFile(tellerNo,resID,svrFileName,resStr,sizeOfResStr,fileRecved)) < 0)
	{
		UnionUserErrLog("in UnionMngClientDownloadSpecFileToLocalFile:: UnionMngClientDownloadSpecFile!\n");
		return(ret);
	}
	if ((localFileName != NULL) && (strlen(localFileName) != 0))
		return(UnionRenameFile(UnionGetCurrentMngSvrTempFileName(),localFileName));
	return(ret);
}

// 客户端向服务器上传一个资源文件
// tellerNo,resID,是输入参数，分别对应操作员标识、资源
// sizeOfResStr，是输入参数，指示了接收数据的缓冲区的大小
// resStr,是输出参数，是响应数据
// fileName,是输入参数，是要传输的文件名。
// svrFileName，输入参数，文件在服务器端的名称
// 返回值是resStr中数据的长度
int UnionMngClientUploadFileWithSvrFileName(char *fileName,char *svrFileName,char *tellerNo,int resID,
		char *resStr,int sizeOfResStr)
{
	unsigned char 	tmpBuf[2048+1];
	int		len;
	int		ret;
	char		ipAddr[100];
	int		port;
	int		sckHDL = -1;
	int		isFileSended = 0;
	int		responseCode;
	
	if ((fileName == NULL) || (tellerNo == NULL))
	{
		UnionUserErrLog("in UnionMngClientUploadFileWithSvrFileName:: parameter error!\n");
		return(errCodeParameter);
	}
	// 获取ESSC的管理地址和端口
	memset(ipAddr,0,sizeof(ipAddr));
	if ((ret = UnionGetConfOfMngSvr(ipAddr,&port)) < 0)
	{
		UnionUserErrLog("in UnionMngClientUploadFileWithSvrFileName:: UnionGetConfOfMngSvr!\n");
		return(ret);
	}
	// 建立通讯连接
	if ((sckHDL = UnionCreateSocketClient(ipAddr,port)) < 0)
	{
		UnionUserErrLog("in UnionMngClientUploadFileWithSvrFileName:: UnionCreateSocketClient! [%s] [%d]\n",ipAddr,port);
		return(sckHDL);
	}
	// 打请求包
	if (svrFileName == NULL)
		ret = UnionPackMngSvrRequestPackage(" ",0,tellerNo,resID,conResCmdUploadFile,(char *)tmpBuf+2,sizeof(tmpBuf)-2);
	else
		ret = UnionPackMngSvrRequestPackage(svrFileName,strlen(svrFileName),tellerNo,resID,conResCmdUploadFile,(char *)tmpBuf+2,sizeof(tmpBuf)-2);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionMngClientUploadFileWithSvrFileName:: UnionPackMngSvrRequestPackage!\n");
		goto errorExit;
	}
	tmpBuf[0] = ret / 256;
	tmpBuf[1] = ret % 256;
	// 发送请求
	if ((ret = UnionSendToSocket(sckHDL,tmpBuf,2+ret)) < 0)
	{
		UnionUserErrLog("in UnionMngClientUploadFileWithSvrFileName:: UnionSendToSocket!\n");
		goto errorExit;
	}
	goto recvAnswer;
sendFile:
	// 发送文件
	if ((ret = UnionTransferMngDataFile(sckHDL,fileName,tellerNo,resID)) < 0)
	{
		UnionUserErrLog("in UnionMngClientUploadFileWithSvrFileName:: UnionTransferMngDataFile!\n");
		goto errorExit;
	}
	isFileSended = 1;	// 设文件已发送标识
recvAnswer:	
	// 接收响应包
	memset(tmpBuf,0,2);
	if ((ret = UnionReceiveFromSocketUntilLen(sckHDL,tmpBuf,2)) < 0)
	{
		UnionUserErrLog("in UnionMngClientUploadFileWithSvrFileName:: UnionReceiveFromSocketUntilLen!\n");
		goto errorExit;
	}
	if (ret == 0)
	{
		UnionDebugLog("in UnionMngClientUploadFileWithSvrFileName:: svr close socket!\n");
		ret = errCodeEsscMDL_NoResponsePackFromMngSvr;
		goto errorExit;
	}
	if (((len = tmpBuf[0] * 256 + tmpBuf[1]) <= 0) || (len > sizeof(tmpBuf)))
	{
		UnionUserErrLog("in UnionMngClientUploadFileWithSvrFileName:: len [%d] too large or too small!\n",len);
		ret = errCodeEsscMDL_DataLenFromMngSvr;
		goto errorExit;
	}
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = UnionReceiveFromSocketUntilLen(sckHDL,tmpBuf,len)) < 0)
	{
		UnionUserErrLog("in UnionMngClientUploadFileWithSvrFileName:: UnionReceiveFromSocketUntilLen!\n");
		goto errorExit;
	}
	if (ret == 0)
	{
		UnionDebugLog("in UnionMngClientUploadFileWithSvrFileName:: svr close socket!\n");
		ret = errCodeEsscMDL_NoResponsePackFromMngSvr;
		goto errorExit;
	}
	if ((ret = UnionUnpackMngSvrResponsePackage((char *)tmpBuf,len,NULL,NULL,NULL,&responseCode,(char *)resStr,sizeOfResStr)) < 0)
	{
		UnionUserErrLog("in UnionMngClientUploadFileWithSvrFileName:: UnionUnpackMngSvrResponsePackage!\n");
		goto errorExit;
	}
	if (responseCode < 0)		// 服务器返回的值不为0
	{
		ret = responseCode;
		goto errorExit;
	}
	// 检查文件是不是已发送
	if (!isFileSended)
		goto sendFile;
errorExit:
	if (sckHDL >= 0)
		UnionCloseSocket(sckHDL);
	return(ret);
}


// 客户端向服务器上传一个资源文件
// tellerNo,resID,是输入参数，分别对应操作员标识、资源
// sizeOfResStr，是输入参数，指示了接收数据的缓冲区的大小
// resStr,是输出参数，是响应数据
// fileName,是输入参数，是要传输的文件名。
// 返回值是resStr中数据的长度
int UnionMngClientUploadFile(char *fileName,char *tellerNo,int resID,
		char *resStr,int sizeOfResStr)
{
	return(UnionMngClientUploadFileWithSvrFileName(fileName,NULL,tellerNo,resID,resStr,sizeOfResStr));
}

// MngSvr从客户端接收数据文件
int UnionMngSvrRecvFileFromClient(int sckHDL,char *overwrittenFileName)
{
	unsigned char 	tmpBuf[2048+1];
	int		len;
	int		ret;
	FILE		*fp = NULL;
	//int		recvDataLen = 0;
	//int		resStrLen = 0;
	int		mngSvrResCmd;
	char		*recvFileName;
	char		resID[10+1];
	char		tellerNo[40+1];
		
	// 读资源号
	memset(resID,0,sizeof(resID));
	if ((ret = UnionReadResMngRequestPackageFld(conMngSvrPackFldNameResID,resID,sizeof(resID))) < 0)
	{
		UnionUserErrLog("in UnionMngSvrRecvFileFromClient:: UnionReadResMngRequestPackageFld = [%s]!\n",conMngSvrPackFldNameResID);
		return(ret);
	}
	// 读柜员号
	memset(tellerNo,0,sizeof(tellerNo));
	if ((ret = UnionReadResMngRequestPackageFld(conMngSvrPackFldNameTellerNo,tellerNo,sizeof(tellerNo))) < 0)
	{
		UnionUserErrLog("in UnionMngSvrRecvFileFromClient:: UnionReadResMngRequestPackageFld = [%s]!\n",conMngSvrPackFldNameTellerNo);
		return(ret);
	}
	// 发送一个同意接收的数据包
	// 先打包
	if ((ret = UnionPackMngSvrResponsePackage(" ",0,tellerNo,atoi(resID),conResCmdUploadFile,0,(char *)tmpBuf+2,sizeof(tmpBuf)-2)) < 0)
	{
		UnionUserErrLog("in UnionMngSvrRecvFileFromClient:: UnionPackMngSvrResponsePackage!\n");
		return(ret);
	}
	// 再发送
	tmpBuf[0] = ret / 256;
	tmpBuf[1] = ret % 256;
	if ((ret = UnionSendToSocket(sckHDL,tmpBuf,ret+2)) < 0)
	{
		UnionUserErrLog("in UnionMngSvrRecvFileFromClient:: UnionSendToSocket!\n");
		return(ret);
	}
	// 接收数据
	recvFileName = NULL;
	for (;;)
	{
		// 接收一个数据包
		memset(tmpBuf,0,2);
		if ((ret = UnionReceiveFromSocketUntilLen(sckHDL,tmpBuf,2)) < 0)
		{
			UnionUserErrLog("in UnionMngSvrRecvFileFromClient:: UnionReceiveFromSocketUntilLen!\n");
			goto errorExit;
		}
		if (ret == 0)
		{
			UnionDebugLog("in UnionMngSvrRecvFileFromClient:: peer close socket!\n");
			ret = errCodeEsscMDL_NoResponsePackFromMngSvr;
			goto errorExit;
		}
		if (((len = tmpBuf[0] * 256 + tmpBuf[1]) < 0) || (len > sizeof(tmpBuf)))
		{
			UnionUserErrLog("in UnionMngSvrRecvFileFromClient:: len [%d] too large or too small!\n",len);
			ret = errCodeEsscMDL_DataLenFromMngSvr;
			goto errorExit;
		}
		if (len == 0)
			continue;
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReceiveFromSocketUntilLen(sckHDL,tmpBuf,len)) < 0)
		{
			UnionUserErrLog("in UnionMngSvrRecvFileFromClient:: UnionReceiveFromSocketUntilLen!\n");
			goto errorExit;
		}
		if (ret == 0)
		{
			UnionDebugLog("in UnionMngSvrRecvFileFromClient:: peer close socket!\n");
			ret = errCodeEsscMDL_NoResponsePackFromMngSvr;
			goto errorExit;
		}
		// 解这个请求包
		if ((len = UnionUnpackMngSvrRequestPackage((char *)tmpBuf,len,NULL,NULL,&mngSvrResCmd,(char *)tmpBuf,sizeof(tmpBuf))) < 0)
		{
			UnionUserErrLog("in UnionMngSvrRecvFileFromClient:: UnionUnpackMngSvrRequestPackage!\n");
			ret = len;
			goto errorExit;
		}
		// 判断是不是数据开始传送包
		if (mngSvrResCmd == conResCmdStartSendingData) 
		{
			if (recvFileName != NULL)	// 已经收到过一个数据传输开始包
			{
				UnionUserErrLog("in UnionMngSvrRecvFileFromClient:: another data start sending pack received again!\n");
				ret = errCodeEsscMDL_AnotherStartingSendDataCmd;
				goto errorExit;
			}
			if ((recvFileName = UnionGenerateMngSvrTempFile()) == NULL)	// 不能获得一个暂时存储文件名
			{
				UnionUserErrLog("in UnionMngSvrRecvFileFromClient:: UnionGenerateMngSvrTempFile!\n");
				ret = errCodeEsscMDL_FailToGetTempFile;
				goto errorExit;
			}
			if ((fp = fopen(recvFileName,"wb")) == NULL)	// 打不开暂时文件
			{
				UnionSystemErrLog("in UnionMngSvrRecvFileFromClient:: fopen [%s]!\n",recvFileName);
				ret = errCodeUseOSErrCode;
				goto errorExit;
			}
			continue;
		}
		// 判断是不是数据结束传送包
		if (mngSvrResCmd == conResCmdFinishSendingData) 
		{
			if (recvFileName == NULL)	// 没有收到过数据传输开始包
			{
				UnionUserErrLog("in UnionMngSvrRecvFileFromClient:: no start sending pack received ever!\n");
				ret = errCodeEsscMDL_NoStartingSendDataCmdBefore;
				goto errorExit;
			}
			ret = 0;
			break;
		}	
		// 判断是不是数据包
		if (mngSvrResCmd == conResCmdSendingData)
		{
			if ((recvFileName == NULL) || (fp == NULL))	// 没有收到过数据传输开始包
			{
				UnionUserErrLog("in UnionMngSvrRecvFileFromClient:: no start sending pack received ever!\n");
				ret = errCodeEsscMDL_NoStartingSendDataCmdBefore;
				goto errorExit;
			}
			fwrite(tmpBuf,len,1,fp);
			continue;
		}
		UnionUserErrLog("in UnionMngSvrRecvFileFromClient:: wrong request package received! [%03d]\n",mngSvrResCmd);
		ret = errCodeEsscMDL_InvalidOperation;
		goto errorExit;
	}
	fclose(fp);
	fp = NULL;
UnionAuditNullLog("in UnionMngSvrRecvFileFromClient:: overwrittenFileName=[%s],recvFileName=[%s]\n",overwrittenFileName,recvFileName);
	// 先备份要覆盖的文件
	if ((ret = UnionBackupFileToTmpDir(overwrittenFileName)) < 0)
	{
		UnionUserErrLog("in UnionMngSvrRecvFileFromClient:: UnionBackupFileToTmpDir [%s]\n",overwrittenFileName);
		return(ret);
	}
	//sprintf((char *)tmpBuf,"cp %s %s",overwrittenFileName,getenv("UNIONTEMP"));
	//system((char *)tmpBuf);
	// 再覆盖文件
	if ((ret = UnionRenameFile(recvFileName,overwrittenFileName)) < 0)
	{
		UnionUserErrLog("in UnionMngSvrRecvFileFromClient:: UnionRenameFile [%s] to [%s]\n",recvFileName,overwrittenFileName);
		UnionRestoreFileFromTmpDir(overwrittenFileName);
		return(ret);
	}
	//sprintf((char *)tmpBuf,"cp %s %s",recvFileName,overwrittenFileName);
	//system((char *)tmpBuf);
	//UnionDeleteMngSvrTempFile();
errorExit:
	if (fp != NULL)
		fclose(fp);
	return(ret);	
}

int UnionReceiveDataPackage(int sckHDL,char *resStr,int sizeOfResStr,int *fileRecved)
{
        unsigned char   tmpBuf[8192+1];
        int             len;
        int             ret;
        FILE            *fp = NULL;
        int             mngSvrResCmd;
        int             dataRecvFinished = 0;
        char            *recvFileName;
        int             responseCode;

        if(sckHDL < 0 || fileRecved == NULL)
        {
                UnionUserErrLog("in UnionReceiveDataPackage:: parameter error!\n");
                return(errCodeParameter);
        }
        recvFileName = NULL;
        *fileRecved = 0;
        gunionIsMngSvrReturnedError = 0;
        for (;;)
        {
                // 接收一个数据包
                memset(tmpBuf,0,2);

                if ((ret = UnionReceiveFromSocketUntilLen(sckHDL,tmpBuf,2)) < 0)
                {
                        UnionUserErrLog("in UnionReceiveDataPackage:: UnionReceiveFromSocketUntilLen!\n");
                        goto errorExit;
                }

                if (ret == 0)
                {
                        UnionDebugLog("in UnionReceiveDataPackage:: svr close socket!\n");
                        ret = errCodeEsscMDL_NoResponsePackFromMngSvr;
                        goto errorExit;
                }
                if (((len = tmpBuf[0] * 256 + tmpBuf[1]) < 0) || (len > sizeof(tmpBuf)))
                {
                        UnionUserErrLog("in UnionReceiveDataPackage:: len [%d] too large or too small!\n",len);
                        ret = errCodeEsscMDL_DataLenFromMngSvr;
                        goto errorExit;
                }
                if (len == 0)
                        continue;

                memset(tmpBuf,0,sizeof(tmpBuf));
                if ((ret = UnionReceiveFromSocketUntilLen(sckHDL,tmpBuf,len)) < 0)
                {
                        UnionUserErrLog("in UnionReceiveDataPackage:: UnionReceiveFromSocketUntilLen!\n");
                        goto errorExit;
                }

                if (ret == 0)
                {
                        UnionDebugLog("in UnionReceiveDataPackage svr close socket!\n");
                        ret = errCodeEsscMDL_NoResponsePackFromMngSvr;
                        goto errorExit;
                }

                // 判断是不是响应包
                if (UnionIsMngSvrResponsePackage((char *)tmpBuf,len))  // 是响应包
                {
                        if (recvFileName != NULL)      // 是否有数据传输
                        {
                                if (!dataRecvFinished) // 有数据传，是否传输结束
                                {
                                        UnionUserErrLog("in UnionReceiveDataPackage:: data send not finished yet!\n");
                                        ret = errCodeEsscMDL_NotFinishSendingData;
                                        goto errorExit;
                                }
                        }
                        break;
                }
                // 是请求包，解这个请求包
                if ((len = UnionUnpackMngSvrRequestPackage((char *)tmpBuf,len,NULL,NULL,&mngSvrResCmd,(char *)tmpBuf,sizeof(tmpBuf))) < 0)
                {
                        UnionUserErrLog("in UnionReceiveDataPackage:: UnionUnpackMngSvrRequestPackage!\n");
                        ret = len;
                        goto errorExit;
                }
                // 判断是不是数据开始传送包
                if (mngSvrResCmd == conResCmdStartSendingData)
                {
                        if (recvFileName != NULL)    // 已经收到过一个数据传输开始包
                        {
                                UnionUserErrLog("in UnionReceiveDataPackage:: another data start sending pack received again!\n");
                                ret = errCodeEsscMDL_AnotherStartingSendDataCmd;
                                goto errorExit;
                        }
                        if ((recvFileName = UnionGenerateMngSvrTempFile()) == NULL)     // 不能获得一个暂时存储文件名
                        {
                                UnionUserErrLog("in UnionReceiveDataPackage:: UnionGenerateMngSvrTempFile!\n");
                                ret = errCodeEsscMDL_FailToGetTempFile;
                                goto errorExit;
                        }

                        if ((fp = fopen(recvFileName,"wb")) == NULL)   // 打不开暂时文件
                        {
                                UnionSystemErrLog("in UnionReceiveDataPackage:: fopen [%s]!\n",recvFileName);
                                ret = errCodeUseOSErrCode;
                                goto errorExit;
                        }
                        continue;
                }
                // 判断是不是数据结束传送包
                if (mngSvrResCmd == conResCmdFinishSendingData)
                {
                        if (recvFileName == NULL)    // 没有收到过数据传输开始包
                        {
                                UnionUserErrLog("in UnionReceiveDataPackage:: no start sending pack received ever!\n");
                                ret = errCodeEsscMDL_NoStartingSendDataCmdBefore;
                                goto errorExit;
                        }
                        dataRecvFinished = 1;
                        continue;
                }
                // 判断是不是数据包
                if (mngSvrResCmd == conResCmdSendingData)
                {
                        if ((recvFileName == NULL) || (fp == NULL))     // 没有收到过数据传输开始包
                        {
                                UnionUserErrLog("in UnionReceiveDataPackage:: no start sending pack received ever!\n");
                                ret = errCodeEsscMDL_NoStartingSendDataCmdBefore;
                                goto errorExit;
                        }

                        fwrite(tmpBuf,len,1,fp);
                        continue;
                }
                UnionUserErrLog("in UnionReceiveDataPackage:: wrong request package received! [%03d]\n",mngSvrResCmd);
                ret = errCodeEsscMDL_InvalidOperation;
                goto errorExit;
        }
        if ((ret = UnionUnpackMngSvrResponsePackage((char *)tmpBuf,len,NULL,NULL,NULL,&responseCode,(char *)resStr,sizeOfResStr)) < 0)
        {
                UnionUserErrLog("in UnionReceiveDataPackage:: UnionUnpackMngSvrResponsePackage!\n");
                goto errorExit;
        }
        if (responseCode < 0)           // 服务器返回的值不为0
        {
                ret = responseCode;
                // 2007/11/22增加
                gunionIsMngSvrReturnedError = 1;
                memset(gunionMsgSvrReturnedErrorInfo,0,sizeof(gunionMsgSvrReturnedErrorInfo));
                if (ret >= sizeof(gunionMsgSvrReturnedErrorInfo))
                        memcpy(gunionMsgSvrReturnedErrorInfo,resStr,sizeof(gunionMsgSvrReturnedErrorInfo)-1);
                else
                        memcpy(gunionMsgSvrReturnedErrorInfo,resStr,ret);
                // 2007/11/22增加结束
                goto errorExit;
        }

        if (recvFileName != NULL)
                *fileRecved = 1;
errorExit:
        if (fp != NULL)
                fclose(fp);
        return ret;

}

// 与MngSvr通讯   上传文件
// tellerNo,resID,serviceID,reqStr,lenOfReqStr是输入参数，分别对应操作员标识、资源、命令标识，请求数据，请求数据长度
// fileName,是输入参数,要上传的文件名
// sizeOfResStr，是输入参数，指示了接收数据的缓冲区的大小
// resStr,是输出参数，是响应数据
// recvFileName,是输出参数，如果不等于NULL，表示服务器端发送来一个文件，是文件在本地的存储名称，如果是NULL，表示没有文件发送来
// 返回值是resStr中数据的长度
int UnionCommunicationWithSpecMngSvrWithUploadFile(char *ipAddr,int port,char *tellerNo,int resID,int serviceID, char *reqStr,int lenOfReqStr, char *fileName, char *resStr,int sizeOfResStr, int *fileRecved)
{
        int             ret;
        int             sckHDL = -1;

        char tmpBuf[8192+1];
        int  tmpfile;
        int len;
        int responseCode;


        if ((reqStr == NULL) || (resStr == NULL) || (lenOfReqStr < 0) || (sizeOfResStr < 0) || (tellerNo == NULL) || (tellerNo == NULL))
        {
                UnionUserErrLog("in UnionCommunicationWithSpecMngSvrWithUploadFile parameter error!\n");
                return(errCodeParameter);
        }

        gunionIsMngSvrReturnedError = 0;        // 2007/11/22增加


        if ((sckHDL = UnionCreateSocketClient(ipAddr,port)) < 0)
        {
                UnionUserErrLog("in UnionCommunicationWithSpecMngSvrWithUploadFile:: UnionCreateSocketClient! [%s] [%d]\n",ipAddr,port);
                return(sckHDL);
        }

	// 打请求包
	memset(tmpBuf, 0, sizeof tmpBuf);
        ret = UnionPackMngSvrRequestPackage(reqStr,lenOfReqStr,tellerNo,resID,serviceID,(char *)tmpBuf+2,sizeof(tmpBuf)-2);
	if (ret < 0)
	{
		UnionUserErrLog("in UnionSendRequestPackage:: UnionPackMngSvrRequestPackage!\n");
                goto errorExit;
	}
        tmpBuf[0] = ret / 256;
	tmpBuf[1] = ret % 256;
	if ((ret = UnionSendToSocket(sckHDL,tmpBuf,2+ret)) < 0)
	{
		UnionUserErrLog("in UnionSendRequestPackage:: UnionSendToSocket!\n");
                goto errorExit;
	}

        //接收一个响应包
	memset(tmpBuf, 0, sizeof tmpBuf);
        if( (ret = UnionReceiveDataPackage(sckHDL,tmpBuf,sizeof(tmpBuf),&tmpfile)) < 0)
        {
                UnionUserErrLog("in UnionCommunicationWithSpecMngSvrWithUploadFile:: UnionReceiveDataPackage not response packge!");
                goto errorExit;
        }

        //发送文件
        if ((ret = UnionTransferMngDataFile(sckHDL,fileName,tellerNo,resID)) < 0)
        {
                UnionUserErrLog("in UnionCommunicationWithSpecMngSvrWithUploadFile:: UnionTransferMngDataFile!\n");
                goto errorExit;
        }
#ifdef __UserProgressBar__
         if( gunionShowProgressBar )
                UnionShowMainFormProgressBar();
#endif
        // 接收数据
        if( (ret = UnionReceiveDataPackage(sckHDL,resStr,sizeOfResStr,fileRecved)) < 0)
        {
                UnionUserErrLog("in UnionCommunicationWithSpecMngSvrWithUploadFile::UnionReceiveDataPackage error!\n");
        }
#ifdef __UserProgressBar__
        if( gunionShowProgressBar )
                UnionHideMainFormProgressBar();
#endif
errorExit:
        UnionCloseSocket(sckHDL);
        return(ret);
}
