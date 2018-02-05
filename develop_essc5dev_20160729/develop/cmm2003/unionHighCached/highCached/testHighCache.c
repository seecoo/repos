#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_
#endif
#ifndef _UnionLogMDL_3_x_
#define _UnionLogMDL_3_x_
#endif

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>

#include "UnionLog.h"
#include "unionCommand.h"
#include "unionVersion.h"
#include "UnionTask.h"

#include "highCached.h"
#include "unionHighCachedAPI.h"

int UnionTaskActionBeforeExit()
{
	return 0;
}

int main(int argc,char **argv)
{
	int		ret = 0;
	while(ret < 1000)
	{
		UnionCheckMemcachedStatus();
		sleep(1);
		ret ++;
	}
	return 0;
}
