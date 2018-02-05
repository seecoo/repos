//	Author:		Wolfgang Wang
//	Date:		2003/09/22


#define _UnionTask_3_x_	

#include <stdio.h>
#include <string.h>
#include <signal.h>

#include "UnionTask.h"
#include "unionCommand.h"
#include "UnionStr.h"
#include "UnionLog.h"
#include "unionVersion.h"

#include "unionServiceBuffer.h"

PUnionTaskInstance	ptaskInstance = NULL;


int UnionTaskActionBeforeExit()
{
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	//return(exit(0));
	exit(0);
}
	
int UnionHelp()
{
	printf("�÷�:: %s ����\n",UnionGetApplicationName());
	printf(" ��������::\n");
	printf(" -status	��ʾ���񻺳�����״̬\n");
	printf(" -clear		������񻺳����е�����\n");
	printf(" -guard [time]	ά�����񻺳���,time��ά���������ʱ����\n");
	return(0);
}

int main(int argc,char *argv[])
{
	char	command[128+1];
	int	ret = 0;
	int	interval = 0;
	int	index;
	
	// ���ó�������
	UnionSetApplicationName(argv[0]);
	
	if (argc < 2)	// ��������
		return(UnionHelp());
	
	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(0);

	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s",UnionGetApplicationName())) == NULL)
	{
		printf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}

	// ִ������
	strcpy(command,argv[1]);
	UnionToLowerCase(command);
	if ((strcmp(command,"-status") == 0) || (strcmp(command,"status") == 0))
	{
		if ((ret = UnionPrintStatusOfServiceBufferToFp(stdout)) < 0)
			printf("���ú���[UnionPrintStatusOfServiceBufferToFp]����!ret=[%d]!\n",ret);
	}
	else if ((strcmp(command,"-clear") == 0) || (strcmp(command,"clear") == 0))
	{
		if ((ret = UnionMaintainServiceBuffer()) < 0)
			printf("���ú���[UnionMaintainServiceBuffer]����!ret=[%d]!\n",ret);
		else
			printf("�������[%d]��������Ϣ!\n",ret);
	}
	else if ((strcmp(command,"-guard") == 0) || (strcmp(command,"guard") == 0))
	{
		// ����ʱ����
		if (argc >= 3)
			interval = atoi(argv[2]);
		if (interval <= 0)
			interval = 10;
		ret = UnionStartServiceBufferGuarder(interval);
	}
	else
		ret = UnionHelp();
		
	UnionTaskActionBeforeExit();
	
	return(ret);
}

// ����һ�������ػ�����
int UnionStartServiceBufferGuarder(int interval)
{
	int	ret;
	
	if (UnionCreateProcess() > 0)
		return(0);

	// ��������ʵ��
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s guard %d",UnionGetApplicationName(),interval)) < 0)
	{
		printf("���ú���[UnionCreateTaskInstance]��������ʵ��[%s guard %d]����!ret=[%d]!\n",UnionGetApplicationName(),interval,ret);
		return(ret);
	}
	for (;;)
	{
		if ((ret = UnionMaintainServiceBuffer()) < 0)
		{
			UnionUserErrLog("in UnionStartServiceBufferGuarder:: ���ú���[UnionMaintainServiceBuffer]ά���������߳���!");
			break;
		}
		if (ret > 0)
			UnionAuditLog("UnionStartServiceBufferGuarder","�����[%d]��������Ϣ!",ret);
		sleep(interval);
	}
	return(ret);
}
