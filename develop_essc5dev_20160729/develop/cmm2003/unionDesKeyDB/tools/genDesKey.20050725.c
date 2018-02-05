//	Wolfgang Wang
//	2003/09/09

#define _UnionTask_3_x_
#define _UnionDesKeyDB_2_x_
#define _UnionLogMDL_3_x_

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <time.h>

#include "UnionLog.h"
#include "unionCommand.h"
#include "unionVersion.h"
#include "UnionTask.h"
#include "unionModule.h"

#include "sjl06.h"

#include "unionDesKeyDB.h"


TUnionDesKey		gdesKey;
int			gcomponentNum = 2;
TUnionSJL06		gsjl06;
char			gparam[20][80];
char			gkeyName[128];

PUnionTaskInstance	ptaskInstance = NULL;

int UnionTaskActionBeforeExit()
{
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	UnionDisconnectDesKeyDB();
	exit(0);	//return(exit(0));
}

int UnionHelp()
{
	printf("Usage:: %s\n",UnionGetApplicationName());
	printf(" this executable is used to generate keys and print key components.");

	EditHelp();
	
	return(0);
}

int EditHelp()
{
	printf(" edit commands\n");
	printf("	input directly hsm ipAddr/fullKeyName/keyComponentNum\n");
	printf("	type set key type\n");
	printf("	length set key length\n");
	printf("	comnum set key component num\n");
	printf("	generate batch generate key values\n");
	printf("	par set parameter 1 ~ 9\n");
	return(0);
}

int main(int argc,char **argv)
{
	int	ret;
	
	UnionSetApplicationName(argv[0]);
		
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s",UnionGetApplicationName())) == NULL)
	{
		printf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}

	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(UnionTaskActionBeforeExit());
		
	if ((ret = UnionConnectDesKeyDB()) < 0)
	{
		printf("in main:: UnionConnectDesKeyDB Error! ret = [%d]\n",ret);
		UnionTaskActionBeforeExit();
		return(ret);
	}

	return(BatchEditDesKeys());
}


int BatchEditDesKeys()
{
	char	*p;
	int	ret;
	//TUnionDesKey	defaultKey;
	memset(&gdesKey,0,sizeof(gdesKey));
	gdesKey.type = conZMK;
	gdesKey.length = con128BitsDesKey;	
loopEdit:
	p = UnionInput("\n\nCommand>");
	strcpy(gkeyName,p);
	if (UnionIsValidDesKeyFullName(p))
	{
		strcpy(gdesKey.fullName,p);
		if ((ret = UnionReadDesKeyFromKeyDBAnyway(&gdesKey)) < 0)
		{
			printf("%s not exists!\n",gdesKey.fullName);
		}			
		else
		{
			UnionPrintDesKey(&gdesKey);
			if (EditGenerate() < 0)
				goto loopEdit;
			if (!UnionConfirm("Are you sure to update value of [%s] in desKeyDB",gdesKey.fullName))
				goto loopEdit;
			if ((ret = UnionUpdateDesKeyInKeyDB(&gdesKey)) < 0)
				printf("update [%s] in desKeyDB failure!\n",gdesKey.fullName);
			else
				printf("update [%s] in desKeyDB OK!\n",gdesKey.fullName);
		}
	}
	else if (UnionIsValidIPAddrStr(p))
	{
		memset(&gsjl06,0,sizeof(gsjl06));
		strcpy(gsjl06.staticAttr.ipAddr,p);
		if ((ret = UnionSelectSJL06Rec(&gsjl06)) < 0)
		{
			printf("UnionSelectSJL06Rec [%s] Error\n",p);
		}
		else
			gsjl06.dynamicAttr.status = conOnlineSJL06;
	}
	else if (UnionIsQuit(p))
		return(UnionTaskActionBeforeExit());
	else if (strcmp(p,"COMNUM") == 0)
		EditComponentNum();
	else if (strcmp(p,"GENERATE") == 0)
		EditGenerate();
	else if (strcmp(p,"LENGTH") == 0)
		UnionInputDesKeyLength();
	else if (strcmp(p,"TYPE") == 0)
		UnionInputDesKeyType();
	else if (strcmp(p,"HELP") == 0)
		EditHelp();		
	else if (strcmp(p,"PAR") == 0)
		EditInputPar();
	else 
	{
		if ((strlen(p) == 1) && isdigit(p[0]))
			gcomponentNum = atoi(p);
		if (strlen(p) > sizeof(gdesKey.fullName))
			goto loopEdit;
		else
		{
			strcpy(gdesKey.fullName,gkeyName);
			EditGenerate();
		}
	}
	
	goto loopEdit;
}

