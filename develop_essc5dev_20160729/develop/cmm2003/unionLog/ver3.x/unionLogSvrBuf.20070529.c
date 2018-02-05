//	Author:		Wolfgang Wang
//	Date:		2003/11/04
//	Version:	5.0

//	5.1 2005/05/30，在5.0基础上升级
//	修改了	UnionApplyNewMsgIndexOfLogSvrBuf

#ifndef _UnionLogMDL_3_x_
#define _UnionLogMDL_3_x_
#endif
#define _UnionEnv_3_x_

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <signal.h>

#include "UnionLog.h"
#include "unionModule.h"
#include "UnionEnv.h"
#include "unionCommand.h"
#include "unionIndexTBL.h"
#include "unionLogSvrBuf.h"
#include "unionErrCode.h"
#include "unionREC.h"

PUnionLogSvrBufHDL		pgunionLogSvrBufHDL = NULL;
unsigned char			*pgunionLogSvrBuf = NULL;
PUnionIndexStatusTBL		pgunionLogSvrInfoStatusTBL = NULL;
PUnionSharedMemoryModule	pgunionLogSvrBufMDL = NULL;

int UnionReconnectLogSvrBufMDLAnyway()
{
	PUnionSharedMemoryModule	pmdl;
	int				ret;
	int				maxNumOfMsg;
	int				maxSizeOfMsg;
	TUnionLogSvrBufHDL		def;
		
	if (UnionIsLogSvrBufMDLConnected())
		UnionDisconnectLogSvrBufMDL();

	//UnionAuditLog("in UnionReconnectLogSvrBufMDLAnyway:: entering ...\n");
	if ((ret = UnionInitLogSvrBufDef(&def)) < 0)
	{
		UnionUserErrLog("in UnionReconnectLogSvrBufMDLAnyway:: UnionInitLogSvrBufDef!\n");
		return(ret);
	}
	
	if ((pgunionLogSvrBufMDL = UnionConnectSharedMemoryModule(conMDLNameOfLogSvrBuf,
			sizeof(*pgunionLogSvrBufHDL) + 
			(sizeof(TUnionLogSvrInfoHeader) + sizeof(unsigned char) * def.maxSizeOfMsg) * def.maxNumOfMsg)) == NULL)
	{
		UnionUserErrLog("in UnionReconnectLogSvrBufMDLAnyway:: UnionConnectSharedMemoryModule!\n");
		return(errCodeSharedMemoryModule);
	}

	if ((pgunionLogSvrBufHDL = (PUnionLogSvrBufHDL)UnionGetAddrOfSharedMemoryModuleUserSpace(pgunionLogSvrBufMDL)) == NULL)
	{
		UnionUserErrLog("in UnionReconnectLogSvrBufMDLAnyway:: PUnionLogSvrBufHDL!\n");
		return(errCodeSharedMemoryModule);
	}
	
	if ((pgunionLogSvrBuf = (unsigned char *)((unsigned char *)pgunionLogSvrBufHDL + sizeof(*pgunionLogSvrBufHDL))) == NULL)
	{
		UnionUserErrLog("in UnionReconnectLogSvrBufMDLAnyway:: unsigned char *!\n");
		return(errCodeSharedMemoryModule);
	}
	pgunionLogSvrBufHDL->pmsgBuf = pgunionLogSvrBuf;
	
	if (UnionIsNewCreatedSharedMemoryModule(pgunionLogSvrBufMDL))
	{
		pgunionLogSvrBufHDL->maxSizeOfMsg = def.maxSizeOfMsg;
		pgunionLogSvrBufHDL->maxNumOfMsg = def.maxNumOfMsg;
	}

	if ((pgunionLogSvrInfoStatusTBL = UnionConnectIndexStatusTBL(UnionGetUserIDOfSharedMemoryModule(pgunionLogSvrBufMDL),
		pgunionLogSvrBufHDL->maxNumOfMsg)) == NULL)
	{
		UnionUserErrLog("in UnionReconnectLogSvrBufMDLAnyway:: UnionConnectIndexStatusTBL!\n");
		return(errCodeMsgBufMDL_ConnectIndexTBL);
	}
	
	if (UnionIsNewCreatedSharedMemoryModule(pgunionLogSvrBufMDL))
	{
		UnionResetAllLogSvrInfoPos();
	}
	
	//UnionAuditLog("in UnionReconnectLogSvrBufMDLAnyway:: exit ...\n");
	return(0);
}

