//	Author:		Wolfgang Wang
//	Date:		2003/09/19
//	Version:	1.0

//	2003/09/19�������ڽ� UnionTask2.1.c����Ϊ unionTask3.0.c ʱ����ͨ�õĽ��̹������������������С�

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>

#ifndef _WIN32
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#else
#include <IdGlobal.hpp>
#endif

#include "UnionLog.h"

#include "UnionProc.h"

int UnionGetTaskID()
{
#ifndef _WIN32
	return(UnionGetPID());
#else
	return(UnionGetThreadID());
#endif
}

int UnionGetPID()
{
	return(getpid());
}

int UnionGetThreadID()
{
#ifdef _WIN32
	return(GetCurrentThreadId());
#else
	//return(getpid());
	return(pthread_self());
#endif
}

int UnionCreateProcess()
{
#ifndef _WIN32
	int i;
	int pid;

	for (i=0;i<10;i++)
	{
		if ((pid = fork()) != -1)
			break;

		else
		{
			UnionSystemErrLog("in UnionCreateProcess:: fork!\n");
			usleep(1000);

		}
	}
	return(pid);
#else
        return(UnionGetPID());
#endif
}

#ifndef _WIN32
int UnionIgnoreSignals()
{

	setsid();

	signal(SIGALRM,SIG_IGN);
	signal(SIGINT,SIG_IGN);
	signal(SIGQUIT,SIG_IGN);
	signal(SIGHUP,SIG_IGN);
//#ifdef _AIX_
	signal(SIGCLD,SIG_IGN);
//#endif
	return(0);
}
#endif

