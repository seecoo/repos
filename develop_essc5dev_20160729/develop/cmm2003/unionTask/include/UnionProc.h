//	Author:		Wolfgang Wang
//	Date:		2003/09/19
//	Version:	1.0

//	2003/09/19王纯军在将 UnionTask2.1.c升级为 unionTask3.0.c 时，将通用的进程管理函数分流到本程序中。

#ifndef _unionProc_
#define _unionProc_


#ifdef _WIN32_
#include <process.h>
#endif


int UnionGetPID();

int UnionGetThreadID();

int UnionCreateProcess();

#ifndef _WIN32_
int UnionIgnoreSignals();
#endif

int UnionGetTaskID();

#endif


