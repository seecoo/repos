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
	char	keyValue[100],checkValue[100];
	char	terminalKeyIndex[20];
			
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
		if (UnionIsQuit((p = UnionInput("ÇëÊäÈëÃÜÂë»úIPµØÖ·(ExitÍË³ö)::"))))
			return(UnionTaskActionBeforeExit());
		if (!UnionIsValidIPAddrStr(p))
		{
			printf("IPµØÖ·´íÎó!ÇëÖØÊä!!\n");
			goto loopInputIPAddr;
		}
		strcpy(ipAddr,p);
	}
	else
		strcpy(ipAddr,argv[1]);
	
	if (argc < 3)
	{
loopInputBMKIndex:
		if (UnionIsQuit((p = UnionInput("ÇëÊäÈëBMKË÷ÒýºÅ(ExitÍË³ö)::"))))
			return(UnionTaskActionBeforeExit());
		memset(bmkIndex,0,sizeof(bmkIndex));
		sprintf(bmkIndex,"%03d",atoi(p));
	}
	else
		strcpy(bmkIndex,argv[2]);
	
	if (argc < 4)
	{
loopInputTerminalKeyIndex:
		if (UnionIsQuit((p = UnionInput("ÇëÊäÈëTMKË÷ÒýºÅ(ExitÍË³ö)::"))))
			return(UnionTaskActionBeforeExit());
		memset(terminalKeyIndex,0,sizeof(terminalKeyIndex));
		sprintf(terminalKeyIndex,"%03d",atoi(p));
	}
	
	if (argc < 5)
	{
		if (UnionIsQuit((p = UnionInput("ÇëÊäÈëÖÕ¶ËÃÜÔ¿ÃÜÎÄ(ExitÍË³ö)::"))))
			return(UnionTaskActionBeforeExit());
		strcpy(keyValue,p);	
	}
	else
		strcpy(keyValue,argv[4]);
			
	if (argc < 6)
	{
		if (UnionIsQuit((p = UnionInput("ÇëÊäÈëÖÕ¶ËÃÜÔ¿Ð£ÑéÖµ(ExitÍË³ö)::"))))
			return(UnionTaskActionBeforeExit());
		strcpy(checkValue,p);	
	}
	else
		strcpy(checkValue,argv[5]);
			
	UnionStoreTerminalKey(ipAddr,bmkIndex,terminalKeyIndex,keyValue,checkValue);

	return(UnionTaskActionBeforeExit());
}

int UnionHelp()
{
	printf("Usage:: %s ipAddr bmkIndex keyValue checkValue\n",UnionGetApplicationName());
	return(0);
}
	
int UnionStoreTerminalKey(char *ipAddr,char *bmkIndex,char *terminalKeyIndex,char *keyValue,char *checkValue)
{
	char		errCode[2+1],localCheckValue[16+1];
	int		len;
	int		ret;
	TUnionSJL06	sjl06;
	
	if (!UnionIsValidIPAddrStr(ipAddr))
	{
		printf("ÃÜÂë»úIPµØÖ·[%s]´íÎó!\n",ipAddr);
		return(-1);
	}
	
	memset(&sjl06,0,sizeof(sjl06));
	strcpy(sjl06.staticAttr.ipAddr,ipAddr);
	if ((ret = UnionSelectSJL06Rec(&sjl06)) < 0)
	{
		printf("¶ÁÃÜÂë»ú[%s]µÄÅäÖÃÎÄ¼þ³ö´í!\n",ipAddr);
		return(ret);
	}
	sjl06.dynamicAttr.status = conOnlineSJL06;
			
	memset(errCode,0,sizeof(errCode));
	if ((ret = SJL06Cmd21(-1,&sjl06,bmkIndex,terminalKeyIndex,keyValue,errCode)) < 0)
	{
		printf("´æ´¢ÃÜÂë»ú[%s]µÄÖÕ¶ËÃÜÔ¿[%s]³ö´í! ´íÎóÂë=[%d]\n",ipAddr,terminalKeyIndex,ret);
		return(ret);
	}
	if (strncmp(errCode,"00",2) != 0)
	{
		printf("´æ´¢ÃÜÂë»ú[%s]µÄÖÕ¶ËÃÜÔ¿[%s]³ö´í! ÃÜÂë»ú´íÎóÂë[%s]\n",ipAddr,terminalKeyIndex,errCode);
		return(-1);
	}
	memset(errCode,0,sizeof(errCode));
	memset(localCheckValue,0,sizeof(localCheckValue));
	if ((ret = SJL06Cmd13(-1,&sjl06,bmkIndex,keyValue,localCheckValue,errCode)) < 0)
	{
		printf("Éú³ÉÃÜÂë»ú[%s]µÄTerminalKey[%s]µÄÐ£ÑéÖµ³ö´í! ´íÎóÂë=[%d]\n",ipAddr,terminalKeyIndex,ret);
		return(ret);
	}
	if (strncmp(errCode,"00",2) != 0)
	{
		printf("Éú³ÉÃÜÂë»ú[%s]µÄTerminalKey[%s]µÄÐ£ÑéÖµ³ö´í! ÃÜÂë»ú´íÎóÂë[%s]\n",ipAddr,terminalKeyIndex,errCode);
		return(-1);
	}
	if (strncmp(localCheckValue,checkValue,4) != 0)
	{
		printf("±¾µØÐ£ÑéÖµ[%s] != [%s]\n",localCheckValue,checkValue);
		return(-1);
	}
	printf("´æ´¢ÃÜÂë»ú[%s]µÄÖÕ¶ËÃÜÔ¿[%s]=[%s]³É¹¦\n",ipAddr,terminalKeyIndex,keyValue);

	return(0);
}
