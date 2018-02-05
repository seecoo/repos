//	Author:		Wolfgang Wang
//	Created Date:	2002/2/1

#include <stdio.h>
#include <sys/msg.h>

#include "unionMsgQueue.h"
#include "UnionEnv.h"
#include "unionErrCode.h"
#include "UnionLog.h"

TUnionMsgQueueGrp		gunionMsgQueueGrp;
PUnionMsgQueueGrp		pgunionMsgQueueGrp = NULL;
int				gunionQueueMDLConnected = 0;

int UnionGetConfFileNameOfMsgQueue(char *fileName)
{
	sprintf(fileName,"%s/msgQueue.conf",getenv("UNIONETC"));
	return(0);
}

int UnionConnectMsgQueueMDLInEnvFile()
{
	int			ret;
	char			fileName[512];
	char			*p;
	int			i,j;
	int			realNum;
	char        		caValue[2048];
	char			caPrimaryKey[512];		
	
	memset(fileName,0,sizeof(fileName));
	UnionGetConfFileNameOfMsgQueue(fileName);

	pgunionMsgQueueGrp 	= &gunionMsgQueueGrp;
	pgunionMsgQueueGrp->num = 0;
	
	UnionProgramerLog("in UnionConnectMsgQueueMDLInEnvFile().\n");
	for (realNum = 0;(pgunionMsgQueueGrp->num < conMaxNumOfMsgQueue); realNum++)
	{
		memset(caPrimaryKey, 0, sizeof(caPrimaryKey));
		sprintf(caPrimaryKey, "queueID%02d", realNum);
		
		// 读取 userID
		j		= 1;
		memset(caValue, 0, sizeof(caValue));
		ret             = UnionReadEnviVarValueOfName(fileName, caPrimaryKey, j, caValue);
		
		if (ret >= 0)
			pgunionMsgQueueGrp->queue[pgunionMsgQueueGrp->num].userID	= atoi(caValue);
		else		// 记录已读完
			break;		
		
		UnionProgramerLog("in UnionConnectMsgQueueMDLInEnvFile:: caPrimaryKey: [%s], caValue: [%d].\n", caPrimaryKey, pgunionMsgQueueGrp->queue[pgunionMsgQueueGrp->num].userID);
		
		// 获取 systemID
		if ((pgunionMsgQueueGrp->queue[pgunionMsgQueueGrp->num].systemID 
		    	= msgget(pgunionMsgQueueGrp->queue[pgunionMsgQueueGrp->num].userID,0666)) < 0)
		{
			UnionSystemErrLog("in UnionConnectMsgQueueMDLInEnvFile:: msgget [%d]\n",pgunionMsgQueueGrp->queue[pgunionMsgQueueGrp->num].userID);
			continue;
		}
		pgunionMsgQueueGrp->num++;
	}
	
	if ((pgunionMsgQueueGrp->num) > 0)
		gunionQueueMDLConnected = 1;
	
	return(pgunionMsgQueueGrp->num);
}

int UnionConnectMsgQueueMDL()
{
	int	i;
	char	*start;
	char	*end;
	int	ret;
	char	tmpBuf[1024+1];
	FILE	*fp;
	
	if (gunionQueueMDLConnected)
		return((pgunionMsgQueueGrp->num));
	
	memset(tmpBuf,0,sizeof(tmpBuf));
	UnionGetConfFileNameOfMsgQueue(tmpBuf);
	if ((fp = fopen(tmpBuf,"r")) == NULL)
	{
		UnionSystemErrLog("in UnionConnectMsgQueueMDL:: fopen [%s]\n",tmpBuf);
		return(errCodeUseOSErrCode);
	}
	
	pgunionMsgQueueGrp = &gunionMsgQueueGrp;
	pgunionMsgQueueGrp->num = 0;
	while (!feof(fp))
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadOneFileLine(fp,tmpBuf)) < 0)
			break;
		if (ret == 0)
			continue;
		if (UnionIsUnixShellRemarkLine(tmpBuf))
			continue;
		if ((start = strstr(tmpBuf,"queueID=")) == NULL)
			continue;
		if ((pgunionMsgQueueGrp->queue[pgunionMsgQueueGrp->num].userID = atoi(start+8)) <= 0)
			continue;
		if ((pgunionMsgQueueGrp->queue[pgunionMsgQueueGrp->num].systemID 
		    	= msgget(pgunionMsgQueueGrp->queue[pgunionMsgQueueGrp->num].userID,0666)) < 0)
		{
			UnionSystemErrLog("in UnionConnectMsgQueueMDL:: msgget [%d]\n",pgunionMsgQueueGrp->queue[pgunionMsgQueueGrp->num].userID);
			continue;
		}
		if (++(pgunionMsgQueueGrp->num) >= conMaxNumOfMsgQueue)
			break;
	}
	fclose(fp);
	if ((pgunionMsgQueueGrp->num) > 0)
		gunionQueueMDLConnected = 1;
	if (pgunionMsgQueueGrp->num > 0)
		return(pgunionMsgQueueGrp->num);
	else
		return(UnionConnectMsgQueueMDLInEnvFile());
}

int UnionDisconnectMsgQueueMDL()
{
	if (pgunionMsgQueueGrp != NULL);
		pgunionMsgQueueGrp->num = 0;
	gunionQueueMDLConnected = 0;
	return(0);
}

int UnionQueryAllMsgQueueStatus()
{
	struct msqid_ds 	pmsgq;
	int			index;
	int			ret;
	PUnionMsgQueueRec	prec;
	
	if ((ret = UnionConnectMsgQueueMDL()) < 0)
	{
		UnionUserErrLog("in UnionQueryAllMsgQueueStatus:: UnionConnectMsgQueueMDL!\n");
		return(ret);
	}
	
	for (index = 0; index < pgunionMsgQueueGrp->num; index++)
	{
		prec = &(pgunionMsgQueueGrp->queue[index]);
		if (msgctl(prec->systemID,IPC_STAT,&pmsgq) != 0)
		{
			UnionSystemErrLog("in UnionQueryAllMsgQueueStatus::Cannot Control Message Queue %d",prec->systemID);
			continue;
		}
		prec->msgNum = pmsgq.msg_qnum;
		prec->charNum = pmsgq.msg_cbytes;
	}
	return(0);
}

// 2008/11/25,王纯军增加
PUnionMsgQueueGrp UnionGetAllMsgQueueStatus()
{
	int	ret;
	
	if ((ret = UnionQueryAllMsgQueueStatus()) < 0)
	{
		return(NULL);
	}
	return(pgunionMsgQueueGrp);
}
