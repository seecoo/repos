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

PUnionTaskInstance	ptaskInstance = NULL;

int UnionHelp()
{
	printf("Usage:: %s command\n",UnionGetApplicationName());
	printf(" Where command as:\n");
	printf("  -reload 		���ع����ڴ�[���ٻ����]\n");
	printf("  -all			�鿴���й����ڴ�[���ٻ����]\n");
	printf("  -clear 		��ջ�������\n");
	printf("  -key [����]		�鿴�������\n");
	printf("  -print 		�鿴memcached��magent��״̬\n");  //add by lusj 20151215
	printf("  -check		���memcached��magent��״̬\n");	//add by lusj 20151215	
	return(0);
}

int UnionTaskActionBeforeExit()
{
	UnionDisconnectHighCached();
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	exit(0);
}


//add begin by lusj 20151215  memcached��magent��⺯��
int UnionCheckMemcached(int seconds)
{
	int sleepTime;
	int i=0;
	
	if (UnionCreateProcess() > 0)
    		return(UnionTaskActionBeforeExit());

	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s -check",UnionGetApplicationName())) == NULL)
	{
		printf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}
	

	if(seconds < 5)
		sleepTime = 1;
	else
		sleepTime = seconds;

	for (;;)
	{
		UnionLog("in check memcached status!\n");	
		UnionCheckMemcachedStatus();
		sleep(sleepTime);
	}	
	return(0);
}
//add end by lusj 20151215

int main(int argc,char *argv[])
{
	int	ret;
	char	*p = NULL;
	char	tmpBuf[1024];
	char	value[81920];
		
	UnionSetApplicationName(argv[0]);
	if (argc < 2)
		return(UnionHelp());
		
	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(0);
	
	/*
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s",UnionGetApplicationName())) == NULL)
	{
		printf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}
	*/

	if (strcasecmp(argv[1],"-RELOAD") == 0)
	{
		if (!UnionConfirm("ȷ�����ع����ڴ�[���ٻ����]��?"))
			return(-1);
		if ((ret = UnionReloadHighCached()) < 0)
			printf("***** %-30s Error!\n","���ع����ڴ�[���ٻ����]");
		else
			printf("***** %-30s OK!\n","���ع����ڴ�[���ٻ����]");
		
		//add by lusj 20151231 ���¼��ظ��ٻ���״̬
                if((ret = UnionReloadMemcachedConf()) < 0)
                        printf("***** UnionReloadMemcachedConf Error!\n");
                else
                        printf("***** UnionReloadMemcachedConf OK!\n");
		//add end by lusj 20151231
	}
	else if (strcasecmp(argv[1],"-RELOADANYWAY") == 0)
	{
		if ((ret = UnionReloadHighCached()) < 0)
			printf("***** %-30s Error!\n","���ع����ڴ�[���ٻ����]");
		else
			printf("***** %-30s OK!\n","���ع����ڴ�[���ٻ����]");
	}
	else if (strcasecmp(argv[1],"-CLEAR") == 0)
	{
		if (!UnionConfirm("ȷ�����[���ٻ�������]��?"))
		{
			return(-1);
		}
		UnionFlushHighCachedService();
	}
	else if (strcasecmp(argv[1],"-ALL") == 0)
		UnionPrintHighCachedTBLToFile(stdout);
	else if (strcasecmp(argv[1],"-KEY") == 0)
	{
		if (argc > 2)
		{
			UnionPrintHighCachedKeysToFile(0,stdout,argv[2]);
			return(UnionTaskActionBeforeExit());
		}
		for (;;)
		{
			p = UnionInput("������KEY (exit/quit �˳�;all ����)::");
	
			memset(tmpBuf,0,sizeof(tmpBuf));		
			if (strcasecmp(p,"all") != 0)
				strcpy(tmpBuf,p);
	
			if (UnionIsQuit(p))
				return(UnionTaskActionBeforeExit());
			UnionPrintHighCachedKeysToFile(0,stdout,tmpBuf);
		}
	}
	else if (strcasecmp(argv[1],"-VALUE") == 0)
	{
		if (argc > 2)
		{
			memset(value,0,sizeof(value));
			ret = UnionGetHighCachedValue(argv[2],value,sizeof(value));
			fprintf(stdout,"len = [%d]\nvalue = [%s]\n",ret,value);
			return(UnionTaskActionBeforeExit());
		}
		for (;;)
		{
			p = UnionInput("������KEY (exit/quit �˳�)::");
	
			memset(tmpBuf,0,sizeof(tmpBuf));		
			strcpy(tmpBuf,p);
	
			if (UnionIsQuit(p))
				return(UnionTaskActionBeforeExit());

			memset(value,0,sizeof(value));
			ret = UnionGetHighCachedValue(tmpBuf,value,sizeof(value));
			fprintf(stdout,"len = [%d]\nvalue = [%s]\n",ret,value);
		}
	}	//add begin by lusj 20151215
	else if (strcasecmp(argv[1],"-PRINT") == 0)
		UnionPrintMemcachedConfToFile(stdout);
	else if (strcasecmp(argv[1],"-CHECK") == 0)
	{
		if(argc < 3)
			UnionCheckMemcached(1);
		else
			UnionCheckMemcached(atoi(argv[2]));

		return(UnionTaskActionBeforeExit());
	}		//add end by lusj 20151215
	else 
		return(UnionHelp());
	return(UnionTaskActionBeforeExit());
}

