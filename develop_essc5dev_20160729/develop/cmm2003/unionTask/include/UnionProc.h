//	Author:		Wolfgang Wang
//	Date:		2003/09/19
//	Version:	1.0

//	2003/09/19�������ڽ� UnionTask2.1.c����Ϊ unionTask3.0.c ʱ����ͨ�õĽ��̹������������������С�

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


