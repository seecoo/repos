//	Author:		Wolfgang Wang
//	Date:		2003/09/19
//	Version:	1.0

//	2003/09/19�������ڽ� UnionTask2.1.c����Ϊ unionTask3.0.c ʱ����ͨ�õĽ��̹������������������С�

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#define _UnionTask_3_x_
#include "UnionTask.h"
#include "UnionLog.h"

int UnionGetPID()
{
	return(getpid());
}

int UnionCreateProcess()
{
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
}

int UnionIgnoreSignals()
{

	setsid();

	signal(SIGALRM,SIG_IGN);
	signal(SIGINT,SIG_IGN);
	signal(SIGQUIT,SIG_IGN);
	signal(SIGHUP,SIG_IGN);
	signal(SIGCLD,SIG_IGN);
	return(0);
}


