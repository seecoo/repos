//	Author:		Wolfgang Wang
//	Copyright:	Union Tech. Guangzhou
//	Date:		2004/11/20

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>

#include "unionModule.h"
#include "UnionStr.h"
#define _UnionEnv_3_x_
#include "UnionEnv.h"
#include "unionDesKey.h"
#include "unionDesKeyDBLog.h"

#include "keyDBBackuper.h"
#include "unionErrCode.h"

#include "UnionLog.h"

PUnionSharedMemoryModule	pgunionKeyDBBackupServerMDL = NULL;
PUnionKeyDBBackupServer		pgunionKeyDBBackupServer = NULL;

int				gisKeyDBBackupServer = 0;
//int				gsckConnToBrotherBackupServer = -1;

int UnionIsKeyDBBackupServer()
{
	return(gisKeyDBBackupServer);
}

int UnionSetAsKeyDBBackupServer()
{
	gisKeyDBBackupServer = 1;
	return(0);
}

int UnionGetNameOfKeyDBBackupServer(char *fileName)
{
	sprintf(fileName,"%s/unionKeyDBBackupServer.Def",getenv("UNIONETC"));
	return(0);
}

int UnionIsKeyDBBackupServerConnected()
{
	// 2007/10/26修改
	if ((pgunionKeyDBBackupServer == NULL) || (!UnionIsSharedMemoryInited(conMDLNameOfUnionKeyDBBackupServer)))
	//if ((pgunionKeyDBBackupServerMDL == NULL) || (pgunionKeyDBBackupServer == NULL))
		return(0);
	else
		return(1);
}

// Module Layer Functions
int UnionConnectKeyDBBackupServer()
{
	int	ret;
	
	if (UnionIsKeyDBBackupServerConnected())
		return(0);
		
	if ((pgunionKeyDBBackupServerMDL = UnionConnectSharedMemoryModule(conMDLNameOfUnionKeyDBBackupServer,
			sizeof(TUnionKeyDBBackupServer))
			) == NULL)
	{
		UnionUserErrLog("in UnionConnectKeyDBBackupServer:: UnionConnectSharedMemoryModule!\n");
		return(errCodeSharedMemoryModule);
	}
	if ((pgunionKeyDBBackupServer = (PUnionKeyDBBackupServer)UnionGetAddrOfSharedMemoryModuleUserSpace(pgunionKeyDBBackupServerMDL)) == NULL)
	{
		UnionUserErrLog("in UnionConnectKeyDBBackupServer:: PUnionKeyDBBackupServer!\n");
		UnionRemoveKeyDBBackupServerInMemory();
		return(errCodeSharedMemoryModule);
	}
	
	if (UnionIsNewCreatedSharedMemoryModule(pgunionKeyDBBackupServerMDL))
	{
		memset(pgunionKeyDBBackupServer,0,sizeof(*pgunionKeyDBBackupServer));
		if ((ret = UnionLoadKeyDBBackupServerIntoMemory()) < 0)
		{
			UnionUserErrLog("in UnionConnectKeyDBBackupServer:: UnionLoadKeyDBBackupServerIntoMemory!\n");
			UnionRemoveKeyDBBackupServerInMemory();
		}
		return(ret);
	}
	else
		return(0);
}

int UnionDisconnectKeyDBBackupServer()
{
	pgunionKeyDBBackupServer = NULL;
	
	return(UnionDisconnectShareModule(pgunionKeyDBBackupServerMDL));
}

int UnionRemoveKeyDBBackupServerInMemory()
{
	UnionDisconnectKeyDBBackupServer();
	return(UnionRemoveSharedMemoryModule(conMDLNameOfUnionKeyDBBackupServer));
}

