#include <stdio.h>
#include <string.h>
#include <time.h>

#ifndef _UnionTask_3_x_
#define _UnionTask_3_x_
#endif

#include "unionMDLID.h"
#include "UnionTask.h"
#include "unionVersion.h"

#include "unionCommand.h"
#include "transSpierBuf.h"
#include "unionMonitorTask.h"
#include "unionMonitorHsm.h"
#include "unionMonitorEsscAvailable.h"
#include "unionMonitorMem.h"
#include "unionMonitorCpu.h"
#include "unionMonitorDisk.h"
#include "unionMonitorTransDetail.h"
#include "unionMonitorIOStats.h"

PUnionTaskInstance	ptaskInstance = NULL;

int 	gunionIntervalTimeOfMon = 60;	// 间隙时间
int	gunionResId = 0;		// 资源ID
int	gunionSendTestResDataFlag = 0;	// 是否发送测试报文


int UnionTaskActionBeforeExit()
{
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	exit(0);
}

int UnionHelp()
{
	printf("USAGE: %s command\n", UnionGetApplicationName());
	printf("  Where command as:\n");
	printf("    -task intervalTime		monitor task resource\n");
	printf("    -esscavailable port		monitor esscavailable resource\n");
	printf("    -hsm intervalTime		monitor hsm resource\n");
	printf("    -transdetail 		monitor transdetail resource\n");
	printf("    -cpuinfo intervalTime	monitor cpuinfo resource\n");
	printf("    -meminfo intervalTime	monitor meminfo resource\n");
	printf("    -diskinfo intervalTime	monitor diskinfo resource\n");
	printf("    -ioinfo intervalTime	monitor ioinfo resource\n");
	printf("    -sysmaxconn intervalTime	monitor sysmaxconn resource\n");
	printf("    -hsmcmd intervalTime	monitor hsmcmd resource\n");
	printf("    -queue intervalTime		monitor queue resource\n");
	return 0;
}

int main(int argc,char *argv[])
{
	//int	ret;
	char	instanceName[256];

	UnionSetApplicationName(argv[0]);
	
	if (argc < 2)
		return(UnionHelp());

	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(0);

	if (UnionCreateProcess() > 0)
		return(0);

	UnionSetLongConnwithMonitor();

	snprintf(instanceName, sizeof(instanceName), "%s %s", argv[0], argv[1]);
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit, "%s", instanceName)) == NULL)
	{
		UnionPrintf("in %s:: UnionCreateTaskInstance '%s' Error!\n",UnionGetApplicationName(),argv[1]);
                return(UnionTaskActionBeforeExit());
	}

	if (argc > 2)
		gunionIntervalTimeOfMon = atoi(argv[2]);
	if (strcasecmp(argv[1], "-TASK") == 0)
	{
		UnionStartTaskSpier();
	}
	else if (strcasecmp(argv[1], "-ESSCAVAILABLE") == 0)
	{
		UnionStartEsscAvailableSvr(atoi(argv[2]), UnionTaskActionBeforeExit);
	}
	else if (strcasecmp(argv[1], "-HSM") == 0)
	{
		UnionStartHsmSpier();
	}
	else if (strcasecmp(argv[1], "-TRANSDETAIL") == 0)
	{
		UnionStartTransDetailSpier();
	}
	else if (strcasecmp(argv[1], "-CPUINFO") == 0)
	{
		UnionStartCpuSpier();
	}
	else if (strcasecmp(argv[1], "-MEMINFO") == 0)
	{
		UnionStartMemSpier();
	}
	else if (strcasecmp(argv[1], "-DISKINFO") == 0)
	{
		UnionStartDiskSpier();
	}
	else if (strcasecmp(argv[1], "-IOINFO") == 0)
	{
		UnionStartIOStatSpier();
	}
	else if (strcasecmp(argv[1], "-SYSMAXCONN") == 0)
	{
		UnionStartSysMaxConnSpier();
	}
	else if (strcasecmp(argv[1], "-HSMCMD") == 0)
	{
		UnionStartHsmCmdSpier();
	}
	else if (strcasecmp(argv[1], "-Queue") == 0)
	{
		UnionStartMsgQueueSpier();
	}
	else
	{
		UnionPrintf("in %s:: parameter %s error!\n",UnionGetApplicationName(),argv[1]);
		UnionHelp();
	}

	return(UnionTaskActionBeforeExit());
}

