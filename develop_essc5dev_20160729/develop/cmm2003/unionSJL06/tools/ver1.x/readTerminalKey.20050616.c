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
	char	terminalKeyIndex[20],bmkIndex[20];
			
	UnionSetApplicationName(argv[0]);
		
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,UnionGetApplicationName())) == NULL)
	{
		printf("in %s::UnionCreateTaskInstance Error!\n",UnionGetApplicationName());
		return(UnionTaskActionBeforeExit());
	}

	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(UnionTaskActionBeforeExit());
	
	if (argc < 2)
	{
loopInputIPAddr:
		memset(ipAddr,0,sizeof(ipAddr));
		if (UnionIsQuit((p = UnionInput("�����������IP��ַ(Exit�˳�)::"))))
			return(UnionTaskActionBeforeExit());
		if (!UnionIsValidIPAddrStr(p))
		{
			printf("IP��ַ����!������!!\n");
			goto loopInputIPAddr;
		}
		strcpy(ipAddr,p);
	}
	else
		strcpy(ipAddr,argv[1]);
	
	if (argc < 3)
	{
loopInputBMKIndex:
		if (UnionIsQuit((p = UnionInput("������BMK������(Exit�˳�)::"))))
			return(UnionTaskActionBeforeExit());
		memset(bmkIndex,0,sizeof(bmkIndex));
		sprintf(bmkIndex,"%03d",atoi(p));
	}
	else
		strcpy(bmkIndex,argv[2]);
	
	if (argc < 4)
	{
loopInputTerminalKeyIndex:
		if (UnionIsQuit((p = UnionInput("������TMK������(Exit�˳�)::"))))
			return(UnionTaskActionBeforeExit());
		memset(terminalKeyIndex,0,sizeof(terminalKeyIndex));
		sprintf(terminalKeyIndex,"%03d",atoi(p));
		UnionReadTerminalKey(ipAddr,bmkIndex,terminalKeyIndex);
		goto loopInputTerminalKeyIndex;
	}
	
	for (index = 3; index < argc; index++)
	{
		memset(terminalKeyIndex,0,sizeof(terminalKeyIndex));
		sprintf(terminalKeyIndex,"%03d",atoi(argv[index]));
		UnionReadTerminalKey(ipAddr,bmkIndex,terminalKeyIndex);
	}
	return(UnionTaskActionBeforeExit());
}

int UnionHelp()
{
	printf("Usage:: %s ipAddr bmkIndex terminalKeyIndex ...\n",UnionGetApplicationName());
	return(0);
}
	
int UnionReadTerminalKey(char *ipAddr,char *bmkIndex,char *terminalKeyIndex)
{
	char		terminalKeyValue[48+1];
	char		checkValue[16+1];
	char		errCode[2+1];
	int		len;
	int		ret;
	TUnionSJL06	sjl06;
	
	if (!UnionIsValidIPAddrStr(ipAddr))
	{
		printf("�����IP��ַ[%s]����!\n",ipAddr);
		return(-1);
	}
	
	memset(&sjl06,0,sizeof(sjl06));
	strcpy(sjl06.staticAttr.ipAddr,ipAddr);
	if ((ret = UnionSelectSJL06Rec(&sjl06)) < 0)
	{
		printf("�������[%s]�������ļ�����!\n",ipAddr);
		return(ret);
	}
	sjl06.dynamicAttr.status = conOnlineSJL06;
			
	memset(errCode,0,sizeof(errCode));
	memset(terminalKeyValue,0,sizeof(terminalKeyValue));
	if ((ret = SJL06Cmd32(-1,&sjl06,bmkIndex,terminalKeyIndex,terminalKeyValue,errCode)) < 0)
	{
		printf("��ȡ�����[%s]��TerminalKey[%s]����! ������=[%d]\n",ipAddr,terminalKeyIndex,ret);
		return(ret);
	}
	if (strncmp(errCode,"00",2) != 0)
	{
		printf("��ȡ�����[%s]��TerminalKey[%s]����! �����������[%s]\n",ipAddr,terminalKeyIndex,errCode);
		return(-1);
	}
	memset(errCode,0,sizeof(errCode));
	if ((ret = SJL06Cmd15(-1,&sjl06,bmkIndex,terminalKeyValue,terminalKeyValue,errCode)) < 0)
	{
		printf("�������[%s]��TerminalKey[%s]������żУ�����! ������=[%d]\n",ipAddr,terminalKeyIndex,ret);
		return(ret);
	}
	if (strncmp(errCode,"00",2) != 0)
	{
		printf("�������[%s]��TerminalKey[%s]������żУ�����! �����������[%s]\n",ipAddr,terminalKeyIndex,errCode);
		return(-1);
	}
	memset(errCode,0,sizeof(errCode));
	memset(checkValue,0,sizeof(checkValue));
	if ((ret = SJL06Cmd13(-1,&sjl06,bmkIndex,terminalKeyValue,checkValue,errCode)) < 0)
	{
		printf("���������[%s]��TerminalKey[%s]��У��ֵ����! ������=[%d]\n",ipAddr,terminalKeyIndex,ret);
		return(ret);
	}
	if (strncmp(errCode,"00",2) != 0)
	{
		printf("���������[%s]��TerminalKey[%s]��У��ֵ����! �����������[%s]\n",ipAddr,terminalKeyIndex,errCode);
		return(-1);
	}
	printf("[%s] [%s] [%s] [%s] [%s]\n",ipAddr,bmkIndex,terminalKeyIndex,terminalKeyValue,checkValue);

	return(0);
}