int UnionLoadKeyDBBackupServerIntoMemory()
{
	int				ret;
	FILE				*fp;
	char				fileName[256];
	char				*p;
	int				index;
	
	if ((ret = UnionConnectKeyDBBackupServer()) < 0)
	{
		UnionUserErrLog("in UnionLoadKeyDBBackupServerIntoMemory:: UnionConnectKeyDBBackupServer!\n");
		return(ret);
	}
	
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfKeyDBBackupServer(fileName);
        if ((ret = UnionInitEnvi(fileName)) < 0)
        {
                UnionUserErrLog("in UnionLoadKeyDBBackupServerIntoMemory:: UnionInitEnvi [%s]!\n",fileName);
                return(ret);
        }

	if ((p = UnionGetEnviVarByName("active")) == NULL)
	{
		UnionUserErrLog("in UnionLoadKeyDBBackupServerIntoMemory:: UnionGetEnviVarByName for [%s]\n!","active");
		ret = errCodeKeyDBBackuperMDL_ConfError;
		goto exitNormally;
	}
	pgunionKeyDBBackupServer->active = atoi(p);
	
	memset(&(pgunionKeyDBBackupServer->myself),0,sizeof(pgunionKeyDBBackupServer->myself));
	if ((index = UnionGetVarIndexOfTheVarName("myself")) < 0)
	{
		UnionUserErrLog("in UnionLoadKeyDBBackupServerIntoMemory:: UnionGetEnviVarByName for [%s]\n!","myself");
		ret = errCodeKeyDBBackuperMDL_ConfError;
		goto exitNormally;
	}
	if (!UnionIsValidIPAddrStr(p = UnionGetEnviVarOfTheIndexByIndex(index,1)))
	{
		UnionUserErrLog("in UnionLoadKeyDBBackupServerIntoMemory:: get ipAddr for [myself]!\n");
		ret = errCodeKeyDBBackuperMDL_ConfError;
		goto exitNormally;
	}
	strcpy(pgunionKeyDBBackupServer->myself.ipAddr,p);
	if ((p = UnionGetEnviVarOfTheIndexByIndex(index,2)) == NULL)
	{
		UnionUserErrLog("in UnionLoadKeyDBBackupServerIntoMemory:: get port for [myself]!\n");
		ret = errCodeKeyDBBackuperMDL_ConfError;
		goto exitNormally;
	}
	pgunionKeyDBBackupServer->myself.port = atoi(p);
	UnionLog("in UnionLoadKeyDBBackupServerIntoMemory:: port = [%s]\n",p);
			
	memset(&(pgunionKeyDBBackupServer->brother),0,sizeof(pgunionKeyDBBackupServer->brother));
	if ((index = UnionGetVarIndexOfTheVarName("brother")) < 0)
	{
		UnionUserErrLog("in UnionLoadKeyDBBackupServerIntoMemory:: UnionGetEnviVarByName for [%s]\n!","brother");
		ret = errCodeKeyDBBackuperMDL_ConfError;
		goto exitNormally;
	}
	if (!UnionIsValidIPAddrStr(p = UnionGetEnviVarOfTheIndexByIndex(index,1)))
	{
		UnionUserErrLog("in UnionLoadKeyDBBackupServerIntoMemory:: get ipAddr for [brother]!\n");
		ret = errCodeKeyDBBackuperMDL_ConfError;
		goto exitNormally;
	}
	strcpy(pgunionKeyDBBackupServer->brother.ipAddr,p);
	if ((p = UnionGetEnviVarOfTheIndexByIndex(index,2)) == NULL)
	{
		UnionUserErrLog("in UnionLoadKeyDBBackupServerIntoMemory:: get port for [brother]!\n");
		ret = errCodeKeyDBBackuperMDL_ConfError;
		goto exitNormally;
	}
	pgunionKeyDBBackupServer->brother.port = atoi(p);
	UnionLog("in UnionLoadKeyDBBackupServerIntoMemory:: port = [%s]\n",p);
	ret = 0;

exitNormally:
	UnionClearEnvi();
	return(ret);
}			
	
int UnionPrintKeyDBBackupServerToFile(FILE *fp)
{
	int	ret;
	long	i;
	
	if (fp == NULL)
	{
		UnionUserErrLog("in UnionPrintKeyDBBackupServerToFile:: fp is NULL pointer!\n");
		return(errCodeParameter);
	}
	if ((ret = UnionConnectKeyDBBackupServer()) < 0)
	{
		UnionUserErrLog("in UnionPrintKeyDBBackupServerToFile:: UnionConnectKeyDBBackupServer!\n");
		return(ret);
	}
	
	fprintf(fp,"KeyDBBackupServer Configuration::\n");
	if (pgunionKeyDBBackupServer->active)
		fprintf(fp," Active!\n");
	else
		fprintf(fp," Inactive!\n");
	fprintf(fp," myself  %15s %d\n",pgunionKeyDBBackupServer->myself.ipAddr,pgunionKeyDBBackupServer->myself.port);
	fprintf(fp," brother %15s %d\n",pgunionKeyDBBackupServer->brother.ipAddr,pgunionKeyDBBackupServer->brother.port);
	
	return(0);
}

