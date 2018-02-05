//	Author:		Wolfgang Wang
//	Copyright:	Union Tech. Guangzhou
//	Date:		2007/6/9

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

#include "keyDBBackuper.h"
#include "unionErrCode.h"
#include "keyDBSvr.h"
#include "unionREC.h"
#include "UnionLog.h"

char *UnionGetIPAddrOfKDBSvr()
{
	return(UnionReadStringTypeRECVar("ipAddrOfDBSvr"));
}

int UnionGetPortOfKDBSvr()
{
	return(UnionReadIntTypeRECVar("portOfDBSvr"));
}

int UnionGetTimeoutOfKDBSvr()
{
	return(UnionReadIntTypeRECVar("timeoutOfKDBSvr"));
}

int UnionApplyKDBService(TUnionKeyDBOperation operation,char keyDBType,unsigned char *keyRec,int sizeOfKeyRec)
{
	int				ret;
	int				sck = -1;
	unsigned char			tmpBuf[100];
	int				len;
	TUnionKeyDBBackuperRecordHeader	header;
	time_t				start,finish;
	int				timeout;
	
	if ((keyRec == NULL) || (sizeOfKeyRec <= 0))
		return(0);

	if ((timeout = UnionGetTimeoutOfKDBSvr()) <= 0)
		timeout = 2;

	time(&start);
loop:
	if ((sck = UnionCreateSocketClient(UnionGetIPAddrOfKDBSvr(),UnionGetPortOfKDBSvr())) < 0)
	{
		time(&finish);
		if (finish - start >= timeout)
		{
			UnionUserErrLog("in UnionApplyKDBService:: UnionCreateSocketClient!\n");
			return(sck);
		}
		goto loop;
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
		UnionUserErrLog("in UnionApplyKDBService:: first UnionSendToSocket!\n");
		goto synchronizeFailure;
	}
	if ((ret = UnionSendToSocket(sck,keyRec,sizeOfKeyRec)) < 0)
	{
		UnionUserErrLog("in UnionApplyKDBService:: first UnionSendToSocket!\n");
		goto synchronizeFailure;
	}
	if ((ret = UnionReceiveFromSocketUntilLen(sck,tmpBuf,2)) != 2)
	{
		UnionUserErrLog("in UnionApplyKDBService::UnionReceiveFromSocketUntilLen Error!\n");
		goto synchronizeFailure;
	}
	if ((len = tmpBuf[0] * 256 + tmpBuf[1]) >= sizeof(tmpBuf))
	{
		UnionUserErrLog("in UnionApplyKDBService::answer len [%d] too long Error!\n",len);
		goto synchronizeFailure;
	}
	if ((ret = UnionReceiveFromSocketUntilLen(sck,tmpBuf,len)) != len)
	{
		UnionUserErrLog("in UnionApplyKDBService::UnionReceiveFromSocketUntilLen Error!\n");
		goto synchronizeFailure;
	}
	tmpBuf[len] = 0;
	if ((ret = atoi(tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionApplyKDBService::answer from kdb server = [%d]!\n",ret);
		goto synchronizeFailure;
	}
	UnionCloseSocket(sck);
	return(ret);

synchronizeFailure:
	UnionCloseSocket(sck);
	if (ret >= 0)
		return(errCodeUserForgetSetErrCode);
	else
		return(ret);
}

int UnionReadKDBServiceRequest(int handle,TUnionKeyDBOperation *operation,char *keyDBType,unsigned char *keyRec,int sizeOfKeyRecBuf)
{
	int			ret;
	int			sck;
	unsigned char		tmpBuf[100];
	int			len;
	TUnionKeyDBBackuperRecordHeader	header;
	int			lenOfReq;
		
	if ((ret = UnionReceiveFromSocketUntilLen(handle,tmpBuf,2)) != 2)
	{
		UnionUserErrLog("in UnionReadKDBServiceRequest::UnionReceiveFromSocketUntilLen Error!\n");
		return(ret);
	}
	lenOfReq = tmpBuf[0] * 256 + tmpBuf[1];
	if (lenOfReq > sizeof(header) + sizeOfKeyRecBuf)
	{
		UnionUserErrLog("in UnionReadKDBServiceRequest::sizeOfKeyRecBuf [%d] < expected [%d]\n",
				sizeOfKeyRecBuf,lenOfReq-sizeof(header));
		return(errCodeParameter);
	}
	memset(&header,0,sizeof(header));
	if ((ret = UnionReceiveFromSocketUntilLen(handle,&header,sizeof(header))) != sizeof(header))
	{
		if (ret != 0)
			UnionUserErrLog("in UnionReadKDBServiceRequest:: UnionReceiveFromSocketUntilLen expected [%d] real = [%d]!\n",sizeof(header),ret);
		else
			UnionLog("in UnionReadKDBServiceRequest:: Connection Closed by Client!\n");
		return(ret);
	}
	if ((ret = UnionReceiveFromSocketUntilLen(handle,keyRec,lenOfReq-sizeof(header))) !=  lenOfReq-sizeof(header))
	{
		if (ret != 0)
			UnionUserErrLog("in UnionReadKDBServiceRequest:: UnionReceiveFromSocketUntilLen expected [%d] real = [%d]!\n",lenOfReq-sizeof(header),ret);
		else
			UnionLog("in UnionReadKDBServiceRequest:: Connection Closed by Client!\n");
		return(ret);
	}
	*operation = header.operation;
	*keyDBType = header.keyDBType;
	if (lenOfReq - sizeof(header) < header.lenOfKeyRec)
	{
		UnionUserErrLog("in UnionReadKDBServiceRequest:: lenOfKeyRec [%d] < defined [%d]\n",
			lenOfReq-sizeof(header),header.lenOfKeyRec);
		return(errCodeParameter);
	}
	else
		return(header.lenOfKeyRec);

}

int UnionAnswerKDBService(int handle,int errCode)
{
	unsigned char	tmpBuf[100];
	int	ret;
	int	len;
	
	sprintf(tmpBuf+2,"%d",errCode);
	len = strlen(tmpBuf+2);
	tmpBuf[0] = len / 256;
	tmpBuf[1] = len % 256;
	if ((ret = UnionSendToSocket(handle,tmpBuf,len + 2)) < 0)
	{
		UnionUserErrLog("in UnionAnswerKDBService:: UnionSendToSocket!\n");
		return(ret);
	}
	return(ret);
}
