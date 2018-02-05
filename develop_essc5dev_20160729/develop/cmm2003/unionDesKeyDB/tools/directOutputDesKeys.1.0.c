// Author:	Wolfgang Wang
// Date:	2005/06/07
// modify:      xusj 2009/10/14 可以根据应用编号导出密钥

#include <stdio.h>
#include <string.h>

#include "unionDesKeyDB.h"

#include "sjl06.h"
#include "UnionStr.h"
#include "UnionSocket.h"
#include "UnionTask.h"
#include "unionCommand.h"
#include "UnionLog.h"

extern PUnionDesKeyDB			pgunionDesKeyDB;
extern PUnionDesKey			pgunionDesKey;

PUnionTaskInstance	ptaskInstance = NULL;

int UnionTaskActionBeforeExit()
{
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	UnionDisconnectDesKeyDB();
	exit(0);
}


int UnionHelp()
{
	printf("Usage:: all       --output all deskey in db\n",UnionGetApplicationName());
	printf("        appno     --output deskey of appno in db\n",UnionGetApplicationName());
	printf("        fullname  --output one deskey in db\n",UnionGetApplicationName());
	return(0);
}

int OutputAllKeysFromDesKeyDB(char *fileName)
{
	int	ret;
	long	i;
	long	successNum = 0;
	FILE	*fp = NULL;
	char    thisAppNo[40];

	memset (thisAppNo, 0, sizeof thisAppNo);
	
	if ((ret = UnionConnectDesKeyDB()) < 0)
	{
		UnionUserErrLog("in OutputAllKeysFromDesKeyDB:: UnionConnectDesKeyDB!\n");
		goto abnormalExit;
	}
	
	if ((fp = fopen(fileName,"w")) == NULL)	
	{
		UnionUserErrLog("in OutputAllKeysFromDesKeyDB:: fopen [%s]!\n",fileName);
		goto abnormalExit;
	}
	for (i = 0; i < pgunionDesKeyDB->num; i++)
	{
		UnionPrintDesKeyToRecStrTxtFile(pgunionDesKey+i,fp);
		successNum++;
	}
	printf("successNum = [%ld]\n",successNum);
	printf("totalNum =   [%ld]\n",pgunionDesKeyDB->num);
	fclose(fp);
	return(0);
abnormalExit:
	if (fp)
		fclose(fp);
	return(-1);
}

int OutputAllKeysOfAppFromDesKeyDB(char *fileName,char *appNo)
{
	int	ret;
	long	i;
	long	successNum = 0;
	FILE	*fp = NULL;
	char    thisAppNo[40];

	memset (thisAppNo, 0, sizeof thisAppNo);
	
	if ((ret = UnionConnectDesKeyDB()) < 0)
	{
		UnionUserErrLog("in OutputAllKeysOfAppFromDesKeyDB:: UnionConnectDesKeyDB!\n");
		goto abnormalExit;
	}
	
	if ((fp = fopen(fileName,"w")) == NULL)	
	{
		UnionUserErrLog("in OutputAllKeysOfAppFromDesKeyDB:: fopen [%s]!\n",fileName);
		goto abnormalExit;
	}
	for (i = 0; i < pgunionDesKeyDB->num; i++)
	{
		UnionGetAppNoFromFullKeyName((pgunionDesKey+i)->fullName, thisAppNo);
		if(strcasecmp(thisAppNo, appNo) != 0)
			continue;
		UnionPrintDesKeyToRecStrTxtFile(pgunionDesKey+i,fp);
		successNum++;
	}
	printf("successNum = [%ld]\n",successNum);
	printf("totalNum =   [%ld]\n",pgunionDesKeyDB->num);
	fclose(fp);
	return(0);
abnormalExit:
	if (fp)
		fclose(fp);
	return(-1);
}

int OutputOneKeyFromDesKeyDB(char *fileName,char *fullName)
{
	int	ret;
	long	i;
	long	successNum = 0;
	FILE	*fp = NULL;

	if ((ret = UnionConnectDesKeyDB()) < 0)
	{
		UnionUserErrLog("in OutputOneKeyFromDesKeyDB:: UnionConnectDesKeyDB!\n");
		goto abnormalExit;
	}
	
	if ((fp = fopen(fileName,"w")) == NULL)	
	{
		UnionUserErrLog("in OutputOneKeyFromDesKeyDB:: fopen [%s]!\n",fileName);
		goto abnormalExit;
	}
	for (i = 0; i < pgunionDesKeyDB->num; i++)
	{
		if(strcasecmp((pgunionDesKey+i)->fullName, fullName) != 0)
			continue;
		UnionPrintDesKeyToRecStrTxtFile(pgunionDesKey+i,fp);
		successNum++;
	}
	printf("successNum = [%ld]\n",successNum);
	printf("totalNum =   [%ld]\n",pgunionDesKeyDB->num);
	fclose(fp);
	return(0);
abnormalExit:
	if (fp)
		fclose(fp);
	return(-1);
}

int main(int argc,char *argv[])
{
	int	ret;
	char	*p;
	char	fileName[512];
	char	datetime[100];
	char	appNo[40];
	
	UnionSetApplicationName(argv[0]);
	if (argc < 2)
		return(UnionHelp());

	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s",UnionGetApplicationName())) == NULL)
	{
		printf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}

	if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
		return(UnionTaskActionBeforeExit());
	
	memset(datetime,0,sizeof(datetime));
	UnionGetFullSystemDate(datetime);
	memset(fileName,0,sizeof(fileName));
	sprintf(fileName,"%s/tmp/desKeyDB.output.%s.txt",getenv("HOME"),datetime);
	if (strcasecmp(argv[1],"ALL") == 0)
	{
		if ((ret = OutputAllKeysFromDesKeyDB(fileName)) < 0)
			printf("OutputAllKeysFromDesKeyDB Failure!\n");
		else
		{
			printf("OutputAllKeysFromDesKeyDB OK!\n");
			printf("keys stored in file [%s]\n",fileName);
		}
	}
	if (strcasecmp(argv[1],"APPNO") == 0)
	{
		if ((ret = OutputAllKeysOfAppFromDesKeyDB(fileName,UnionInput("Input AppNo::"))) < 0)
                        printf("OutputAllKeysOfAppFromDesKeyDB Failure!\n");
                else
                {
                        printf("OutputAllKeysOfAppFromDesKeyDB OK!\n");
                        printf("keys stored in file [%s]\n",fileName);
                }
	}
	if (strcasecmp(argv[1],"FULLNAME") == 0)
	{
		if ((ret = OutputOneKeyFromDesKeyDB(fileName,UnionInput("Input fullName::"))) < 0)
                        printf("OutputOneKeyFromDesKeyDB Failure!\n");
                else
                {
                        printf("OutputOneKeyFromDesKeyDB OK!\n");
                        printf("keys stored in file [%s]\n",fileName);
                }
	}
		
	return(UnionTaskActionBeforeExit());
}
