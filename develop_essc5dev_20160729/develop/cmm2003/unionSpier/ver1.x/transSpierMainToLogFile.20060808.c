// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2005/12/30
// Version:	1.0

#include <stdio.h>
#include <string.h>

#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_
#endif

#include "unionMDLID.h"
#include "UnionTask.h"
#include "unionVersion.h"
#include "unionResID.h"
#include "transSpierBuf.h"
#include "UnionLog.h"
#include "unionCommand.h"

PUnionTaskInstance	ptaskInstance = NULL;

int UnionTaskActionBeforeExit()
{
	UnionDisconnectTransSpierBufMDL();
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	exit(0);
}

int UnionStartTransSpier(char *ipAddr)
{
	int				ret;
	unsigned char			data[512+1];
	int				len;
	int				resID;
	TUnionTransSpierMsgHeader	msgHeader;
	char				header[512+1];

loop:
	// 获取要监控的信息
	memset(data,0,sizeof(data));
	if ((len = UnionReadFirstTransSpierMsgWithMsgHeaderUntilSuccess(data,sizeof(data)-1,&resID,&msgHeader)) < 0)
	{
		UnionUserErrLog("in UnionStartTransSpier:: UnionReadMsgOfSpecifiedTypeUntilSuccess!\n");
		return(len);
	}
	memset(header,0,sizeof(header));
	if ((ret = UnionPutTransSpierMsgHeaderIntoStr(&msgHeader,header,sizeof(header))) < 0)
	{
		UnionUserErrLog("in UnionStartTransSpier:: UnionPutTransSpierMsgHeaderIntoStr!\n");
		return(ret);
	}
	UnionNullLog("[%s][%s]\n",header,data);
	goto loop;
}

int UnionHelp()
{
	printf("Usage:: %s\n",UnionGetApplicationName());
	return(0);
}

int main(int argc,char *argv[])
{
	UnionSetApplicationName(argv[0]);
		
	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(0);
	
	if (UnionCreateProcess() > 0)
		return(0);

	//UnionSetMyModuleType(conEsscMDLTypeTransSpier);

	if (UnionCreateProcess() > 0)
		return(0);
		
	if (UnionExistsAnotherTaskOfName(UnionGetApplicationName()) > 0)
	{
		printf("Another task of name [%s] already exists!\n",UnionGetApplicationName());
		return(UnionTaskActionBeforeExit());
	}

	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s",UnionGetApplicationName())) == NULL)
	{
		UnionPrintf("in %s:: UnionCreateTaskInstance '%s' Error!\n",UnionGetApplicationName(),argv[1]);
		return(UnionTaskActionBeforeExit());
	}

	UnionStartTransSpier(argv[1]);
	
	return(UnionTaskActionBeforeExit());
}

