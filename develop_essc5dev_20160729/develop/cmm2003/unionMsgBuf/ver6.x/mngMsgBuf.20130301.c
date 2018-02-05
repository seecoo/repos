//	Author:		Wolfgang Wang
//	Date:		2001/08/29
//	Version:	2.0

#define _UnionLogMDL_3_x_
#define _UnionTask_3_x_

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>

#include "UnionLog.h"
#include "unionCommand.h"
#include "unionVersion.h"
#include "UnionTask.h"

#include "unionMsgBuf6.x.h"

PUnionTaskInstance	ptaskInstance = NULL;

/*
int UnionHelp()
{
	printf("Usage:: %s command\n",UnionGetApplicationName());
	printf(" Where command as:\n");
	printf("  -reload 	reload the msgBufDef\n");
	printf("  -available 	print the available msgBuf pos\n");
	printf("  -occupied 	print the occupied msgBuf pos\n");
	printf("  -status	print the status of msgBuf\n");
	printf("  -printall	print the whole status of msgBuf\n");
	printf("  -clear	clear rubbish message\n");
	printf("  -resetall	reset all index\n");
	printf("  -setlostindex index ...\n");
	printf("  -repair index ...\n");
	printf("  -repairall\n");
	return(0);
}
*/

int UnionHelp()
{
	printf("Usage:: %s command\n",UnionGetApplicationName());
	printf(" Where command as:\n");
	printf("  -reload 	[mdlName]       reload the msgBufDef\n");
	printf("  -available 	[mdlName]       print the available msgBuf pos\n");
	printf("  -occupied 	[mdlName]       print the occupied msgBuf pos\n");
	printf("  -status	[mdlName]       print the status of msgBuf\n");
	printf("  -printall	[mdlName]       print the whole status of msgBuf\n");
	printf("  -clear	[mdlName]       clear rubbish message\n");
	printf("  -clearall			clear rubbish message for all massage queue\n");
	printf("  -resetall	[mdlName]       reset all index\n");
	printf("  -setlostindex [mdlName] index...\n");
	printf("  -repair [mdlName] index...\n");
	printf("  -repairall [mdlName]\n");
	return(0);
}

int UnionTaskActionBeforeExit()
{
	UnionDisconnectMsgBufMDL();
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	//return(exit(0));
	exit(0);
}

int UnionLoadMsgBufNameGrp(char grp[][40+1])
{
	char			fileName[128];
	char			lineBuf[256];
	FILE			*fp = NULL;
	int			num = 0;
	char			*ptr = NULL;

	memset(fileName, 0, sizeof(fileName));
	UnionGetNameOfSharedMemoryModuleTBL(fileName);
	

        if ((fp = fopen(fileName,"r")) == NULL)
	{
		UnionUserErrLog("in UnionLoadMsgBufNameGrp:: fopen [%s]\n", fileName);
		return(-1);
	}

	while (!feof(fp))
	{
		memset(lineBuf, 0, sizeof(lineBuf));
		if (UnionReadOneDataLineFromTxtFile(fp, lineBuf, sizeof(lineBuf)) <= 0)
		{
			continue;
		}

		if(lineBuf[0] != '[')
		{
			continue;
		}

		if((ptr = strstr(lineBuf, "]")) == NULL)
		{
			continue;
		}

		*ptr = 0;

		if(strstr(lineBuf + 1, "MsgBuf") == NULL)
		{
			continue;
		}

		strcpy(grp[num++], lineBuf + 1);

        }
	fclose(fp);

	return(num);
}

