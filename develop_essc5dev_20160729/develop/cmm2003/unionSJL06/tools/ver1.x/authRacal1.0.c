// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2003/09/28
// Version:	1.0

#define _UnionTask_3_x_

#include <stdio.h>
#include <string.h>

#include "sjl06.h"
#include "UnionTask.h"
#include "sjl06Cmd.h"
#include "unionCommand.h"
#include "unionVersion.h"

PUnionTaskInstance	ptaskInstance = NULL;

int UnionTaskActionBeforeExit()
{
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	exit(0);
}

int main(int argc,char *argv[])
{
	int	ret;
	char	*p;
	int	index;
	char	ipAddr[80];
		
	UnionSetApplicationName(argv[0]);
	if (argc < 2)
		return(UnionHelp());
		
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"authRacal")) == NULL)
	{
		printf("in authRacal::UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}

	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(UnionTaskActionBeforeExit());
	
	if (argc < 2)
	{
		UnionHelp();
		return(UnionTaskActionBeforeExit());
	}
	if (argc < 3)
	{
loop:
		memset(ipAddr,0,sizeof(ipAddr));
		if (UnionIsQuit((p = UnionInput("Input Hsm IPAddr (exit/quit to exit)::"))))
			return(UnionTaskActionBeforeExit());
		if (!UnionIsValidIPAddrStr(p))
		{
			printf("Invalid IPAddr!\n");
			goto loop;
		}
		if (strcasecmp(argv[1],"APPLY") == 0)
			UnionAuthHsm(p);
		if (strcasecmp(argv[1],"FREE") == 0)
			UnionDisAuthHsm(p);
		goto loop;
	}
	
	for (index = 2; index < argc; index++)
	{
		if (strcasecmp(argv[1],"APPLY") == 0)
			UnionAuthHsm(argv[index]);
		if (strcasecmp(argv[1],"FREE") == 0)
			UnionDisAuthHsm(argv[index]);
	}
	return(UnionTaskActionBeforeExit());
}

int UnionHelp()
{
	printf("Usage:: %s apply|free ipAddr ...\n",UnionGetApplicationName());
	return(0);
}
	
int UnionAuthHsm(char *ipAddr)
{
	char		resBuf[100];
	int		len;
	int		ret;
	TUnionSJL06	sjl06;
	
	if (!UnionIsValidIPAddrStr(ipAddr))
	{
		printf("Invalid IPAddr [%s]\n",ipAddr);
		return(-1);
	}
	
	memset(&sjl06,0,sizeof(sjl06));
	strcpy(sjl06.staticAttr.ipAddr,ipAddr);
	if ((ret = UnionSelectSJL06Rec(&sjl06)) < 0)
	{
		printf("UnionSelectSJL06Rec [%s] Error\n",ipAddr);
		return(ret);
	}
	sjl06.dynamicAttr.status = conOnlineSJL06;
			
	memset(resBuf,0,sizeof(resBuf));
	if ((ret = RacalCmd0A(-1,&sjl06,resBuf)) < 0)
	{
		printf("get authority [%s] Error\n",ipAddr);
		return(ret);
	}
	
	if (strncmp(resBuf,"00",2) == 0)
	{
		printf("Authorize [%s] OK!\n",ipAddr);
		return(0);
	}
	else
	{
		printf("Authorize [%s] Failure! errCode = [%s]!\n",ipAddr,resBuf);
		return(-1);
	}
}

int UnionDisAuthHsm(char *ipAddr)
{
	char		resBuf[100];
	int		len;
	int		ret;
	TUnionSJL06	sjl06;
	
	if (!UnionIsValidIPAddrStr(ipAddr))
	{
		printf("Invalid IPAddr [%s]\n",ipAddr);
		return(-1);
	}
	
	memset(&sjl06,0,sizeof(sjl06));
	strcpy(sjl06.staticAttr.ipAddr,ipAddr);
	if ((ret = UnionSelectSJL06Rec(&sjl06)) < 0)
	{
		printf("UnionSelectSJL06Rec [%s] Error\n",ipAddr);
		return(ret);
	}
	sjl06.dynamicAttr.status = conOnlineSJL06;
			
	memset(resBuf,0,sizeof(resBuf));
	if ((ret = RacalCmdRA(-1,&sjl06,resBuf)) < 0)
	{
		printf("get authority [%s] Error\n",ipAddr);
		return(ret);
	}
	
	if (strncmp(resBuf,"00",2) == 0)
	{
		printf("Disauthorize [%s] OK!\n",ipAddr);
		return(0);
	}
	else
	{
		printf("Disauthorize [%s] Failure! errCode = [%s]!\n",ipAddr,resBuf);
		return(-1);
	}
}