int UnionPrintKeyDBBackupServer()
{
	int	ret;
	
	if ((ret = UnionConnectKeyDBBackupServer()) < 0)
	{
		UnionUserErrLog("in UnionPrintKeyDBBackupServer:: UnionConnectKeyDBBackupServer!\n");
		return(ret);
	}
	UnionPrintKeyDBBackupServerToFile(stdout);
	UnionPrintSharedMemoryModuleToFile(pgunionKeyDBBackupServerMDL,stdout);
	return(0);
}

int UnionPrintStatusOfKeyDBBackupServerToFile(FILE *fp)
{
	int	ret;
	
	if ((ret = UnionConnectKeyDBBackupServer()) < 0)
	{
		UnionUserErrLog("in UnionPrintStatusOfKeyDBBackupServerToFile:: UnionConnectKeyDBBackupServer!\n");
		return(ret);
	}
	if (fp == NULL)
		return(errCodeParameter);
	return(UnionPrintSharedMemoryModuleToFile(pgunionKeyDBBackupServerMDL,fp));
}

int UnionSetKeyDBBackupServerActive()
{
	int	ret;
	char	fileName[512+1];
		
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfKeyDBBackupServer(fileName);
	if ((ret = UnionUpdateEnviVar(fileName,"active","[1]")) < 0)
	{
		UnionUserErrLog("in UnionSetKeyDBBackupServerActive:: UnionUpdateEnviVar!\n");
		return(ret);
	}

	if ((ret = UnionConnectKeyDBBackupServer()) < 0)
	{
		UnionUserErrLog("in UnionSetKeyDBBackupServerActive:: UnionConnectKeyDBBackupServer!\n");
		return(ret);
	}
	pgunionKeyDBBackupServer->active = 1;
	return(0);
}

int UnionSetKeyDBBackupServerInactive()
{
	int	ret;
	char	fileName[512+1];
		
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfKeyDBBackupServer(fileName);
	if ((ret = UnionUpdateEnviVar(fileName,"active","[0]")) < 0)
	{
		UnionUserErrLog("in UnionSetKeyDBBackupServerInactive:: UnionUpdateEnviVar!\n");
		return(ret);
	}

	if ((ret = UnionConnectKeyDBBackupServer()) < 0)
	{
		UnionUserErrLog("in UnionSetKeyDBBackupServerInactive:: UnionConnectKeyDBBackupServer!\n");
		return(ret);
	}
	pgunionKeyDBBackupServer->active = 0;
	return(0);
}

