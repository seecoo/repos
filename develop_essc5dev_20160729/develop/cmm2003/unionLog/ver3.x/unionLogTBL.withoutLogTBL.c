// 2010-12-12,王纯军
// 为了在不定义任务表和日志表时，也能使用UnionLog.*.c而设计

#ifndef _UnionLogMDL_3_x_
#define _UnionLogMDL_3_x_
#endif



#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>

#include "UnionLog.h"

PUnionLogFile UnionFindLogFile(char *logFileName __attribute__((unused)))
{
	return(NULL);
}

PUnionLogFile UnionGetLogFile()
{
	return(NULL);
}

