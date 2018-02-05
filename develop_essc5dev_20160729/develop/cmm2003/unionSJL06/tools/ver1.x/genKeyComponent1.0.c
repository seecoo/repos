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
char			pgPrintFormat[] = ">L>L>L>L>L>030^1>L>030^2>L>030^3>L>023 第^0个成分>L>023 ^P>L>023 ^4>L>023 ^5>L>023 ^6>L>023 ^7>L>023 ^8>L>023 ^9>L";

int UnionTaskActionBeforeExit()
{
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	exit(0);
}

int main(int argc,char *argv[])
{
	int	ret;
	char	ipAddr[80];
	char	type[10];
	int	length;
	char	format[10][80];
	int	componentNum;
	int	formatNum,formatIndex;
	char	*p;
	
	UnionSetApplicationName(argv[0]);
	//if (argc < 2)
	//	return(UnionHelp());
		
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"genKeyComponent")) == NULL)
	{
		printf("in genKeyComponent::UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}

	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(UnionTaskActionBeforeExit());

	memset(ipAddr,0,sizeof(ipAddr));
	if (argc >= 2)
		strcpy(ipAddr,argv[1]);
inputIPAddr:
	if (!UnionIsValidIPAddrStr(ipAddr))
	{
		if (strlen(ipAddr) != 0)
			printf("Waringing!! Invalid IPAddr [%s]!\n",ipAddr);
		if (UnionIsQuit(p = UnionInput("Input Hsm IPAddr::")))
			return(UnionTaskActionBeforeExit());
		memset(ipAddr,0,sizeof(ipAddr));
		strcpy(ipAddr,p);
		goto inputIPAddr;
	}
	
	memset(type,0,sizeof(type));
	if (argc >= 3)
		strcpy(type,argv[2]);
inputType:
	if (UnionConvertDesKeyType(type) < 0)
	{
		if (strlen(type) != 0)
			printf("Waringing!! Invalid key type [%s]!\n",type);
		if (UnionIsQuit(p = UnionInput("Input Key Type::")))
			return(UnionTaskActionBeforeExit());
		strcpy(type,p);
		goto inputType;
	}
	
	length = -100;
	if (argc >= 4)
		length = atoi(argv[3]);
inputLength:
	if (UnionConvertDesKeyLength(length) < 0)
	{
		if (length != -100)
			printf("Waringing!! Invalid key length [%d]!\n",length);
		if (UnionIsQuit(p = UnionInput("Input Key Length::")))
			return(UnionTaskActionBeforeExit());
		length = atoi(p);
		goto inputLength;
	}
	
	componentNum = -100;
	if (argc >= 5)
		componentNum = atoi(argv[4]);
inputComponentNum:
	if (componentNum <= 0 || componentNum > 9)
	{
		if (componentNum != -100)
			printf("Waringing!! Invalid key component num [%d]!\n",componentNum);
		if (UnionIsQuit(p = UnionInput("Input Key Component Num::")))
			return(UnionTaskActionBeforeExit());
		componentNum = atoi(p);
		goto inputComponentNum;
	}

	formatNum = argc - 5;
	for (formatIndex = 0; formatIndex < formatNum && formatIndex < 10; formatIndex++)
	{
		memset(format[formatIndex],0,sizeof(format[formatIndex]));
		strcpy(format[formatIndex],argv[5+formatIndex]);
	}
	if (formatNum <= 0)
	{
		for (formatIndex = 0,formatNum = 0;formatNum < 10;formatIndex++)
		{
			p = UnionInput("Input Key Description%02d (quit to finish input)::\n",formatIndex);
			memset(format[formatIndex],0,sizeof(format[formatIndex]));
			strcpy(format[formatIndex],p);
			if (UnionIsQuit(p))
				break;
			formatNum++;
		}
	}
	return(UnionGenerateKeyComponent(ipAddr,type,length,componentNum,formatNum,format));
}

int UnionHelp()
{
	printf("Usage:: %s hsmIPAddr keyName keyLength number application owner keyName\n",UnionGetApplicationName());
	return(0);
}
	
int UnionGenerateKeyComponent(char *ipAddr,char *type,int length,int componentNum,int argc,char argv[][80])
{
	char		resBuf[100];
	int		len;
	int		ret;
	TUnionSJL06	sjl06;
	int		i;
	char		fld[10][80];
	
	printf("Hsm IPAddr         [%s]\n",ipAddr);
	printf("Key Type           [%s]\n",type);
	printf("Key Length         [%d]\n",length);
	printf("Component Num      [%d]\n",componentNum);
	printf("Description Num    [%d]\n",argc);
	for (i = 0; i < argc; i++)
		printf("Description %02d     [%s]\n",i,argv[i]);
	if (!UnionConfirm("Are you sure of generate key components?"))
		return(UnionTaskActionBeforeExit());

	memset(&sjl06,0,sizeof(sjl06));
	strcpy(sjl06.staticAttr.ipAddr,ipAddr);
	if ((ret = UnionSelectSJL06Rec(&sjl06)) < 0)
	{
		printf("UnionSelectSJL06Rec [%s] Error\n",ipAddr);
		return(UnionTaskActionBeforeExit());
	}
	sjl06.dynamicAttr.status = conOnlineSJL06;
	
	memset(resBuf,0,sizeof(resBuf));
	if ((ret = RacalCmdPA(-1,&sjl06,pgPrintFormat,resBuf)) < 0)
	{
		printf("RacalCmdPA [%s] Error\n",ipAddr);
		return(UnionTaskActionBeforeExit());
	}
	
	if (strncmp(resBuf,"00",2) == 0)
		printf("Set Print Format for [%s] OK!\n",ipAddr);
	else
	{
		printf("Set Print Format for [%s] OK Failure! errCode = [%s]!\n",ipAddr,resBuf);
		return(UnionTaskActionBeforeExit());
	}
	
	for (i = 1; (i <= argc) && (i < 10); i++)
	{
		memset(fld[i],0,sizeof(fld[i]));
		strcpy(fld[i],argv[i-1]);
		//printf("fld [%d] = [%s]\n",i,fld[i]);
	}
			
	for (i = 0; i < componentNum; i++)
	{
		memset(resBuf,0,sizeof(resBuf));
		memset(fld[0],0,sizeof(fld[0]));
		sprintf(fld[0],"%02d",i+1);
		if ((ret = RacalCmdA2(-1,&sjl06,UnionConvertDesKeyType(type),UnionConvertDesKeyLength(length),argc,fld,resBuf,resBuf+60)) < 0)
		{
			printf("RacalCmdA2 [%s] Error\n",ipAddr);
			return(UnionTaskActionBeforeExit());
		}
		if (strncmp(resBuf+60,"00",2) == 0)
			printf("Generate [%0d]th OK!\n",i+1);
		else
		{
			printf("Generate [%0d]th Error! errCode = [%s]\n",i+1,resBuf+60);
			return(UnionTaskActionBeforeExit());
		}
	}
	printf("Generate OK!\n");
	return(UnionTaskActionBeforeExit());
}
