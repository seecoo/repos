//	Wolfgang Wang
//	2003/09/09

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "UnionStr.h"
#include "unionCommand.h"
#include "UnionEnv.h"
#include "UnionLog.h"

char	pgUnionInputStr[256];
char	pgUnionCmd[64][20+1];
int	gUnionCmdNum;

int UnionConnectCommandMDL(char *fileName)
{
	int	ret;
	char	tmpBuf[256];
	int	i;
	char	*p;
	
	memset(tmpBuf,0,sizeof(tmpBuf));
	sprintf(tmpBuf,"%s/etc/unionCmd/%s.CMD",getenv("HOME"),fileName);
	
	if ((ret = UnionInitEnvi(tmpBuf)) < 0)
	{
		UnionUserErrLog("in UnionConnectCommandMDL:: UnionInitEnvi [%s]!\n",tmpBuf);
		return(ret);
	}
	
	gUnionCmdNum = 64;
	for (i = 0; i < 64; i++)
	{
		memset(tmpBuf,0,sizeof(tmpBuf));
		sprintf(tmpBuf,"COMMAND%02d",i);
		memset(pgUnionCmd[i],0,sizeof(pgUnionCmd[i]));
		if ((p = UnionGetEnviVarByName(tmpBuf)) == NULL)
			continue;
		if (strlen(p) == 0)
			continue;
		if (strlen(p) > 20)
		{
			UnionUserErrLog("in UnionConnectCommandMDL:: command too long!\n");
			continue;
		}
		UnionToUpperCase(p);
		strcpy(pgUnionCmd[i],p);
	}
	
	UnionClearEnvi();
	return(gUnionCmdNum);
}

int UnionConfirm(char *fmt,...)
{
	va_list args;

	va_start(args,fmt);
	vprintf(fmt,args);
	va_end(args);
	
	printf("(Y/N)");
	memset(pgUnionInputStr,0,sizeof(pgUnionInputStr));
	scanf("%s",pgUnionInputStr);
	UnionToUpperCase(pgUnionInputStr);
	if (strncmp(pgUnionInputStr,"Y",1) == 0)
		return(1);
	else
		return(0);
}

int UnionIsQuit(char *p)
{
	if (p == NULL)
		return(0);
	if ((strcasecmp(p,"QUIT") == 0) || (strcasecmp(p,"EXIT") == 0))
		return(1);
	else
		return(0);
}

char *UnionInput(char *fmt,...)
{
	va_list args;

	va_start(args,fmt);
	vprintf(fmt,args);
	va_end(args);
	
	scanf("%s",pgUnionInputStr);
	return(pgUnionInputStr);
}

int UnionCommandInterprotor(char *fileName)
{
	int	i;
	char	command[256];
	int	ret;
	
	if ((ret = UnionConnectCommandMDL(fileName)) < 0)
	{
		UnionUserErrLog("in UnionCommandInterprotor:: UnionConnectCommandMDL!\n");
		return(ret);
	}
	
	for (;;)
	{
		memset(command,0,sizeof(command));
		printf("Command>");
		scanf("%s",command);
		UnionToUpperCase(command);
		if (strcmp(command,"HELP") == 0)
		{
			UnionHelp();
			continue;
		}
		if (strcmp(command,"EXIT") == 0)
			return(UnionExit());
		for (i = 0; i < gUnionCmdNum; i++)
		{
			if (strcmp(command,pgUnionCmd[i]) != 0)
				continue;
			else
				break;
		}
		if (i == gUnionCmdNum)
		{
			printf("Invalid Command!\n");
			continue;
		}
		UnionCallCommandFunction(i);
	}

}

int UnionCallCommandFunction(int cmdIndex)
{
	switch (cmdIndex)
	{
		case	0:
			return(UnionCommand0());
		case	1:
			return(UnionCommand1());
		case	2:
			return(UnionCommand2());
		case	3:
			return(UnionCommand3());
		case	4:
			return(UnionCommand4());
		case	5:
			return(UnionCommand5());
		case	6:
			return(UnionCommand6());
		case	7:
			return(UnionCommand7());
		case	8:
			return(UnionCommand8());
		case	9:
			return(UnionCommand9());
		case	10:
			return(UnionCommand10());
		case	11:
			return(UnionCommand11());
		case	12:
			return(UnionCommand12());
		case	13:
			return(UnionCommand13());
		case	14:
			return(UnionCommand14());
		case	15:
			return(UnionCommand15());
		case	16:
			return(UnionCommand16());
		case	17:
			return(UnionCommand17());
		case	18:
			return(UnionCommand18());
		case	19:
			return(UnionCommand19());
		case	20:
			return(UnionCommand20());
		case	21:
			return(UnionCommand21());
		case	22:
			return(UnionCommand22());
		case	23:
			return(UnionCommand23());
		case	24:
			return(UnionCommand24());
		case	25:
			return(UnionCommand25());
		case	26:
			return(UnionCommand26());
		case	27:
			return(UnionCommand27());
		case	28:
			return(UnionCommand28());
		case	29:
			return(UnionCommand29());
		case	30:
			return(UnionCommand30());
		case	31:
			return(UnionCommand31());
		case	32:
			return(UnionCommand32());
		case	33:
			return(UnionCommand33());
		case	34:
			return(UnionCommand34());
		case	35:
			return(UnionCommand35());
		case	36:
			return(UnionCommand36());
		case	37:
			return(UnionCommand37());
		case	38:
			return(UnionCommand38());
		case	39:
			return(UnionCommand39());
		case	40:
			return(UnionCommand40());
		case	41:
			return(UnionCommand41());
		case	42:
			return(UnionCommand42());
		case	43:
			return(UnionCommand43());
		case	44:
			return(UnionCommand44());
		case	45:
			return(UnionCommand45());
		case	46:
			return(UnionCommand46());
		case	47:
			return(UnionCommand47());
		case	48:
			return(UnionCommand48());
		case	49:
			return(UnionCommand49());
		case	50:
			return(UnionCommand50());
		case	51:
			return(UnionCommand51());
		case	52:
			return(UnionCommand52());
		case	53:
			return(UnionCommand53());
		case	54:
			return(UnionCommand54());
		case	55:
			return(UnionCommand55());
		case	56:
			return(UnionCommand56());
		case	57:
			return(UnionCommand57());
		case	58:
			return(UnionCommand58());
		case	59:
			return(UnionCommand59());
		case	60:
			return(UnionCommand60());
		case	61:
			return(UnionCommand61());
		case	62:
			return(UnionCommand62());
		case	63:
			return(UnionCommand63());
		case	64:
			return(UnionCommand64());
		default:
			return(0);
	}
}			
