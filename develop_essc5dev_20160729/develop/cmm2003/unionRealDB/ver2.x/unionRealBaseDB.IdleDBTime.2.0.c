#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include <unistd.h>
#include <signal.h>

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_
#endif

#include "UnionTask.h"
#include "unionVersion.h"
#include "UnionStr.h"
#include "UnionLog.h"
#include "unionREC.h"

void		UnionTaskActionBeforeExit();

jmp_buf 	gunionDBJmpEnvForIdleDBTime;


void UnionIdleDBTimeout()
{
	UnionUserErrLog("in UnionOperateDBTimeout:: Timeout!\n");
#ifdef _LINUX_
	siglongjmp(gunionDBJmpEnvForIdleDBTime,10);
#else
	longjmp(gunionDBJmpEnvForIdleDBTime,10);
#endif
}

//数据库操作起时控制，若超时，将自身进行退出操作
int UnionSetIdleDBTime(int seconds)
{
	int	timeoutOfDB = 0;

	if(seconds >= 0)
	{
		timeoutOfDB = seconds;
	}
	else
	{
		if ((timeoutOfDB = UnionReadIntTypeRECVar("timeoutOfDB")) < 0)
		{
			timeoutOfDB = 10;
		}
	}

#ifdef _LINUX_
	if (sigsetjmp(gunionDBJmpEnvForIdleDBTime,1) != 0) // 超时退出
#else
	if (setjmp(gunionDBJmpEnvForIdleDBTime) != 0)   // 超时退出
#endif
	{
		UnionTaskActionBeforeExit();
	}

	alarm(timeoutOfDB);

	signal(SIGALRM,UnionIdleDBTimeout);
	return(0);
}

void UnionCloseIdleDBAlrm()
{
	alarm(0);
}
