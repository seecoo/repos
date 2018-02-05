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
	char	bmkIndex[20];
	char	bmkValue[100],checkValue[100];
			
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
		if (UnionIsQuit((p = UnionInput("������BMK����(Exit�˳�)::"))))
			return(UnionTaskActionBeforeExit());
		strcpy(bmkValue,p);	
	}
	else
		strcpy(bmkValue,argv[3]);
			
	if (argc < 5)
	{
		if (UnionIsQuit((p = UnionInput("������BMKУ��ֵ(Exit�˳�)::"))))
			return(UnionTaskActionBeforeExit());
		strcpy(checkValue,p);	
	}
	else
		strcpy(checkValue,argv[4]);
			
	UnionStoreBMK(ipAddr,bmkIndex,bmkValue,checkValue);

	return(UnionTaskActionBeforeExit());
}

int UnionHelp()
{
	printf("Usage:: %s ipAddr bmkIndex bmkValue checkValue\n",UnionGetApplicationName());
	return(0);
}
	
int UnionStoreBMK(char *ipAddr,char *bmkIndex,char *bmkValue,char *checkValue)
{
	char		errCode[2+1],localCheckValue[16+1];
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
	if ((ret = SJL06Cmd2A(-1,&sjl06,bmkIndex,bmkValue,errCode)) < 0)
	{
		printf("�洢�����[%s]��BMK[%s]����! ������=[%d]\n",ipAddr,bmkIndex,ret);
		return(ret);
	}
	if (strncmp(errCode,"00",2) != 0)
	{
		printf("�洢�����[%s]��BMK[%s]����! �����������[%s]\n",ipAddr,bmkIndex,errCode);
		return(-1);
	}
	memset(errCode,0,sizeof(errCode));
	memset(localCheckValue,0,sizeof(localCheckValue));
	if (((ret = SJL06Cmd32(-1,&sjl06,bmkIndex,"000",localCheckValue,errCode)) < 0) || (strncmp(errCode,"00",2) != 0))
	{
		printf("���������[%s]��BMK[%s]��У��ֵ����! �����������[%s]\n",ipAddr,bmkIndex,errCode);
		//return(-1);
	}
	if (strncmp(localCheckValue,checkValue,4) != 0)
	{
		printf("�����[%s]��BMK[%s]����У��ֵ[%s]!=[%s]\n",ipAddr,bmkIndex,localCheckValue,checkValue);
		//return(-1);
	}
	printf("�洢�����[%s]��BMK[%s]=[%s]�ɹ�\n",ipAddr,bmkIndex,bmkValue);

	return(0);
}
