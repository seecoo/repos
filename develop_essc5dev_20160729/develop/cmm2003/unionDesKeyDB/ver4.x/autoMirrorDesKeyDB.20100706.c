/*
	Author:
		HuangBaoxin
	Data:
		2010/07/06
	Desc:
		auto mirror deskeyDB to file
*/
#define _UnionTask_3_x_
#define _UnionDesKeyDB_2_x_
#define _UnionLogMDL_3_x_

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <time.h>

#ifndef _UnionDesKey_3_x_
#define _UnionDesKey_3_x_
#endif


#include "UnionLog.h"
#include "unionCommand.h"
#include "unionVersion.h"
#include "UnionTask.h"
#include "unionModule.h"

#include "unionDesKeyDB.h"

PUnionTaskInstance	ptaskInstance = NULL;

int UnionTaskActionBeforeExit()
{
	UnionDisconnectDesKeyDB();
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	exit(0);	//return(exit(0));
}

int UnionHelp()
{
	printf("Usage:: %s seconds\n",UnionGetApplicationName());
	return(0);
}
/*
	函数功能:
		自动将密钥库从共享内存写回密钥库文件
*/
int UnionAutoMirrorDesKeyDB(int seconds)
{
	char	lastUpdateTimeBuf[20];
	char	currUpdateTimeBuf[20];
	int	ret, retryTimes = 0;
	
	memset(lastUpdateTimeBuf, 0, sizeof(lastUpdateTimeBuf));
	
	if (seconds <= 0)
		seconds = 300;

	for (;;)
	{
		UnionLog("in UnionAutoMirrorDesKeyDB:: loop to mirror desKeyDB every [%d] seconds!\n", seconds);
		sleep(seconds);
		
		retryTimes = 0;
		memset(currUpdateTimeBuf, 0, sizeof(currUpdateTimeBuf));
		strcpy(currUpdateTimeBuf, UnionGetDesKeyDBLastUpdatedTime());
		if (strcmp(currUpdateTimeBuf, lastUpdateTimeBuf) == 0)
			continue;
reTry:
		retryTimes++;
		if ((ret =UnionWritingLockDesKeyTBL()) < 0)
		{
			UnionLog("in UnionAutoMirrorDesKeyDB::UnionWritingLockDesKeyTBL!\n");
			if (retryTimes < 100)
			{
				usleep(1000*100);
				goto reTry;
			}
			else
				continue;
		}
		
		if ((ret = UnionMirrorDesKeyDBIntoDiskArtifically()) < 0)
		{
			UnionUserErrLog("in UnionAutoMirrorDesKeyDB::UnionMirrorDesKeyDBIntoDiskArtifically!\n");
			UnionWritingUnlockDesKeyTBL();
			continue;
		}
		else
		{
			UnionLog("in UnionAutoMirrorDesKeyDB::mirror deskeyDB into Disk succ!\n");
			strcpy(lastUpdateTimeBuf, UnionGetDesKeyDBLastUpdatedTime());
			UnionWritingUnlockDesKeyTBL();
		}
	}
}

int main(int argc,char **argv)
{
	int	ret;
	int	i;
	
	UnionSetApplicationName(argv[0]);
	if (argc < 2)
		return(UnionHelp());

	if (UnionCreateProcess() > 0)
		return(UnionTaskActionBeforeExit());
			
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s",UnionGetApplicationName())) == NULL)
	{
		printf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}

	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(UnionTaskActionBeforeExit());

	UnionAutoMirrorDesKeyDB(atoi(argv[1]));
	
	return(UnionTaskActionBeforeExit());
}
