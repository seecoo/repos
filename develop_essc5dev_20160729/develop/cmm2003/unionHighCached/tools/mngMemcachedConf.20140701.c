//	Author:	linxj
//	Date:	20140701

#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <libmemcached/memcached.h>
#include "UnionTask.h"

#include "UnionLog.h"
#include "unionModule.h"
#include "UnionEnv.h"
#include "unionCommand.h"
#include "unionErrCode.h"

#include "unionREC.h"
#include "UnionStr.h"
#include "unionRecFile.h"
#include "unionMemcached.h"
#include "unionVersion.h"

PUnionTaskInstance	ptaskInstance = NULL;

int UnionTaskActionBeforeExit()
{

	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	UnionCloseMemcached_Conf(); 	//close memcached connect
	exit(0);
}

int UnionHelp()
{
	printf("Usage:: %s Command Arguments\n",UnionGetApplicationName());
	printf("Where:: Command as follow:\n");
	printf(" -print \n");
	printf("	Print memcached server msg .\n");
	printf(" -reload\n");
	printf("	reload memcached conf .\n");	
	printf(" -check\n");
	printf("	Check memcached status\n");
	printf(" -clean\n");
	printf("	clean memcached Cache data\n");
	printf(" -set\n");
	printf("	set memcached state\n");
	return(0);
}

int UnionExcutePreDefinedCommand(int argc,char *argv[])
{
	if (argc <= 0)
		return(0);
	
	UnionToUpperCase(argv[0]);
	
	if ((strcmp(argv[0],"HELP") == 0) || (strcmp(argv[0],"-?") == 0))
	{
		UnionHelp();
		return(1);
	}

	if (strcmp(argv[0],"VERSION") == 0)
	{
		UnionPrintProductVersionToFile(stderr);
		return(1);
	}
	
	return(0);
}


int main(int argc,char **argv)
{
	int	ret;
	char memcached_ip_for_clean[17]={0};
	char memcached_port_for_clean[8]={0};
	char memcached_cmd_for_clean[2]={0};	

	UnionSetApplicationName(argv[0]);

	if (argc < 2)
		return(UnionHelp());
/*	
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s",UnionGetApplicationName())) == NULL)
	{
		printf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}
*/
	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
	return(UnionTaskActionBeforeExit());
	

	if (strcmp(argv[1],"-CHECK") == 0)
	{
		if(argc < 3)
			UnionCheckMemcached(5);
		else
			UnionCheckMemcached(atoi(argv[2]));

		return(UnionTaskActionBeforeExit());
	}

	if (strcmp(argv[1],"-PRINT") == 0)
		return(UnionPrintMemcachedConfToFile(stdout));

	if(strcmp(argv[1],"-RELOAD") == 0)
	{
		if (!UnionConfirm("Are you sure of reloading running enviroment configuration?"))
			return(-1);
		if((ret = UnionReloadMemcachedConf()) < 0)
			UnionPrintf("in mngMemcachedConf:: UnionReloadMemcachedConf Error! ret = [%d]\n",ret);
		else
			UnionPrintf("in mngMemcachedConf:: UnionReloadMemcachedConf OK!\n");
		return(UnionTaskActionBeforeExit());
	}

	if(strcmp(argv[1],"-RELOADANYWAY") == 0)
	{
		if((ret = UnionReloadMemcachedConf()) < 0)
			UnionPrintf("in mngMemcachedConf:: UnionReloadMemcachedConf Error! ret = [%d]\n",ret);
		else
			UnionPrintf("in mngMemcachedConf:: UnionReloadMemcachedConf OK!\n");
		return(UnionTaskActionBeforeExit());
	}

//add begin by lusj 20151026
	/***************************清除高速缓存***********************************/	
	if(strcmp(argv[1],"-CLEAN") == 0)
	{
		memset(memcached_ip_for_clean,0,sizeof(memcached_ip_for_clean));
		memset(memcached_port_for_clean,0,sizeof(memcached_port_for_clean));
		memset(memcached_cmd_for_clean,0,sizeof(memcached_cmd_for_clean));

		printf("请输入需要清除缓存的memcached_IP地址:");
		scanf("%s",memcached_ip_for_clean);
		
		printf("请输入对应的memcached_port端口:");
		scanf("%s",memcached_port_for_clean);

		printf("警告!!请确认是否清除memcached:%s %s 中的缓存[Y/N]!!:\n",memcached_ip_for_clean,memcached_port_for_clean);
		scanf("%s",memcached_cmd_for_clean);	

		if((strncmp(memcached_cmd_for_clean,"Y",1) == 0) ||(strncmp(memcached_cmd_for_clean,"y",1) == 0) )
		{
			printf("正在清除高速缓存!\n");
			
			if((ret =UnionFlushMemcached_Of_IP(memcached_ip_for_clean,atoi(memcached_port_for_clean)))<0)
				printf(">>[in UnionFlushMemcached_Of_IP]清空缓存::错误 ret = [%d]\n",ret);
			else
				printf(">>清空缓存::成功\n");	
			return(UnionTaskActionBeforeExit());

		}
		else
		{
			printf("取消高速缓存!\n");
			return(UnionTaskActionBeforeExit());
		}
	}	
//add end by lusj 20151026


	if(strcmp(argv[1],"-SET") == 0)
	{
		UnionPrintMemcachedConfToFile(stdout);
		
		if((ret = UnionSetMemcachedStatus()) < 0)
			UnionPrintf("in UnionSetMemcachedStatus::  Error! ret = [%d]\n",ret);
		else
			UnionPrintMemcachedConfToFile(stdout);

		return(UnionTaskActionBeforeExit());
	}



	
	return(UnionTaskActionBeforeExit());
}


int UnionCheckMemcached(int seconds)
{
	int sleepTime;
	if (UnionCreateProcess() > 0)
                return(UnionTaskActionBeforeExit());

	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s -check",UnionGetApplicationName())) == NULL)
	{
		printf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}

	if(seconds < 5)
		sleepTime = 5;
	else
		sleepTime = seconds;

	for (;;)
	{
		UnionCheckMemcachedStatus();
		sleep(sleepTime);
	}	

	return(0);
}

