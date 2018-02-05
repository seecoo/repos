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
	char	vkBckFile[128+1];
			
	UnionSetApplicationName(argv[0]);
		
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,UnionGetApplicationName())) == NULL)
	{
		printf("in %s::UnionCreateTaskInstance Error!\n",UnionGetApplicationName());
		return(UnionTaskActionBeforeExit());
	}

	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(UnionTaskActionBeforeExit());
	
	memset(ipAddr,0,sizeof(ipAddr));
	memset(vkIndex,0,sizeof(vkIndex));
	memset(vkBckFile,0,sizeof(vkBckFile));
	
	for (index = 1; index < argc; index++)
	{
		if (UnionIsValidIPAddrStr(argv[index]))	// ������IP��ַ
		{
			strcpy(ipAddr,argv[index]);
			continue;
		}
		if (UnionIsDigitStr(argv[index]))	// ��������
		{
			strcpy(vkIndex,argv[index]);
			continue;
		}
				
	}
	if (strlen(ipAddr) == 0)	// û�ж��������IP��ַ
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
	
	if (strlen(vkIndex) == 0)	// û�ж���˽Կ������
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
		// ��˽Կ�����ļ�
		memset(vkBckFile,0,sizeof(vkBckFile));
		sprintf(vkBckFile,"%s_%s.vk",ipAddr,vkIndex);
		UnionRestoreVK(ipAddr,vkIndex,vkBckFile);
		goto loopInputVKIndex;
	}
	if (strlen(vkBckFile) == 0)	// û�ж���˽Կ�����ļ�
	{
loopInputVKBckFile:
		memset(vkBckFile,0,sizeof(vkBckFile));
		if (UnionIsQuit((p = UnionInput("\n\n������˽Կ�����ļ�����(Exit�˳�)::"))))
			return(UnionTaskActionBeforeExit());
		strcpy(ipAddr,p);
	}
	
	UnionRestoreVK(ipAddr,vkIndex,vkBckFile);

	return(UnionTaskActionBeforeExit());
}

int UnionHelp()
{
	printf("Usage:: %s hsmIPAddr vkIndex vkBackupFile\n",UnionGetApplicationName());
	return(0);
}
	
int UnionRestoreVK(char *ipAddr,char *vkIndex,char *vkBckFile)
{
	char		tmpBuf[4096+100+1];
	char		ascVK[8192+200+1];
	char		sign[512+1];
	char		errCode[2+1];
	int		vkLen;
	int		ret;
	TUnionSJL06	sjl06;
	FILE		*fp;
	char		fileName[512];
	
	memset(&sjl06,0,sizeof(sjl06));
	strcpy(sjl06.staticAttr.ipAddr,ipAddr);
	if ((ret = UnionSelectSJL06Rec(&sjl06)) < 0)
	{
		printf("�������[%s]�������ļ�����!\n\n",ipAddr);
		return(ret);
	}
	sjl06.dynamicAttr.status = conOnlineSJL06;
			
	memset(fileName,0,sizeof(fileName));
	strcpy(fileName,vkBckFile);
	if ((fp = fopen(fileName,"r")) == NULL)
	{
		sprintf(fileName,"%s/keyFile/%s",getenv("UNIONETC"),vkBckFile);
		if ((fp = fopen(fileName,"r")) == NULL)
		{
			sprintf(fileName,"%s/tmp/%s",getenv("HOME"),vkBckFile);
			if ((fp = fopen(fileName,"r")) == NULL)
			{
				printf("���ļ�:%s����!\n\n",vkBckFile);
				return(-1);
			}
		}
	}
	printf("vkBckFile=%s\n",fileName);
	fscanf(fp,"%d",&vkLen);
	memset(ascVK,0,sizeof(ascVK));
	fscanf(fp,"%s",ascVK);
	memset(sign,0,sizeof(sign));
	fscanf(fp,"%s",sign);
	fclose(fp);
	
	printf("Ҫ�ָ�����Կ:\n");
	switch (vkLen / 512)
	{
		case 0:
			printf("	512λ\n");
			break;
		case 1:
			printf("	1024λ\n");
			break;
		case 2:
			printf("	2048λ\n");
			break;
		case 3:
			printf("	4096λ\n");
			break;
		default:
			printf("	����λ��\n");
			break;
	}
	printf("	У��ֵ [%s]\n",sign);
	printf("�ָ���Կ������ԭ�е���Կ!!\n");
	printf("ȷ��Ҫ�������[%s]�лָ�VK[%s]��(Y/N)?",ipAddr,vkIndex);
	scanf("%s",tmpBuf);
	if ((tmpBuf[0] != 'Y') && (tmpBuf[0] != 'y'))
	{
		printf("\n\n");
		return(-1);
	}
		
	memset(errCode,0,sizeof(errCode));
	memset(tmpBuf,0,sizeof(tmpBuf));
	aschex_to_bcdhex(ascVK,vkLen * 2,tmpBuf);
	if ((ret = SJL06Cmd35(-1,&sjl06,vkIndex,tmpBuf,vkLen,errCode)) < 0)
	{
		printf("�洢�����[%s]��VK[%s]����! ������=[%d]\n\n",ipAddr,vkIndex,ret);
		return(ret);
	}
	if (strncmp(errCode,"00",2) != 0) 
	{
		printf("�洢�����[%s]��VK[%s]����! �����������[%s]\n\n",ipAddr,vkIndex,errCode);
		return(-1);
	}

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
	if (strncmp(tmpBuf+8,sign,16) != 0)
	{
		printf("���VK��У��ֵ����! �����ļ��е�ֵ [%s] != ���ɵ�ֵ [%s]\n\n",sign,tmpBuf+8);
		return(-1);
	}
	printf("�ָ������[%s]��VK[%s]�ɹ���У����ֵ = [%s]\n\n",ipAddr,vkIndex,tmpBuf+8);

	return(0);
}