int UnionIsLogSvrBufMDLConnected()
{
	if ((pgunionLogSvrBufHDL == NULL) || (pgunionLogSvrBuf == NULL) || (pgunionLogSvrInfoStatusTBL == NULL) || (pgunionLogSvrBufMDL== NULL))
		return(0);
	else
		return(1);
}

int UnionGetFileNameOfLogSvrBufDef(char *fileName)
{
	sprintf(fileName,"%s/unionLogSvrBuf.Def",getenv("UNIONETC"));
	return(0);
}

int UnionGetTotalNumOfLogSvrBufMDL()
{
	int	ret;
	
	if ((ret = UnionConnectLogSvrBufMDL()) < 0)
		return(ret);
	return(pgunionLogSvrBufHDL->maxNumOfMsg);
}

int UnionInitLogSvrBufDef(PUnionLogSvrBufHDL pdef)
{
	int		ret;
	char		fileName[512];
	char		*p;

	if (pdef == NULL)
		return(errCodeParameter);
			
	memset(fileName,0,sizeof(fileName));
	UnionGetFileNameOfLogSvrBufDef(fileName);
	if ((ret = UnionInitEnvi(fileName)) < 0)
	{
		UnionUserErrLog("in UnionInitLogSvrBufDef:: UnionInitEnvi [%s]!\n",fileName);
		return(ret);
	}

	if ((p = UnionGetEnviVarByName("maxNumOfMsg")) == NULL)
	{
		UnionUserErrLog("in UnionInitLogSvrBufDef:: UnionGetEnviVarByName for [%s]\n!","maxNumOfMsg");
		goto abnormalExit;
	}
	pdef->maxNumOfMsg = atoi(p);
	
	if ((p = UnionGetEnviVarByName("maxSizeOfMsg")) == NULL)
	{
		UnionUserErrLog("in UnionInitLogSvrBufDef:: UnionGetEnviVarByName for [%s]\n!","maxSizeOfMsg");
		goto abnormalExit;
	}
	pdef->maxSizeOfMsg = atol(p);
	
	UnionClearEnvi();
	return(0);

abnormalExit:	
	UnionClearEnvi();
	return(errCodeMsgBufMDL_InitDef);
}


int UnionConnectLogSvrBufMDL()
{
	int	ret;
	
	if (UnionIsLogSvrBufMDLConnected())
		return(0);

	if ((ret = UnionReconnectLogSvrBufMDLAnyway()) < 0)
	{
		UnionUserErrLog("in UnionConnectLogSvrBufMDL:: UnionReconnectLogSvrBufMDLAnyway!\n");
		return(ret);
	}
	return(ret);
}	
	
int UnionDisconnectLogSvrBufMDL()
{
	pgunionLogSvrBuf = NULL;
	pgunionLogSvrBufHDL = NULL;
	UnionDisconnectIndexStatusTBL(pgunionLogSvrInfoStatusTBL);
	return(UnionDisconnectShareModule(pgunionLogSvrBufMDL));
}

int UnionRemoveLogSvrBufMDL()
{
	pgunionLogSvrBuf = NULL;
	pgunionLogSvrBufHDL = NULL;
	if (pgunionLogSvrBufMDL != NULL)
	{
		UnionRemoveIndexStatusTBL(UnionGetUserIDOfSharedMemoryModule(pgunionLogSvrBufMDL));
	}
	pgunionLogSvrBufMDL = NULL;
	return(UnionRemoveSharedMemoryModule(conMDLNameOfLogSvrBuf));
}

int UnionReloadLogSvrBufDef()
{
	int				ret;
	TUnionLogSvrBufHDL		def;
	
	//UnionAuditLog("in UnionReloadLogSvrBufDef:: UnionConnectLogSvrBufMDL entering... !\n");
	if ((ret = UnionConnectLogSvrBufMDL()) < 0)
	{
		UnionUserErrLog("in UnionReloadLogSvrBufDef:: UnionConnectLogSvrBufMDL!\n");
		return(ret);
	}
	//UnionAuditLog("in UnionReloadLogSvrBufDef:: UnionConnectLogSvrBufMDL exit... !\n");
	return(UnionResetAllLogSvrInfoPos());
}

