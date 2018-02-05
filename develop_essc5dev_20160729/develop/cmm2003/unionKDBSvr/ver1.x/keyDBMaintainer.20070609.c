// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2004/11/20
// Version:	1.0

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

#ifndef _UnionSocket_3_x_
#define _UnionSocket_3_x_
#endif
#include "UnionSocket.h"

#include "unionKeyDB.h"
#include "unionDesKeyDB.h"
#include "unionPK.h"
#include "unionPKDB.h"
#include "unionVersion.h"

#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_			// 选用3.x版本的Task模块
#endif
#include "UnionTask.h"

#include "UnionStr.h"
#include "UnionLog.h"

int 			gsynchTCPIPSvrSckHDL;
PUnionTaskInstance	ptaskInstance = NULL;

int UnionTaskActionBeforeExit()
{
	if (gsynchTCPIPSvrSckHDL >= 0)
		UnionCloseSocket(gsynchTCPIPSvrSckHDL);
	//UnionDisconnectDesKeyDB();
	UnionDisconnectPKDB();
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	exit(0);
}

int UnionHelp()
{
	printf("Usage:: %s [pkDB|desDB|both]\n",UnionGetApplicationName());
	return(0);
}

int main(int argc,char *argv[])
{
	int			ret;
	int			interval = 1;
	int			isPKMaintained = 1;
	int			isDesMaintained = 1;
	int			i;
	char			*type = NULL;
	char			tmpBuf[100];
	char			lastUpdateTime[20+1];
	char			*ptr;
	int			locks;
	int			ensureTimeInterval = 10;
	char			lockExistsTime[20+1];
	
	UnionSetApplicationName(argv[0]);

	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(0);

	for (i = 1; i < argc; i++)
	{
		if (strcasecmp(argv[i],"PKDB") == 0)
		{
			isDesMaintained = 0;
			type = argv[i];
		}
		else if (strcasecmp(argv[i],"DESDB") == 0)
		{
			isPKMaintained = 0;
			type = argv[i];
		}
		else if (strcasecmp(argv[i],"BOTH") != 0)
			return(UnionHelp());
	}		
		
	// 判断是否已经启动了一个维护任务
	if (UnionExistsAnotherTaskOfName(UnionGetApplicationName()))
	{
		printf("another task of name [%s] already exists!\n",UnionGetApplicationName());
		return(-1);
	}
	if (type != NULL)
	{
		sprintf(tmpBuf,"%s %s",UnionGetApplicationName(),type);
		if (UnionExistsAnotherTaskOfName(tmpBuf))
		{
			printf("another task of name [%s] already exists!\n",tmpBuf);
			return(-1);
		}
	}
	else
		strcpy(tmpBuf,UnionGetApplicationName());

	// 创建一个后台任务		
	if (UnionCreateProcess() > 0)
		return(0);
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,tmpBuf)) == NULL)
	{
		printf("in %s:: UnionCreateTaskInstance [%s] Error!\n",UnionGetApplicationName(),tmpBuf);
		return(UnionTaskActionBeforeExit());
	}
	
	// 后台任务的处理过程
loop:
	if ((interval = UnionReadIntTypeRECVar("intervalOfDBMaintainer")) <= 0)
		interval = 1;
	if ((ensureTimeInterval = UnionReadIntTypeRECVar("ensureIntervalOfDBMaintainer")) <= 0)
		ensureTimeInterval = 10;
	// 先维护PK库
	// 第一次检查
	if (!isPKMaintained)
		goto maintainDesKeyDB;
	if ((locks = UnionGetWritingLocksOfPKDB()) < 0)	// 判断是否被锁住了
	{
		UnionUserErrLog("in main::UnionGetWritingLocksOfPKDB! ret = [%d]\n",locks);
		goto maintainDesKeyDB;
	}
	if (locks == 0)	// 没有被写锁
		goto maintainDesKeyDB;
	// PK被写锁住了,获取最近一次库更新的时间
	if ((ptr = UnionGetPKDBLastUpdatedTime()) == NULL)
	{
		UnionUserErrLog("in main::UnionGetPKDBLastUpdatedTime!\n");
		goto maintainDesKeyDB;
	}
	memset(lockExistsTime,0,sizeof(lockExistsTime));
	UnionGetFullSystemDateTime(lockExistsTime);
	memset(lastUpdateTime,0,sizeof(lastUpdateTime));
	strcpy(lastUpdateTime,ptr);
	UnionLog("in main:: locks [%02d] checked! PKDB lastUpdateTime = [%s]\n",locks,lastUpdateTime);
	// 等待一段时间再次检查
	sleep(ensureTimeInterval);
	// 第二次检查
	if ((locks = UnionGetWritingLocksOfPKDB()) < 0)	// 判断是否被锁住了
	{
		UnionUserErrLog("in main::UnionGetWritingLocksOfPKDB! ret = [%d]\n",locks);
		goto maintainDesKeyDB;
	}
	if (locks == 0)	// 写锁已解
	{
		UnionLog("in main:: locks have been released\n");
		goto maintainDesKeyDB;
	}
	// 锁仍未解
	if ((ptr = UnionGetPKDBLastUpdatedTime()) == NULL)
	{
		UnionUserErrLog("in main::UnionGetPKDBLastUpdatedTime!\n");
		goto maintainDesKeyDB;
	}
	// 比较两次库更新时间
	if (strncmp(ptr,lastUpdateTime,14) != 0)	// 两次写锁不是同一个锁
	{
		UnionLog("in main:: locks [%02d] checked! PKDB lastUpdateTime changed to [%s]\n",locks,ptr);
		goto maintainDesKeyDB;
	}
	// 两次查到的写锁是同一写锁
	UnionAuditLog("in main:: the writingLocks = [%02d] exists since [%s]! while the lastUpdateTime of PKDB is [%s]\n",
				locks,lockExistsTime,lastUpdateTime);
	if ((ret = UnionResetWritingLocksOfPKDB()) < 0)
	{
		UnionUserErrLog("in main:: UnionResetWritingLocksOfPKDB failure! ret = [%d]\n",ret);
		goto maintainDesKeyDB;
	}
	UnionAuditLog("in main:: UnionResetWritingLocksOfPKDB OK!\n");
	if ((ret = UnionLoadPKDBIntoMemory()) < 0)
	{
		UnionUserErrLog("in main:: UnionLoadPKDBIntoMemory failure! ret = [%d]\n",ret);
		goto maintainDesKeyDB;
	}
	UnionAuditLog("in main:: UnionLoadPKDBIntoMemory OK!\n");
	// 维护公钥库结束
maintainDesKeyDB:
	// 维护DES密钥库

	//维护DES密钥库结束
	
	sleep(interval);
	goto loop; 
}
