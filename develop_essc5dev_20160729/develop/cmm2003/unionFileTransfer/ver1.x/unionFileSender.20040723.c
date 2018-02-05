//	Wolfgang Wang, 2004/7/23

#include <stdio.h>
#include <string.h>
#include <time.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

#ifndef _UnionLogMDL_3_x_
#define _UnionLogMDL_3_x_
#endif

#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_
#endif

#include "UnionStr.h"
#include "UnionLog.h"		//  π”√3.x∞Ê±æ
#include "unionFileTransfer.h"
#include "UnionTask.h"
#include "unionVersion.h"
#include "unionCommand.h"

PUnionTaskInstance		ptaskInstance = NULL;
PUnionFileTransferHandle	pgfileTransferHandle = NULL;

int UnionTaskActionBeforeExit()
{
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	//return(exit(0));
	exit(0);
}

int UnionHelp()
{
	printf("Usage:: %s ipAddr port dir file [desDir [desFileName]]\n",UnionGetApplicationName());
	return(0);
}

int main(int argc,char **argv)
{
	int				ret;

	UnionSetApplicationName(argv[0]);
	if (argc < 5)
		return(UnionHelp());
		
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s",argv[0])) == NULL)
	{
		printf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}

	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(UnionTaskActionBeforeExit());
		
	if (UnionCreateProcess() > 0)
		return(UnionTaskActionBeforeExit());

	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s %s %s",argv[0],argv[1],argv[2])) == NULL)
	{
		printf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}

#ifdef _unionFileTransfer_2_x_
	char	*desDir = NULL;
	char	*desFileName = NULL;

	if (argc >= 6)
		desDir = argv[5];
	if (argc >= 7)
		desFileName = argv[6];
	if ((pgfileTransferHandle = UnionCreateFileTransferHandle(argv[1],atoi(argv[2]),argv[3],argv[4],desDir,desFileName)) == NULL)
	{
		printf("in %s:: UnionCreateFileTransferHandle Error!\n",argv[0]);
		return(UnionTaskActionBeforeExit());
	}
#else
	if ((pgfileTransferHandle = UnionCreateFileTransferHandle(argv[1],atoi(argv[2]),argv[3],argv[4])) == NULL)
	{
		printf("in %s:: UnionCreateFileTransferHandle Error!\n",argv[0]);
		return(UnionTaskActionBeforeExit());
	}
#endif	
	printf("in %s:: now begin to send file:\n",argv[0]);
	printf("fileName [%s]\n",pgfileTransferHandle->destinitionFileName);
	printf("fileDir  [%s]\n",pgfileTransferHandle->destinitionDir);
	printf("fileLen  [%lld]\n",pgfileTransferHandle->totalFileLength);
	
	if ((ret = UnionTransferFile(pgfileTransferHandle)) < 0)
		printf("in %s::UnionTransferFile failure!\n",argv[0]);
	else
		printf("in %s::UnionTransferFile OK!\n",argv[0]);

	printf("time used = [%ld]\n",pgfileTransferHandle->finishTime - pgfileTransferHandle->startTime);
	
	UnionReleaseFileTransferHandle(pgfileTransferHandle);
	
	return(UnionTaskActionBeforeExit());
}	
