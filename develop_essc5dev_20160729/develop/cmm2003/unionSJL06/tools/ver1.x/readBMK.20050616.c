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
		if (UnionIsQuit((p = UnionInput("请输入密码机IP地址(Exit退出)::"))))
			return(UnionTaskActionBeforeExit());
		if (!UnionIsValidIPAddrStr(p))
		{
			printf("IP地址错误!请重输!!\n");
			goto loopInputIPAddr;
		}
		strcpy(ipAddr,p);
	}
	else
		strcpy(ipAddr,argv[1]);
	
	if (argc < 3)
	{
loopInputBMKIndex:
		if (UnionIsQuit((p = UnionInput("请输入BMK索引号(Exit退出)::"))))
			return(UnionTaskActionBeforeExit());
		if ((atoi(p) < 0) || (atoi(p) > 512))
		{
			printf("BMK索引号错!请重输!!\n");
			goto loopInputBMKIndex;
		}
		memset(bmkIndex,0,sizeof(bmkIndex));
		sprintf(bmkIndex,"%03d",atoi(p));
		UnionReadBMK(ipAddr,bmkIndex);
		goto loopInputBMKIndex;
	}
	
	for (index = 2; index < argc; index++)
	{
		memset(bmkIndex,0,sizeof(bmkIndex));
		sprintf(bmkIndex,"%03d",atoi(argv[index]));
		UnionReadBMK(ipAddr,bmkIndex);
	}
	return(UnionTaskActionBeforeExit());
}

int UnionHelp()
{
	printf("Usage:: %s ipAddr bmkIndex ...\n",UnionGetApplicationName());
	return(0);
}
	
int UnionReadBMK(char *ipAddr,char *bmkIndex)
{
	char		bmkValue[48+1],checkValue[16+1];
	char		errCode[2+1];
	int		len;
	int		ret;
	TUnionSJL06	sjl06;
	
	if (!UnionIsValidIPAddrStr(ipAddr))
	{
		printf("密码机IP地址[%s]错误!\n",ipAddr);
		return(-1);
	}
	
	memset(&sjl06,0,sizeof(sjl06));
	strcpy(sjl06.staticAttr.ipAddr,ipAddr);
	if ((ret = UnionSelectSJL06Rec(&sjl06)) < 0)
	{
		printf("读密码机[%s]的配置文件出错!\n",ipAddr);
		return(ret);
	}
	sjl06.dynamicAttr.status = conOnlineSJL06;
			
	memset(errCode,0,sizeof(errCode));
	memset(bmkValue,0,sizeof(bmkValue));
	if ((ret = SJL06Cmd31(-1,&sjl06,bmkIndex,bmkValue,errCode)) < 0)
	{
		printf("读取密码机[%s]的BMK[%s]出错! 错误码=[%d]\n",ipAddr,bmkIndex,ret);
		return(ret);
	}
	if (strncmp(errCode,"00",2) != 0)
	{
		printf("读取密码机[%s]的BMK[%s]出错! 密码机错误码[%s]\n",ipAddr,bmkIndex,errCode);
		return(-1);
	}

	memset(errCode,0,sizeof(errCode));
	memset(checkValue,0,sizeof(checkValue));
	if (((ret = SJL06Cmd32(-1,&sjl06,bmkIndex,"000",checkValue,errCode)) < 0) || (strncmp(errCode,"00",2) != 0))
	{
		printf("生成密码机[%s]的BMK[%s]的校验值出错! 密码机错误码[%s]\n",ipAddr,bmkIndex,errCode);
		//return(-1);
	}
	printf("[%s] [%s] [%s] [%s]\n",ipAddr,bmkIndex,bmkValue,checkValue);

	return(0);
}
