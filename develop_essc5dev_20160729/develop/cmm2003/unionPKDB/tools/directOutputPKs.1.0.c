// Author:	Wolfgang Wang
// Date:	2005/06/07
// modify:      xusj 2009/10/14 可以根据应用编号导出密钥

#include <stdio.h>
#include <string.h>

#include "unionPKDB.h"

#include "sjl06.h"
#include "UnionStr.h"
#include "UnionSocket.h"
#include "UnionTask.h"
#include "unionCommand.h"
#include "UnionLog.h"

PUnionPKDB			pgunionPKDB;
PUnionPK			pgunionPK;

PUnionTaskInstance	ptaskInstance = NULL;

int UnionTaskActionBeforeExit()
{
	UnionDisconnectTaskTBL();
	UnionDisconnectLogFileTBL();
	UnionDisconnectPKDB();
	exit(0);
}


int UnionHelp()
{
	printf("Usage:: all       --output all pk in db\n",UnionGetApplicationName());
	printf("        appno     --output pk of appno in db\n",UnionGetApplicationName());
	printf("        fullname  --output one pk in db\n",UnionGetApplicationName());
	return(0);
}

int OutputAllKeysFromPKDB(char *fileName)
{
	int	ret;
	long	i;
	long	successNum = 0;
	FILE	*fp = NULL;
	char    thisAppNo[40];

	memset (thisAppNo, 0, sizeof thisAppNo);
	
	if ((ret = UnionConnectPKDB()) < 0)
	{
		UnionUserErrLog("in OutputAllKeysFromPKDB:: UnionConnectPKDB!\n");
		goto abnormalExit;
	}
	
	if ((fp = fopen(fileName,"w")) == NULL)	
	{
		UnionUserErrLog("in OutputAllKeysFromPKDB:: fopen [%s]!\n",fileName);
		goto abnormalExit;
	}
	for (i = 0; i < pgunionPKDB->num; i++)
	{
		UnionPrintPKToRecStrTxtFile(pgunionPK+i,fp);
		successNum++;
	}
	printf("successNum = [%ld]\n",successNum);
	printf("totalNum =   [%ld]\n",pgunionPKDB->num);
	fclose(fp);
	return(0);
abnormalExit:
	if (fp)
		fclose(fp);
	return(-1);
}

int OutputAllKeysOfAppFromPKDB(char *fileName,char *appNo)
{
	int	ret;
	long	i;
	long	successNum = 0;
	FILE	*fp = NULL;
	char    thisAppNo[40];

	memset (thisAppNo, 0, sizeof thisAppNo);
	
	if ((ret = UnionConnectPKDB()) < 0)
	{
		UnionUserErrLog("in OutputAllKeysOfAppFromPKDB:: UnionConnectPKDB!\n");
		goto abnormalExit;
	}
	
	if ((fp = fopen(fileName,"w")) == NULL)	
	{
		UnionUserErrLog("in OutputAllKeysOfAppFromPKDB:: fopen [%s]!\n",fileName);
		goto abnormalExit;
	}
	for (i = 0; i < pgunionPKDB->num; i++)
	{
		UnionGetAppNoFromFullKeyName((pgunionPK+i)->fullName, thisAppNo);
		if(strcasecmp(thisAppNo, appNo) != 0)
			continue;
		UnionPrintPKToRecStrTxtFile(pgunionPK+i,fp);
		successNum++;
	}
	printf("successNum = [%ld]\n",successNum);
	printf("totalNum =   [%ld]\n",pgunionPKDB->num);
	fclose(fp);
	return(0);
abnormalExit:
	if (fp)
		fclose(fp);
	return(-1);
}

int OutputOneKeyFromPKDB(char *fileName,char *fullName)
{
	int	ret;
	long	i;
	long	successNum = 0;
	FILE	*fp = NULL;

	if ((ret = UnionConnectPKDB()) < 0)
	{
		UnionUserErrLog("in OutputOneKeyFromPKDB:: UnionConnectPKDB!\n");
		goto abnormalExit;
	}
	
	if ((fp = fopen(fileName,"w")) == NULL)	
	{
		UnionUserErrLog("in OutputOneKeyFromPKDB:: fopen [%s]!\n",fileName);
		goto abnormalExit;
	}
	for (i = 0; i < pgunionPKDB->num; i++)
	{
		if(strcasecmp((pgunionPK+i)->fullName, fullName) != 0)
			continue;
		UnionPrintPKToRecStrTxtFile(pgunionPK+i,fp);
		successNum++;
	}
	printf("successNum = [%ld]\n",successNum);
	printf("totalNum =   [%ld]\n",pgunionPKDB->num);
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
	sprintf(fileName,"%s/tmp/PKDB.output.%s.txt",getenv("HOME"),datetime);
	if (strcasecmp(argv[1],"ALL") == 0)
	{
		if ((ret = OutputAllKeysFromPKDB(fileName)) < 0)
			printf("OutputAllKeysFromPKDB Failure!\n");
		else
		{
			printf("OutputAllKeysFromPKDB OK!\n");
			printf("keys stored in file [%s]\n",fileName);
		}
	}
	if (strcasecmp(argv[1],"APPNO") == 0)
	{
		if ((ret = OutputAllKeysOfAppFromPKDB(fileName,UnionInput("Input AppNo::"))) < 0)
                        printf("OutputAllKeysOfAppFromPKDB Failure!\n");
                else
                {
                        printf("OutputAllKeysOfAppFromPKDB OK!\n");
                        printf("keys stored in file [%s]\n",fileName);
                }
	}
	if (strcasecmp(argv[1],"FULLNAME") == 0)
	{
		if ((ret = OutputOneKeyFromPKDB(fileName,UnionInput("Input fullName::"))) < 0)
                        printf("OutputOneKeyFromPKDB Failure!\n");
                else
                {
                        printf("OutputOneKeyFromPKDB OK!\n");
                        printf("keys stored in file [%s]\n",fileName);
                }
	}
		
	return(UnionTaskActionBeforeExit());
}
