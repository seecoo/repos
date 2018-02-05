// Wolfgang Wang
// 2010-12-20

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <setjmp.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <time.h>

#include "unionMDLID.h"
#include "unionCommBetweenMDL.h"
#include "unionREC.h"
#include "unionMsgBuf6.x.h"
#include "UnionStr.h"
#include "UnionLog.h"
#include "unionErrCode.h"

int UnionInitServiceBuffer(PUnionServiceBuffer pbuffer)
{
	int			index;
	int			ret;
	PUnionServiceChanel	pchanel;
	char			fileName[256+1];
	FILE			*fp;
	char			recStr[1024+1];
	int			lenOfRecStr;
	
	if (pbuffer == NULL)
	{
		UnionUserErrLog("in UnionInitServiceBuffer:: pbuffer is NULL!\n");
		return(UnionSetUserDefinedErrorCode(errCodeNullPointer));
	}
	sprintf(fileName,"%s/unionServiceBuffer.Def",getenv("UNIONETC"));
	if ((fp = fopen(fileName,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionInitServiceBuffer:: fopen [%s]!\n",fileName);
		return(UnionSetUserDefinedErrorCode(errCodeUseOSErrCode));
	}
	memset(pbuffer,0,sizeof(pbuffer));
	while (!feof(fp))
	{
		memset(recStr,0,sizeof(recStr));
		if ((lenOfRecStr = UnionReadOneDataLineFromTxtFile(fp,recStr,sizeof(recStr))) < 0)
		{
			UnionUserErrLog("in UnionInitServiceBuffer:: UnionReadOneDataLineFromTxtFile!\n");
			continue;
		}
		if (lenOfRecStr == 0)
			continue;
		if (UnionIsUnixShellRemarkLine(recStr))
			continue;
		pchanel = &(pbuffer->chanelGrp[pbuffer->chanelNum]);
		if ((ret = UnionReadLongTypeRecFldFromRecStr(recStr,lenOfRecStr,"serviceID",&(pchanel->serviceID))) < 0)
			continue;
#ifdef _useSerivceChanelToken_
		if ((ret = UnionReadIntTypeRecFldFromRecStr(recStr,lenOfRecStr,"tokenQueueID",&(pchanel->userTokenQueueID))) < 0)
			continue;
		if ((pchanel->sysTokenQueueID = msgget(pchanel->userTokenQueueID,0666|IPC_CREAT)) == -1)
		{
			UnionSystemErrLog("in UnionInitServiceBuffer:: msgget [%d]!\n",pchanel->userTokenQueueID);
			continue;
		}
#endif
		if ((ret = UnionReadIntTypeRecFldFromRecStr(recStr,lenOfRecStr,"requestQueueID",&(pchanel->userRequestQueueID))) < 0)
			continue;
		if ((pchanel->sysRequestQueueID = msgget(pchanel->userRequestQueueID,0666|IPC_CREAT)) == -1)
		{
			UnionSystemErrLog("in UnionInitServiceBuffer:: msgget [%d]!\n",pchanel->userRequestQueueID);
			continue;
		}
		if ((ret = UnionReadIntTypeRecFldFromRecStr(recStr,lenOfRecStr,"responseQueueID",&(pchanel->userResponseQueueID))) < 0)
			continue;
		if ((pchanel->sysResponseQueueID = msgget(pchanel->userResponseQueueID,0666|IPC_CREAT)) == -1)
		{
			UnionSystemErrLog("in UnionInitServiceBuffer:: msgget [%d]!\n",pchanel->userResponseQueueID);
			continue;
		}
		pbuffer->chanelNum += 1;
		if (pbuffer->chanelNum >= conMaxNumOfServiceChanel)
			break;
	}
	fclose(fp);
	return(pbuffer->chanelNum);
}