int UnionBufferLogSvrInfo(char *msg,int lenOfMsg)
{
	int			index;
	int			ret;
	PUnionLogSvrInfoHeader	pmsgHeader;
	PUnionLogFile		plogFile;
	
	if ((ret = UnionConnectLogSvrBufMDL()) < 0)
	{
		UnionUserErrLog("in UnionBufferLogSvrInfo:: UnionConnectLogSvrBufMDL!\n");
		return(ret);
	}	
	if ((msg == NULL) || (lenOfMsg <= 0))
	{
		UnionUserErrLog("in UnionBufferLogSvrInfo:: wrong parameter!\n");
		return(errCodeParameter);
	}
	if ((plogFile = UnionGetLogFile()) == NULL)
	{
		UnionUserErrLog("in UnionBufferLogSvrInfo:: plogFile is NULL!\n");
		return(errCodeLogMDL_LogFileNotExists);
	}
	
	if (((index = UnionGetAvailableIndexNoWait(pgunionLogSvrInfoStatusTBL)) < 0) || (index >= pgunionLogSvrBufHDL->maxNumOfMsg))
	{
		//UnionUserErrLog("in UnionBufferLogSvrInfo:: UnionGetAvailableIndexNoWait!\n");
		return(index);
	}
	pmsgHeader = (PUnionLogSvrInfoHeader)(pgunionLogSvrBuf + conSizeOfEachLogSvrInfo * index);
	if (lenOfMsg >= pgunionLogSvrBufHDL->maxSizeOfMsg)
		pmsgHeader->len = pgunionLogSvrBufHDL->maxSizeOfMsg-1;
	else
		pmsgHeader->len = lenOfMsg;
	memset(pmsgHeader->name,0,sizeof(pmsgHeader->name));
	if ((ret = UnionGetNameOfMyLogFile(plogFile,pmsgHeader->name)) < 0)
	{
		UnionUserErrLog("in UnionBufferLogSvrInfo:: UnionGetNameOfMyLogFile!\n");
		return(ret);
	}
	memcpy(pgunionLogSvrBuf + conSizeOfEachLogSvrInfo * index + sizeof(TUnionLogSvrInfoHeader),msg,pmsgHeader->len);
	return(UnionSetIndexWithUserStatusNoWait(pgunionLogSvrInfoStatusTBL,index,conLogSvrInfoFlag));
}

int UnionReadFirstLogSvrInfoUntilSuccess(unsigned char *msg,int sizeOfMsg,char *logFileName)
{
	int			index;
	int			len;
	PUnionLogSvrInfoHeader 	pmsgHeader;
	int			ret;
		
	if ((ret = UnionConnectLogSvrBufMDL()) < 0)
	{
		UnionUserErrLog("in UnionReadFirstLogSvrInfoUntilSuccess:: UnionConnectLogSvrBufMDL!\n");
		return(ret);
	}	
	if ((msg == NULL) || (sizeOfMsg <= 0))
	{
		UnionUserErrLog("in UnionReadFirstLogSvrInfoUntilSuccess:: wrong parameter!\n");
		return(errCodeParameter);
	}
loopRead:
	if (((index = UnionGetFirstIndexOfUserStatusUntilSuccess(pgunionLogSvrInfoStatusTBL,conLogSvrInfoFlag)) < 0)
		 || (index >= pgunionLogSvrBufHDL->maxNumOfMsg))
	{
		UnionUserErrLog("in UnionReadFirstLogSvrInfoUntilSuccess:: UnionGetFirstIndexOfUserStatus!\n");
		return(index);
	}
	pmsgHeader = (PUnionLogSvrInfoHeader)(pgunionLogSvrBuf + conSizeOfEachLogSvrInfo * index);
	if ((len = pmsgHeader->len) <= 0)
	{
		UnionSetIndexAvailable(pgunionLogSvrInfoStatusTBL,index);
		goto loopRead;
	}
	if (len >= sizeOfMsg)
	{
		len = sizeOfMsg-4;
		memcpy(msg,pgunionLogSvrBuf + conSizeOfEachLogSvrInfo * index + sizeof(TUnionLogSvrInfoHeader),len);
		memset(msg+len,'.',3);
		len = sizeOfMsg - 1;
	}
	else
		memcpy(msg,pgunionLogSvrBuf + conSizeOfEachLogSvrInfo * index + sizeof(TUnionLogSvrInfoHeader),len);
	pmsgHeader->len = -1;
	strcpy(logFileName,pmsgHeader->name);
	UnionSetIndexAvailable(pgunionLogSvrInfoStatusTBL,index);

	return(len);
}