int UnionSynchronizeKeyDBOperation(TUnionKeyDBOperation operation,char keyDBType,unsigned char *keyRec,int sizeOfKeyRec)
{
	int			ret;
	int			sck;
	unsigned char		tmpBuf[100];
	int			len;
	TUnionKeyDBBackuperRecordHeader	header;
	
	if ((gisKeyDBBackupServer) || (keyRec == NULL) || (sizeOfKeyRec <= 0))
		return(0);

	if ((ret = UnionConnectKeyDBBackupServer()) < 0)
	{
		UnionUserErrLog("in UnionSynchronizeKeyDBOperation:: UnionConnectKeyDBBackupServer!\n");
		return(ret);
	}
	if (!pgunionKeyDBBackupServer->active)
		return(0);

	if ((sck = UnionCreateSocketClient(pgunionKeyDBBackupServer->brother.ipAddr,pgunionKeyDBBackupServer->brother.port)) < 0)
	{
		UnionUserErrLog("in UnionSynchronizeKeyDBOperation:: UnionCreateSocketClient!\n");
		ret = sck;
		goto synchronizeFailure;
	}
	memset(tmpBuf,0,sizeof(tmpBuf));
	tmpBuf[0] = (sizeOfKeyRec + sizeof(header))/256;
	tmpBuf[1] = (sizeOfKeyRec + sizeof(header))%256;
	memset(&header,0,sizeof(header));
	header.keyDBType = keyDBType;
	header.operation = operation;
	header.lenOfKeyRec = sizeOfKeyRec;
	memcpy(tmpBuf+2,&header,sizeof(header));
	if ((ret = UnionSendToSocket(sck,tmpBuf,2+sizeof(header))) < 0)
	{
		UnionUserErrLog("in UnionSynchronizeKeyDBOperation:: first UnionSendToSocket!\n");
		UnionCloseSocket(sck);
		goto synchronizeFailure;
	}
	if ((ret = UnionSendToSocket(sck,keyRec,sizeOfKeyRec)) < 0)
	{
		UnionUserErrLog("in UnionSynchronizeKeyDBOperation:: first UnionSendToSocket!\n");
		UnionCloseSocket(sck);
		goto synchronizeFailure;
	}
	
	if ((ret = UnionReceiveFromSocketUntilLen(sck,tmpBuf,2)) != 2)
	{
		UnionUserErrLog("in UnionSynchronizeKeyDBOperation::UnionReceiveFromSocketUntilLen Error!\n");
		UnionCloseSocket(sck);
		goto synchronizeFailure;
	}
	if ((len = tmpBuf[0] * 256 + tmpBuf[1]) > sizeof(tmpBuf))
	{
		UnionUserErrLog("in UnionSynchronizeKeyDBOperation::answer len [%d] too long Error!\n",len);
		UnionCloseSocket(sck);
		goto synchronizeFailure;
	}
	if ((ret = UnionReceiveFromSocketUntilLen(sck,tmpBuf,len)) != len)
	{
		UnionUserErrLog("in UnionSynchronizeKeyDBOperation::UnionReceiveFromSocketUntilLen Error!\n");
		UnionCloseSocket(sck);
		goto synchronizeFailure;
	}
	if (strncmp((char *)tmpBuf,"00",2) != 0)
	{
		tmpBuf[2] = 0;
		UnionUserErrLog("in UnionSynchronizeKeyDBOperation::answer from backup server = [%s] Error!\n",tmpBuf);
		UnionCloseSocket(sck);
		goto synchronizeFailure;
	}
	UnionCloseSocket(sck);
	return(ret);

synchronizeFailure:
	return(ret);
}

int UnionGetPortOfMineOfKeyDBBackupServer()
{
	int	ret;
	if ((ret = UnionConnectKeyDBBackupServer()) < 0)
	{
		UnionUserErrLog("in UnionGetPortOfMineOfKeyDBBackupServer:: UnionConnectKeyDBBackupServer!\n");
		return(ret);
	}
	return(pgunionKeyDBBackupServer->myself.port);
}

int UnionReadSynchronizingKeyDBOperation(int handle,TUnionKeyDBOperation *operation,char *keyDBType,unsigned char *keyRec,int sizeOfKeyRecBuf)
{
	int			ret;
	int			sck;
	unsigned char		tmpBuf[100];
	int			len;
	TUnionKeyDBBackuperRecordHeader	header;
	int			lenOfReq;
		
	if ((ret = UnionReceiveFromSocketUntilLen(handle,tmpBuf,2)) != 2)
	{
		UnionUserErrLog("in UnionReadSynchronizingKeyDBOperation::UnionReceiveFromSocketUntilLen Error!\n");
		return(ret);
	}
	lenOfReq = tmpBuf[0] * 256 + tmpBuf[1];
	if (lenOfReq > sizeof(header) + sizeOfKeyRecBuf)
	{
		UnionUserErrLog("in UnionReadSynchronizingKeyDBOperation::sizeOfKeyRecBuf [%d] < expected [%d]\n",
				sizeOfKeyRecBuf,lenOfReq-sizeof(header));
		return(errCodeParameter);
	}
	memset(&header,0,sizeof(header));
	if ((ret = UnionReceiveFromSocketUntilLen(handle,&header,sizeof(header))) != sizeof(header))
	{
		if (ret != 0)
			UnionUserErrLog("in UnionReadSynchronizingKeyDBOperation:: UnionReceiveFromSocketUntilLen expected [%d] real = [%d]!\n",sizeof(header),ret);
		else
			UnionLog("in UnionReadSynchronizingKeyDBOperation:: Connection Closed by Client!\n");
		return(ret);
	}
	if ((ret = UnionReceiveFromSocketUntilLen(handle,keyRec,lenOfReq-sizeof(header))) !=  lenOfReq-sizeof(header))
	{
		if (ret != 0)
			UnionUserErrLog("in UnionReadSynchronizingKeyDBOperation:: UnionReceiveFromSocketUntilLen expected [%d] real = [%d]!\n",lenOfReq-sizeof(header),ret);
		else
			UnionLog("in UnionReadSynchronizingKeyDBOperation:: Connection Closed by Client!\n");
		return(ret);
	}
	*operation = header.operation;
	*keyDBType = header.keyDBType;
	if (lenOfReq - sizeof(header) < header.lenOfKeyRec)
	{
		UnionUserErrLog("in UnionReadSynchronizingKeyDBOperation:: lenOfKeyRec [%d] < defined [%d]\n",
			lenOfReq-sizeof(header),header.lenOfKeyRec);
		return(errCodeParameter);
	}
	else
		return(header.lenOfKeyRec);

}

