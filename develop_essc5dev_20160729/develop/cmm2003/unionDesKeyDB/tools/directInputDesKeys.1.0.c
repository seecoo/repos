// Author:	Wolfgang Wang
// Date:	2005/06/07

#include <stdio.h>
#include <string.h>

#include "unionDesKeyDB.h"

#include "sjl06.h"
#include "UnionStr.h"
#include "UnionSocket.h"
#include "UnionTask.h"
#include "unionCommand.h"
#include "UnionLog.h"

/***
extern PUnionDesKeyDB			pgunionDesKeyDB;
extern PUnionDesKey			pgunionDesKey;
***/

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
	printf("Usage:: %s ipAddrOfHsm zmkValue fileName\n",UnionGetApplicationName());
	return(0);
}

int InputAllKeysIntoDesKeyDB(char *fileName)
{
	int	ret;
	long	index;
	long	successNum = 0,totalNum=0;
	FILE	*fp = NULL;
	TUnionDesKey	key;
	char    buf[1024];

	memset(&key, 0, sizeof key);
	memset(buf, 0, sizeof buf);
	
/***
	if ((ret = UnionConnectDesKeyDB()) < 0)
	{
		UnionUserErrLog("in InputAllKeysIntoDesKeyDB:: UnionConnectDesKeyDB!\n");
		goto abnormalExit;
	}
***/
	
	if ((fp = fopen(fileName,"r")) == NULL)	
	{
		UnionUserErrLog("in InputAllKeysIntoDesKeyDB:: fopen [%s]!\n",fileName);
		goto abnormalExit;
	}
	while ( fgets(buf, sizeof(buf), fp) != NULL )
	{
		buf[strlen(buf)-1]=0;
		if ( strlen(buf) == 0 )
			continue;

		totalNum++;
		if (UnionReadDesKeyFromRecStr(buf, strlen(buf), &key) < 0)
		{
			UnionUserErrLog("in InputAllKeysIntoDesKeyDB:: UnionReadDesKeyFromRecStr [%s]!\n", buf);
			continue;
		}

/***
		if ((index = UnionFindDesKeyPosInKeyDB(key.fullName)) < 0)
		{
			UnionUserErrLog("in InputAllKeysIntoDesKeyDB:: UnionFindDesKeyPosInKeyDB [%s]!\n",key.fullName);
			printf("Noexist:: %40s\n",key.fullName);
			continue;
		}
		memcpy((pgunionDesKey+index), &key, sizeof(key));
***/
		if ( UnionInsertDesKeyIntoKeyDB(&key) < 0 )
		{
			UnionUserErrLog("in InputAllKeysIntoDesKeyDB:: UnionInsertDesKeyIntoKeyDB [%s]!\n",key.fullName);
			printf("Noexist:: %40s\n",key.fullName);
			continue;
		}
		successNum++;
	}
	printf("successNum = [%ld]\n",successNum);
	printf("totalNum =   [%ld]\n",totalNum);
/***
	if ((ret = UnionMirrorDesKeyDBIntoDisk(pgunionDesKeyDB,pgunionDesKey)) < 0)
		printf("in InputAllKeysIntoDesKeyDB:: UnionMirrorDesKeyDBIntoDisk!\n");
***/
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
	char	fileName[100],fullFileName[512];
	char	datetime[100];
	
	UnionSetApplicationName(argv[0]);
	if ((ptaskInstance = UnionCreateTaskInstance(UnionTaskActionBeforeExit,"%s",UnionGetApplicationName())) == NULL)
	{
		printf("UnionCreateTaskInstance Error!\n");
		return(UnionTaskActionBeforeExit());
	}

	//if (UnionExcutePreDefinedCommand(argc-1,&argv[1]))
	//	return(UnionTaskActionBeforeExit());
	
	memset(fileName,0,sizeof(fileName));
	if (argc >= 2)
		strcpy(fileName,argv[1]);
inputFileName:
	if (strlen(fileName) == 0)
	{
		strcpy(fileName,p=UnionInput("请输入ZMK加密的文件名(exit退出)::"));
		if (UnionIsQuit(p))
			return(UnionTaskActionBeforeExit());
	}
	sprintf(fullFileName,"%s/tmp/%s",getenv("HOME"),fileName);
	if ((ret = InputAllKeysIntoDesKeyDB(fullFileName)) < 0)
		printf("InputAllKeysIntoDesKeyDB Failure!\n");
	else
	{
		printf("InputAllKeysIntoDesKeyDB OK!\n");
		printf("keys restored from file [%s]\n",fullFileName);
	}
		
	return(UnionTaskActionBeforeExit());
}
