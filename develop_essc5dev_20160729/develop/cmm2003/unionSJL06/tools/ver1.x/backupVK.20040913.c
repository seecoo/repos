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
	char	vkIndex[20];
			
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
		if (UnionIsQuit((p = UnionInput("\n\n请输入密码机IP地址(Exit退出)::"))))
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
loopInputVKIndex:
		if (UnionIsQuit((p = UnionInput("请输入私钥索引号(Exit退出)::"))))
			return(UnionTaskActionBeforeExit());
		if ((atoi(p) < 0) || (atoi(p) > 20))
		{
			printf("私钥索引号错!请重输!!\n");
			goto loopInputVKIndex;
		}
		memset(vkIndex,0,sizeof(vkIndex));
		sprintf(vkIndex,"%02d",atoi(p));
		UnionReadVK(ipAddr,vkIndex);
		goto loopInputVKIndex;
	}
	
	for (index = 2; index < argc; index++)
	{
		memset(vkIndex,0,sizeof(vkIndex));
		sprintf(vkIndex,"%02d",atoi(argv[index]));
		UnionReadVK(ipAddr,vkIndex);
	}
	return(UnionTaskActionBeforeExit());
}

int UnionHelp()
{
	printf("Usage:: %s ipAddr vkIndex ...\n",UnionGetApplicationName());
	return(0);
}
	
int UnionReadVK(char *ipAddr,char *vkIndex)
{
	char		tmpBuf[4096+100+1];
	char		ascVK[8192+200+1];
	char		errCode[2+1];
	int		vkLen;
	int		ret;
	TUnionSJL06	sjl06;
	FILE		*fp;
	char		fileName[512];
	
	if (!UnionIsValidIPAddrStr(ipAddr))
	{
		printf("密码机IP地址[%s]错误!\n\n",ipAddr);
		return(-1);
	}
	
	memset(&sjl06,0,sizeof(sjl06));
	strcpy(sjl06.staticAttr.ipAddr,ipAddr);
	if ((ret = UnionSelectSJL06Rec(&sjl06)) < 0)
	{
		printf("读密码机[%s]的配置文件出错!\n\n",ipAddr);
		return(ret);
	}
	sjl06.dynamicAttr.status = conOnlineSJL06;
			
	memset(errCode,0,sizeof(errCode));
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = SJL06Cmd36(-1,&sjl06,vkIndex,tmpBuf,sizeof(tmpBuf),errCode)) < 0)
	{
		printf("读取密码机[%s]的VK[%s]出错! 错误码=[%d]\n\n",ipAddr,vkIndex,ret);
		return(ret);
	}
	if (strncmp(errCode,"00",2) != 0) 
	{
		printf("读取密码机[%s]的VK[%s]出错! 密码机错误码[%s]\n\n",ipAddr,vkIndex,errCode);
		return(-1);
	}
	//printf("密码机[%s]的VK[%s] = [%s]\n",ipAddr,vkIndex,tmpBuf);
	if ((vkLen = ret) == 0)
	{
		printf("密码机[%s]的VK[%s]不存在!\n\n",ipAddr,vkIndex);
		return(-1);
	}
	memset(ascVK,0,sizeof(ascVK));
	bcdhex_to_aschex(tmpBuf,vkLen,ascVK);
		
	memset(errCode,0,sizeof(errCode));
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = SJL06Cmd37(-1,&sjl06,'1',vkIndex,16,"0000000000000000",tmpBuf,sizeof(tmpBuf),errCode)) < 0)
	{
		printf("使用密码机[%s]的VK[%s]签名出错! 错误码=[%d]\n\n",ipAddr,vkIndex,ret);
		return(ret);
	}
	if (strncmp(errCode,"00",2) != 0)
	{
		printf("使用密码机[%s]的VK[%s]签名出错! 密码机错误码[%s]\n\n",ipAddr,vkIndex,errCode);
		return(-1);
	}
	bcdhex_to_aschex(tmpBuf,8,tmpBuf+8);
	tmpBuf[8+16] = 0;
	printf("密码机[%s]的VK[%s]校验码值 = [%s]\n",ipAddr,vkIndex,tmpBuf+8);

	memset(fileName,0,sizeof(fileName));
	sprintf(fileName,"%s/tmp/%s_%s.vk",getenv("HOME"),ipAddr,vkIndex);
	if ((fp = fopen(fileName,"w")) == NULL)
	{
		printf("打开文件:%s出错!\n\n",fileName);
		return(-1);
	}
	fprintf(fp,"%04d\n",vkLen);
	fprintf(fp,"%s\n",ascVK);
	fprintf(fp,"%s\n",tmpBuf+8);
	fflush(fp);
	fclose(fp);
	
	printf("密钥备份在文件[%s]中!\n\n",fileName);
	
	return(0);
}