int UnionAnswerKeyDBSynchronizingRequest(int handle,char *errCode)
{
	char	tmpBuf[100];
	int	ret;
	
	tmpBuf[0] = 0x00;
	tmpBuf[1] = 0x02;
	memcpy(tmpBuf+2,errCode,2);
	if ((ret = UnionSendToSocket(handle,tmpBuf,4)) != 4)
	{
		UnionUserErrLog("in UnionAnswerKeyDBSynchronizingRequest:: UnionSendToSocket!\n");
		return(ret);
	}
	return(0);
}

int UnionPrintKeyDBBackupServerToSpecFile(char *fileName)
{
	FILE	*fp;
	int	ret;
	
	if ((fileName == NULL) || (strlen(fileName) == 0))
		fp = stdout;
	else
	{
		if ((fp = fopen(fileName,"w")) == NULL)
		{
			UnionSystemErrLog("in UnionPrintKeyDBBackupServerToSpecFile:: fopen [%s]\n",fileName);
			return(errCodeUseOSErrCode);
		}
	}
	ret = UnionPrintKeyDBBackupServerToFile(fp);
	if (fp != stdout)
		fclose(fp);
	return(ret);
}


// 20060825 增加
int UnionUpdateLocalKeyDBBackuperDef(char *ipAddr,int port)
{
	int		ret;
	char		fileName[512+1];
		
	if (ipAddr == NULL)
		return(errCodeParameter);
	
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfKeyDBBackupServer(fileName);
	if ((ret = UnionUpdateEnviVar(fileName,"myself","[%s][%d]",ipAddr,port)) < 0)
	{
		UnionUserErrLog("in UnionUpdateLocalKeyDBBackuperDef:: UnionUpdateEnviVar!\n");
		return(ret);
	}

	// 重新加载定义文件
	if ((ret = UnionLoadKeyDBBackupServerIntoMemory()) < 0)
	{
		UnionUserErrLog("in UnionUpdateLocalKeyDBBackuperDef:: UnionLoadKeyDBBackupServerIntoMemory!\n");
		return(ret);
	}
	
	return(ret);
}

// 20060825 增加
int UnionUpdateRemoteKeyDBBackuperDef(char *ipAddr,int port)
{
	int		ret;
	char		fileName[512+1];
		
	if (ipAddr == NULL)
		return(errCodeParameter);
	
	memset(fileName,0,sizeof(fileName));
	UnionGetNameOfKeyDBBackupServer(fileName);
	if ((ret = UnionUpdateEnviVar(fileName,"brother","[%s][%d]",ipAddr,port)) < 0)
	{
		UnionUserErrLog("in UnionUpdateRemoteKeyDBBackuperDef:: UnionUpdateEnviVar!\n");
		return(ret);
	}

	// 重新加载定义文件
	if ((ret = UnionLoadKeyDBBackupServerIntoMemory()) < 0)
	{
		UnionUserErrLog("in UnionUpdateRemoteKeyDBBackuperDef:: UnionLoadKeyDBBackupServerIntoMemory!\n");
		return(ret);
	}
	
	return(ret);
}

