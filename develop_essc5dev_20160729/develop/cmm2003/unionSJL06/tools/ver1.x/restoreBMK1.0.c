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
		if ((atoi(p) < 0) || (atoi(p) > 512))
		{
			printf("BMKË÷ÒýºÅ´í!ÇëÖØÊä!!\n");
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
	printf("Usage:: %s ipAddr bmkIndex bmkValue\n",UnionGetApplicationName());
	return(0);
}
	
int UnionReadBMK(char *ipAddr,char *bmkIndex)
{
	char		bmkValue[48+1];
	char		errCode[2+1];
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
			
	memset(errCode,0,sizeof(errCode));
	memset(bmkValue,0,sizeof(bmkValue));
	if ((ret = SJL06Cmd31(-1,&sjl06,bmkIndex,bmkValue,errCode)) < 0)
	{
		printf("¶ÁÈ¡ÃÜÂë»ú[%s]µÄBMK[%s]³ö´í! ´íÎóÂë=[%d]\n",ipAddr,bmkIndex,ret);
		return(ret);
	}
	if (strncmp(errCode,"00",2) != 0)
	{
		printf("¶ÁÈ¡ÃÜÂë»ú[%s]µÄBMK[%s]³ö´í! ÃÜÂë»ú´íÎóÂë[%s]\n",ipAddr,bmkIndex,errCode);
		return(-1);
	}
	printf("ÃÜÂë»ú[%s]µÄBMK[%s] = [%s]\n",ipAddr,bmkIndex,bmkValue);

	return(0);
}
