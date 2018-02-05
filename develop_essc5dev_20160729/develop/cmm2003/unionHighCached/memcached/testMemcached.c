#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_
#endif

#ifndef _UnionLogMDL_3_x_
#define _UnionLogMDL_3_x_
#endif

#include "unionErrCode.h"
#include "UnionLog.h"
#include "UnionStr.h"
#include "UnionTask.h"
#include "unionVersion.h"
#include "unionMemcached.h"

PUnionTaskInstance ptaskInstance = NULL;

int UnionTaskActionBeforeExit()
{
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	exit(0);
}

int UnionHelp() 
{
	printf("Usage:: %s loopTimes\n",UnionGetApplicationName());
	return(0);
}

int main(int argc,char *argv[])
{
//	int	i = 0;
//	int	ret;
	int	loopTimes;
	//char	keyName[128+1];
	//char	value[512+1];
	//char	tmpBuf[512+1];

	UnionSetApplicationName(argv[0]);

	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(0);

	if( argc < 2 )
		return(UnionHelp());
	
	loopTimes = atoi(argv[1]);
	
	if( UnionCreateProcess() > 0 )
		return(0);
	
	if( (ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s",UnionGetApplicationName())) == NULL )
	{
		printf("in %s:: UnionCreateTaskInstance Error!\n",UnionGetApplicationName());
		return(0);
	}
	
	/*
	for (i = 0; i < loopTimes; i++)
	{
		UnionLogD("i[%d]\n",i);
		memset(tmpBuf,0,sizeof(tmpBuf));
		strcpy(tmpBuf,"select auditFlag from serviceCode where serviceCode = '0000'");
		

		memset(keyName,0,sizeof(keyName));
		//sprintf(keyName,"key%d",i);
		UnionMD5(tmpBuf,strlen(tmpBuf),keyName);

		memset(value,0,sizeof(value));
		//sprintf(value,"value%d",i);
		strcpy(value,"<?xml version=\"1.0\" encoding=\"GBK\"?><union><totalNum>1</totalNum><detail ID=\"1\"><auditFlag>0</auditFlag></detail></union>");
		if ((ret = UnionInitRequestXMLPackage(NULL,value,strlen(value))) < 0)
		{
			printf("[%d]UnionInitRequestXMLPackage[%s] ret = [%d]!\n",i,value,ret);
			goto sleepTag;
		}
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionReadRequestXMLPackageValue("totalNum",tmpBuf,sizeof(tmpBuf))) < 0)
		{
			printf("[%d]UnionReadRequestXMLPackageValue[%s] ret = [%d]!\n",i,value,ret);
			goto sleepTag;
		}
		memset(value,0,sizeof(value));
		if ((ret = UnionRequestXMLPackageToBuf(value,sizeof(value))) < 0)
		{
			printf("[%d]UnionRequestXMLPackageToBuf[%s] ret = [%d]!\n",i,value,ret);
			goto sleepTag;
		}
		
		//printf("value[%s]\n",value);
		if ((ret = UnionSetMemcachedValue(keyName,value,strlen(value),0)) < 0)
		{
			printf("[%d]UnionSetMemcachedValue[%s][%s] ret = [%d]!\n",i,keyName,value,ret);
			goto sleepTag;
		}
		
		memset(tmpBuf,0,sizeof(tmpBuf));
		if ((ret = UnionGetMemcachedValue(keyName,tmpBuf,sizeof(tmpBuf))) < 0)
		{
			printf("[%d]UnionSetMemcachedValue ret = [%d]!\n",i,ret);
			goto sleepTag;
		}

		//printf("get [%s][%s]\n",keyName,tmpBuf);
sleepTag:
		//sleep(1);
		continue;
	}
		*/
	zlog_fini();
	//UnionCloseMemcached();
	return(UnionTaskActionBeforeExit());
}
