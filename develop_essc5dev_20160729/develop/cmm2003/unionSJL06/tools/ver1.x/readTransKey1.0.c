// Copyright:	Union Tech.
// Author:	Wolfgang Wang
// Date:	2003/09/28
// Version:	1.0

#define _UnionTask_3_x_

#include <stdio.h>
#include <string.h>

#include "sjl06.h"
#include "UnionTask.h"
#define _SJL06CmdForJK_IC_RSA_
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
	char	ipAddr[80];
	char	version[10],group[10],index[10];
			
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

loopInput:	
	memset(version,0,sizeof(version));
	if (argc < 3)
	{
loopInputVersion:
		if (UnionIsQuit((p = UnionInput("������汾��(Exit�˳�)::"))))
			return(UnionTaskActionBeforeExit());
		/*
		if ((atoi(p) < 0) || (atoi(p) > 2))
		{
			printf("�汾�Ŵ�!������!!\n");
			goto loopInputVersion;
		}
		*/
		sprintf(version,"%0d",atoi(p));
	}
	else
		sprintf(version,"%0d",atoi(argv[2]));
		
	memset(group,0,sizeof(group));
	if (argc < 4)
	{
loopInputGroup:
		if (UnionIsQuit((p = UnionInput("���������(Exit�˳�)::"))))
			return(UnionTaskActionBeforeExit());
		/*
		if ((atoi(p) < 1) || (atoi(p) > 32))
		{
			printf("��Ŵ�!������!!\n");
			goto loopInputGroup;
		}
		*/
		sprintf(group,"%02d",atoi(p));
	}
	else
		sprintf(group,"%02d",atoi(argv[3]));
	memset(index,0,sizeof(index));
	if (argc < 5)
	{
loopInputIndex:
		if (UnionIsQuit((p = UnionInput("������������(Exit�˳�)::"))))
			return(UnionTaskActionBeforeExit());
		/*
		if ((atoi(p) < 1) || (atoi(p) > 32))
		{
			printf("�����Ŵ�!������!!\n");
			goto loopInputIndex;
		}
		*/
		sprintf(index,"%02d",atoi(p));
	}
	else
		sprintf(index,"%02d",atoi(argv[4]));
	UnionReadTransKey(ipAddr,version,group,index);
	if (argc < 2)
	{
		printf("\n");
		goto loopInput;
	}
	return(UnionTaskActionBeforeExit());
}

int UnionHelp()
{
	printf("Usage:: %s ipAddr version ...\n",UnionGetApplicationName());
	return(0);
}
	
int UnionReadTransKey(char *ipAddr,char *version,char *group,char *index)
{
	char		transKeyValue[48+1];
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
			
	memset(errCode,0,sizeof(errCode));
	memset(transKeyValue,0,sizeof(transKeyValue));
	memset(checkValue,0,sizeof(checkValue));
	if ((ret = SJL06CmdE6(-1,&sjl06,version,group,index,transKeyValue,checkValue,errCode)) < 0)
	{
		printf("��ȡ�����[%s]�Ľ�������Կ[%s%s%s]����! ������=[%d]\n",ipAddr,version,group,index,ret);
		return(ret);
	}
	if (strncmp(errCode,"00",2) != 0)
	{
		printf("��ȡ�����[%s]�Ľ�������Կ[%s%s%s]����! �����������[%s]\n",ipAddr,version,group,index,errCode);
		return(-1);
	}
	printf("\n��ȡ�����[%s]������Կ�ɹ�::\n",ipAddr);
	printf("�汾[%s]���[%s]������[%s]\n",version,group,index);
	printf("��Կֵ[%s]\n",transKeyValue);
	printf("У��ֵ[%s]\n",checkValue);

	return(0);
}
