//	Author:		zhang yong ding
//	Copyright:	Union Tech. Guangzhou
//	Date:		2013-04-23

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef _realBaseDB_2_x_
#define _realBaseDB_2_x_
#endif

#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_
#endif

#include "unionREC.h"
#include "UnionTask.h"
#include "unionVersion.h"
#include "unionCommBetweenMDL.h"
#include "unionRealBaseDB.h"
#include "UnionLog.h"
#include "unionXMLPackage.h"
#include "unionCommand.h"
#include "unionRealDBCommon.h"
#include "unionMsgBufGroup.h"
#include "unionMDLID.h"
#include "UnionStr.h"

PUnionTaskInstance	ptaskInstance = NULL;

void UnionDealCloseDatabase();

int UnionHelp()
{
	printf("Usage:: %s \n",UnionGetApplicationName());
	return(0);
}

int UnionTaskActionBeforeExit()
{
	UnionCloseDatabase();
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	exit(0);
}

int main(int argc,char *argv[])
{
	int			len;
	TUnionModuleID		applierMDLID;
	unsigned char		buf[1024];
	TUnionMessageHeader	msgHeader;
	
	UnionSetApplicationName(argv[0]);

	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(0);

	if (UnionCreateProcess() > 0)
		return(0);
	
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,UnionGetApplicationName())) == NULL)
	{
		printf("UnionCreateTaskInstance Error!\n");
		return(0);
	}

	UnionSetGroupIDOfMsgBuf(defMsgBufGroupOfMon);
	for (;;)
	{
		if ((len = UnionReadRequestToSpecifiedModuleWithMsgHeader(conMDLTypeOffsetOfMonSvr + conMDLTypeUnionMonSvr,buf,sizeof(buf),&applierMDLID,&msgHeader)) <= 0)
		{
			UnionUserErrLog("in %s:: UnionReadRequestToSpecifiedModuleWithMsgHeader [%d]!\n",UnionGetApplicationName(),conMDLTypeOffsetOfMonSvr + conMDLTypeUnionMonSvr);
			continue;
		}
		buf[len] = 0;
	}
}	
