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
	printf("用法:: %s 命令\n",UnionGetApplicationName());
	printf(" 命令如下::\n");
	printf(" -status	显示服务缓冲区的状态\n");
	printf(" -clear		清除服务缓冲区中的垃圾\n");
	printf(" -guard [time]	维护服务缓冲区,time是维护动作间的时间间隔\n");
	return(0);
}

int main(int argc,char *argv[])
{
	char	command[128+1];
	int	ret = 0;
	int	interval = 0;
	int	index;
	
	// 设置程序名称
	UnionSetApplicationName(argv[0]);
	
	if (argc < 2)	// 参数不足
		return(UnionHelp());
	
	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(0);

	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s",UnionGetApplicationName())) == NULL)
	{
		printf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}

	// 执行命令
	strcpy(command,argv[1]);
	UnionToLowerCase(command);
	if ((strcmp(command,"-status") == 0) || (strcmp(command,"status") == 0))
	{
		if ((ret = UnionPrintStatusOfServiceBufferToFp(stdout)) < 0)
			printf("调用函数[UnionPrintStatusOfServiceBufferToFp]出错!ret=[%d]!\n",ret);
	}
	else if ((strcmp(command,"-clear") == 0) || (strcmp(command,"clear") == 0))
	{
		if ((ret = UnionMaintainServiceBuffer()) < 0)
			printf("调用函数[UnionMaintainServiceBuffer]出错!ret=[%d]!\n",ret);
		else
			printf("共清除了[%d]个垃圾信息!\n",ret);
	}
	else if ((strcmp(command,"-guard") == 0) || (strcmp(command,"guard") == 0))
	{
		// 计算时间间隔
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

// 启动一个任务守护程序
int UnionStartServiceBufferGuarder(int interval)
{
	int	ret;
	
	if (UnionCreateProcess() > 0)
		return(0);

	// 创建任务实例
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s guard %d",UnionGetApplicationName(),interval)) < 0)
	{
		printf("调用函数[UnionCreateTaskInstance]创建任务实例[%s guard %d]出错!ret=[%d]!\n",UnionGetApplicationName(),interval,ret);
		return(ret);
	}
	for (;;)
	{
		if ((ret = UnionMaintainServiceBuffer()) < 0)
		{
			UnionUserErrLog("in UnionStartServiceBufferGuarder:: 调用函数[UnionMaintainServiceBuffer]维护服务总线出错!");
			break;
		}
		if (ret > 0)
			UnionAuditLog("UnionStartServiceBufferGuarder","清除了[%d]个垃圾信息!",ret);
		sleep(interval);
	}
	return(ret);
}
