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
	char	startVKIndex[20];
	int	vkNum = 1;
			
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
		if (UnionIsQuit((p = UnionInput("�������������IP��ַ(Exit�˳�)::"))))
			return(UnionTaskActionBeforeExit());
		if (!UnionIsValidIPAddrStr(p))
		{
			printf("IP��ַ����!������!!\n");
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
		if (UnionIsQuit((p = UnionInput("������������IP��ַ(Exit�˳�)::"))))
		{
			if (argc > 1)
				return(UnionTaskActionBeforeExit());
			goto loop1;
		}
		if (!UnionIsValidIPAddrStr(p))
		{
			printf("IP��ַ����!������!!\n");
			goto loopInputFirstIPAddr;
		}
		strcpy(secondIPAddr,p);
	}

loop3:
	if (argc > 3)
		strcpy(startVKIndex,argv[3]);
	else
	{
loopInputVKIndex:
		if (UnionIsQuit((p = UnionInput("��������ʼ˽Կ������(Exit�˳�)::"))))
		{
			if (argc > 1)
				return(UnionTaskActionBeforeExit());
			goto loop2;
		}
		strcpy(startVKIndex,p);
	}
	UnionToUpperCase(startVKIndex);

loop4:
	if (argc > 4)
		vkNum = atoi(argv[4]);
	else
	{
loopInputVKNum:
		if (strcmp(startVKIndex,"ALL") != 0)
		{
			if (UnionIsQuit((p = UnionInput("������˽Կ��Ŀ(Exit�˳�)::"))))
			{
				if (argc > 1)
					return(UnionTaskActionBeforeExit());
				goto loop3;
			}
			vkNum = atoi(p);
		}
	}
			
	UnionSynchronizeVK(firstIPAddr,secondIPAddr,startVKIndex,vkNum);
	
	switch (argc)
	{
		case	1:
			goto loop1;
		case	2:
			goto loop2;
		case	3:
			goto loop3;
		case	4:
			if (strcmp(startVKIndex,"ALL") == 0)
				break;
			goto loop4;
		default:
			break;
	}
	return(UnionTaskActionBeforeExit());
}

int UnionHelp()
{
	printf("Usage:: %s firstIPAddr secondeIPAddr startVKIndex vkNum\n",UnionGetApplicationName());
	return(0);
}
	
int UnionSynchronizeVK(char *firstIPAddr,char *secondIPAddr,char *startVKIndex,int vkNum)
{
	char		errCode[2+1];
	int		len;
	int		ret;
	TUnionSJL06	firstSJL06,secondSJL06;
	int		index,firstIndex,maxIndex;
	int		maxNum,realNum;
	char		value[4096+1];
	char		vkIndex[10];
	
	memset(&firstSJL06,0,sizeof(firstSJL06));
	strcpy(firstSJL06.staticAttr.ipAddr,firstIPAddr);
	if ((ret = UnionSelectSJL06Rec(&firstSJL06)) < 0)
	{
		printf("�������[%s]�������ļ�����!\n",firstIPAddr);
		return(ret);
	}
	firstSJL06.dynamicAttr.status = conOnlineSJL06;
	memset(&secondSJL06,0,sizeof(secondSJL06));
	strcpy(secondSJL06.staticAttr.ipAddr,secondIPAddr);
	if ((ret = UnionSelectSJL06Rec(&secondSJL06)) < 0)
	{
		printf("�������[%s]�������ļ�����!\n",firstIPAddr);
		return(ret);
	}
	secondSJL06.dynamicAttr.status = conOnlineSJL06;
	
	if (strcmp(startVKIndex,"ALL") == 0)
	{
		firstIndex = 0;
		maxIndex = 20;
	}
	else
	{
		firstIndex = atoi(startVKIndex);
		maxIndex = firstIndex + vkNum - 1;
	}
	if (!UnionConfirm("�������[%s]��˽Կ[%03d]~[%03d]ͬ���������[%s]����",firstIPAddr,firstIndex,maxIndex,secondIPAddr))
		return(0);
		
	for (index = firstIndex; index <= maxIndex; index++)
	{	
		memset(errCode,0,sizeof(errCode));
		memset(value,0,sizeof(value));
		sprintf(vkIndex,"%02d",index);
		if ((ret = SJL06Cmd36(-1,&firstSJL06,vkIndex,value,sizeof(value),errCode)) < 0)
		{
			printf("��ȡ�����[%s]��˽Կ[%s]����! ������=[%d]\n\n",firstIPAddr,vkIndex,ret);
			continue;
		}
		if (ret == 0)
			continue;
		if (strncmp(errCode,"00",2) != 0)
		{
			if (strncmp(errCode,"91",2) == 0)
				continue;
			printf("��ȡ�����[%s]��˽Կ[%s]����! �����������[%s]\n",firstIPAddr,vkIndex,errCode);
			//return(-1);
			continue;
		}
		if ((ret = SJL06Cmd35(-1,&secondSJL06,vkIndex,value,ret,errCode)) < 0)
		{
			printf("ͬ��[%s]�������[%s]����! ������=[%d]\n",vkIndex,secondIPAddr,ret);
			continue;
		}
		if (strncmp(errCode,"00",2) != 0)
		{
			printf("ͬ�������[%s]��˽Կ[%s]����! �����������[%s]\n",secondIPAddr,vkIndex,errCode);
			continue;
		}
		printf("ͬ��[%s]�������[%s]�ɹ�!\n",vkIndex,secondIPAddr);
		realNum++;
	}
	printf("ͬ��[%03d]��˽Կ�������[%s]\n",realNum,secondIPAddr);

	return(realNum);
}