int UnionInputDesKeyLength()
{
	char *param;
	
loopUnionInput:
	if (UnionIsQuit(param = UnionInput("Des Key Length:(64/128/192/quit)")))
		return(0);
	if (strcmp(param,"64") == 0)
		gdesKey.length = con64BitsDesKey;
	else if (strcmp(param,"128") == 0)
		gdesKey.length = con128BitsDesKey;
	else if (strcmp(param,"192") == 0)
		gdesKey.length = con192BitsDesKey;
	else
	{
		printf("Invalid Des Key Length [%s]!\n",param);
		goto loopUnionInput;
	}
	
	return(0);
}

int UnionInputDesKeyType()
{
	char	*param;

loopUnionInput:
	if (UnionIsQuit(param = UnionInput("Key Type,quit to exit\n")))
		return(0);
	else if (strcmp(param,"ZPK") == 0)
		gdesKey.type = conZPK;
	else if (strcmp(param,"ZAK") == 0)
		gdesKey.type = conZAK;
	else if (strcmp(param,"ZMK") == 0)
		gdesKey.type = conZMK;
	else if (strcmp(param,"TMK") == 0)
		gdesKey.type = conTMK;
	else if (strcmp(param,"TPK") == 0)
		gdesKey.type = conTPK;
	else if (strcmp(param,"TAK") == 0)
		gdesKey.type = conTAK;
	else if (strcmp(param,"PVK") == 0)
		gdesKey.type = conPVK;
	else if (strcmp(param,"CVK") == 0)
		gdesKey.type = conCVK;
	else if (strcmp(param,"ZEK") == 0)
		gdesKey.type = conZEK;
	else if (strcmp(param,"WWK") == 0)
		gdesKey.type = conWWK;
	else if (strcmp(param,"BDK") == 0)
		gdesKey.type = conBDK;
	else if (strcmp(param,"USER") == 0)
		gdesKey.type = conSelfDefinedKey;
	else
	{
		printf("Invalid KeyType [%s]!\n",param);
		goto loopUnionInput;
	}
	
	return(0);
}

int EditComponentNum()
{
	char *p;
	
reinput:
	if (UnionIsQuit(p=UnionInput("ComponentNum::")))
		return(0);
	if ((atoi(p) <= 0) || (atoi(p) > 9))
	{
		printf("ComponentNum must be in [1,9]\n");
		goto reinput;
	}	
	gcomponentNum = atoi(p);
	return(0);
}

int EditInputPar()
{
	int	index;
	char	*p;
loopInput:
	if (UnionIsQuit(p = UnionInput("Input par index::")))
		return(0);
	if (((index = atoi(p)) < 0) || (index >= 20))
	{
		printf("par index must be of 0~%d\n",20);
		goto loopInput;
	}
loopInputPar:
	p = UnionInput("Input par[%02d]::\n",index);
	if (strlen(p) > sizeof(gparam[index]))
	{
		printf("par too long!");
		goto loopInputPar;
	}
	strcpy(gparam[index],p);
	if (UnionIsQuit(p))
	{
		memset(gparam[index],0,sizeof(gparam[index]));
		return(0);
	}
	goto loopInput;
}
	
int EditGenerate()
{
	long	i;
	int	ret;
	char	keyTypeName[40];
	int	parNum;
	
	//system("clear");
	printf("\n\n\n");
	printf("name       [%s]\n",gdesKey.fullName);
	memset(keyTypeName,0,sizeof(keyTypeName));
	UnionGetNameOfDesKey(gdesKey.type,keyTypeName);
	printf("type       [%s]\n",keyTypeName);
	printf("length     [%d]\n",UnionGetDesKeyBitsLength(gdesKey.length));
	printf("components [%d]\n",gcomponentNum);
	for (i = 0; i < 20; i++)
	{
		if (strlen(gparam[i]) > 0)
		{
			printf("par%02d      [%s]\n",i,gparam[i]);
		}
		else
		{
			parNum = i;
			break;
		}
	}
				
	if (!UnionConfirm("Are you sure to generate such a key use Hsm [%s]?",gsjl06.staticAttr.ipAddr))
		return(-1);
	
	printf("generate [%s] ...\n",gdesKey.fullName);
	
	ret = UnionGenerateKeyComponent(&gsjl06,&gdesKey,gcomponentNum,parNum,gparam);
	if (ret < 0)
	{
		printf("generate [%s] failure!\n",gdesKey.fullName);
		return(-2);
	}
	printf("generate [%s] ok!\n",gdesKey.fullName);
	return(0);
}