int main(int argc,char *argv[])
{
	int	ret;
	char	*p;
	int	i;
	char	mdlName[40+1];
	char	cmdPre[128];

	UnionSetApplicationName(argv[0]);
	if (argc < 2)
		return(UnionHelp());

	if(strcasecmp("-reload", argv[1]) == 0)
	{
	}
	else if(strcasecmp("-reloadanyway", argv[1]) == 0)
	{
	}
	else if(strcasecmp("-available", argv[1]) == 0)
	{
	}
	else if(strcasecmp("-occupied", argv[1]) == 0)
	{
	}
	else if(strcasecmp("-status", argv[1]) == 0)
	{
	}
	else if(strcasecmp("-printall", argv[1]) == 0)
	{
	}
	else if(strcasecmp("-clear", argv[1]) == 0)
	{
	}
	else if(strcasecmp("-clearall", argv[1]) == 0)
	{
	}
	else if(strcasecmp("-resetall", argv[1]) == 0)
	{
	}
	else if(strcasecmp("-setlostindex", argv[1]) == 0)
	{
	}
	else if(strcasecmp("-repair", argv[1]) == 0)
	{
	}
	else if(strcasecmp("-repairall", argv[1]) == 0)
	{
	}
	else
	{
		return(UnionHelp());
	}

	memset(mdlName, 0, sizeof(mdlName));
	strcpy(mdlName, "UnionMsgBufMDL");
	if(argc >= 3 && strncmp(argv[2], "Union", 5) == 0)
	{
		memset(mdlName, 0, sizeof(mdlName));
		strncpy(mdlName, argv[2], sizeof(mdlName) - 1);
	}
		
	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(UnionTaskActionBeforeExit());
	
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,UnionGetApplicationName())) == NULL)
	{
		UnionPrintf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}

	UnionSetMDLNameOfMsgBuf(mdlName); 
	if ((ret = UnionConnectMsgBufMDL()) < 0)
	{
		UnionPrintf("UnionConnectMsgBufMDL Error! ret = [%d]\n",ret);
		return(UnionTaskActionBeforeExit());
	}
	
	if (strcasecmp(argv[1],"-RELOAD") == 0)
	{
		if (!UnionConfirm("确定加载共享内存[消息表]吗?"))
			return(-1);
		if ((ret = UnionReloadMsgBufDef()) < 0)
			printf("***** %-30s Error!\n","加载共享内存[消息表]");
		else
			printf("***** %-30s OK!\n","加载共享内存[消息表]");
		return(UnionTaskActionBeforeExit());
	}
	if (strcasecmp(argv[1],"-RELOADANYWAY") == 0)
	{
		if ((ret = UnionReloadMsgBufDef()) < 0)
			printf("***** %-30s Error!\n","加载共享内存[消息表]");
		else
			printf("***** %-30s OK!\n","加载共享内存[消息表]");
		return(UnionTaskActionBeforeExit());
	}

	if (strcasecmp(argv[1],"-AVAILABLE") == 0)
	{
		if ((ret = UnionPrintAvailablMsgBufPosToFile(stdout)) < 0)
			UnionPrintf("in mngMsgBuf:: UnionPrintAvailablMsgBufPosToFile Error! ret = [%d]\n",ret);
		return(UnionTaskActionBeforeExit());
	}
	if (strcasecmp(argv[1],"-OCCUPIED") == 0)
	{
		if ((ret = UnionPrintInavailabeMsgBufPosToFile(stdout)) < 0)
			UnionPrintf("in mngMsgBuf:: UnionPrintInavailabeMsgBufPosToFile Error! ret = [%d]\n",ret);
		return(UnionTaskActionBeforeExit());
	}

	if (strcasecmp(argv[1],"-STATUS") == 0)
	{
		if ((ret = UnionPrintMsgBufStatusToFile(stdout)) < 0)
			UnionPrintf("in mngMsgBuf:: UnionPrintMsgBufStatusToFile Error! ret = [%d]\n",ret);
		return(UnionTaskActionBeforeExit());
	}

	if (strcasecmp(argv[1],"-PRINTALL") == 0)
	{
		if ((ret = UnionPrintMsgBufToFile(stdout)) < 0)
			UnionPrintf("in mngMsgBuf:: UnionPrintMsgBufToFile Error! ret = [%d]\n",ret);
		return(UnionTaskActionBeforeExit());
	}

	if (strcasecmp(argv[1],"-CLEAR") == 0)
	{
		memset(cmdPre, 0, sizeof(cmdPre));
		if(!isdigit(argv[2][0]))
		{
			sprintf(cmdPre, "%s %s", "mngMsgBuf -clear", argv[2]);
			return(StartClearTask(argc-3, &argv[3], cmdPre));
		}
		else
		{
			strcpy(cmdPre, "mngMsgBuf -clear");
			return(StartClearTask(argc-2, &argv[2], cmdPre));
		}
	}
	if (strcasecmp(argv[1],"-CLEARALL") == 0)
	{
		memset(cmdPre, 0, sizeof(cmdPre));
		strcpy(cmdPre, "mngMsgBuf -clearall");
		return(StartClearTask(argc-2, &argv[2], cmdPre));
	}

	if (strcasecmp(argv[1],"-RESETALL") == 0)
	{
		if (UnionConfirm("Are you sure of reset all the index available?"))
		{
			if ((ret = UnionResetAllMsgIndexAvailable()) < 0)
				printf("UnionResetAllMsgIndexAvailable failure! ret = [%d]\n",ret);
			else
				printf("UnionResetAllMsgIndexAvailable OK!\n");
		}
		return(UnionTaskActionBeforeExit());
	}
	
	if (strcasecmp(argv[1],"-SETLOSTINDEX") == 0)
	{
		for (i = 2; i < argc; i++)
		{
			if(!isdigit(argv[i][0]))
			{
				continue;
			}
			if (!UnionConfirm("Are you sure of set the index [%04d] of msgBuf losted?",atoi(argv[i])))
				continue;
			if ((ret = UnionSetSpecMsgBufIndexLosted(atoi(argv[i]))) < 0)
				printf("UnionSetSpecMsgBufIndexLosted [%04d] failure! ret = [%d]\n",atoi(argv[i]),ret);
			else
				printf("UnionSetSpecMsgBufIndexLosted [%04d] OK!\n",atoi(argv[i]));
		}
		return(UnionTaskActionBeforeExit());		
	}
	
	if (strcasecmp(argv[1],"-REPAIR") == 0)
	{
		for (i = 2; i < argc; i++)
		{
			if(!isdigit(argv[i][0]))
			{
				continue;
			}
			if (!UnionConfirm("Are you sure of set the index [%04d] of msgBuf free?",atoi(argv[i])))
				continue;
			if ((ret = UnionRepairSpecMsgBufIndex(atoi(argv[i]))) < 0)
				printf("UnionRepairSpecMsgBufIndex [%04d] failure! ret = [%d]\n",atoi(argv[i]),ret);
			else
				printf("UnionRepairSpecMsgBufIndex [%04d] OK!\n",atoi(argv[i]));
		}
		return(UnionTaskActionBeforeExit());		
	}

	if (strcasecmp(argv[1],"-REPAIRALL") == 0)
	{
		if (UnionConfirm("Are you sure of repair all the indexes?"))
		{
			if ((ret = UnionRepairAllMsgBufIndex()) < 0)
				printf("UnionRepairAllMsgBufIndex failure! ret = [%d]\n",ret);
			else
				printf("UnionRepairAllMsgBufIndex OK!\n");
		}
		return(UnionTaskActionBeforeExit());		
	}

	printf("wrong command = [%s]\n",argv[1]);
	UnionHelp();
	return(UnionTaskActionBeforeExit());
}