int UnionPrintAvailablLogSvrBufPosToFile(FILE *fp)
{
	int			index;
	PUnionLogSvrInfoHeader	pmsgHeader;
	int			num;
	int			ret;
	
	if (fp == NULL)
		fp = stdout;

	if ((ret = UnionConnectLogSvrBufMDL()) < 0)
	{
		UnionUserErrLog("in UnionPrintAvailablLogSvrBufPosToFile:: UnionConnectLogSvrBufMDL!\n");
		return(ret);
	}	
	for (index = 0,num = 0; index < pgunionLogSvrBufHDL->maxNumOfMsg; index++)
	{
		if ((pmsgHeader = (PUnionLogSvrInfoHeader)(pgunionLogSvrBuf + conSizeOfEachLogSvrInfo * index)) == NULL)
			return(errCodeMsgBufMDL_OutofRange);
		if (pmsgHeader->len > 0)
			continue;
		fprintf(fp,"%06d\n",index);
		num++;
		if ((num % 23 == 0) && (num != 0) && ((fp == stdout) || (fp == stderr)))
		{
			if (UnionIsQuit(UnionPressAnyKey("Press any key to continue or exit/quit to exit ...")))
				break;
		}
	}
	fprintf(fp,"Total Available Num = [%d]\n",num);
	return(0);
}

int UnionPrintInavailabeLogSvrBufPosToFile(FILE *fp)
{
	int				index;
	PUnionLogSvrInfoHeader	pmsgHeader;
	int				num;
	int				ret;
	
	if (fp == NULL)
		return(errCodeParameter);

	if ((ret = UnionConnectLogSvrBufMDL()) < 0)
	{
		UnionUserErrLog("in UnionPrintInavailabeLogSvrBufPosToFile:: UnionConnectLogSvrBufMDL!\n");
		return(ret);
	}	
	for (index = 0,num = 0; index < pgunionLogSvrBufHDL->maxNumOfMsg; index++)
	{
		if ((pmsgHeader = (PUnionLogSvrInfoHeader)(pgunionLogSvrBuf + conSizeOfEachLogSvrInfo * index)) == NULL)
			return(errCodeMsgBufMDL_OutofRange);
		if (pmsgHeader->len <= 0)
			continue;
		fprintf(fp,"%06d %04d %s\n",index,pmsgHeader->len,pmsgHeader->name);
		num++;
		if ((num % 22 == 0) && (num != 0) && ((fp == stdout) || (fp == stderr))) 
		{
			if (UnionIsQuit(UnionPressAnyKey("Press any key to continue or exit/quit to exit ...")))
				break;
		}
	}
	fprintf(fp,"Total Inavailable Num = [%d]\n",num);
	return(0);
}

int UnionPrintLogSvrBufStatusToFile(FILE *fp)
{
	int	ret;
	
	if (fp == NULL)
		return(errCodeParameter);

	if ((ret = UnionConnectLogSvrBufMDL()) < 0)
	{
		UnionUserErrLog("in UnionPrintLogSvrBufStatusToFile:: UnionConnectLogSvrBufMDL!\n");
		return(ret);
	}	
	fprintf(fp,"\n");
	fprintf(fp,"maxSizeOfMsg		[%ld]\n",pgunionLogSvrBufHDL->maxSizeOfMsg);
	fprintf(fp,"maxNumOfMsg			[%d]\n",pgunionLogSvrBufHDL->maxNumOfMsg);
	fprintf(fp,"\n");
	if (UnionIsQuit(UnionPressAnyKey("Press any key to continue or quit/exit to exit...")))
		return(0);
	UnionPrintSharedMemoryModuleToFile(pgunionLogSvrBufMDL,fp);
	return(0);
}

int UnionPrintLogSvrBufToFile(FILE *fp)
{
	UnionPrintAvailablLogSvrBufPosToFile(fp);
	UnionPrintInavailabeLogSvrBufPosToFile(fp);
	UnionPrintLogSvrBufStatusToFile(fp);
	return(0);
}

int UnionResetAllLogSvrInfoPos()
{
	int	ret;
	int	index;
	PUnionLogSvrInfoHeader 	pmsgHeader;
	
	if ((ret = UnionConnectLogSvrBufMDL()) < 0)
	{
		UnionUserErrLog("in UnionResetAllLogSvrInfoPos:: UnionConnectLogSvrBufMDL!\n");
		return(ret);
	}
	for (index = 0; index < pgunionLogSvrBufHDL->maxNumOfMsg; index++)
	{
		if ((pmsgHeader = (PUnionLogSvrInfoHeader)(pgunionLogSvrBuf + conSizeOfEachLogSvrInfo * index)) == NULL)
			continue;
		memset(pmsgHeader,0,sizeof(*pmsgHeader));
	}
	return(UnionResetAllIndexAvailable(pgunionLogSvrInfoStatusTBL));
}
