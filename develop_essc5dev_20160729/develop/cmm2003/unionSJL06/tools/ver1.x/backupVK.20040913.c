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
		if (UnionIsQuit((p = UnionInput("\n\n�����������IP��ַ(Exit�˳�)::"))))
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
loopInputVKIndex:
		if (UnionIsQuit((p = UnionInput("������˽Կ������(Exit�˳�)::"))))
			return(UnionTaskActionBeforeExit());
		if ((atoi(p) < 0) || (atoi(p) > 20))
		{
			printf("˽Կ�����Ŵ�!������!!\n");
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
		printf("�����IP��ַ[%s]����!\n\n",ipAddr);
		return(-1);
	}
	
	memset(&sjl06,0,sizeof(sjl06));
	strcpy(sjl06.staticAttr.ipAddr,ipAddr);
	if ((ret = UnionSelectSJL06Rec(&sjl06)) < 0)
	{
		printf("�������[%s]�������ļ�����!\n\n",ipAddr);
		return(ret);
	}
	sjl06.dynamicAttr.status = conOnlineSJL06;
			
	memset(errCode,0,sizeof(errCode));
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = SJL06Cmd36(-1,&sjl06,vkIndex,tmpBuf,sizeof(tmpBuf),errCode)) < 0)
	{
		printf("��ȡ�����[%s]��VK[%s]����! ������=[%d]\n\n",ipAddr,vkIndex,ret);
		return(ret);
	}
	if (strncmp(errCode,"00",2) != 0) 
	{
		printf("��ȡ�����[%s]��VK[%s]����! �����������[%s]\n\n",ipAddr,vkIndex,errCode);
		return(-1);
	}
	//printf("�����[%s]��VK[%s] = [%s]\n",ipAddr,vkIndex,tmpBuf);
	if ((vkLen = ret) == 0)
	{
		printf("�����[%s]��VK[%s]������!\n\n",ipAddr,vkIndex);
		return(-1);
	}
	memset(ascVK,0,sizeof(ascVK));
	bcdhex_to_aschex(tmpBuf,vkLen,ascVK);
		
	memset(errCode,0,sizeof(errCode));
	memset(tmpBuf,0,sizeof(tmpBuf));
	if ((ret = SJL06Cmd37(-1,&sjl06,'1',vkIndex,16,"0000000000000000",tmpBuf,sizeof(tmpBuf),errCode)) < 0)
	{
		printf("ʹ�������[%s]��VK[%s]ǩ������! ������=[%d]\n\n",ipAddr,vkIndex,ret);
		return(ret);
	}
	if (strncmp(errCode,"00",2) != 0)
	{
		printf("ʹ�������[%s]��VK[%s]ǩ������! �����������[%s]\n\n",ipAddr,vkIndex,errCode);
		return(-1);
	}
	bcdhex_to_aschex(tmpBuf,8,tmpBuf+8);
	tmpBuf[8+16] = 0;
	printf("�����[%s]��VK[%s]У����ֵ = [%s]\n",ipAddr,vkIndex,tmpBuf+8);

	memset(fileName,0,sizeof(fileName));
	sprintf(fileName,"%s/tmp/%s_%s.vk",getenv("HOME"),ipAddr,vkIndex);
	if ((fp = fopen(fileName,"w")) == NULL)
	{
		printf("���ļ�:%s����!\n\n",fileName);
		return(-1);
	}
	fprintf(fp,"%04d\n",vkLen);
	fprintf(fp,"%s\n",ascVK);
	fprintf(fp,"%s\n",tmpBuf+8);
	fflush(fp);
	fclose(fp);
	
	printf("��Կ�������ļ�[%s]��!\n\n",fileName);
	
	return(0);
}