int StartClearTask(int argc,char *argv[], char *cmdPre)
{
	int	ret, i;
	int	interval;
	int	num = 0, errNum = 0;
	int	clearall = 0;
	char	grp[8][40+1];
	
	if (UnionCreateProcess() > 0)
		return(UnionTaskActionBeforeExit());
	
	if (argc <= 0)
		interval = 1;
	else
	{
		if ((interval = atoi(argv[0])) <= 0)
			interval = 1;
	}

	//if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"mngMsgBuf -clear %d",interval)) == NULL)
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s %d", cmdPre, interval)) == NULL)
	{
		UnionPrintf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}
	UnionLog("UnionCreateTaskInstance [%s %d] OK!\n", cmdPre, interval);

	memset(grp, 0, sizeof(grp));
	if(strstr(cmdPre, "clearall") != 0)
	{
		num = UnionLoadMsgBufNameGrp(grp);
		clearall = 1;
	}

	for (;;)
	{
		if(clearall)
		{
			errNum = 0;
			for(i = 0; i < num; i++)
			{
				UnionSetMDLNameOfMsgBuf(grp[i]);
				UnionLog("in StartClearTask:: UnionFreeRubbishMsg for [%s]!\n", grp[i]);
				if ((ret = UnionFreeRubbishMsg()) < 0)
				{
					errNum++;
					UnionUserErrLog("in StartClearTask:: UnionFreeRubbishMsg!\n");
					//return(UnionTaskActionBeforeExit());
					continue;
				}
			}
			if(errNum == num) // 所有队列都错
			{
				return(UnionTaskActionBeforeExit());
			}
		}
		else
		{
			if ((ret = UnionFreeRubbishMsg()) < 0)
			{
				UnionUserErrLog("in StartClearTask:: UnionFreeRubbishMsg!\n");
				return(UnionTaskActionBeforeExit());
			}
		}
		sleep(interval);
	}
}
