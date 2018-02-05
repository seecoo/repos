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
	char	firstIPAddr[80];
	char	secondIPAddr[80];
	char	startBmkIndex[20];
	int	bmkNum = 1;
			
	UnionSetApplicationName(argv[0]);
		
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,UnionGetApplicationName())) == NULL)
	{
		printf("in %s::UnionCreateTaskInstance Error!\n",UnionGetApplicationName());
		return(UnionTaskActionBeforeExit());
	}

	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(UnionTaskActionBeforeExit());
	
loop1:
	if (argc > 1)
		strcpy(firstIPAddr,argv[1]);
	else
	{
loopInputFirstIPAddr:
		memset(firstIPAddr,0,sizeof(firstIPAddr));
		if (UnionIsQuit((p = UnionInput("请输入主密码机IP地址(Exit退出)::"))))
			return(UnionTaskActionBeforeExit());
		if (!UnionIsValidIPAddrStr(p))
		{
			printf("IP地址错误!请重输!!\n");
			goto loopInputFirstIPAddr;
		}
		strcpy(firstIPAddr,p);
	}

loop2:
	if (argc > 2)
		strcpy(secondIPAddr,argv[2]);
	else
	{
loopInputSecondIPAddr:
		memset(secondIPAddr,0,sizeof(secondIPAddr));
		if (UnionIsQuit((p = UnionInput("请输入从密码机IP地址(Exit退出)::"))))
		{
			if (argc > 1)
				return(UnionTaskActionBeforeExit());
			goto loop1;
		}
		if (!UnionIsValidIPAddrStr(p))
		{
			printf("IP地址错误!请重输!!\n");
			goto loopInputFirstIPAddr;
		}
		strcpy(secondIPAddr,p);
	}

loop3:
	if (argc > 3)
		strcpy(startBmkIndex,argv[3]);
	else
	{
loopInputBMKIndex:
		if (UnionIsQuit((p = UnionInput("请输入起始BMK索引号(Exit退出)::"))))
		{
			if (argc > 1)
				return(UnionTaskActionBeforeExit());
			goto loop2;
		}
		strcpy(startBmkIndex,p);
	}
	UnionToUpperCase(startBmkIndex);

loop4:
	if (argc > 4)
		bmkNum = atoi(argv[4]);
	else
	{
loopInputBMKNum:
		if (strcmp(startBmkIndex,"ALL") != 0)
		{
			if (UnionIsQuit((p = UnionInput("请输入BMK数目(Exit退出)::"))))
			{
				if (argc > 1)
					return(UnionTaskActionBeforeExit());
				goto loop3;
			}
			bmkNum = atoi(p);
		}
	}
			
	UnionSynchronizeBmk(firstIPAddr,secondIPAddr,startBmkIndex,bmkNum);
	
	switch (argc)
	{
		case	1:
			goto loop1;
		case	2:
			goto loop2;
		case	3:
			goto loop3;
		case	4:
			if (strcmp(startBmkIndex,"ALL") == 0)
				break;
			goto loop4;
		default:
			break;
	}
	return(UnionTaskActionBeforeExit());
}

int UnionHelp()
{
	printf("Usage:: %s firstIPAddr secondeIPAddr startBmkIndex bmkNum\n",UnionGetApplicationName());
	return(0);
}
	
int UnionSynchronizeBmk(char *firstIPAddr,char *secondIPAddr,char *startBmkIndex,int bmkNum)
{
	char		errCode[2+1];
	int		len;
	int		ret;
	TUnionSJL06	firstSJL06,secondSJL06;
	int		index,firstIndex,maxIndex;
	int		maxNum,realNum;
	char		value[48+1];
	char		bmkIndex[10];
	
	memset(&firstSJL06,0,sizeof(firstSJL06));
	strcpy(firstSJL06.staticAttr.ipAddr,firstIPAddr);
	if ((ret = UnionSelectSJL06Rec(&firstSJL06)) < 0)
	{
		printf("读密码机[%s]的配置文件出错!\n",firstIPAddr);
		return(ret);
	}
	firstSJL06.dynamicAttr.status = conOnlineSJL06;
	memset(&secondSJL06,0,sizeof(secondSJL06));
	strcpy(secondSJL06.staticAttr.ipAddr,firstIPAddr);
	if ((ret = UnionSelectSJL06Rec(&secondSJL06)) < 0)
	{
		printf("读密码机[%s]的配置文件出错!\n",firstIPAddr);
		return(ret);
	}
	secondSJL06.dynamicAttr.status = conOnlineSJL06;
	
	if (strcmp(startBmkIndex,"ALL") == 0)
	{
		firstIndex = 0;
		maxIndex = 511;
	}
	else
	{
		firstIndex = atoi(startBmkIndex);
		maxIndex = firstIndex + bmkNum - 1;
	}
	if (!UnionConfirm("将密码机[%s]的BMK[%03d]~[%03d]同步到密码机[%s]中吗",firstIPAddr,firstIndex,maxIndex,secondIPAddr))
		return(0);
		
	for (index = firstIndex; index <= maxIndex; index++)
	{	
		memset(errCode,0,sizeof(errCode));
		memset(value,0,sizeof(value));
		sprintf(bmkIndex,"%03d",index);
		if ((ret = SJL06Cmd31(-1,&firstSJL06,bmkIndex,value,errCode)) < 0)
		{
			printf("读取密码机[%s]的BMK[%s]出错! 错误码=[%d]\n",firstIPAddr,bmkIndex,ret);
			continue;
		}
		if (strncmp(errCode,"00",2) != 0)
		{
			if (strncmp(errCode,"91",2) == 0)
				continue;
			printf("读取密码机[%s]的BMK[%s]出错! 密码机错误码[%s]\n",firstIPAddr,bmkIndex,errCode);
			//return(-1);
			continue;
		}
		if ((ret = SJL06Cmd2A(-1,&secondSJL06,bmkIndex,value,errCode)) < 0)
		{
			printf("同步[%s]至密码机[%s]出错! 错误码=[%d]\n",bmkIndex,secondIPAddr,ret);
			continue;
		}
		if (strncmp(errCode,"00",2) != 0)
		{
			printf("同步密码机[%s]的BMK[%s]出错! 密码机错误码[%s]\n",secondIPAddr,bmkIndex,errCode);
			continue;
		}
		printf("同步[%s]至密码机[%s]成功!\n",bmkIndex,secondIPAddr);
		realNum++;
	}
	printf("同步[%03d]个BMK至密码机[%s]\n",realNum,secondIPAddr);

	return(realNum);
}
